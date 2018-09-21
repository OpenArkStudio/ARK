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

#include "AFCTCPServer.h"

#if ARK_PLATFORM == PLATFORM_WIN
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma  comment(lib,"Ws2_32.lib")
#elif ARK_PLATFORM == PLATFORM_APPLE
#include <arpa/inet.h>
#endif

AFCTCPServer::AFCTCPServer()
{
    brynet::net::base::InitSocket();

    m_pTcpService = brynet::net::TcpService::Create();
    m_pListenThread = brynet::net::ListenThread::Create();
}

AFCTCPServer::~AFCTCPServer()
{
    Shutdown();
    brynet::net::base::DestroySocket();
}

void AFCTCPServer::Update()
{
    ProcessMsgLogicThread();
}

bool AFCTCPServer::Start(const int busid, const std::string& ip, const int port, const int thread_num, const unsigned int max_client, bool ip_v6/* = false*/)
{

    m_pTcpService->startWorkerThread(thread_num);
    mnServerID = busid;

    m_pListenThread->startListen(ip_v6, ip, port, [ = ](brynet::net::TcpSocket::PTR socket)
    {
        AFCTCPServer* pTcpServer = this;
        socket->SocketNodelay();
        auto OnEnterCallback = [pTcpServer](const brynet::net::DataSocket::PTR & session)
        {
            AFTCPMsg* pMsg = new AFTCPMsg(session);
            pMsg->xClientID.nLow = pTcpServer->mnNextID++;
            pMsg->nType = CONNECTED;

            do
            {
                AFScopeWrLock xGuard(pTcpServer->mRWLock);

                AFTCPEntityPtr pEntity = ARK_NEW AFTCPEntity(pTcpServer, pMsg->xClientID, session);
                session->setUD(int64_t(pEntity));

                if (pTcpServer->AddNetEntity(pMsg->xClientID, pEntity))
                {
                    pEntity->mxNetMsgMQ.Push(pMsg);
                }
            } while (false);

            session->setDataCallback([pTcpServer, session](const char* buffer, size_t len)
            {
                auto pUD = brynet::net::cast<int64_t>(session->getUD());

                if (nullptr != pUD)
                {
                    const AFTCPEntityPtr pEntity = AFTCPEntityPtr(*pUD);
                    pEntity->AddBuff(buffer, len);
                    pTcpServer->DismantleNet(pEntity);
                }

                return len;
            });

            session->setDisConnectCallback([](const brynet::net::DataSocket::PTR & session)
            {
                auto pUD = brynet::net::cast<int64_t>(session->getUD());

                if (nullptr == pUD)
                {
                    return;
                }

                const AFTCPEntityPtr pEntity = AFTCPEntityPtr(*pUD);
                AFTCPMsg* pMsg = new AFTCPMsg(session);
                pMsg->xClientID = pEntity->GetClientID();
                pMsg->nType = DISCONNECTED;

                pEntity->mxNetMsgMQ.Push(pMsg);
            });

            session->setHeartBeat(ARK_NET_HEART_TIME);
        };

        m_pTcpService->addDataSocket(std::move(socket),
                                     brynet::net::TcpService::AddSocketOption::WithEnterCallback(OnEnterCallback),
                                     brynet::net::TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_TCP_RECV_BUFFER_SIZE));
    });

    SetWorking(true);
    return true;
}

void AFCTCPServer::ProcessMsgLogicThread()
{
    std::list<AFGUID> xNeedRemoveList;

    do
    {
        AFScopeRdLock xGuard(mRWLock);

        for (std::map<AFGUID, AFTCPEntityPtr>::iterator iter = mmObject.begin(); iter != mmObject.end(); ++iter)
        {
            ProcessMsgLogicThread(iter->second);

            if (!iter->second->NeedRemove())
            {
                continue;
            }

            xNeedRemoveList.push_back(iter->second->GetClientID());
        }
    } while (false);

    for (auto iter : xNeedRemoveList)
    {
        AFScopeWrLock xGuard(mRWLock);
        RemoveNetEntity(iter);
    }
}

