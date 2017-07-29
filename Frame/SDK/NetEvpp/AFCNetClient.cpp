// -------------------------------------------------------------------------
//    @FileName         ��    AFCNet.cpp
//    @Author           ��    Ark Game Tech
//    @Date             ��    2013-12-15
//    @Module           ��    AFIPacket
//    @Desc             :     CNet
// -------------------------------------------------------------------------

#include "AFCNetClient.h"
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

std::uint64_t AFCNetClient::mObjectIndex = 0;
bool AFCNetClient::Execute()
{
    ProcessMsgLogicThread();
    return true;
}

void AFCNetClient::ProcessMsgLogicThread()
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
        case MsgFromNetInfo::RECIVEDATA:
            {
                NetObject* pObject = GetNetObject(pMsgFromNet->xClientID);
                if(NULL != pObject)
                {
                    pObject->AddBuff(pMsgFromNet->strMsg.c_str(), pMsgFromNet->strMsg.length());
                    Dismantle(pObject);
                }
            }
            break;
        case MsgFromNetInfo::CONNECTED:
            {
                NetObject* pObject = new NetObject(this, pMsgFromNet->xClientID, pMsgFromNet->mTCPConPtr);
                AddNetObject(pMsgFromNet->xClientID, pObject);
                mEventCB(0, (NF_NET_EVENT)pMsgFromNet->nType, pMsgFromNet->xClientID, mnServerID);
            }
            break;
        case MsgFromNetInfo::DISCONNECTED:
            {
                mEventCB(0, (NF_NET_EVENT)pMsgFromNet->nType, pMsgFromNet->xClientID, mnServerID);
                RemoveNetObject(pMsgFromNet->xClientID);
            }
            break;
        default:
            break;
        }

        delete pMsgFromNet;
    }
}

void AFCNetClient::Initialization(const char* strIP, const unsigned short nPort, const int nServerID)
{
    mstrIP = strIP;
    mnPort = nPort;

    mnServerID = nServerID;
    m_pThread.reset(new evpp::EventLoopThread);
    m_pThread->SetName("TCPClientThread");
    m_pThread->Start();

    std::string strPort;
    NF_ToStr(strPort, nPort);
    std::string addr = mstrIP + ":" + strPort;
    m_pClient.reset(new evpp::TCPClient(m_pThread->event_loop(), addr, "TCPPingPongClient"));
    m_pClient->SetConnectionCallback(std::bind(&AFCNetClient::OnClientConnection, std::placeholders::_1, (void*) this));
    m_pClient->SetMessageCallback(std::bind(&AFCNetClient::OnMessage, std::placeholders::_1, std::placeholders::_2, (void*)this));
    m_pClient->Connect();
}


bool AFCNetClient::Final()
{
    CloseSocketAll();
    return true;
}

bool AFCNetClient::CloseSocketAll()
{
    m_pClient->Disconnect();
    m_pThread->Stop(true);
    std::map<AFGUID, NetObject*>::iterator it = mmObject.begin();
    for(it; it != mmObject.end(); ++it)
    {
        it->second->GetConnPtr()->Close();

        delete it->second;
    }

    mmObject.clear();

    return true;
}

bool AFCNetClient::SendMsg(const char* msg, const uint32_t nLen, const AFGUID& xClient)
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

bool AFCNetClient::CloseNetObject(const AFGUID& xClient)
{
    NetObject* pObject = GetNetObject(xClient);
    if(pObject)
    {
        pObject->GetConnPtr()->Close();
        RemoveNetObject(xClient);
    }

    //m_pClient->Disconnect();
    return true;
}

bool AFCNetClient::Dismantle(NetObject* pObject)
{
    int len = pObject->GetBuffLen();
    for(; len > AFIMsgHead::NF_Head::NF_HEAD_LENGTH;)
    {
        AFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pObject->GetBuff(), len, xHead);
        if(nMsgBodyLength > 0 && xHead.GetMsgID() > 0)
        {
            if(mRecvCB)
            {
                mRecvCB(pObject->GetRealFD(), xHead.GetMsgID(), pObject->GetBuff() + AFIMsgHead::NF_Head::NF_HEAD_LENGTH, nMsgBodyLength, pObject->GetClientID());
            }

            pObject->RemoveBuff(0, nMsgBodyLength + AFIMsgHead::NF_Head::NF_HEAD_LENGTH);
            len = pObject->GetBuffLen();
        }
        break;
    }

    return true;
}

bool AFCNetClient::AddNetObject(const AFGUID& xClientID, NetObject* pObject)
{
    return mmObject.insert(std::make_pair(xClientID, pObject)).second;
}
bool AFCNetClient::RemoveNetObject(const AFGUID& xClientID)
{
    NetObject* pNetObject = GetNetObject(xClientID);
    if(pNetObject)
    {
        delete pNetObject;
    }
    return mmObject.erase(xClientID);
}

NetObject* AFCNetClient::GetNetObject(const AFGUID& xClientID)
{
    std::map<AFGUID, NetObject*>::iterator it = mmObject.find(xClientID);
    if(it != mmObject.end())
    {
        return it->second;
    }

    return NULL;
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
        MsgFromNetInfo* pMsg = new MsgFromNetInfo(conn);
        pMsg->xClientID = 0;
        conn->SetID(pMsg->xClientID.n64Value);
        pMsg->nType = MsgFromNetInfo::CONNECTED;
        mqMsgFromNet.Push(pMsg);
    }
    else
    {
        MsgFromNetInfo* pMsg = new MsgFromNetInfo(conn);
        pMsg->xClientID = conn->GetID();
        pMsg->nType = MsgFromNetInfo::DISCONNECTED;
        mqMsgFromNet.Push(pMsg);
    }
}

void AFCNetClient::OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* msg, void* pData)
{
    AFCNetClient * pClent = (AFCNetClient*)pData;
    if(pClent)
    {
        pClent->OnMessageInner(conn, msg);
    }
}

void AFCNetClient::OnMessageInner(const evpp::TCPConnPtr& conn, evpp::Buffer* msg)
{
    MsgFromNetInfo* pMsg = new MsgFromNetInfo(conn);
    pMsg->nType = MsgFromNetInfo::RECIVEDATA;
    pMsg->xClientID = conn->GetID();
    if(msg)
    {
        int nLen = msg->WritableBytes();
        pMsg->strMsg = msg->NextString(nLen);
    }

    mqMsgFromNet.Push(pMsg);
}
bool AFCNetClient::SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const AFGUID & xClientID)
{
    std::string strOutData;
    int nAllLen = EnCode(nMsgID, msg, nLen, strOutData);
    if(nAllLen == nLen + AFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        return SendMsg(strOutData.c_str(), strOutData.length(), xClientID);
    }

    return false;
}

int AFCNetClient::EnCode(const uint16_t unMsgID, const char* strData, const uint32_t unDataLen, std::string & strOutData)
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

int AFCNetClient::DeCode(const char* strData, const uint32_t unAllLen, AFCMsgHead & xHead)
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
