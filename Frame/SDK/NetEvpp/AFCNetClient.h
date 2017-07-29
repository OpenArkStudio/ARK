// -------------------------------------------------------------------------
//    @FileName         ��    AFCNetClient.h
//    @Author           ��    Ark Game Tech
//    @Date             ��    2013-12-15
//    @Module           ��    AFIPacket
//    @Desc             :     CNet
// -------------------------------------------------------------------------

#ifndef AFC_NET_H
#define AFC_NET_H

#include "AFINet.h"
#include <evpp/exp.h>
#include "SDK/Core/AFQueue.h"
#include "common/readerwriterqueue.h"
#include "SDK/Core/AFLockFreeQueue.h"

#include <evpp/libevent_headers.h>
#include <evpp/libevent_watcher.h>
#include <evpp/event_loop.h>
#include <evpp/event_loop_thread.h>
#include <evpp/tcp_server.h>
#include <evpp/buffer.h>
#include <evpp/tcp_conn.h>
#include <evpp/tcp_client.h>
#include <thread>

#pragma pack(push, 1)


class AFCNetClient : public AFINet
{
public:
    AFCNetClient()
    {
        mstrIP = "";
        mnPort = 0;
        mnServerID = 0;
    }

    template<typename BaseType>
    AFCNetClient(BaseType* pBaseType, void (BaseType::*handleRecieve)(const int, const int, const char*, const uint32_t, const AFGUID&), void (BaseType::*handleEvent)(const int, const NF_NET_EVENT, const AFGUID&, const int))
    {
        base = NULL;
        mRecvCB = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        mEventCB = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        mstrIP = "";
        mnPort = 0;
        mnServerID = 0;
    }

    virtual ~AFCNetClient()
    {
        Final();
    };

public:
    virtual bool Execute();
    virtual void Initialization(const char* strIP, const unsigned short nPort, const int nServerID);
    virtual bool Final();
    virtual bool SendMsgWithOutHead(const int16_t nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID = 0);

    virtual bool CloseNetObject(const AFGUID& xClient);

    virtual bool IsServer();
    virtual bool Log(int severity, const char* msg);

public:
    static void OnClientConnection(const evpp::TCPConnPtr& conn, void* pData);
    void OnClientConnectionInner(const evpp::TCPConnPtr& conn);

    static void OnMessage(const evpp::TCPConnPtr& conn,
                          evpp::Buffer* msg, void* pData);
    void OnMessageInner(const evpp::TCPConnPtr& conn,
                        evpp::Buffer* msg);
private:
    bool SendMsg(const char* msg, const uint32_t nLen, const AFGUID& xClient = 0);

    bool AddNetObject(const AFGUID& xClientID, NetObject* pObject);
    bool RemoveNetObject(const AFGUID& xClientID = 0);
    NetObject* GetNetObject(const AFGUID& xClientID = 0);

    bool Dismantle(NetObject* pObject);
    void ProcessMsgLogicThread();
    bool CloseSocketAll();

    static void log_cb(int severity, const char* msg);

protected:
    int DeCode(const char* strData, const uint32_t unLen, AFCMsgHead& xHead);
    int EnCode(const uint16_t unMsgID, const char* strData, const uint32_t unDataLen, std::string& strOutData);

private:
    std::unique_ptr<evpp::EventLoopThread> m_pThread;
    std::unique_ptr<evpp::TCPClient> m_pClient;

    std::map<AFGUID, NetObject*> mmObject;

    std::string mstrIP;
    int mnPort;
    int mnServerID;
    NET_RECEIVE_FUNCTOR mRecvCB;
    NET_EVENT_FUNCTOR mEventCB;
    AFLockFreeQueue<MsgFromNetInfo*> mqMsgFromNet;

private:
    static uint64_t mObjectIndex;
};

#pragma pack(pop)

#endif
