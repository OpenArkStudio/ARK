/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include "Common/AFBaseStruct.hpp"
#include "AFCTCPClient.h"

namespace ark
{

    AFCTCPClient::AFCTCPClient(const brynet::net::TcpService::PTR& service/* = nullptr*/, const brynet::net::AsyncConnector::PTR& connector/* = nullptr*/)
    {
        brynet::net::base::InitSocket();

        m_pTcpService = (service != nullptr ? service : brynet::net::TcpService::Create());
        m_pConnector = (connector != nullptr ? connector : brynet::net::AsyncConnector::Create());
    }

    AFCTCPClient::~AFCTCPClient()
    {
        Shutdown();
        brynet::net::base::DestroySocket();
    }

    void AFCTCPClient::Update()
    {
        ProcessMsgLogicThread();
    }

    bool AFCTCPClient::Start(const int target_busid, const std::string& ip, const int port, bool ip_v6/* = false*/)
    {
        m_pTcpService->startWorkerThread(1);
        m_pConnector->startWorkerThread();

        brynet::net::TcpSocket::PTR socket = brynet::net::SyncConnectSocket(ip, port, ARK_CONNECT_TIMEOUT, m_pConnector);
        if (socket == nullptr)
        {
            return false;
        }

        socket->SocketNodelay();
        auto OnEnterCallback = [ = ](const brynet::net::DataSocket::PTR & session)
        {
            AFCTCPClient* pTcpClient = this;
            AFTCPMsg* pMsg = new AFTCPMsg(session);
            pMsg->xClientID.nLow = (++pTcpClient->mnNextID);
            session->setUD(static_cast<int64_t>(pMsg->xClientID.nLow));
            pMsg->nType = CONNECTED;

            do
            {
                AFScopeWrLock xGuard(pTcpClient->mRWLock);

                AFTCPEntity* pEntity = new AFTCPEntity(pTcpClient, pMsg->xClientID, session);
                pTcpClient->m_pClientEntity.reset(pEntity);
                pEntity->mxNetMsgMQ.Push(pMsg);
            } while (false);

            //data cb
            session->setDataCallback([pTcpClient, session](const char* buffer, size_t len)
            {
                const auto ud = brynet::net::cast<int64_t>(session->getUD());
                AFGUID xClient(0, *ud);

                AFScopeRdLock xGuard(pTcpClient->mRWLock);

                if (pTcpClient->m_pClientEntity->GetClientID() == xClient)
                {
                    pTcpClient->m_pClientEntity->AddBuff(buffer, len);
                    pTcpClient->DismantleNet(pTcpClient->m_pClientEntity.get());
                }

                return len;
            });

            //disconnect cb
            session->setDisConnectCallback([pTcpClient](const brynet::net::DataSocket::PTR & session)
            {
                const auto ud = brynet::net::cast<int64_t>(session->getUD());
                AFGUID xClient(0, *ud);

                AFTCPMsg* pMsg = new AFTCPMsg(session);
                pMsg->xClientID = xClient;
                pMsg->nType = DISCONNECTED;

                do
                {
                    AFScopeWrLock xGuard(pTcpClient->mRWLock);
                    pTcpClient->m_pClientEntity->mxNetMsgMQ.Push(pMsg);
                } while (false);
            });
        };

        m_pTcpService->addDataSocket(std::move(socket),
                                     brynet::net::TcpService::AddSocketOption::WithEnterCallback(OnEnterCallback),
                                     brynet::net::TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_TCP_RECV_BUFFER_SIZE));
        //////////////////////////////////////////////////////////////////////////
        //m_pConnector->asyncConnect(ip, port, ARK_CONNECT_TIMEOUT, [ = ](brynet::net::TcpSocket::PTR socket)
        //{
        //    AFCTCPClient* pTcpClient = this;
        //    socket->SocketNodelay();
        //    auto OnEnterCallback = [pTcpClient](const brynet::net::DataSocket::PTR & session)
        //    {
        //        AFTCPMsg* pMsg = new AFTCPMsg(session);
        //        pMsg->xClientID.nLow = (++pTcpClient->mnNextID);
        //        session->setUD(static_cast<int64_t>(pMsg->xClientID.nLow));
        //        pMsg->nType = CONNECTED;

