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

#include <string.h>
#include <memory>

#include <brynet/net/SyncConnector.h>
#include "AFCBryWebSocktClient.h"
#include "brynet/net/http/HttpFormat.h"


void AFCBryWebSocktClient::Update()
{
    ProcessMsgLogicThread();
}

void AFCBryWebSocktClient::ProcessMsgLogicThread()
{
    {
        AFScopeRdLock xGuard(mRWLock);
        ProcessMsgLogicThread(m_pClientObject.get());
    }

    if(m_pClientObject != nullptr && m_pClientObject->NeedRemove())
    {
        AFScopeWrLock xGuard(mRWLock);
        m_pClientObject.release();
    }

}

void AFCBryWebSocktClient::ProcessMsgLogicThread(BryHttpNetObject* pEntity)
{
    if(pEntity == nullptr)
    {
        return;
    }

    //Handle Msg;
    size_t nReceiveCount = pEntity->mqMsgFromNet.Count();
    for(size_t i = 0; i < nReceiveCount; ++i)
    {
        MsgFromBryHttpNetInfo* pMsgFromNet(nullptr);
        if(!pEntity->mqMsgFromNet.Pop(pMsgFromNet))
        {
            break;
        }

        if(pMsgFromNet == nullptr)
        {
            continue;
        }

        switch(pMsgFromNet->nType)
        {
        case RECIVEDATA:
            {
                int nRet = 0;
                if(mRecvCB)
                {
                    mRecvCB(pMsgFromNet->xHead, pMsgFromNet->xHead.GetMsgID(), pMsgFromNet->strMsg.c_str(), pMsgFromNet->strMsg.size(), pEntity->GetClientID());
                }
            }
            break;
        case CONNECTED:
            {
                mEventCB((NetEventType)pMsgFromNet->nType, pMsgFromNet->xClientID, mnServerID);
            }
            break;
        case DISCONNECTED:
            {
                mEventCB((NetEventType)pMsgFromNet->nType, pMsgFromNet->xClientID, mnServerID);
                pEntity->SetNeedRemove(true);
            }
            break;
        default:
            break;
        }

        delete pMsgFromNet;
    }
}

void AFCBryWebSocktClient::Start(const std::string& strAddrPort, const int nServerID)
{
    mnServerID = nServerID;
    m_pServer->startWorkThread(1);

    std::string strIp = "127.0.0.1";
    int nPort = 8001;
    const int nTimeOut = 20;

    SplitHostPort(strAddrPort, strIp, nPort);
    sock fd = brynet::net::base::Connect(false, strIp, nPort);
    auto SocketPtr = brynet::net::TcpSocket::Create(fd, false);
    SocketPtr->SocketNodelay();

    std::cout << "connect success" << std::endl;
    auto enterCallback = [this](const brynet::net::TCPSession::PTR & session)
    {
        brynet::net::HttpService::setup(session, std::bind(&AFCBryWebSocktClient::OnHttpConnect, this, std::placeholders::_1));
    };

    m_pServer->addSession(std::move(SocketPtr),
                          brynet::net::AddSessionOption::WithEnterCallback(enterCallback),
                          brynet::net::AddSessionOption::WithMaxRecvBufferSize(1024 * 1024));
}

void AFCBryWebSocktClient::OnHttpConnect(const brynet::net::HttpSession::PTR& httpSession)
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
        MsgFromBryHttpNetInfo* pMsg = new MsgFromBryHttpNetInfo(httpSession);
        httpSession->setUD(static_cast<int64_t>(pMsg->xClientID.nLow));
        pMsg->nType = CONNECTED;
        {
            AFScopeWrLock xGuard(mRWLock);

            BryHttpNetObject* pEntity = new BryHttpNetObject(this, pMsg->xClientID, httpSession);
            m_pClientObject.reset(pEntity);
            pEntity->mqMsgFromNet.Push(pMsg);
        }
    });

    httpSession->setWSCallback(std::bind(&AFCBryWebSocktClient::OnWebSockMessageCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    httpSession->setCloseCallback(std::bind(&AFCBryWebSocktClient::OnHttpDisConnection, this, std::placeholders::_1));
}

void AFCBryWebSocktClient::OnWebSockMessageCallBack(const brynet::net::HttpSession::PTR & httpSession,
        brynet::net::WebSocketFormat::WebSocketFrameType opcode,
        const std::string & payload)
{
    const auto ud = brynet::net::cast<brynet::net::TcpService::SESSION_TYPE>(httpSession->getUD());
    AFGUID xClient(0, *ud);

    AFScopeRdLock xGuard(mRWLock);

    m_pClientObject->AddBuff(payload.c_str(), payload.size());
    DismantleNet(m_pClientObject.get());
}
bool AFCBryWebSocktClient::Final()
{
    if(!CloseSocketAll())
    {
        //add log
    }

    m_pConector->stopWorkerThread();
    m_pServer->stopWorkThread();
    bWorking = false;
    return true;
}

void AFCBryWebSocktClient::OnHttpDisConnection(const brynet::net::HttpSession::PTR & httpSession)
{
    const auto ud = brynet::net::cast<int64_t>(httpSession->getUD());
    AFGUID xClient(0, 0);
    xClient.nLow = *ud;
    AFScopeWrLock xGuard(mRWLock);

    MsgFromBryHttpNetInfo* pMsg = new MsgFromBryHttpNetInfo(httpSession);
    pMsg->xClientID = xClient;
    pMsg->nType = DISCONNECTED;

    m_pClientObject->mqMsgFromNet.Push(pMsg);
}
bool AFCBryWebSocktClient::CloseSocketAll()
{
    m_pClientObject->GetConnPtr()->postClose();
    return true;
}

bool AFCBryWebSocktClient::SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient)
{
    auto frame = std::make_shared<std::string>();
    brynet::net::WebSocketFormat::wsFrameBuild(msg,
            nLen,
            *frame,
            brynet::net::WebSocketFormat::WebSocketFrameType::BINARY_FRAME,
            true,
            false);

    if(m_pClientObject->GetConnPtr())
    {
        m_pClientObject->GetConnPtr()->send(frame);
    }

    return true;
}

