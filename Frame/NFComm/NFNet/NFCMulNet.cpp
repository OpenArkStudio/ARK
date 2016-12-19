// -------------------------------------------------------------------------
//    @FileName         ��    NFCMulNet.cpp
//    @Author           ��    Chaunbo.Guo
//    @Date             ��    2013-12-15
//    @Module           ��    NFCMulNet
//    @Desc             :     NFCMulNet
// -------------------------------------------------------------------------

#include "NFCMulNet.h"
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

std::atomic_uint64_t NFCMulNet::mObjectIndex = 0;
std::atomic_uint64_t NFCMulNet::mThreadExit = 0;

void NFCMulNet::conn_writecb(struct bufferevent* bev, void* user_data)
{
    //ÿ���յ�������Ϣ��ʱ���¼�
    //  struct evbuffer *output = bufferevent_get_output(bev);
}

void NFCMulNet::conn_eventcb(struct bufferevent* bev, short events, void* user_data)
{
    NetObject* pObject = (NetObject*)user_data;
    NFCMulNet* pNet = (NFCMulNet*)pObject->GetNet();

    if (events & BEV_EVENT_CONNECTED)
    {
        //must to set it's state before the "EventCB" functional be called[maybe user will send msg in the callback function]
        pNet->mbWorking = true;
    }
    else
    {
        if (!pNet->mbServer)
        {
            pNet->mbWorking = false;
        }
    }

    EventInfo xMsg;
    xMsg.nSockIndex = pObject->GetRealFD();
    xMsg.nEvent = events;
    xMsg.xClientID = pObject->GetClientID();
    pNet->mqEventInfo.Push(xMsg);

    if (events & BEV_EVENT_CONNECTED)
    {
        //printf("%d Connection successed\n", pObject->GetFd());/*XXX win32*/
    }
    else
    {
        pNet->CloseNetObject(pObject->GetRealFD());
    }
}

void NFCMulNet::listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data)
{
    //����������
    NFCMulNet* pNet = (NFCMulNet*)user_data;
    bool bClose = pNet->NetThreadCloseNetObject(fd);
    if (bClose)
    {
        return;
    }

    if (pNet->mmObject.size() >= pNet->mnMaxConnect)
    {
        //Ӧ��T�����ܾ�
        return;
    }

    struct event_base* base = pNet->base;
    //����һ������socket��bufferevent
//     int flag = 1;
//     int nRet = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)& flag, sizeof(flag));
    struct bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev)
    {
        //Ӧ��T�����ܾ�
        fprintf(stderr, "Error constructing bufferevent!");
        //event_base_loopbreak(base);
        return;
    }

    //�һ���һ�������ӡ�Ϊ�䴴��һ��bufferevent--FD��Ҫ����
    struct sockaddr_in* pSin = (sockaddr_in*)sa;

    NetObject* pObject = new NetObject(pNet, fd, *pSin, bev);
    pNet->NetThreadAddNetObject(fd, pObject);

    //Ϊbufferevent���ø��ֻص�
    bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, (void*)pObject);

    //����bufferevent�Ķ�д
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    //ģ���ͻ����������¼�
    conn_eventcb(bev, BEV_EVENT_CONNECTED, (void*)pObject);
}


