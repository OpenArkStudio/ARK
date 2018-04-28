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
#include "AFCNetClient.h"

#if ARK_PLATFORM == PLATFORM_WIN
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma  comment(lib,"Ws2_32.lib")
#pragma  comment(lib,"event.lib")
#pragma  comment(lib,"event_core.lib")
#elif ARK_PLATFORM == PLATFORM_APPLE
#include <arpa/inet.h>
#endif

#include "event2/bufferevent_struct.h"
#include "event2/event.h"


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

void AFCNetClient::ProcessMsgLogicThread(NFCEvppNetObject* pEntity)
{
    if(pEntity == nullptr)
    {
        return;
    }

    //Handle Msg;
    size_t nReceiveCount = pEntity->mqMsgFromNet.Count();
    for(size_t i = 0; i < nReceiveCount; ++i)
    {
        MsgEvppFromNetInfo* pMsgFromNet(nullptr);
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
#if ARK_PLATFORM == PLATFORM_WIN
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif

    mstrIPPort = strAddrPort;
    mnServerID = nServerID;
#if defined(HAVE_LANG_CXX14) || defined(HAVE_LANG_CXX17)
    m_pThread = std::make_unique<evpp::EventLoopThread>();
#else
    m_pThread.reset(new evpp::EventLoopThread);
#endif
    m_pThread->set_name("TCPClientThread");
    m_pThread->Start();

#if defined(HAVE_LANG_CXX14) || defined(HAVE_LANG_CXX17)
    m_pClient = std::make_unique<evpp::TCPClient>(m_pThread->loop(), mstrIPPort, "TCPClient");
#else
    m_pClient.reset(new evpp::TCPClient(m_pThread->loop(), mstrIPPort, "TCPClient"));
#endif

    m_pClient->SetConnectionCallback(std::bind(&AFCNetClient::OnClientConnectionInner, this, std::placeholders::_1));
    m_pClient->SetMessageCallback(std::bind(&AFCNetClient::OnMessageInner, this, std::placeholders::_1, std::placeholders::_2));
    m_pClient->set_auto_reconnect(false);
    m_pClient->Connect();

    bWorking = true;
}

bool AFCNetClient::Final()
{
    if(!CloseSocketAll())
    {
        //add log
    }

    m_pThread->Stop(true);
    bWorking = false;
    return true;
}

bool AFCNetClient::CloseSocketAll()
{
    m_pClient->Disconnect();
    return true;
}

bool AFCNetClient::SendMsg(const char* msg, const size_t nLen, const AFGUID& xClient)
{
    if(m_pClient->conn() != nullptr)
    {
        m_pClient->conn()->Send(msg, nLen);
        return true;
    }

    return false;
}

bool AFCNetClient::CloseNetObject(const AFGUID& xClient)
{
    m_pClient->Disconnect();
    return true;
}

bool AFCNetClient::DismantleNet(NFCEvppNetObject* pEntity)
{
    for(; pEntity->GetBuffLen() >= AFIMsgHead::ARK_MSG_HEAD_LENGTH;)
    {
        AFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pEntity->GetBuff(), pEntity->GetBuffLen(), xHead);
        if(nMsgBodyLength >= 0 && xHead.GetMsgID() > 0)
        {
            MsgEvppFromNetInfo* pNetInfo = new MsgEvppFromNetInfo(pEntity->GetConnPtr());
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

bool AFCNetClient::StopAfter(double dTime)
{
    m_pThread->loop()->RunAfter(evpp::Duration(dTime), [&]()
    {
        bWorking = false;
    });
    return true;
}

void AFCNetClient::OnClientConnection(const evpp::TCPConnPtr& conn, void* pData)
{
    AFCNetClient * pClient = (AFCNetClient*)(pData);
    if(pClient)
    {
        pClient->OnClientConnectionInner(conn);
    }
}

void AFCNetClient::OnClientConnectionInner(const evpp::TCPConnPtr& conn)
{
    if(conn->IsConnected())
    {
        conn->SetTCPNoDelay(true);
        MsgEvppFromNetInfo* pMsg = new MsgEvppFromNetInfo(conn);
        pMsg->xClientID = conn->id();
        pMsg->nType = CONNECTED;
        {
            AFScopeWrLock xGuard(mRWLock);

            NFCEvppNetObject* pEntity = new NFCEvppNetObject(this, pMsg->xClientID, conn);
            conn->set_context(evpp::Any(pEntity));
            m_pClientObject.reset(pEntity);
            pEntity->mqMsgFromNet.Push(pMsg);
        }
    }
    else
    {
        MsgEvppFromNetInfo* pMsg = new MsgEvppFromNetInfo(conn);
        pMsg->xClientID = conn->id();
        pMsg->nType = DISCONNECTED;

        if(!conn->context().IsEmpty())
        {
            NFCEvppNetObject* pEntity = evpp::any_cast<NFCEvppNetObject*>(conn->context());
            pEntity->mqMsgFromNet.Push(pMsg);
            conn->set_context(evpp::Any(nullptr));
        }
    }
}

void AFCNetClient::OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* msg, void* pData)
{
    AFCNetClient * pNet = (AFCNetClient*)pData;
    if(pNet)
    {
        pNet->OnMessageInner(conn, msg);
    }
}

void AFCNetClient::OnMessageInner(const evpp::TCPConnPtr& conn, evpp::Buffer* msg)
{
    if(msg == nullptr)
    {
        return;
    }

    nReceiverSize += msg->length();
    NFCEvppNetObject* pEntity = evpp::any_cast<NFCEvppNetObject*>(conn->context());
    if(pEntity == nullptr)
    {
        return;
    }

    evpp::Slice xMsgBuff;
    xMsgBuff = msg->NextAll();
    int nRet = pEntity->AddBuff(xMsgBuff.data(), xMsgBuff.size());
    bool bRet = DismantleNet(pEntity);
    if(!bRet)
    {
        //add log
    }
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