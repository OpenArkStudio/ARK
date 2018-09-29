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

#include "AFCWebSocktServer.h"

namespace ark
{

    AFCWebSocktServer::AFCWebSocktServer()
    {
        brynet::net::base::InitSocket();
        m_pTcpService = brynet::net::TcpService::Create();
        m_pListenThread = brynet::net::ListenThread::Create();
    }

    AFCWebSocktServer::~AFCWebSocktServer()
    {
        Shutdown();
        brynet::net::base::DestroySocket();
    }

    void AFCWebSocktServer::Update()
    {
        ProcessMsgLogicThread();
    }

    bool AFCWebSocktServer::Start(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6/* = false*/)
    {
        mnServerID = busid;
        m_pTcpService->startWorkerThread(thread_num);

        m_pListenThread->startListen(ip_v6, ip, port, [ = ](brynet::net::TcpSocket::PTR socket)
        {
            AFCWebSocktServer* pWSServer = this;
            auto OnEnterCallback = [pWSServer](const brynet::net::DataSocket::PTR & session)
            {
                brynet::net::HttpService::setup(session, [pWSServer](const brynet::net::HttpSession::PTR & httpSession)
                {
                    AFHttpMsg* pMsg = new AFHttpMsg(httpSession);
                    pMsg->xClientID.nLow = pWSServer->nNextID++;
                    httpSession->setUD(static_cast<int64_t>(pMsg->xClientID.nLow));
                    pMsg->nType = CONNECTED;

                    do
                    {
                        AFScopeWrLock xGuard(pWSServer->mRWLock);

                        AFHttpEntity* pEntity = new AFHttpEntity(pWSServer, pMsg->xClientID, httpSession);

                        if (pWSServer->AddNetEntity(pMsg->xClientID, pEntity))
                        {
                            pEntity->mxNetMsgMQ.Push(pMsg);
                        }
                    } while (false);

                    httpSession->setWSCallback([pWSServer](const brynet::net::HttpSession::PTR & httpSession,
                                                           brynet::net::WebSocketFormat::WebSocketFrameType opcode,
                                                           const std::string & payload)
                    {
                        switch (opcode)
                        {
                        case brynet::net::WebSocketFormat::WebSocketFrameType::ERROR_FRAME:
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::CONTINUATION_FRAME:
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::TEXT_FRAME:
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::BINARY_FRAME:
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::CLOSE_FRAME:
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::PING_FRAME:
                            {
                                auto frame = std::make_shared<std::string>();
                                brynet::net::WebSocketFormat::wsFrameBuild(payload.c_str(),
                                        payload.size(),
                                        *frame,
                                        brynet::net::WebSocketFormat::WebSocketFrameType::PONG_FRAME,
                                        true,
                                        false);
                                httpSession->send(frame);
                                return;
                            }
                            break;
                        case brynet::net::WebSocketFormat::WebSocketFrameType::PONG_FRAME:
                            break;
                        default:
                            break;
                        }

                        const auto ud = brynet::net::cast<int64_t>(httpSession->getUD());
                        AFGUID xClient(0, *ud);

                        AFScopeRdLock xGuard(pWSServer->mRWLock);
                        auto xFind = pWSServer->mxNetEntities.find(xClient);
                        if (xFind == pWSServer->mxNetEntities.end())
                        {
                            return;
                        }

                        xFind->second->AddBuff(payload.c_str(), payload.size());
                        pWSServer->DismantleNet(xFind->second);
                    });

                    httpSession->setHttpCallback([](const brynet::net::HTTPParser & httpParser, const brynet::net::HttpSession::PTR & session)
                    {
                        brynet::net::HttpResponse response;
                        response.setBody("<html>Hello ark</html>");
                        std::string result = response.getResult();
                        session->send(result.c_str(), result.size(), [session]()
                        {
                            session->postShutdown();
                        });
                    });

                    httpSession->setCloseCallback([pWSServer](const brynet::net::HttpSession::PTR & httpSession)
                    {
                        const auto ud = brynet::net::cast<int64_t>(httpSession->getUD());
                        AFGUID xClient(0, 0);
                        xClient.nLow = *ud;
                        AFScopeWrLock xGuard(pWSServer->mRWLock);

                        auto xFind = pWSServer->mxNetEntities.find(xClient);
                        if (xFind == pWSServer->mxNetEntities.end())
                        {
                            return;
                        }

                        AFHttpMsg* pMsg = new AFHttpMsg(httpSession);
                        pMsg->xClientID = xClient;
                        pMsg->nType = DISCONNECTED;

                        xFind->second->mxNetMsgMQ.Push(pMsg);
                    });
                });
            };

            m_pTcpService->addDataSocket(std::move(socket),
                                         brynet::net::TcpService::AddSocketOption::WithEnterCallback(OnEnterCallback),
                                         brynet::net::TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_HTTP_RECV_BUFFER_SIZE));
        });

