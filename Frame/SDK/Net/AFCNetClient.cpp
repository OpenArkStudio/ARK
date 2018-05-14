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
#include "AFCNetClient.h"


void AFCNetClient::Update()
{
    ProcessMsgLogicThread();
}

void AFCNetClient::ProcessMsgLogicThread()
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

void AFCNetClient::ProcessMsgLogicThread(BryNetObject* pEntity)
{
    if(pEntity == nullptr)
    {
        return;
    }

    //Handle Msg;
    size_t nReceiveCount = pEntity->mqMsgFromNet.Count();
    for(size_t i = 0; i < nReceiveCount; ++i)
    {
        MsgFromBryNetInfo* pMsgFromNet(nullptr);
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


void AFCNetClient::Start(const std::string& strAddrPort, const int nServerID)
{
    mnServerID = nServerID;
    m_pServer->startWorkThread(1);
    m_pConector->startWorkerThread();

    std::string strIp = "127.0.0.1";
    int nPort = 8001;
    const int nTimeOut = 20;

    SplitHostPort(strAddrPort, strIp, nPort);
    brynet::net::TcpSocket::PTR SocketPtr = brynet::net::SyncConnectSocket(strIp, nPort, std::chrono::milliseconds(nTimeOut), m_pConector);
    if(SocketPtr == nullptr)
    {
        return;
    }

    std::cout << "connect success" << std::endl;
    SocketPtr->SocketNodelay();
    auto enterCallback = std::bind(&AFCNetClient::OnClientConnectionInner, this, std::placeholders::_1);

    m_pServer->addSession(std::move(SocketPtr),
                          brynet::net::AddSessionOption::WithEnterCallback(enterCallback),
                          brynet::net::AddSessionOption::WithMaxRecvBufferSize(1024 * 1024));
}

bool AFCNetClient::Final()
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

bool AFCNetClient::CloseSocketAll()
{
    m_pClientObject->GetConnPtr()->postDisConnect();
    return true;
}

bool AFCNetClient::SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient)
{
    if(m_pClientObject->GetConnPtr())
    {
        m_pClientObject->GetConnPtr()->send(msg, nLen);
    }

    return true;
}

bool AFCNetClient::CloseNetObject(const AFGUID& xClient)
{
    if(m_pClientObject->GetClientID() == xClient)
    {
        m_pClientObject->GetConnPtr()->postDisConnect();
    }
    return true;
}

bool AFCNetClient::DismantleNet(BryNetObject* pEntity)
{
    for(; pEntity->GetBuffLen() >= AFIMsgHead::ARK_MSG_HEAD_LENGTH;)
    {
        AFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pEntity->GetBuff(), pEntity->GetBuffLen(), xHead);
        if(nMsgBodyLength >= 0 && xHead.GetMsgID() > 0)
        {
            MsgFromBryNetInfo* pNetInfo = new MsgFromBryNetInfo(pEntity->GetConnPtr());
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

void AFCNetClient::log_cb(int severity, const char* msg)
{

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

    MsgFromBryNetInfo* pMsg = new MsgFromBryNetInfo(session);
    const auto ud = brynet::net::cast<brynet::net::TcpService::SESSION_TYPE>(session->getUD());
    pMsg->xClientID.nLow = nNextID++;
    session->setUD(static_cast<int64_t>(pMsg->xClientID.nLow));
    pMsg->nType = CONNECTED;
    {
        AFScopeWrLock xGuard(mRWLock);

        BryNetObject* pEntity = new BryNetObject(this, pMsg->xClientID, session);
        m_pClientObject.reset(pEntity);
        pEntity->mqMsgFromNet.Push(pMsg);
    }
}

void AFCNetClient::OnClientDisConnectionInner(const brynet::net::TCPSession::PTR& session)
{
    const auto ud = brynet::net::cast<brynet::net::TcpService::SESSION_TYPE>(session->getUD());
    AFGUID xClient(0, *ud);

    MsgFromBryNetInfo* pMsg = new MsgFromBryNetInfo(session);
    pMsg->xClientID = xClient;
    pMsg->nType = DISCONNECTED;

    {
        AFScopeWrLock xGuard(mRWLock);
        m_pClientObject->mqMsgFromNet.Push(pMsg);
    }
}

size_t AFCNetClient::OnMessageInner(const brynet::net::TCPSession::PTR& session, const char* buffer, size_t len)
{
    const auto ud = brynet::net::cast<brynet::net::TcpService::SESSION_TYPE>(session->getUD());
    AFGUID xClient(0, *ud);

    AFScopeRdLock xGuard(mRWLock);

    if(m_pClientObject->GetClientID() == xClient)
    {
        m_pClientObject->AddBuff(buffer, len);
        DismantleNet(m_pClientObject.get());
    }

    return len;
}

bool AFCNetClient::SendMsgWithOutHead(const uint16_t nMsgID, const char* msg, const size_t nLen, const AFGUID & xClientID, const AFGUID& xPlayerID)
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

int AFCNetClient::EnCode(const AFCMsgHead& xHead, const char* strData, const size_t len, std::string& strOutData)
{
    char szHead[AFIMsgHead::ARK_MSG_HEAD_LENGTH] = { 0 };
    int nSize = xHead.EnCode(szHead);

    strOutData.clear();
    strOutData.append(szHead, AFIMsgHead::ARK_MSG_HEAD_LENGTH);
    strOutData.append(strData, len);

    return xHead.GetBodyLength() + AFIMsgHead::ARK_MSG_HEAD_LENGTH;
}

int AFCNetClient::DeCode(const char* strData, const size_t len, AFCMsgHead & xHead)
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