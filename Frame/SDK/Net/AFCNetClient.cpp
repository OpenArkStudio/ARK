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

#include <brynet/net/SyncConnector.h>
#include "AFCNetClient.h"

void AFCNetClient::Update()
{
    ProcessMsgLogicThread();
}

void AFCNetClient::ProcessMsgLogicThread()
{
    do
    {
        AFScopeRdLock xGuard(mRWLock);
        ProcessMsgLogicThread(m_pClientEntity.get());
    } while (0);

    if (m_pClientEntity != nullptr && m_pClientEntity->NeedRemove())
    {
        AFScopeWrLock xGuard(mRWLock);
        m_pClientEntity.reset(nullptr);
    }

}

void AFCNetClient::ProcessMsgLogicThread(AFTCPEntity* pEntity)
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


void AFCNetClient::Start(const std::string& strAddrPort, const int nServerID)
{
    mnServerID = nServerID;
    m_pServer->startWorkThread(1);
    m_pConector->startWorkerThread();

    std::string strIp;
    int nPort(0);
    const int nTimeOut(20);

    SplitHostPort(strAddrPort, strIp, nPort);
    brynet::net::TcpSocket::PTR SocketPtr = brynet::net::SyncConnectSocket(strIp, nPort, std::chrono::milliseconds(nTimeOut), m_pConector);

    if (SocketPtr == nullptr)
    {
        return;
    }

    CONSOLE_LOG_NO_FILE << "connect success" << std::endl;
    SocketPtr->SocketNodelay();
    auto enterCallback = std::bind(&AFCNetClient::OnClientConnectionInner, this, std::placeholders::_1);

    m_pServer->addSession(std::move(SocketPtr),
                          brynet::net::AddSessionOption::WithEnterCallback(enterCallback),
                          brynet::net::AddSessionOption::WithMaxRecvBufferSize(1024 * 1024));
    SetWorking(true);
}

bool AFCNetClient::Final()
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

bool AFCNetClient::CloseSocketAll()
{
    if (nullptr != m_pClientEntity)
    {
        m_pClientEntity->GetSession()->postDisConnect();
    }

    return true;
}

bool AFCNetClient::SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient)
{
    if (nullptr != m_pClientEntity && m_pClientEntity->GetSession())
    {
        m_pClientEntity->GetSession()->send(msg, nLen);
    }

    return true;
}

bool AFCNetClient::CloseNetEntity(const AFGUID& xClient)
{
    if (nullptr != m_pClientEntity && m_pClientEntity->GetClientID() == xClient)
    {
        m_pClientEntity->GetSession()->postDisConnect();
    }

    return true;
}

bool AFCNetClient::DismantleNet(AFTCPEntity* pEntity)
{
    while (pEntity->GetBuffLen() >= AFIMsgHead::ARK_MSG_HEAD_LENGTH)
    {
        AFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pEntity->GetBuff(), pEntity->GetBuffLen(), xHead);

        if (nMsgBodyLength >= 0 && xHead.GetMsgID() > 0)
        {
            AFTCPMsg* pMsg = ARK_NEW AFTCPMsg(pEntity->GetSession());
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

void AFCNetClient::log_cb(int severity, const char* msg)
{
    //Will add log
}

bool AFCNetClient::IsServer()
{
    return false;
}

bool AFCNetClient::Log(int severity, const char* msg)
{
    log_cb(severity, msg);
    return true;
}

void AFCNetClient::OnClientConnectionInner(const brynet::net::TCPSession::PTR& session)
{
    session->setDataCallback(std::bind(&AFCNetClient::OnMessageInner, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    session->setDisConnectCallback(std::bind(&AFCNetClient::OnClientDisConnectionInner, this, std::placeholders::_1));

    AFTCPMsg* pMsg = new AFTCPMsg(session);
    pMsg->xClientID.nLow = (++mnNextID);
    session->setUD(static_cast<int64_t>(pMsg->xClientID.nLow));
    pMsg->nType = CONNECTED;

    do
    {
        AFScopeWrLock xGuard(mRWLock);

        AFTCPEntity* pEntity = new AFTCPEntity(this, pMsg->xClientID, session);
        m_pClientEntity.reset(pEntity);
        pEntity->mxNetMsgMQ.Push(pMsg);
    } while (0);
}

void AFCNetClient::OnClientDisConnectionInner(const brynet::net::TCPSession::PTR& session)
{
    const auto ud = brynet::net::cast<brynet::net::TcpService::SESSION_TYPE>(session->getUD());
    AFGUID xClient(0, *ud);

    AFTCPMsg* pMsg = new AFTCPMsg(session);
    pMsg->xClientID = xClient;
    pMsg->nType = DISCONNECTED;

    do
    {
        AFScopeWrLock xGuard(mRWLock);
        m_pClientEntity->mxNetMsgMQ.Push(pMsg);
    } while (0);
}

size_t AFCNetClient::OnMessageInner(const brynet::net::TCPSession::PTR& session, const char* buffer, size_t len)
{
    const auto ud = brynet::net::cast<brynet::net::TcpService::SESSION_TYPE>(session->getUD());
    AFGUID xClient(0, *ud);

    AFScopeRdLock xGuard(mRWLock);

    if (m_pClientEntity->GetClientID() == xClient)
    {
        m_pClientEntity->AddBuff(buffer, len);
        DismantleNet(m_pClientEntity.get());
    }

    return len;
}

bool AFCNetClient::SendMsgWithOutHead(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID)
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
    else
    {
        return false;
    }
}

int AFCNetClient::EnCode(const AFCMsgHead& xHead, const char* strData, const size_t len, std::string& strOutData)
{
    char szHead[AFIMsgHead::ARK_MSG_HEAD_LENGTH] = { 0 };
    xHead.EnCode(szHead);

    strOutData.clear();
    strOutData.append(szHead, AFIMsgHead::ARK_MSG_HEAD_LENGTH);
    strOutData.append(strData, len);

    return xHead.GetBodyLength() + AFIMsgHead::ARK_MSG_HEAD_LENGTH;
}

int AFCNetClient::DeCode(const char* strData, const size_t len, AFCMsgHead& xHead)
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