void AFCTCPServer::ProcessMsgLogicThread(AFTCPEntityPtr pEntity)
{
    //Handle Msg
    size_t nReceiveCount = pEntity->mxNetMsgMQ.Count();

    for (size_t i = 0; i < nReceiveCount && i < 100; ++i)
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

        ARK_DELETE(pMsg);
    }
}

bool AFCTCPServer::Shutdown()
{
    SetWorking(false);
    return true;
}

bool AFCTCPServer::IsServer()
{
    return true;
}

bool AFCTCPServer::SendMsgToAllClient(const char* msg, const size_t nLen)
{
    for (auto it : mmObject)
    {
        AFTCPEntityPtr pNetObject = (AFTCPEntityPtr)it.second;

        if (pNetObject != nullptr && !pNetObject->NeedRemove())
        {
            pNetObject->GetSession()->send(msg, nLen);
        }
    }

    return true;
}

bool AFCTCPServer::SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient)
{
    AFScopeRdLock xGuard(mRWLock);

    AFTCPEntityPtr pNetObject = GetNetEntity(xClient);

    if (pNetObject == nullptr)
    {
        return false;
    }
    else
    {
        pNetObject->GetSession()->send(msg, nLen);
        return true;
    }
}

bool AFCTCPServer::AddNetEntity(const AFGUID& xClientID, AFTCPEntityPtr pEntity)
{
    return mmObject.insert(std::make_pair(xClientID, pEntity)).second;
}

bool AFCTCPServer::RemoveNetEntity(const AFGUID& xClientID)
{
    AFTCPEntityPtr pEntity = GetNetEntity(xClientID);

    if (pEntity != nullptr)
    {
        ARK_DELETE(pEntity);
    }

    return mmObject.erase(xClientID);
}

bool AFCTCPServer::CloseNetEntity(const AFGUID& xClientID)
{
    AFTCPEntityPtr pEntity = GetNetEntity(xClientID);

    if (pEntity != nullptr)
    {
        pEntity->GetSession()->postDisConnect();
        return true;
    }
    else
    {
        return true;
    }
}

bool AFCTCPServer::Log(int severity, const char* msg)
{
    //will add msg log
    return true;
}

bool AFCTCPServer::DismantleNet(AFTCPEntityPtr pEntity)
{
    while (pEntity->GetBuffLen() >= ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH)
    {
        ARK_PKG_CS_HEAD xHead;
        int nMsgBodyLength = DeCode(pEntity->GetBuff(), pEntity->GetBuffLen(), xHead);

        if (nMsgBodyLength >= 0 && xHead.GetMsgID() > 0)
        {
            AFTCPMsg* pNetInfo = new AFTCPMsg(pEntity->GetSession());
            pNetInfo->xHead = xHead;
            pNetInfo->nType = RECVDATA;
            pNetInfo->strMsg.append(pEntity->GetBuff() + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH, nMsgBodyLength);
            pEntity->mxNetMsgMQ.Push(pNetInfo);
            pEntity->RemoveBuff(nMsgBodyLength + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH);
        }
        else
        {
            break;
        }
    }

    return true;
}

bool AFCTCPServer::CloseSocketAll()
{
    for (auto it : mmObject)
    {
        it.second->GetSession()->postDisConnect();
        ARK_DELETE(it.second);
    }

    mmObject.clear();
    return true;
}

AFCTCPServer::AFTCPEntityPtr AFCTCPServer::GetNetEntity(const AFGUID& xClientID)
{
    auto it = mmObject.find(xClientID);
    return (it != mmObject.end() ? it->second : nullptr);
}

bool AFCTCPServer::SendRawMsg(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID)
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

bool AFCTCPServer::SendRawMsgToAllClient(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xPlayerID)
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
    else
    {
        return false;
    }
}

int AFCTCPServer::EnCode(const ARK_PKG_CS_HEAD& xHead, const char* strData, const size_t len, std::string& strOutData)
{
    char szHead[ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH] = { 0 };
    xHead.EnCode(szHead);

    strOutData.clear();
    strOutData.append(szHead, ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH);
    strOutData.append(strData, len);

    return xHead.GetBodyLength() + ARK_PKG_BASE_HEAD::ARK_CS_HEADER_LENGTH;
}

int AFCTCPServer::DeCode(const char* strData, const size_t len, ARK_PKG_CS_HEAD& xHead)
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