        return true;
    }

    void AFCWebSocktServer::ProcessMsgLogicThread()
    {
        std::list<AFGUID> xNeedRemoveList;

        do
        {
            AFScopeRdLock xGuard(mRWLock);

            for (std::map<AFGUID, AFHttpEntity*>::iterator iter = mxNetEntities.begin(); iter != mxNetEntities.end(); ++iter)
            {
                ProcessMsgLogicThread(iter->second);

                if (!iter->second->NeedRemove())
                {
                    continue;
                }

                xNeedRemoveList.push_back(iter->second->GetClientID());
            }
        } while (0);

        for (auto iter : xNeedRemoveList)
        {
            AFScopeWrLock xGuard(mRWLock);
            RemoveNetEntity(iter);
        }
    }

    void AFCWebSocktServer::ProcessMsgLogicThread(AFHttpEntity* pEntity)
    {
        //Handle Msg
        size_t nReceiveCount = pEntity->mxNetMsgMQ.Count();

        for (size_t i = 0; i < nReceiveCount; ++i)
        {
            AFHttpMsg* pMsg(nullptr);

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
                mEventCB((NetEventType)pMsg->nType, pMsg->xClientID, mnServerID);
                break;

            case DISCONNECTED:
                {
                    mEventCB((NetEventType)pMsg->nType, pMsg->xClientID, mnServerID);
                    pEntity->SetNeedRemove(true);
                }
                break;

            default:
                break;
            }

            delete pMsg;
        }
    }

    bool AFCWebSocktServer::Shutdown()
    {
        SetWorking(false);
        return true;
    }

    bool AFCWebSocktServer::IsServer()
    {
        return true;
    }

    bool AFCWebSocktServer::SendMsgToAllClient(const char* msg, const size_t nLen)
    {
        auto frame = std::make_shared<std::string>();
        brynet::net::WebSocketFormat::wsFrameBuild(msg,
                nLen,
                *frame,
                brynet::net::WebSocketFormat::WebSocketFrameType::BINARY_FRAME,
                true,
                false);

        std::map<AFGUID, AFHttpEntity*>::iterator it = mxNetEntities.begin();

        for (; it != mxNetEntities.end(); ++it)
        {
            AFHttpEntity* pNetObject = (AFHttpEntity*)it->second;

            if (pNetObject && !pNetObject->NeedRemove())
            {
                pNetObject->GetSession()->send(frame);
            }
        }

        return true;
    }

    bool AFCWebSocktServer::SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient)
    {
        AFScopeRdLock xGuard(mRWLock);

        AFHttpEntity* pNetObject = GetNetEntity(xClient);

        if (pNetObject == nullptr)
        {
            return false;
        }

        auto frame = std::make_shared<std::string>();
        brynet::net::WebSocketFormat::wsFrameBuild(msg,
                nLen,
                *frame,
                brynet::net::WebSocketFormat::WebSocketFrameType::BINARY_FRAME,
                true,
                false);

        pNetObject->GetSession()->send(frame);
        return true;
    }

    bool AFCWebSocktServer::AddNetEntity(const AFGUID& xClientID, AFHttpEntity* pEntity)
    {
        return mxNetEntities.insert(std::make_pair(xClientID, pEntity)).second;
    }

    bool AFCWebSocktServer::RemoveNetEntity(const AFGUID& xClientID)
    {
        AFHttpEntity* pNetObject = GetNetEntity(xClientID);

        if (pNetObject)
        {
            delete pNetObject;
        }

        return mxNetEntities.erase(xClientID);
    }

    bool AFCWebSocktServer::CloseNetEntity(const AFGUID& xClientID)
    {
        AFHttpEntity* pEntity = GetNetEntity(xClientID);

        if (pEntity)
        {
            pEntity->GetSession()->postShutdown();
        }

        return true;
    }

    bool AFCWebSocktServer::Log(int severity, const char* msg)
    {
        return true;
    }

    bool AFCWebSocktServer::DismantleNet(AFHttpEntity* pEntity)
    {
        while (pEntity->GetBuffLen() >= ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
        {
            ARK_PKG_CS_HEAD xHead;
            int nMsgBodyLength = DeCode(pEntity->GetBuff(), pEntity->GetBuffLen(), xHead);

            if (nMsgBodyLength >= 0 && xHead.GetMsgID() > 0)
            {
                AFHttpMsg* pMsg = new AFHttpMsg(pEntity->GetSession());
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

    bool AFCWebSocktServer::CloseSocketAll()
    {
        for (auto it : mxNetEntities)
        {
            it.second->GetSession()->postShutdown();
            delete it.second;
            it.second = nullptr;
        }

        mxNetEntities.clear();

        return true;
    }

    AFHttpEntity* AFCWebSocktServer::GetNetEntity(const AFGUID& xClientID)
    {
        auto it = mxNetEntities.find(xClientID);

        if (it != mxNetEntities.end())
        {
            return it->second;
        }

        return nullptr;
    }

    bool AFCWebSocktServer::SendRawMsg(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID)
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

        return false;
    }

    bool AFCWebSocktServer::SendRawMsgToAllClient(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xPlayerID)
    {
        std::string strOutData;
        ARK_PKG_CS_HEAD xHead;
        xHead.SetMsgID(nMsgID);
        xHead.SetPlayerID(xPlayerID);

        int nAllLen = EnCode(xHead, msg, nLen, strOutData);

        if (nAllLen == nLen + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
        {
            return SendMsgToAllClient(strOutData.c_str(), strOutData.length());
        }

        return false;
    }

    int AFCWebSocktServer::EnCode(const ARK_PKG_CS_HEAD& xHead, const char* strData, const size_t len, std::string& strOutData)
    {
        char szHead[ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH] = { 0 };
        xHead.EnCode(szHead);

        strOutData.clear();
        strOutData.append(szHead, ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH);
        strOutData.append(strData, len);

        return xHead.GetBodyLength() + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH;
    }

    int AFCWebSocktServer::DeCode(const char* strData, const size_t len, ARK_PKG_CS_HEAD& xHead)
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