// -------------------------------------------------------------------------
//    @FileName			:    NFCProxyServerNet_ClientModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    NFCProxyServerNet_ClientModule
//
//     @Changed         : NFCProxyConnectObject Add HeartBeat by hahaya 2013-11-1
// -------------------------------------------------------------------------

#ifndef NFC_PROXYSERVER_NETCLIENT_MODULE_H
#define NFC_PROXYSERVER_NETCLIENT_MODULE_H

#include <string>
#include "NFComm/NFMessageDefine/NFMsgDefine.h"
#include "SDK/Core/NFCHeartBeatManager.h"
#include "SDK/Interface/NFIProxyServerToWorldModule.h"
#include "SDK/Interface/NFIProxyServerNet_ServerModule.h"
#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFIProxyLogicModule.h"
#include "SDK/Interface/NFINetModule.h"
#include "SDK/Interface/NFIElementModule.h"
#include "SDK/Interface/NFILogModule.h"
#include "SDK/Interface/NFIClassModule.h"
#include "SDK/Interface/NFINetClientModule.hpp"
#include "SDK/Interface/NFIProxyServerToGameModule.h"

class NFCProxyServerToWorldModule : public NFIProxyServerToWorldModule
{
public:

    NFCProxyServerToWorldModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

    virtual void LogReceive(const char* str) {}
    virtual void LogSend(const char* str) {}

	virtual NFINetClientModule* GetClusterModule();
    virtual bool VerifyConnectData(const std::string& strAccount, const std::string& strKey);

protected:

    void OnSocketWSEvent(const int nSockIndex, const NF_NET_EVENT eEvent, const AFGUID& xClientID, const int nServerID);

    void Register(const int nServerID);

    void OnSelectServerResultProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnServerInfoProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    void LogServerInfo(const std::string& strServerInfo);

	void OnOtherMessage(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
private:
    struct ClientConnectData
    {
        ClientConnectData()
        {
            strAccount = "";
            strConnectKey = "";
        }

        std::string strAccount;
        std::string strConnectKey;
    };


    NFMapEx<std::string, ClientConnectData> mWantToConnectMap;

private:

    NFILogModule* m_pLogModule;
    NFIProxyLogicModule* m_pProxyLogicModule;
    NFIKernelModule* m_pKernelModule;
    NFIProxyServerNet_ServerModule* m_pProxyServerNet_ServerModule;
    NFIElementModule* m_pElementModule;
    NFIClassModule* m_pClassModule;
    NFIProxyServerToGameModule* m_pProxyServerToGameModule;
	NFINetClientModule* m_pNetClientModule;

};

#endif