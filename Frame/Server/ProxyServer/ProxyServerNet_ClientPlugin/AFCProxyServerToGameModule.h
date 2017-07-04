// -------------------------------------------------------------------------
//    @FileName			:    NFCProxyServerToGameModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    NFCProxyServerToGameModule
//
//     @Changed         : NFCProxyConnectObject Add HeartBeat by hahaya 2013-11-1
// -------------------------------------------------------------------------

#ifndef NFC_PROXYSERVER_TO_GAME_MODULE_H
#define NFC_PROXYSERVER_TO_GAME_MODULE_H

#include <string>
#include "NFComm/NFMessageDefine/NFMsgDefine.h"
#include "SDK/Core/NFCHeartBeatManager.h"
#include "SDK/Interface/NFIProxyServerToGameModule.h"
#include "SDK/Interface/NFIProxyServerNet_ServerModule.h"
#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFIProxyLogicModule.h"
#include "SDK/Interface/NFINetModule.h"
#include "SDK/Interface/NFIElementModule.h"
#include "SDK/Interface/NFILogModule.h"
#include "SDK/Interface/NFIClassModule.h"
#include "SDK/Interface/NFINetClientModule.hpp"


class NFCProxyServerToGameModule : public NFIProxyServerToGameModule
{
public:

    NFCProxyServerToGameModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();
	virtual NFINetClientModule* GetClusterModule();

protected:

    void OnSocketGSEvent(const int nSockIndex, const NF_NET_EVENT eEvent, const AFGUID& xClientID, const int nServerID);

    void Register(const int nServerID);

    void OnAckEnterGame(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void LogServerInfo(const std::string& strServerInfo);

	void Transpond(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

private:

    NFILogModule* m_pLogModule;
    NFIProxyLogicModule* m_pProxyLogicModule;
    NFIKernelModule* m_pKernelModule;
    NFIProxyServerNet_ServerModule* m_pProxyServerNet_ServerModule;
    NFIElementModule* m_pElementModule;
    NFIClassModule* m_pClassModule;
	NFINetClientModule* m_pNetClientModule;
};

#endif