/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "AFCNetClient.h"
#include <string.h>

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

bool AFCNetClient::Execute()
{
    ProcessMsgLogicThread();
    return true;
}

void AFCNetClient::ProcessMsgLogicThread()
{
    {
        AFScopeRdLock xGuard(mRWLock);
        ProcessMsgLogicThread(m_pClientObject.get());
    }

    if(NULL  != m_pClientObject.get() && m_pClientObject->NeedRemove())
    {
        AFScopeWrLock xGuard(mRWLock);
        m_pClientObject.release();
    }

}

void AFCNetClient::ProcessMsgLogicThread(NetObject* pEntity)
{
    if(pEntity == nullptr)
    {
        return;
    }

    //Handle Msg;
    const int nReceiveCount = pEntity->mqMsgFromNet.Count();
    for(size_t i = 0; (i < nReceiveCount); i++)
    {
        MsgFromNetInfo* pMsgFromNet(NULL);
        if(!pEntity->mqMsgFromNet.Pop(pMsgFromNet))
        {
            break;
        }

        if(!pMsgFromNet)
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
void AFCNetClient::Initialization(const std::string& strAddrPort, const int nServerID)
{
#ifdef _MSC_VER
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif

    mstrIPPort = strAddrPort;
    mnServerID = nServerID;
    m_pThread.reset(new evpp::EventLoopThread);
    m_pThread->set_name("TCPClientThread");
    m_pThread->Start();

    m_pClient.reset(new evpp::TCPClient(m_pThread->loop(), mstrIPPort, "TCPPingPongClient"));
    m_pClient->SetConnectionCallback(std::bind(&AFCNetClient::OnClientConnectionInner,this, std::placeholders::_1));
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

bool AFCNetClient::SendMsg(const char* msg, const uint32_t nLen, const AFGUID& xClient)
{
    if(m_pClient->conn().get())
    {
        m_pClient->conn()->Send(msg, nLen);
    }

    return false;
}

bool AFCNetClient::CloseNetObject(const AFGUID& xClient)
{
    m_pClient->Disconnect();
    return true;
}

bool AFCNetClient::DismantleNet(NetObject* pEntity)
{
    for(; pEntity->GetBuffLen() >= AFIMsgHead::AF_Head::NF_HEAD_LENGTH;)
    {
        AFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pEntity->GetBuff(), pEntity->GetBuffLen(), xHead);
        if(nMsgBodyLength >= 0 && xHead.GetMsgID() > 0)
        {
            MsgFromNetInfo* pNetInfo = new MsgFromNetInfo(pEntity->GetConnPtr());
            pNetInfo->xHead = xHead;
            pNetInfo->nType = RECIVEDATA;
            pNetInfo->strMsg.append(pEntity->GetBuff() + AFIMsgHead::AF_Head::NF_HEAD_LENGTH, nMsgBodyLength);
            pEntity->mqMsgFromNet.Push(pNetInfo);
            int nNewSize = pEntity->RemoveBuff(nMsgBodyLength + AFIMsgHead::AF_Head::NF_HEAD_LENGTH);
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
        MsgFromNetInfo* pMsg = new MsgFromNetInfo(conn);
        pMsg->xClientID = conn->id();
        pMsg->nType = CONNECTED;
        {
            AFScopeWrLock xGuard(mRWLock);

            NetObject* pEntity = new NetObject(this, pMsg->xClientID, conn);
            conn->set_context(evpp::Any(pEntity));
            m_pClientObject.reset(pEntity);
            pEntity->mqMsgFromNet.Push(pMsg);
        }
    }
    else
    {
        MsgFromNetInfo* pMsg = new MsgFromNetInfo(conn);
        pMsg->xClientID = conn->id();
        pMsg->nType = DISCONNECTED;


        //主线程不能直接删除。不然这里就野了
        if(!conn->context().IsEmpty())
        {
            NetObject* pEntity = evpp::any_cast<NetObject*>(conn->context());
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
    NetObject* pEntity = evpp::any_cast<NetObject*>(conn->context());
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

bool AFCNetClient::SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const AFGUID & xClientID, const AFGUID& xPlayerID)
{
    std::string strOutData;
    AFCMsgHead xHead;
    xHead.SetMsgID(nMsgID);
    xHead.SetPlayerID(xPlayerID);
    xHead.SetBodyLength(nLen);

    int nAllLen = EnCode(xHead, msg, nLen, strOutData);
    if(nAllLen == nLen + AFIMsgHead::AF_Head::NF_HEAD_LENGTH)
    {
        return SendMsg(strOutData.c_str(), strOutData.length(), xClientID);
    }

    return false;
}

int AFCNetClient::EnCode(const AFCMsgHead& xHead, const char* strData, const uint32_t unDataLen, std::string& strOutData)
{
    char szHead[AFIMsgHead::AF_Head::NF_HEAD_LENGTH] = { 0 };
    int nSize = xHead.EnCode(szHead);

    strOutData.clear();
    strOutData.append(szHead, AFIMsgHead::AF_Head::NF_HEAD_LENGTH);
    strOutData.append(strData, unDataLen);

    return xHead.GetBodyLength() + AFIMsgHead::AF_Head::NF_HEAD_LENGTH;
}

int AFCNetClient::DeCode(const char* strData, const uint32_t unAllLen, AFCMsgHead & xHead)
{
    if(unAllLen < AFIMsgHead::AF_Head::NF_HEAD_LENGTH)
    {
        return -1;
    }

    if(AFIMsgHead::AF_Head::NF_HEAD_LENGTH != xHead.DeCode(strData))
    {
        return -2;
    }

    if(xHead.GetBodyLength() > (unAllLen - AFIMsgHead::AF_Head::NF_HEAD_LENGTH))
    {
        return -3;
    }
    return xHead.GetBodyLength();
}

