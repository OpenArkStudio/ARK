/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @file      AFCNetServer.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFCNetServer                                                  *
*****************************************************************************/
#include "AFCNetServer.h"
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
#include <atomic>

bool AFCNetServer::Execute()
{
    ProcessMsgLogicThread();

    return true;
}

int AFCNetServer::Initialization(const unsigned int nMaxClient, const std::string& strAddrPort, const int nServerID, const int nCpuCount)
{
#ifdef _MSC_VER
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif

    mnMaxConnect = nMaxClient;
    mstrIPPort = strAddrPort;
    m_pListenThread.reset(new evpp::EventLoopThread);
    m_pListenThread->set_name("LisenThread");
    m_pListenThread->Start(true);
    m_pTcpSrv.reset(new evpp::TCPServer(m_pListenThread->loop(), strAddrPort, "tcp_server", nCpuCount));
    m_pTcpSrv->SetMessageCallback(std::bind(&AFCNetServer::OnMessage, std::placeholders::_1, std::placeholders::_2, (void*)this));
    m_pTcpSrv->SetConnectionCallback(std::bind(&AFCNetServer::OnClientConnection, std::placeholders::_1, (void*) this));
    m_pTcpSrv->Init();
    m_pTcpSrv->Start();
    bWorking = true;
    return 0;
}

void AFCNetServer::OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* msg, void* pData)
{
    AFCNetServer * pServer = (AFCNetServer*)pData;
    if(pServer)
    {
        pServer->OnMessageInner(conn, msg);
    }
}

void AFCNetServer::OnMessageInner(const evpp::TCPConnPtr& conn, evpp::Buffer* msg)
{
    if(conn->context().IsEmpty())
    {
        return;
    }

    NetObject* pObject = evpp::any_cast<NetObject*>(conn->context());
    if(pObject)
    {
        evpp::Slice xMsgBuff;
        if(msg)
        {
            xMsgBuff = msg->NextAll();
            pObject->AddBuff(xMsgBuff.data(), xMsgBuff.size());
            DismantleNet(pObject);
        }
    }
}

void AFCNetServer::OnClientConnection(const evpp::TCPConnPtr& conn, void* pData)
{
    AFCNetServer * pServer = (AFCNetServer*)pData;
    if(pServer)
    {
        pServer->OnClientConnectionInner(conn);
    }
}
void AFCNetServer::OnClientConnectionInner(const evpp::TCPConnPtr& conn)
{
    if(conn->IsConnected())
    {
        conn->SetTCPNoDelay(true);
        MsgFromNetInfo* pMsg = new MsgFromNetInfo(conn);
        pMsg->xClientID = conn->id();
        pMsg->nType = CONNECTED;
        {
            AFScopeWrLock xGuard(mRWLock);

            NetObject* pObject = new NetObject(this, pMsg->xClientID, conn);
            AddNetObject(pMsg->xClientID, pObject);
            conn->set_context(evpp::Any(pObject));

            pObject->mqMsgFromNet.Push(pMsg);
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
            NetObject* pObject = evpp::any_cast<NetObject*>(conn->context());
            pObject->mqMsgFromNet.Push(pMsg);
            conn->set_context(evpp::Any(nullptr));
        }
    }
}

void AFCNetServer::ProcessMsgLogicThread()
{
    std::list<AFGUID> xNeedRemoveList;
    {
        AFScopeRdLock xGuard(mRWLock);
        for(std::map<AFGUID, NetObject*>::iterator iter = mmObject.begin(); iter != mmObject.end(); ++iter)
        {
            ProcessMsgLogicThread(iter->second);
            if(iter->second->NeedRemove())
            {
                iter->second->GetClientID();
            }
        }
    }

    for(std::list<AFGUID>::iterator iter = xNeedRemoveList.begin(); iter != xNeedRemoveList.end(); ++iter)
    {
        AFScopeWrLock xGuard(mRWLock);
        RemoveNetObject(*iter);
    }
}

void AFCNetServer::ProcessMsgLogicThread(NetObject* pObject)
{
    //Handle Msg;
    const int nReceiveCount = pObject->mqMsgFromNet.Count();
    for(size_t i = 0; (i < nReceiveCount); i++)
    {
        MsgFromNetInfo* pMsgFromNet(NULL);
        if(!pObject->mqMsgFromNet.Pop(pMsgFromNet))
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
                    mRecvCB(pMsgFromNet->xHead, pMsgFromNet->xHead.GetMsgID(), pMsgFromNet->strMsg.c_str(), pMsgFromNet->strMsg.size(), pObject->GetClientID());
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
                pObject->SetNeedRemove(true);
            }
            break;
        default:
            break;
        }

        delete pMsgFromNet;
    }
}