        //        do
        //        {
        //            AFScopeWrLock xGuard(pTcpClient->mRWLock);

        //            AFTCPEntity* pEntity = new AFTCPEntity(pTcpClient, pMsg->xClientID, session);
        //            pTcpClient->m_pClientEntity.reset(pEntity);
        //            pEntity->mxNetMsgMQ.Push(pMsg);
        //        } while (false);

        //        //data cb
        //        session->setDataCallback([pTcpClient, session](const char* buffer, size_t len)
        //        {
        //            const auto ud = brynet::net::cast<int64_t>(session->getUD());
        //            AFGUID xClient(0, *ud);

        //            AFScopeRdLock xGuard(pTcpClient->mRWLock);

        //            if (pTcpClient->m_pClientEntity->GetClientID() == xClient)
        //            {
        //                pTcpClient->m_pClientEntity->AddBuff(buffer, len);
        //                pTcpClient->DismantleNet(pTcpClient->m_pClientEntity.get());
        //            }

        //            return len;
        //        });

        //        //disconnect cb
        //        session->setDisConnectCallback([pTcpClient](const brynet::net::DataSocket::PTR & session)
        //        {
        //            const auto ud = brynet::net::cast<int64_t>(session->getUD());
        //            AFGUID xClient(0, *ud);

        //            AFTCPMsg* pMsg = new AFTCPMsg(session);
        //            pMsg->xClientID = xClient;
        //            pMsg->nType = DISCONNECTED;

        //            do
        //            {
        //                AFScopeWrLock xGuard(pTcpClient->mRWLock);
        //                pTcpClient->m_pClientEntity->mxNetMsgMQ.Push(pMsg);
        //            } while (false);
        //        });
        //    };

        //    m_pTcpService->addDataSocket(std::move(socket),
        //                                 brynet::net::TcpService::AddSocketOption::WithEnterCallback(OnEnterCallback),
        //                                 brynet::net::TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_TCP_RECV_BUFFER_SIZE));
        //}, [ = ]()
        //{
        //    std::string bus_id = AFBusAddr(target_busid).ToString();
        //    std::string error = ARK_FORMAT("connect failed, target_bus_id:{} ip:{} port:{}", bus_id, ip, port);
        //    CONSOLE_LOG_NO_FILE << error << std::endl;
        //});

