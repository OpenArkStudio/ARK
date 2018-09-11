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

#include <brynet/net/SyncConnector.h>
#include <brynet/net/http/HttpFormat.h>
#include "AFCWebSocktClient.h"

void AFCWebSocktClient::Update()
{
    ProcessMsgLogicThread();
}

void AFCWebSocktClient::ProcessMsgLogicThread()
{
    {
        AFScopeRdLock xGuard(mRWLock);
        ProcessMsgLogicThread(m_pClientEntity.get());
    }

    if (m_pClientEntity != nullptr && m_pClientEntity->NeedRemove())
    {
        AFScopeWrLock xGuard(mRWLock);
        m_pClientEntity.release();
    }
}

void AFCWebSocktClient::ProcessMsgLogicThread(AFHttpEntity* pEntity)
{
    if (pEntity == nullptr)
    {
        return;
    }

    //Handle messages
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

bool AFCWebSocktClient::Start(const int target_busid, const std::string& ip, const int port, bool ip_v6)
{
    mnTargetBusID = target_busid;
    m_pServer->startWorkThread(1);

    //TODO:为什么这里没有超时时间的设置
    sock fd = brynet::net::base::Connect(ip_v6, ip, port);
    auto SocketPtr = brynet::net::TcpSocket::Create(fd, false);
    SocketPtr->SocketNodelay();

    CONSOLE_LOG_NO_FILE << "connect success" << std::endl;
    auto enterCallback = [this](const brynet::net::TCPSession::PTR & session)
    {
        brynet::net::HttpService::setup(session, std::bind(&AFCWebSocktClient::OnHttpConnect, this, std::placeholders::_1));
    };

    return m_pServer->addSession(std::move(SocketPtr),
                                 brynet::net::AddSessionOption::WithEnterCallback(enterCallback),
                                 brynet::net::AddSessionOption::WithMaxRecvBufferSize(1024 * 1024));
}

void AFCWebSocktClient::OnHttpConnect(const brynet::net::HttpSession::PTR& httpSession)
{
    brynet::net::HttpRequest request;
    request.setMethod(brynet::net::HttpRequest::HTTP_METHOD::HTTP_METHOD_GET);
    request.setUrl("/ws");
    std::string strIp;
    int nPort;
    SplitHostPort(mstrIPPort, strIp, nPort);
    request.addHeadValue("Host", strIp);
    request.addHeadValue("Upgrade", "websocket");
    request.addHeadValue("Connection", "Upgrade");
    request.addHeadValue("Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
    request.addHeadValue("Sec-WebSocket-Version", "13");

    std::string requestStr = request.getResult();
    httpSession->send(requestStr.c_str(), requestStr.size());

    httpSession->setWSConnected([this](const brynet::net::HttpSession::PTR & httpSession, const brynet::net::HTTPParser&)
    {
        AFHttpMsg* pMsg = new AFHttpMsg(httpSession);
        httpSession->setUD(static_cast<int64_t>(pMsg->xClientID.nLow));
        pMsg->nType = CONNECTED;

        do
        {
            AFScopeWrLock xGuard(mRWLock);

            AFHttpEntity* pEntity = new AFHttpEntity(this, pMsg->xClientID, httpSession);
            m_pClientEntity.reset(pEntity);
            pEntity->mxNetMsgMQ.Push(pMsg);
        } while (0);
    });

    httpSession->setWSCallback(std::bind(&AFCWebSocktClient::OnWebSockMessageCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    httpSession->setCloseCallback(std::bind(&AFCWebSocktClient::OnHttpDisConnection, this, std::placeholders::_1));
}

void AFCWebSocktClient::OnWebSockMessageCallBack(const brynet::net::HttpSession::PTR& httpSession,
        brynet::net::WebSocketFormat::WebSocketFrameType opcode,
        const std::string& payload)
{
    const auto ud = brynet::net::cast<brynet::net::TcpService::SESSION_TYPE>(httpSession->getUD());
    AFGUID xClient(0, *ud);

    AFScopeRdLock xGuard(mRWLock);

    m_pClientEntity->AddBuff(payload.c_str(), payload.size());
    DismantleNet(m_pClientEntity.get());
}

bool AFCWebSocktClient::Shutdown()
{
    if (!CloseSocketAll())
    {
        //add log
    }

    m_pConector->stopWorkerThread();
    m_pServer->stopWorkThread();
    SetWorking(false);
    return true;
}

void AFCWebSocktClient::OnHttpDisConnection(const brynet::net::HttpSession::PTR& httpSession)
{
    const auto ud = brynet::net::cast<int64_t>(httpSession->getUD());
    AFGUID xClient(0, 0);
    xClient.nLow = *ud;
    AFScopeWrLock xGuard(mRWLock);

    AFHttpMsg* pMsg = new AFHttpMsg(httpSession);
    pMsg->xClientID = xClient;
    pMsg->nType = DISCONNECTED;

    m_pClientEntity->mxNetMsgMQ.Push(pMsg);
}
bool AFCWebSocktClient::CloseSocketAll()
{
    m_pClientEntity->GetSession()->postClose();
    return true;
}

bool AFCWebSocktClient::SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient)
{
    auto frame = std::make_shared<std::string>();
    brynet::net::WebSocketFormat::wsFrameBuild(msg,
            nLen,
            *frame,
            brynet::net::WebSocketFormat::WebSocketFrameType::BINARY_FRAME,
            true,
            false);

    if (m_pClientEntity->GetSession())
    {
        m_pClientEntity->GetSession()->send(frame);
    }

    return true;
}

bool AFCWebSocktClient::CloseNetEntity(const AFGUID& xClient)
{
    if (m_pClientEntity->GetClientID() == xClient)
    {
        m_pClientEntity->GetSession()->postClose();
    }

    return true;
}

bool AFCWebSocktClient::DismantleNet(AFHttpEntity* pEntity)
{
    while (pEntity->GetBuffLen() >= AFIMsgHead::ARK_MSG_HEAD_LENGTH)
    {
        AFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pEntity->GetBuff(), pEntity->GetBuffLen(), xHead);

        if (nMsgBodyLength >= 0 && xHead.GetMsgID() > 0)
        {
            AFHttpMsg* pMsg = new AFHttpMsg(pEntity->GetSession());
            pMsg->xHead = xHead;
            pMsg->nType = RECVDATA;
            pMsg->strMsg.append(pEntity->GetBuff() + AFIMsgHead::ARK_MSG_HEAD_LENGTH, nMsgBodyLength);
            pEntity->mxNetMsgMQ.Push(pMsg);
            pEntity->RemoveBuff(nMsgBodyLength + AFIMsgHead::ARK_MSG_HEAD_LENGTH);
        }
        else
        {
            break;
        }
    }

    return true;
}

void AFCWebSocktClient::log_cb(int severity, const char* msg)
{
    //Will add log
}

bool AFCWebSocktClient::IsServer()
{
    return false;
}

bool AFCWebSocktClient::Log(int severity, const char* msg)
{
    log_cb(severity, msg);
    return true;
}


bool AFCWebSocktClient::SendMsgWithOutHead(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID)
{
    std::string strOutData;
    AFCMsgHead xHead;
    xHead.SetMsgID(nMsgID);
    xHead.SetPlayerID(xPlayerID);
    xHead.SetBodyLength(nLen);

    int nAllLen = EnCode(xHead, msg, nLen, strOutData);

    if (nAllLen == nLen + AFIMsgHead::ARK_MSG_HEAD_LENGTH)
    {
        return SendMsg(strOutData.c_str(), strOutData.length(), xClientID);
    }

    return false;
}

int AFCWebSocktClient::EnCode(const AFCMsgHead& xHead, const char* strData, const size_t len, std::string& strOutData)
{
    char szHead[AFIMsgHead::ARK_MSG_HEAD_LENGTH] = { 0 };
    xHead.EnCode(szHead);

    strOutData.clear();
    strOutData.append(szHead, AFIMsgHead::ARK_MSG_HEAD_LENGTH);
    strOutData.append(strData, len);

    return xHead.GetBodyLength() + AFIMsgHead::ARK_MSG_HEAD_LENGTH;
}

int AFCWebSocktClient::DeCode(const char* strData, const size_t len, AFCMsgHead& xHead)
{
    if (len < AFIMsgHead::ARK_MSG_HEAD_LENGTH)
    {
        return -1;
    }

    if (AFIMsgHead::ARK_MSG_HEAD_LENGTH != xHead.DeCode(strData))
    {
        return -2;
    }

    if (xHead.GetBodyLength() > (len - AFIMsgHead::ARK_MSG_HEAD_LENGTH))
    {
        return -3;
    }

    return xHead.GetBodyLength();
}