void NFCMulNet::conn_readcb(struct bufferevent* bev, void* user_data)
{
    //���ܵ���Ϣ
    NetObject* pObject = (NetObject*)user_data;
    if (!pObject)
    {
        return;
    }

    NFCMulNet* pNet = (NFCMulNet*)pObject->GetNet();
    if (!pNet)
    {
        return;
    }

    if (pObject->NeedRemove())
    {
        return;
    }

    struct evbuffer* input = bufferevent_get_input(bev);
    if (!input)
    {
        return;
    }

    size_t len = evbuffer_get_length(input);

    //���ظ��ͻ���
    //      struct evbuffer *output = bufferevent_get_output(bev);
    //      evbuffer_add_buffer(output, input);
    //      SendMsg(1, strData,len, pObject->GetFd());
    //////////////////////////////////////////////////////////////////////////

    //�����ó�Ա��̬��
    char* strMsg = new char[len];

    if (evbuffer_remove(input, strMsg, len) > 0)
    {
        pObject->AddBuff(strMsg, len);
    }

    delete[] strMsg;

    while (1)
    {
        if (!pNet->Dismantle(pObject))
        {
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////

bool NFCMulNet::Execute()
{
    ProcessMsgLogicThread();

    return true;
}


void NFCMulNet::Initialization(const char* strIP, const unsigned short nPort, const int nServerID )
{
    if (!mpNetThread)
    {
        mpNetThread = new std::thread(&NFCMulNet::MulThreadRunClient, this, strIP, nPort, nServerID);
    }
}

int NFCMulNet::Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nServerID, const int nCpuCount)
{
    if (!mpNetThread)
    {
        mpNetThread = new std::thread(&NFCMulNet::MulThreadRunServer, this, nMaxClient, nPort, nCpuCount, nServerID);
    }

    return 0;
}

void NFCMulNet::MulThreadRunClient(const char* strIP, const unsigned short nPort, const int nServerID /*= 0*/)
{
    mstrIP = strIP;
    mnPort = nPort;
    mnServerID = nServerID;

    InitClientNet();

    while (!mThreadExit)
    {
        NFSLEEP(1);
        ProcessMsgNetThread();
        ProcessNetTaskNetThread();
        ExecuteClose();
        if (base)
        {
            event_base_loop(base, EVLOOP_ONCE | EVLOOP_NONBLOCK);
        }
    }

    CloseSocketAll();
    if (listener)
    {
        evconnlistener_free(listener);
        listener = NULL;
    }

    if (!mbServer)
    {
        if (base)
        {
            event_base_free(base);
            base = NULL;
        }
    }
}

void NFCMulNet::MulThreadRunServer(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount /*= 4*/, const int nServerID/* = 0*/)
{
    mnMaxConnect = nMaxClient;
    mnPort = nPort;
    mnCpuCount = nCpuCount;
    mnServerID = nServerID;

    InitServerNet();

    while (!mThreadExit)
    {
        NFSLEEP(1);
        ProcessMsgNetThread();
        ProcessNetTaskNetThread();

        ExecuteClose();
        if (base)
        {
            event_base_loop(base, EVLOOP_ONCE | EVLOOP_NONBLOCK);
        }
    }

    CloseSocketAll();
    if (listener)
    {
        evconnlistener_free(listener);
        listener = NULL;
    }

    if (!mbServer)
    {
        if (base)
        {
            event_base_free(base);
            base = NULL;
        }
    }
}

void NFCMulNet::ProcessMsgLogicThread()
{
    //Handle Msg;
    const int nReceiveCount = mqReciveMsg.Count();
    for (size_t i = 0; (i < nReceiveCount) && (i < 100 ); i++)
    {
        RecivemsgInfo xMsg;
        if (!mqReciveMsg.Pop(xMsg))
        {
            break;
        }

        mRecvCB(xMsg.nRealSockIndex, xMsg.nMsgID, xMsg.strMsg.data(), xMsg.strMsg.size(), xMsg.xClientID);
    }

    //Handle event;
    const int nEventCount = mqEventInfo.Count();
    for (size_t i = 0; (i < nEventCount) && (i < 100) ; i++)
    {
        EventInfo xMsg;
        if (!mqEventInfo.Pop(xMsg))
        {
            break;
        }

        mEventCB(xMsg.nSockIndex, (NF_NET_EVENT)xMsg.nEvent, xMsg.xClientID, mnServerID);
    }
}

bool NFCMulNet::NetThreadCloseNetObject(const int nSockIndex)
{
    std::map<int, NetObject*>::iterator it = mmObject.find(nSockIndex);
    if (it != mmObject.end())
    {
        NetObject* pObject = it->second;

        pObject->SetNeedRemove(true);
        mvRemoveObject.push_back(nSockIndex);

        return true;
    }

    return false;
}

void NFCMulNet::ProcessMsgNetThread()
{
    //Send Msg;
    const int nCount = mqSendMsg.Count();
    for (size_t i = 0; (i < nCount) && (i < 100); i++)
    {
        SendmsgInfo xMsg;
        if (!mqSendMsg.Pop(xMsg))
        {
            break;
        }

        if (xMsg.bSendAllObject)
        {
            NetThreadSendMsgToAllClient(xMsg.strMsg.c_str(), xMsg.strMsg.size());
        }
        else
        {
            NetThreadSendMsg(xMsg.strMsg.c_str(), xMsg.strMsg.size(), xMsg.nSockIndex);
        }
    }
}

void NFCMulNet::ProcessNetTaskNetThread()
{
    NetTaskInfo xMsg;
    while (mqNetTaskInfo.Pop(xMsg))
    {
        switch (xMsg.nTaskType)
        {
        case NetTaskInfo::NETTASKTYPE_DELETEOBJECT:
        {
            std::map<int, NetObject*>::iterator it = mmObject.find(xMsg.nSockIndex);
            if (it != mmObject.end())
            {
                NetObject* pObject = it->second;

                pObject->SetNeedRemove(true);
                mvRemoveObject.push_back(xMsg.nSockIndex);
            }
        }
        break;
        default:
            break;
        }

    }
}

bool NFCMulNet::Final()
{
    mThreadExit = 1;

    mpNetThread->join();
    return true;
}

bool NFCMulNet::SendMsgToAllClient(const char* msg, const uint32_t nLen)
{
    if (nLen <= 0)
    {
        return false;
    }

    SendmsgInfo xMsgInfo;

    xMsgInfo.nSockIndex = -1;
    xMsgInfo.bSendAllObject = true;
    xMsgInfo.strMsg = std::string(msg, nLen);
    mqSendMsg.Push(xMsgInfo);

    return true;
}

bool NFCMulNet::NetThreadSendMsgToAllClient(const char* msg, const uint32_t nLen)
{
    if (nLen <= 0)
    {
        return false;
    }

    std::map<int, NetObject*>::iterator it = mmObject.begin();
    for (; it != mmObject.end(); ++it)
    {
        NetObject* pNetObject = (NetObject*)it->second;
        if (pNetObject && !pNetObject->NeedRemove())
        {
            bufferevent* bev = pNetObject->GetBuffEvent();
            if (NULL != bev && mbWorking)
            {
                bufferevent_write(bev, msg, nLen);

                mnSendMsgTotal++;
            }
        }
    }

    return true;
}

bool NFCMulNet::SendMsg(const char* msg, const uint32_t nLen, const int nSockIndex)
{
    if (nLen <= 0)
    {
        return false;
    }

    SendmsgInfo xMsgInfo;

    xMsgInfo.nSockIndex = nSockIndex;
    xMsgInfo.strMsg = std::string(msg, nLen);
    mqSendMsg.Push(xMsgInfo);   

    return false;
}

bool NFCMulNet::SendMsg(const char* msg, const uint32_t nLen, const NFGUID& xClient)
{
    if (nLen <= 0)
    {
        return false;
    }
    SendmsgInfo xMsgInfo;

    xMsgInfo.xClientID = xClient;
    xMsgInfo.strMsg = std::string(msg, nLen);
    mqSendMsg.Push(xMsgInfo);


    return false;
}

bool NFCMulNet::SendMsg(const char* msg, const uint32_t nLen, const std::list<int>& fdList)
{
    std::list<int>::const_iterator it = fdList.begin();
    for (; it != fdList.end(); ++it)
    {
        SendMsg(msg, nLen, *it);
    }

    return true;
}

bool NFCMulNet::NetThreadSendMsg(const char* msg, const uint32_t nLen, const int nSockIndex)
{
    if (nLen <= 0)
    {
        return false;
    }

    std::map<int, NetObject*>::iterator it = mmObject.find(nSockIndex);
    if (it != mmObject.end())
    {
        NetObject* pNetObject = (NetObject*)it->second;
        if (pNetObject)
        {
            bufferevent* bev = pNetObject->GetBuffEvent();
            if (NULL != bev && mbWorking)
            {
                bufferevent_write(bev, msg, nLen);

                mnSendMsgTotal++;
                return true;
            }
        }
    }
    return true;
}

bool NFCMulNet::NetThreadSendMsg(const char* msg, const uint32_t nLen, const NFGUID& xClient)
{
    if (nLen <= 0)
    {
        return false;
    }


    for (std::map<int, NetObject*>::iterator it = mmObject.begin(); it != mmObject.end(); ++it)
    {
        NetObject* pNetObject = (NetObject*)it->second;
        if (pNetObject && pNetObject->GetClientID() == xClient)
        {
            bufferevent* bev = pNetObject->GetBuffEvent();
            if (NULL != bev && mbWorking)
            {
                bufferevent_write(bev, msg, nLen);

                mnSendMsgTotal++;
                return true;
            }
        }
    }

    return true;
}

bool NFCMulNet::CloseNetObject(const int nSockIndex)
{
    NetTaskInfo xMsg;
    xMsg.nTaskType = NetTaskInfo::NETTASKTYPE_DELETEOBJECT;
    xMsg.nSockIndex = nSockIndex;
    
    mqNetTaskInfo.Push(xMsg);

    return false;
}

bool NFCMulNet::CloseNetObject(const NFGUID& xClientID)
{
    NetTaskInfo xMsg;
    xMsg.nTaskType = NetTaskInfo::NETTASKTYPE_DELETEOBJECT;
    xMsg.xClientID = xClientID;

    mqNetTaskInfo.Push(xMsg);

    return false;
}

bool NFCMulNet::Dismantle(NetObject* pObject)
{
    bool bNeedDismantle = false;

    int len = pObject->GetBuffLen();
    if (len > NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        NFCMsgHead xHead;
        int nMsgBodyLength = DeCode(pObject->GetBuff(), len, xHead);
        if (nMsgBodyLength > 0 && xHead.GetMsgID() > 0)
        {
            RecivemsgInfo xMsg;

            xMsg.nRealSockIndex = pObject->GetRealFD();
            xMsg.nMsgID = xHead.GetMsgID();
            xMsg.strMsg = std::string(pObject->GetBuff() + NFIMsgHead::NF_Head::NF_HEAD_LENGTH, nMsgBodyLength);
            xMsg.xClientID = pObject->GetClientID();
            mqReciveMsg.Push(xMsg);
            mnReceiveMsgTotal++;

            pObject->RemoveBuff(0, nMsgBodyLength + NFIMsgHead::NF_Head::NF_HEAD_LENGTH);

            Dismantle(pObject);
        }
        else if (0 == nMsgBodyLength)
        {
            //���Ȳ���(�ȴ��´ν���)

            bNeedDismantle = false;
        }
        else
        {
            //�ۼƴ���̫����--�����ʵ����ո�����
            //pObject->IncreaseError();

            bNeedDismantle = false;

        }
    }

    return bNeedDismantle;
}

bool NFCMulNet::NetThreadAddNetObject(const int nSockIndex, NetObject* pObject)
{
    pObject->SetClientID(NFGUID(nSockIndex, mObjectIndex++));

    //lock
    return mmObject.insert(std::map<int, NetObject*>::value_type(nSockIndex, pObject)).second;
}

int NFCMulNet::InitClientNet()
{
    std::string strIP = mstrIP;
    int nPort = mnPort;

    struct sockaddr_in addr;
    struct bufferevent* bev = NULL;

#ifdef _MSC_VER
    WSADATA wsa_data;
    WSAStartup(0x0201, &wsa_data);
#endif

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(nPort);

    if (inet_pton(AF_INET, strIP.c_str(), &addr.sin_addr) <= 0)
    {
        printf("inet_pton");
        return -1;
    }

    base = event_base_new();
    if (base == NULL)
    {
        printf("event_base_new ");
        return -1;
    }

    bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
    if (bev == NULL)
    {
        printf("bufferevent_socket_new ");
        return -1;
    }

    int bRet = bufferevent_socket_connect(bev, (struct sockaddr*)&addr, sizeof(addr));
    if (0 != bRet)
    {
        //int nError = GetLastError();
        printf("bufferevent_socket_connect error");
        return -1;
    }

    int sockfd = bufferevent_getfd(bev);
    NetObject* pObject = new NetObject(this, sockfd, addr, bev);
    if (!NetThreadAddNetObject(0, pObject))
    {
        assert(0);
        return -1;
    }

    mbServer = false;

    bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, (void*)pObject);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    event_set_log_callback(&NFCMulNet::log_cb);
    //event_base_loop(base, EVLOOP_ONCE|EVLOOP_NONBLOCK);

    return sockfd;
}

int NFCMulNet::InitServerNet()
{
    int nMaxClient = mnMaxConnect;
    int nCpuCount = mnCpuCount;
    int nPort = mnPort;

    struct sockaddr_in sin;

#if NF_PLATFORM == NF_PLATFORM_WIN
//#ifdef _MSC_VER
    WSADATA wsa_data;
    WSAStartup(0x0201, &wsa_data);

#endif
    //////////////////////////////////////////////////////////////////////////

    struct event_config* cfg = event_config_new();

#if NF_PLATFORM == NF_PLATFORM_WIN
//#ifdef _MSC_VER

    //event_config_avoid_method(cfg, "iocp");
    //event_config_require_features(cfg, event_method_feature.EV_FEATURE_ET);//������ʽ
    //evthread_use_windows_threads();
    //if(event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP) < 0)
    //{
    //    //ʹ��IOCP
    //    return -1;
    //}

    //if(event_config_set_num_cpus_hint(cfg, nCpuCount) < 0)
    //{
    //    return -1;
    //}

    base = event_base_new_with_config(cfg);

#else

    //event_config_avoid_method(cfg, "epoll");
    if (event_config_set_flag(cfg, EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST) < 0)
    {
        //ʹ��EPOLL
        return -1;
    }

    if (event_config_set_num_cpus_hint(cfg, nCpuCount) < 0)
    {
        return -1;
    }

    base = event_base_new_with_config(cfg);//event_base_new()

#endif
    event_config_free(cfg);

    //////////////////////////////////////////////////////////////////////////

    if (!base)
    {
        fprintf(stderr, "Could not initialize libevent!\n");
        Final();

        return -1;
    }

    //��ʼ��ʱ��
    //gettime(base, &base->event_tv);

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(nPort);

    printf("server started with %d\n", nPort);

    listener = evconnlistener_new_bind(base, listener_cb, (void*)this,
                                       LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
                                       (struct sockaddr*)&sin,
                                       sizeof(sin));

    if (!listener)
    {
        fprintf(stderr, "Could not create a listener!\n");
        Final();

        return -1;
    }

    //     signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)this);
    //
    //     if (!signal_event || event_add(signal_event, NULL)<0)
    //     {
    //         fprintf(stderr, "Could not create/add a signal event!\n");
    //         Final();
    //         return -1;
    //     }

    mbServer = true;

    event_set_log_callback(&NFCMulNet::log_cb);

    return mnMaxConnect;
}

bool NFCMulNet::CloseSocketAll()
{
    std::map<int, NetObject*>::iterator it = mmObject.begin();
    for (it; it != mmObject.end(); ++it)
    {
        int nFD = it->first;
        mvRemoveObject.push_back(nFD);
    }

    ExecuteClose();

    mmObject.clear();

    return true;
}

NetObject* NFCMulNet::NetThreadGetNetObject(const int nSockIndex)
{
    std::map<int, NetObject*>::iterator it = mmObject.find(nSockIndex);
    if (it != mmObject.end())
    {
        return it->second;
    }

    return NULL;
}

void NFCMulNet::CloseObject(const int nSockIndex)
{
    std::map<int, NetObject*>::iterator it = mmObject.find(nSockIndex);
    if (it != mmObject.end())
    {
        NetObject* pObject = it->second;

        struct bufferevent* bev = pObject->GetBuffEvent();

        bufferevent_free(bev);

        mmObject.erase(it);

        delete pObject;
        pObject = NULL;
    }
}

void NFCMulNet::ExecuteClose()
{
    for (int i = 0; i < mvRemoveObject.size(); ++i)
    {
        int nSocketIndex = mvRemoveObject[i];
        NetThreadCloseNetObject(nSocketIndex);
    }

    mvRemoveObject.clear();
}

void NFCMulNet::log_cb(int severity, const char* msg)
{

}

bool NFCMulNet::IsServer()
{
    return mbServer;
}

bool NFCMulNet::Log(int severity, const char* msg)
{
    log_cb(severity, msg);
    return true;
}

bool NFCMulNet::SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const int nSockIndex /*= 0*/)
{
    std::string strOutData;
    int nAllLen = EnCode(nMsgID, msg, nLen, strOutData);
    if (nAllLen == nLen + NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        //�����ɹ�
        return SendMsg(strOutData.c_str(), strOutData.length(), nSockIndex);
    }

    return false;
}