        mnTargetBusID = target_busid;
        SetWorking(true);
        return true;
    }

    bool AFCTCPClient::Shutdown()
    {
        if (!CloseSocketAll())
        {
            //add log
        }

        m_pConnector->stopWorkerThread();
        m_pTcpService->stopWorkerThread();
        SetWorking(false);
        return true;
    }

    bool AFCTCPClient::CloseSocketAll()
    {
        if (nullptr != m_pClientEntity)
        {
            m_pClientEntity->GetSession()->postDisConnect();
        }

        return true;
    }

    bool AFCTCPClient::SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient)
    {
        if (nullptr != m_pClientEntity && m_pClientEntity->GetSession())
        {
            m_pClientEntity->GetSession()->send(msg, nLen);
        }

        return true;
    }

    bool AFCTCPClient::CloseNetEntity(const AFGUID& xClient)
    {
        if (nullptr != m_pClientEntity && m_pClientEntity->GetClientID() == xClient)
        {
            m_pClientEntity->GetSession()->postDisConnect();
        }

        return true;
    }

    bool AFCTCPClient::DismantleNet(AFTCPEntity* pEntity)
    {
        while (pEntity->GetBuffLen() >= ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
        {
            ARK_PKG_CS_HEAD xHead;
            int nMsgBodyLength = DeCode(pEntity->GetBuff(), pEntity->GetBuffLen(), xHead);

            if (nMsgBodyLength >= 0 && xHead.GetMsgID() > 0)
            {
                AFTCPMsg* pMsg = ARK_NEW AFTCPMsg(pEntity->GetSession());
                pMsg->xHead = xHead;
                pMsg->nType = RECVDATA;
                pMsg->strMsg.append(pEntity->GetBuff() + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH, nMsgBodyLength);
                pEntity->mxNetMsgMQ.Push(pMsg);
                pEntity->RemoveBuff(nMsgBodyLength + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH);
            }
            else
            {
                break;
            }
        }

        return true;
    }

    void AFCTCPClient::ProcessMsgLogicThread()
    {
        do
        {
            AFScopeRdLock xGuard(mRWLock);
            ProcessMsgLogicThread(m_pClientEntity.get());
        } while (false);

        if (m_pClientEntity != nullptr && m_pClientEntity->NeedRemove())
        {
            AFScopeWrLock xGuard(mRWLock);
            m_pClientEntity.reset(nullptr);
        }

    }

    void AFCTCPClient::ProcessMsgLogicThread(AFTCPEntity* pEntity)
    {
        if (pEntity == nullptr)
        {
            return;
        }

        size_t nReceiveCount = pEntity->mxNetMsgMQ.Count();

        for (size_t i = 0; i < nReceiveCount; ++i)
        {
            AFTCPMsg* pMsg(nullptr);

            if (!pEntity->mxNetMsgMQ.Pop(pMsg))
            {
                break;
            }

            if (pMsg == nullptr)
            {
                continue;
            }

            switch (pMsg->nType)
            {
            case RECVDATA:
                {
                    if (mRecvCB)
                    {
                        mRecvCB(pMsg->xHead, pMsg->xHead.GetMsgID(), pMsg->strMsg.c_str(), pMsg->strMsg.size(), pEntity->GetClientID());
                    }
                }
                break;

            case CONNECTED:
                mEventCB((NetEventType)pMsg->nType, pMsg->xClientID, mnTargetBusID);
                break;

            case DISCONNECTED:
                {
                    mEventCB((NetEventType)pMsg->nType, pMsg->xClientID, mnTargetBusID);
                    pEntity->SetNeedRemove(true);
                }
                break;

            default:
                break;
            }

            delete pMsg;
        }
    }

    bool AFCTCPClient::IsServer()
    {
        return false;
    }

    bool AFCTCPClient::Log(int severity, const char* msg)
    {
        //Will add log
        return true;
    }

    bool AFCTCPClient::SendRawMsg(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID)
    {
        std::string strOutData;
        ARK_PKG_CS_HEAD xHead;
        xHead.SetMsgID(nMsgID);
        xHead.SetPlayerID(xPlayerID);
        xHead.SetBodyLength(nLen);

        int nAllLen = EnCode(xHead, msg, nLen, strOutData);

        if (nAllLen == nLen + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
        {
            return SendMsg(strOutData.c_str(), strOutData.length(), xClientID);
        }
        else
        {
            return false;
        }
    }

    int AFCTCPClient::EnCode(const ARK_PKG_CS_HEAD& xHead, const char* strData, const size_t len, std::string& strOutData)
    {
        char szHead[ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH] = { 0 };
        xHead.EnCode(szHead);

        strOutData.clear();
        strOutData.append(szHead, ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH);
        strOutData.append(strData, len);

        return xHead.GetBodyLength() + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH;
    }

    int AFCTCPClient::DeCode(const char* strData, const size_t len, ARK_PKG_CS_HEAD& xHead)
    {
        if (len < ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
        {
            return -1;
        }

        if (ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH != xHead.DeCode(strData))
        {
            return -2;
        }

        if (xHead.GetBodyLength() > (len - ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH))
        {
            return -3;
        }

        return xHead.GetBodyLength();
    }

}