/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) AFHttpEntity ArkGame authors.
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

void AFCWebSocktServer::Update()
{
    ProcessMsgLogicThread();
}

int AFCWebSocktServer::Start(const int nServerID, const std::string& strAddrPort, const int nThreadCount, const unsigned int nMaxClient)
{
    std::string strHost;
    int port;
    SplitHostPort(strAddrPort, strHost, port);
    m_plistenThread->startListen(false, strHost, port, std::bind(&AFCWebSocktServer::OnAcceptConnectionInner, this, std::placeholders::_1));

    m_pServer->startWorkThread(nThreadCount);
    return 0;
}

void AFCWebSocktServer::OnAcceptConnectionInner(brynet::net::TcpSocket::PTR socket)
{
    m_pServer->addSession(std::move(socket),
                          brynet::net::AddSessionOption::WithEnterCallback(
                              [this](const brynet::net::TCPSession::PTR & session)
    {
        brynet::net::HttpService::setup(session, std::bind(&AFCWebSocktServer::OnHttpConnect, this, std::placeholders::_1));
    }), brynet::net::AddSessionOption::WithMaxRecvBufferSize(1024 * 1024));
}

void AFCWebSocktServer::OnHttpMessageCallBack(const brynet::net::HTTPParser& httpParser, const brynet::net::HttpSession::PTR& session)
{
    brynet::net::HttpResponse response;
    response.setBody("test");
    std::string result = response.getResult();
    session->send(result.c_str(), result.size(), [session]()
    {
        session->postShutdown();
    });
}

void AFCWebSocktServer::OnWebSockMessageCallBack(const brynet::net::HttpSession::PTR& httpSession,
        brynet::net::WebSocketFormat::WebSocketFrameType opcode,
        const std::string& payload)
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

    const auto ud = brynet::net::cast<brynet::net::TcpService::SESSION_TYPE>(httpSession->getUD());
    AFGUID xClient(0, *ud);

    AFScopeRdLock xGuard(mRWLock);

    auto xFind = mxNetEntities.find(xClient);

    if (xFind == mxNetEntities.end())
    {
        return;
    }

    xFind->second->AddBuff(payload.c_str(), payload.size());
    DismantleNet(xFind->second);
}

void AFCWebSocktServer::OnHttpConnect(const brynet::net::HttpSession::PTR& httpSession)
{
    httpSession->setHttpCallback(std::bind(&AFCWebSocktServer::OnHttpMessageCallBack, this, std::placeholders::_1, std::placeholders::_2));
    httpSession->setWSCallback(std::bind(&AFCWebSocktServer::OnWebSockMessageCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    httpSession->setCloseCallback(std::bind(&AFCWebSocktServer::OnHttpDisConnection, this, std::placeholders::_1));

    AFHttpMsg* pMsg = new AFHttpMsg(httpSession);
    pMsg->xClientID.nLow = nNextID++;
    httpSession->setUD(static_cast<int64_t>(pMsg->xClientID.nLow));
    pMsg->nType = CONNECTED;

    do
    {
        AFScopeWrLock xGuard(mRWLock);

        AFHttpEntity* pEntity = new AFHttpEntity(this, pMsg->xClientID, httpSession);

        if (AddNetEntity(pMsg->xClientID, pEntity))
        {
            pEntity->mxNetMsgMQ.Push(pMsg);
        }
    } while (0);
}

void AFCWebSocktServer::OnHttpDisConnection(const brynet::net::HttpSession::PTR& httpSession)
{
    const auto ud = brynet::net::cast<int64_t>(httpSession->getUD());
    AFGUID xClient(0, 0);
    xClient.nLow = *ud;
    AFScopeWrLock xGuard(mRWLock);

    auto xFind = mxNetEntities.find(xClient);

    if (xFind == mxNetEntities.end())
    {
        return;
    }

    AFHttpMsg* pMsg = new AFHttpMsg(httpSession);
    pMsg->xClientID = xClient;
    pMsg->nType = DISCONNECTED;

    xFind->second->mxNetMsgMQ.Push(pMsg);
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
        case RECIVEDATA:
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

bool AFCWebSocktServer::Final()
{
    SetWorking(false);
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

bool AFCWebSocktServer::DismantleNet(AFHttpEntity* pEntity)
{
    while (pEntity->GetBuffLen() >= AFIMsgHead::ARK_MSG_HEAD_LENGTH)
    {
        AFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pEntity->GetBuff(), pEntity->GetBuffLen(), xHead);

        if (nMsgBodyLength >= 0 && xHead.GetMsgID() > 0)
        {
            AFHttpMsg* pMsg = new AFHttpMsg(pEntity->GetSession());
            pMsg->xHead = xHead;
            pMsg->nType = RECIVEDATA;
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

bool AFCWebSocktServer::SendMsgWithOutHead(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID)
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

bool AFCWebSocktServer::SendMsgToAllClientWithOutHead(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xPlayerID)
{
    std::string strOutData;
    AFCMsgHead xHead;
    xHead.SetMsgID(nMsgID);
    xHead.SetPlayerID(xPlayerID);

    int nAllLen = EnCode(xHead, msg, nLen, strOutData);

    if (nAllLen == nLen + AFIMsgHead::ARK_MSG_HEAD_LENGTH)
    {
        return SendMsgToAllClient(strOutData.c_str(), strOutData.length());
    }

    return false;
}

int AFCWebSocktServer::EnCode(const AFCMsgHead& xHead, const char* strData, const size_t len, std::string& strOutData)
{
    char szHead[AFIMsgHead::ARK_MSG_HEAD_LENGTH] = { 0 };
    xHead.EnCode(szHead);

    strOutData.clear();
    strOutData.append(szHead, AFIMsgHead::ARK_MSG_HEAD_LENGTH);
    strOutData.append(strData, len);

    return xHead.GetBodyLength() + AFIMsgHead::ARK_MSG_HEAD_LENGTH;
}

int AFCWebSocktServer::DeCode(const char* strData, const size_t len, AFCMsgHead& xHead)
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