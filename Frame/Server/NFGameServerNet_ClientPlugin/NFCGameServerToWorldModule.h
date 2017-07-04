// -------------------------------------------------------------------------
//    @FileName         :    NFCGameServerNet_ClientModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    NFCGameServerNet_ClientModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef NFC_GAMESERVER_NETCLIENT_MODULE_H
#define NFC_GAMESERVER_NETCLIENT_MODULE_H

//  the cause of sock'libariy, thenfore "NFCNet.h" much be included first.
#include "NFComm/NFMessageDefine/NFMsgDefine.h"
#include "SDK/Interface/NFINetModule.h"
#include "SDK/Interface/NFINetClientModule.hpp"
#include "SDK/Interface/NFIGameServerNet_ClientModule.h"
#include "SDK/Interface/NFIGameServerNet_ServerModule.h"
#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFIGameLogicModule.h"
#include "SDK/Interface/NFINetModule.h"
#include "SDK/Interface/NFIClassModule.h"
#include "SDK/Interface/NFIElementModule.h"
#include "SDK/Interface/NFILogModule.h"
#include "SDK/Interface/NFIGameServerToWorldModule.h"

class NFCGameServerToWorldModule : public NFIGameServerToWorldModule
{
public:
    NFCGameServerToWorldModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

    virtual void SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen);
    virtual NFINetClientModule* GetClusterClientModule();

protected:
    void OnSocketWSEvent(const int nSockIndex, const NF_NET_EVENT eEvent, const AFGUID& xClientID, const int nServerID);

protected:
    void Register(const int nSeverID);
    void RefreshWorldInfo();
    void TransPBToProxy(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFDataList& var);

private:
    void SendOnline(const AFGUID& self);
    void SendOffline(const AFGUID& self);

private:
    NFILogModule* m_pLogModule;
    NFIKernelModule* m_pKernelModule;
    NFIClassModule* m_pClassModule;
    NFIElementModule* m_pElementModule;
    NFINetClientModule* m_pNetClientModule;
    NFIGameServerNet_ServerModule* m_pGameServerNet_ServerModule;
};

#endif