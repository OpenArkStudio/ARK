// -------------------------------------------------------------------------
//    @FileName			:    AFCProxyServerNet_ClientModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    AFCProxyServerNet_ClientModule
//
//     @Changed         : AFCProxyConnectObject Add HeartBeat by hahaya 2013-11-1
// -------------------------------------------------------------------------

#ifndef AFC_PROXYSERVER_NETCLIENT_MODULE_H
#define AFC_PROXYSERVER_NETCLIENT_MODULE_H

#include <string>
#include "SDK/Proto/NFMsgDefine.h"
#include "SDK/Core/AFCHeartBeatManager.h"
#include "SDK/Interface/AFIProxyServerToWorldModule.h"
#include "SDK/Interface/AFIProxyServerNet_ServerModule.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIProxyLogicModule.h"
#include "SDK/Interface/AFINetModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFINetClientModule.hpp"
#include "SDK/Interface/AFIProxyServerToGameModule.h"

class AFCProxyServerToWorldModule : public AFIProxyServerToWorldModule
{
public:

    AFCProxyServerToWorldModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

    virtual void LogReceive(const char* str) {}
    virtual void LogSend(const char* str) {}

	virtual AFINetClientModule* GetClusterModule();
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


    AFMapEx<std::string, ClientConnectData> mWantToConnectMap;

private:

    AFILogModule* m_pLogModule;
    AFIProxyLogicModule* m_pProxyLogicModule;
    AFIKernelModule* m_pKernelModule;
    AFIProxyServerNet_ServerModule* m_pProxyServerNet_ServerModule;
    AFIElementModule* m_pElementModule;
    AFIClassModule* m_pClassModule;
    AFIProxyServerToGameModule* m_pProxyServerToGameModule;
	AFINetClientModule* m_pNetClientModule;

};

#endif