bool AFCNetServer::Final()
{
    m_pTcpSrv->Stop();
    m_pListenThread->Stop(true);
    bWorking = false;
    return true;
}

bool AFCNetServer::SendMsgToAllClient(const char* msg, const uint32_t nLen)
{
    std::map<AFGUID, NetObject*>::iterator it = mmObject.begin();
    for(; it != mmObject.end(); ++it)
    {
        NetObject* pNetObject = (NetObject*)it->second;
        if(pNetObject && !pNetObject->NeedRemove())
        {
            pNetObject->GetConnPtr()->Send(msg, nLen);
        }
    }

    return true;
}

bool AFCNetServer::SendMsg(const char* msg, const uint32_t nLen, const AFGUID& xClient)
{
    if(nLen <= 0)
    {
        return false;
    }
    AFScopeRdLock xGuard(mRWLock);

    NetObject* pNetObject = GetNetObject(xClient);
    if(NULL == pNetObject)
    {
        return false;
    }

    pNetObject->GetConnPtr()->Send(msg, nLen);
    return false;
}

bool AFCNetServer::AddNetObject(const AFGUID& xClientID, NetObject* pObject)
{
    return mmObject.insert(std::make_pair(xClientID, pObject)).second;
}

bool AFCNetServer::RemoveNetObject(const AFGUID& xClientID)
{
    NetObject* pNetObject = GetNetObject(xClientID);
    if(pNetObject)
    {
        delete pNetObject;
    }
    return mmObject.erase(xClientID);
}

bool AFCNetServer::CloseNetObject(const AFGUID& xClientID)
{
    NetObject* pObject = GetNetObject(xClientID);
    if(pObject)
    {
        pObject->GetConnPtr()->Close();
    }

    return true;
}

bool AFCNetServer::DismantleNet(NetObject* pObject)
{
    for(; pObject->GetBuffLen() >= AFIMsgHead::AF_Head::NF_HEAD_LENGTH;)
    {
        AFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pObject->GetBuff(), pObject->GetBuffLen(), xHead);
        if(nMsgBodyLength >= 0 && xHead.GetMsgID() > 0)
        {
            MsgFromNetInfo* pNetInfo = new  MsgFromNetInfo(pObject->GetConnPtr());
            pNetInfo->xHead = xHead;
            pNetInfo->nType = RECIVEDATA;
            pNetInfo->strMsg.append(pObject->GetBuff() + AFIMsgHead::AF_Head::NF_HEAD_LENGTH, nMsgBodyLength);
            pObject->mqMsgFromNet.Push(pNetInfo);
            pObject->RemoveBuff(nMsgBodyLength + AFIMsgHead::AF_Head::NF_HEAD_LENGTH);
        }
        break;
    }

    return true;
}

bool AFCNetServer::CloseSocketAll()
{
    std::map<AFGUID, NetObject*>::iterator it = mmObject.begin();
    for(it; it != mmObject.end(); ++it)
    {
        it->second->GetConnPtr()->Close();
        delete it->second;
    }

    mmObject.clear();

    return true;
}

NetObject* AFCNetServer::GetNetObject(const AFGUID& xClientID)
{
    std::map<AFGUID, NetObject*>::iterator it = mmObject.find(xClientID);
    if(it != mmObject.end())
    {
        return it->second;
    }

    return NULL;
}

bool AFCNetServer::SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID, const AFGUID& xPlayerID)
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

bool AFCNetServer::SendMsgToAllClientWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xPlayerID)
{
    std::string strOutData;
    AFCMsgHead xHead;
    xHead.SetMsgID(nMsgID);
    xHead.SetPlayerID(xPlayerID);

    int nAllLen = EnCode(xHead, msg, nLen, strOutData);
    if(nAllLen == nLen + AFIMsgHead::AF_Head::NF_HEAD_LENGTH)
    {
        return SendMsgToAllClient(strOutData.c_str(), strOutData.length());
    }

    return false;
}

int AFCNetServer::EnCode(const AFCMsgHead& xHead, const char* strData, const uint32_t unDataLen, std::string& strOutData)
{
    char szHead[AFIMsgHead::AF_Head::NF_HEAD_LENGTH] = { 0 };
    xHead.EnCode(szHead);

    strOutData.clear();
    strOutData.append(szHead, AFIMsgHead::AF_Head::NF_HEAD_LENGTH);
    strOutData.append(strData, unDataLen);

    return xHead.GetBodyLength() + AFIMsgHead::AF_Head::NF_HEAD_LENGTH;
}

int AFCNetServer::DeCode(const char* strData, const uint32_t unAllLen, AFCMsgHead& xHead)
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

