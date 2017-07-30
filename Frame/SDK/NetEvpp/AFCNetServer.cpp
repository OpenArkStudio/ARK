// -------------------------------------------------------------------------
//    @FileName         ��    AFCNetServer.cpp
//    @Author           ��    Chaunbo.Guo
//    @Date             ��    2013-12-15
//    @Module           ��    AFCNetServer
//    @Desc             :     AFCNetServer
// -------------------------------------------------------------------------

#include "AFCNetServer.h"
#include <string.h>

#if NF_PLATFORM == NF_PLATFORM_WIN
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma  comment(lib,"Ws2_32.lib")
#pragma  comment(lib,"libevent.lib")
#pragma  comment(lib,"libevent_core.lib")
#elif NF_PLATFORM == NF_PLATFORM_APPLE
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

bool AFCNetServer::IsStop()
{
    return !bWorking;
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
    MsgFromNetInfo* pMsg = new MsgFromNetInfo(conn);
    pMsg->nType = RECIVEDATA;
    pMsg->xClientID = conn->id();
    if(msg)
    {
        pMsg->strMsg = msg->NextAllString();
    }

    mqMsgFromNet.Push(pMsg);
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
        MsgFromNetInfo* pMsg = new MsgFromNetInfo(conn);
        pMsg->xClientID = conn->id();
        pMsg->nType = CONNECTED;
        mqMsgFromNet.Push(pMsg);
    }
    else
    {
        MsgFromNetInfo* pMsg = new MsgFromNetInfo(conn);
        pMsg->xClientID = conn->id();
        pMsg->nType = DISCONNECTED;
        mqMsgFromNet.Push(pMsg);
    }
}

void AFCNetServer::ProcessMsgLogicThread()
{
    //Handle Msg;
    const int nReceiveCount = mqMsgFromNet.Count();
    for(size_t i = 0; (i < nReceiveCount) && (i < 100); i++)
    {
        MsgFromNetInfo* pMsgFromNet(NULL);
        if(!mqMsgFromNet.Pop(pMsgFromNet))
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
                NetObject* pObject = GetNetObject(pMsgFromNet->xClientID);
                if(NULL != pObject)
                {
                    pObject->AddBuff(pMsgFromNet->strMsg.c_str(), pMsgFromNet->strMsg.length());
                    Dismantle(pObject);
                }
            }
            break;
        case CONNECTED:
            {
                NetObject* pObject = new NetObject(this, pMsgFromNet->xClientID, pMsgFromNet->mTCPConPtr);
                AddNetObject(pMsgFromNet->xClientID, pObject);
                mEventCB((NetEventType)pMsgFromNet->nType, pMsgFromNet->xClientID, mnServerID);
            }
            break;
        case DISCONNECTED:
            {
                mEventCB((NetEventType)pMsgFromNet->nType, pMsgFromNet->xClientID, mnServerID);
                RemoveNetObject(pMsgFromNet->xClientID);
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
        pObject->SetNeedRemove(true);
        pObject->GetConnPtr()->Close();
    }

    RemoveNetObject(xClientID);

    return false;
}

bool AFCNetServer::Dismantle(NetObject* pObject)
{
    int len = pObject->GetBuffLen();
    for(; len > AFIMsgHead::NF_Head::NF_HEAD_LENGTH;)
    {
        AFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pObject->GetBuff(), len, xHead);
        if(nMsgBodyLength > 0 && xHead.GetMsgID() > 0)
        {
            int nRet = 0;
            if(mRecvCB)
            {
                mRecvCB(xHead.GetMsgID(), pObject->GetBuff() + AFIMsgHead::NF_Head::NF_HEAD_LENGTH, nMsgBodyLength, pObject->GetClientID());
            }

            pObject->RemoveBuff(0, nMsgBodyLength + AFIMsgHead::NF_Head::NF_HEAD_LENGTH);
            len = pObject->GetBuffLen();
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

bool AFCNetServer::SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    std::string strOutData;
    int nAllLen = EnCode(nMsgID, msg, nLen, strOutData);
    if(nAllLen == nLen + AFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        return SendMsg(strOutData.c_str(), strOutData.length(), xClientID);
    }

    return false;
}

bool AFCNetServer::SendMsgToAllClientWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen)
{
    std::string strOutData;
    int nAllLen = EnCode(nMsgID, msg, nLen, strOutData);
    if(nAllLen == nLen + AFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        return SendMsgToAllClient(strOutData.c_str(), strOutData.length());
    }

    return false;
}

int AFCNetServer::EnCode(const uint16_t unMsgID, const char* strData, const uint32_t unDataLen, std::string& strOutData)
{
    AFCMsgHead xHead;
    xHead.SetMsgID(unMsgID);
    xHead.SetBodyLength(unDataLen);

    char szHead[AFIMsgHead::NF_Head::NF_HEAD_LENGTH] = { 0 };
    xHead.EnCode(szHead);

    strOutData.clear();
    strOutData.append(szHead, AFIMsgHead::NF_Head::NF_HEAD_LENGTH);
    strOutData.append(strData, unDataLen);

    return xHead.GetBodyLength() + AFIMsgHead::NF_Head::NF_HEAD_LENGTH;
}

int AFCNetServer::DeCode(const char* strData, const uint32_t unAllLen, AFCMsgHead& xHead)
{
    if(unAllLen < AFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        return -1;
    }

    if(AFIMsgHead::NF_Head::NF_HEAD_LENGTH != xHead.DeCode(strData))
    {
        return -2;
    }

    if(xHead.GetBodyLength() > (unAllLen - AFIMsgHead::NF_Head::NF_HEAD_LENGTH))
    {
        return -3;
    }

    return xHead.GetBodyLength();
}
