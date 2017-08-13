// -------------------------------------------------------------------------
//    @FileName         :    AFCGameServerNet_ServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCGameServerNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef AFC_PROXYSERVER_SERVER_MODULE_H
#define AFC_PROXYSERVER_SERVER_MODULE_H

#include "SDK/Base/AFCConsistentHash.hpp"
#include "SDK/Proto/NFMsgDefine.h"
#include "SDK/Interface/AFIProxyServerNet_ServerModule.h"
#include "SDK/Interface/AFIProxyServerToWorldModule.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFINetServerModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIGUIDModule.h"
#include "SDK/Interface/AFIProxyServerToGameModule.h"
#include "SDK/Interface/AFINetClientModule.hpp"

class AFCProxyServerNet_ServerModule : public AFIProxyServerNet_ServerModule
{
public:
    AFCProxyServerNet_ServerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

    virtual int Transpond(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //进入游戏成功
    virtual int EnterGameSuccessEvent(const AFGUID xClientID, const AFGUID xPlayerID);

protected:

    void OnSocketClientEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nSeverID);

    //连接丢失,删2层(连接对象，帐号对象)
    void OnClientDisconnect(const AFGUID& xClientID);
    //有连接
    void OnClientConnected(const AFGUID& xClientID);

    void OnConnectKeyProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnReqServerListProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnSelectServerProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnReqRoleListProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnReqCreateRoleProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnReqDelRoleProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnReqEnterGameServer(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);


    //客户端的连接60秒删掉
    int HB_OnConnectCheckTime(const AFGUID& self, const std::string& strHeartBeat, const float fTime, const int nCount, const AFIDataList& var);
    //////////////////////////////////////////////////////////////////////////

    void OnOtherMessage(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
protected:

    AFMapEx<AFGUID, SessionData> mmSessionData;
    AFCConsistentHash mxConsistentHash;
protected:
    AFIProxyServerToWorldModule* m_pProxyToWorldModule;
    AFIProxyServerToGameModule* m_pProxyServerToGameModule;
    AFIKernelModule* m_pKernelModule;
    AFILogModule* m_pLogModule;
    AFIElementModule* m_pElementModule;
    AFIClassModule* m_pClassModule;
    AFIGUIDModule* m_pUUIDModule;
    AFINetServerModule* m_pNetModule;

};

#endif