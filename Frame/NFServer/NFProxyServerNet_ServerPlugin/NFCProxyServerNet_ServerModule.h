// -------------------------------------------------------------------------
//    @FileName			:    NFCGameServerNet_ServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    NFCGameServerNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef NFC_PROXYSERVER_SERVER_MODULE_H
#define NFC_PROXYSERVER_SERVER_MODULE_H

//  the cause of sock'libariy, thenfore "NFCNet.h" much be included first.
#include "NFComm/NFCore/NFCConsistentHash.hpp"
#include "NFComm/NFMessageDefine/NFMsgDefine.h"
#include "NFComm/NFPluginModule/NFIProxyServerNet_ServerModule.h"
#include "NFComm/NFPluginModule/NFIProxyServerToWorldModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFPluginModule/NFINetModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFIUUIDModule.h"
#include "NFComm/NFPluginModule/NFIProxyServerToGameModule.h"
#include "NFComm/NFPluginModule/NFINetClientModule.hpp"

class NFCProxyServerNet_ServerModule : public NFIProxyServerNet_ServerModule
{
public:
    NFCProxyServerNet_ServerModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

    virtual int Transpond(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);

    //进入游戏成功
    virtual int EnterGameSuccessEvent(const NFGUID xClientID, const NFGUID xPlayerID);

protected:

    void OnSocketClientEvent(const int nSockIndex, const NF_NET_EVENT eEvent, const NFGUID& xClientID, const int nSeverID);

    //连接丢失,删2层(连接对象，帐号对象)
    void OnClientDisconnect(const int nAddress, const NFGUID& xClientID);
    //有连接
    void OnClientConnected(const int nAddress, const NFGUID& xClientID);

    void OnConnectKeyProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);
    void OnReqServerListProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);
    void OnSelectServerProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);
    void OnReqRoleListProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);
    void OnReqCreateRoleProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);
    void OnReqDelRoleProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);
    void OnReqEnterGameServer(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);


    //客户端的连接60秒删掉
    int HB_OnConnectCheckTime(const NFGUID& self, const std::string& strHeartBeat, const float fTime, const int nCount, const AFDataList& var);
    //////////////////////////////////////////////////////////////////////////

	void OnOtherMessage(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);
protected:

    NFMapEx<NFGUID, SessionData> mmSessionData;
    NFCConsistentHash mxConsistentHash;
protected:
    NFIProxyServerToWorldModule* m_pProxyToWorldModule;
    NFIProxyServerToGameModule* m_pProxyServerToGameModule;
    NFIKernelModule* m_pKernelModule;
    NFILogModule* m_pLogModule;
    NFIElementModule* m_pElementModule;
    NFIClassModule* m_pClassModule;
    NFIUUIDModule* m_pUUIDModule;
	NFINetModule* m_pNetModule;

};

#endif