bool NFCMulNet::SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    std::string strOutData;
    int nAllLen = EnCode(nMsgID, msg, nLen, strOutData);
    if (nAllLen == nLen + NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        //�����ɹ�
        return SendMsg(strOutData.c_str(), strOutData.length(), xClientID);
    }

    return false;
}

bool NFCMulNet::SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const std::list<int>& fdList)
{
    std::string strOutData;
    int nAllLen = EnCode(nMsgID, msg, nLen, strOutData);
    if (nAllLen == nLen + NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        //�����ɹ�
        return SendMsg(strOutData.c_str(), strOutData.length(), fdList);
    }

    return false;
}

bool NFCMulNet::SendMsgToAllClientWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen)
{
    std::string strOutData;
    int nAllLen = EnCode(nMsgID, msg, nLen, strOutData);
    if (nAllLen == nLen + NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        //�����ɹ�
        return SendMsgToAllClient(strOutData.c_str(), strOutData.length());
    }

    return false;
}

int NFCMulNet::EnCode(const uint16_t unMsgID, const char* strData, const uint32_t unDataLen, std::string& strOutData)
{
    NFCMsgHead xHead;
    xHead.SetMsgID(unMsgID);
    xHead.SetBodyLength(unDataLen);

    char szHead[NFIMsgHead::NF_Head::NF_HEAD_LENGTH] = { 0 };
    xHead.EnCode(szHead);

    strOutData.clear();
    strOutData.append(szHead, NFIMsgHead::NF_Head::NF_HEAD_LENGTH);
    strOutData.append(strData, unDataLen);

    return xHead.GetBodyLength() + NFIMsgHead::NF_Head::NF_HEAD_LENGTH;
}

int NFCMulNet::DeCode(const char* strData, const uint32_t unAllLen, NFCMsgHead& xHead)
{
    //����--unLenΪbuff�ܳ���,����ʱ���ö����Ƕ���
    if (unAllLen < NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        //���Ȳ���
        return -1;
    }

    if (NFIMsgHead::NF_Head::NF_HEAD_LENGTH != xHead.DeCode(strData))
    {
        //ȡ��ͷʧ��
        return -2;
    }

    if (xHead.GetBodyLength() > (unAllLen - NFIMsgHead::NF_Head::NF_HEAD_LENGTH))
    {
        //�ܳ��Ȳ���
        return -3;
    }

    //copy��ͷ+����
    //      strOutData.clear();
    //      strOutData.append(strData, xHead.GetMsgLength());

    //����ʹ�ù�����
    return xHead.GetBodyLength();
}
