// -------------------------------------------------------------------------
//    @FileName         ��    AFCNet.cpp
//    @Author           ��    Ark Game Tech
//    @Date             ��    2013-12-15
//    @Module           ��    AFIPacket
//    @Desc             :     CNet
// -------------------------------------------------------------------------

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

void AFCNetClient::ProcessMsgLogicThread(NetObject* pObject)
{
    if(NULL ==  pObject)
    {
        return;
    }

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
                    mRecvCB(pMsgFromNet->xHead.GetMsgID(), pMsgFromNet->strMsg.c_str(), pMsgFromNet->strMsg.size(), pObject->GetClientID());
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
    m_pClient->SetConnectionCallback(std::bind(&AFCNetClient::OnClientConnection, std::placeholders::_1, (void*) this));
    m_pClient->SetMessageCallback(std::bind(&AFCNetClient::OnMessage, std::placeholders::_1, std::placeholders::_2, (void*)this));
    m_pClient->Connect();

    bWorking = true;
}

bool AFCNetClient::Final()
{
    CloseSocketAll();
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
    if(nLen <= 0)
    {
        return false;
    }

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

bool AFCNetClient::DismantleNet(NetObject* pObject)
{
    int len = pObject->GetBuffLen();
    for(; pObject->BuffChange() && len > AFIMsgHead::NF_Head::NF_HEAD_LENGTH;)
    {
        AFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pObject->GetBuff(), len, xHead);
        if(nMsgBodyLength > 0 && xHead.GetMsgID() > 0)
        {
            /*int nRet = 0;
            if(mRecvCB)
            {
            mRecvCB(xHead.GetMsgID(), pObject->GetBuff() + AFIMsgHead::NF_Head::NF_HEAD_LENGTH, nMsgBodyLength, pObject->GetClientID());
            }*/
            MsgFromNetInfo* pNetInfo = new  MsgFromNetInfo(pObject->GetConnPtr());
            pNetInfo->xHead = xHead;
            pNetInfo->nType = RECIVEDATA;
            pNetInfo->strMsg.append(pObject->GetBuff() + AFIMsgHead::NF_Head::NF_HEAD_LENGTH, nMsgBodyLength);
            pObject->mqMsgFromNet.Push(pNetInfo);
            pObject->RemoveBuff(nMsgBodyLength + AFIMsgHead::NF_Head::NF_HEAD_LENGTH);
            len = pObject->GetBuffLen();
        }
        break;
    }

    pObject->Reset();

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
        conn->SetTCPNoDelay(true);
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

            NetObject* pObject = new NetObject(this, pMsg->xClientID, conn);
            conn->set_context(evpp::Any(pObject));
            m_pClientObject.reset(pObject);
            pObject->mqMsgFromNet.Push(pMsg);
        }
    }
    else
    {
        MsgFromNetInfo* pMsg = new MsgFromNetInfo(conn);
        pMsg->xClientID = conn->id();
        pMsg->nType = DISCONNECTED;


        //主线程不能直接删除。不然这里就野了
        NetObject* pObject = evpp::any_cast<NetObject*>(conn->context());
        pObject->mqMsgFromNet.Push(pMsg);
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
    nReceiverSize += msg->length();
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
