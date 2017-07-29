// -------------------------------------------------------------------------
//    @FileName         ��    AFCNetServer.h
//    @Author           ��    Ark Game Tech
//    @Date             ��    2013-12-15
//    @Module           ��    AFCNetServer
//    @Desc             :     AFCNetServer
// -------------------------------------------------------------------------

#ifndef AFC_MULNET_H
#define AFC_MULNET_H

#include <thread>
#include <atomic>
#include "AFINet.h"
#include "SDK/Core/AFQueue.h"
#include "common/readerwriterqueue.h"
#include "SDK/Core/AFLockFreeQueue.h"

#include <evpp/exp.h>
#include <evpp/libevent_headers.h>
#include <evpp/libevent_watcher.h>
#include <evpp/event_loop.h>
#include <evpp/event_loop_thread.h>
#include <evpp/tcp_server.h>
#include <evpp/buffer.h>
#include <evpp/tcp_conn.h>
#include <evpp/tcp_client.h>

#pragma pack(push, 1)

class AFCNetServer : public AFINet
{
public:
    AFCNetServer()
    {
        mstrIP = "";
        mnPort = 0;
        mnCpuCount = 0;
    }


    template<typename BaseType>
    AFCNetServer(BaseType* pBaseType, void (BaseType::*handleRecieve)(const int, const int, const char*, const uint32_t, const AFGUID&), void (BaseType::*handleEvent)(const int, const NF_NET_EVENT, const AFGUID&, const int))
    {
        mRecvCB = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        mEventCB = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        mstrIP = "";
        mnPort = 0;
        mnCpuCount = 0;

        mnServerID = 0;

    }

    virtual ~AFCNetServer()
    {
        Final();
    };

public:
    virtual bool Execute();

    virtual int Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nServerID, const int nCpuCount);
    virtual bool Final();
    virtual bool IsServer()
    {
        return true;
    }

    virtual bool SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    virtual bool SendMsgToAllClientWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen);

    virtual bool CloseNetObject(const AFGUID& xClientID);
    virtual bool Log(int severity, const char* msg)
    {
        return true;
    };

public:
    //From Worker Thread
    static void OnMessage(const evpp::TCPConnPtr& conn,
                          evpp::Buffer* msg, void* pData);
    void OnMessageInner(const evpp::TCPConnPtr& conn,
                        evpp::Buffer* msg);

    //From ListenThread
    static void OnClientConnection(const evpp::TCPConnPtr& conn, void* pData);
    void OnClientConnectionInner(const evpp::TCPConnPtr& conn);

private:
    bool SendMsgToAllClient(const char* msg, const uint32_t nLen);
    bool SendMsg(const char* msg, const uint32_t nLen, const AFGUID& xClient);
    bool AddNetObject(const AFGUID& xClientID, NetObject* pObject);
    bool RemoveNetObject(const AFGUID& xClientID);
    NetObject* GetNetObject(const AFGUID& xClientID);

private:
    void ProcessMsgLogicThread();
    bool CloseSocketAll();
    bool Dismantle(NetObject* pObject);

protected:
    int DeCode(const char* strData, const uint32_t unLen, AFCMsgHead& xHead);
    int EnCode(const uint16_t unMsgID, const char* strData, const uint32_t unDataLen, std::string& strOutData);

private:
    std::unique_ptr<evpp::TCPServer> m_pTcpSrv;
    std::unique_ptr<evpp::EventLoopThread> m_pListenThread;
    std::map<AFGUID, NetObject*> mmObject;

    int mnMaxConnect;
    std::string mstrIP;
    int mnPort;
    int mnCpuCount;
    int mnServerID;

    NET_RECEIVE_FUNCTOR mRecvCB;
    NET_EVENT_FUNCTOR mEventCB;

    AFLockFreeQueue<MsgFromNetInfo*> mqMsgFromNet;
    static atomic_uint64_t mObjectIndex;

};

#pragma pack(pop)

#endif