bool AFCBryWebSocktClient::CloseNetObject(const AFGUID& xClient)
{
    if(m_pClientObject->GetClientID() == xClient)
    {
        m_pClientObject->GetConnPtr()->postClose();
    }
    return true;
}

bool AFCBryWebSocktClient::DismantleNet(BryHttpNetObject* pEntity)
{
    for(; pEntity->GetBuffLen() >= AFIMsgHead::ARK_MSG_HEAD_LENGTH;)
    {
        AFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pEntity->GetBuff(), pEntity->GetBuffLen(), xHead);
        if(nMsgBodyLength >= 0 && xHead.GetMsgID() > 0)
        {
            MsgFromBryHttpNetInfo* pNetInfo = new MsgFromBryHttpNetInfo(pEntity->GetConnPtr());
            pNetInfo->xHead = xHead;
            pNetInfo->nType = RECIVEDATA;
            pNetInfo->strMsg.append(pEntity->GetBuff() + AFIMsgHead::ARK_MSG_HEAD_LENGTH, nMsgBodyLength);
            pEntity->mqMsgFromNet.Push(pNetInfo);
            size_t nNewSize = pEntity->RemoveBuff(nMsgBodyLength + AFIMsgHead::ARK_MSG_HEAD_LENGTH);
        }
        else
        {
            break;
        }
    }

    return true;
}

void AFCBryWebSocktClient::log_cb(int severity, const char* msg)
{

}

bool AFCBryWebSocktClient::IsServer()
{
    return false;
}

bool AFCBryWebSocktClient::Log(int severity, const char* msg)
{
    log_cb(severity, msg);
    return true;
}


bool AFCBryWebSocktClient::SendMsgWithOutHead(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID & xClientID, const AFGUID& xPlayerID)
{
    std::string strOutData;
    AFCMsgHead xHead;
    xHead.SetMsgID(nMsgID);
    xHead.SetPlayerID(xPlayerID);
    xHead.SetBodyLength(nLen);

    int nAllLen = EnCode(xHead, msg, nLen, strOutData);
    if(nAllLen == nLen + AFIMsgHead::ARK_MSG_HEAD_LENGTH)
    {
        return SendMsg(strOutData.c_str(), strOutData.length(), xClientID);
    }

    return false;
}

int AFCBryWebSocktClient::EnCode(const AFCMsgHead& xHead, const char* strData, const size_t len, std::string& strOutData)
{
    char szHead[AFIMsgHead::ARK_MSG_HEAD_LENGTH] = { 0 };
    int nSize = xHead.EnCode(szHead);

    strOutData.clear();
    strOutData.append(szHead, AFIMsgHead::ARK_MSG_HEAD_LENGTH);
    strOutData.append(strData, len);

    return xHead.GetBodyLength() + AFIMsgHead::ARK_MSG_HEAD_LENGTH;
}

int AFCBryWebSocktClient::DeCode(const char* strData, const size_t len, AFCMsgHead & xHead)
{
    if(len < AFIMsgHead::ARK_MSG_HEAD_LENGTH)
    {
        return -1;
    }

    if(AFIMsgHead::ARK_MSG_HEAD_LENGTH != xHead.DeCode(strData))
    {
        return -2;
    }

    if(xHead.GetBodyLength() > (len - AFIMsgHead::ARK_MSG_HEAD_LENGTH))
    {
        return -3;
    }
    return xHead.GetBodyLength();
}