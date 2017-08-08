// -------------------------------------------------------------------------
//    @FileName         :    AFCGameServerNet_ClientModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCGameServerNet_ClientModule
//    @Desc             :
// -------------------------------------------------------------------------

#pragma once

#include "SDK/Proto/NFMsgDefine.h"
#include "SDK/Interface/AFINetClientModule.hpp"
#include "SDK/Interface/AFIGameServerNet_ClientModule.h"
#include "SDK/Interface/AFIGameServerNet_ServerModule.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIGameLogicModule.h"

#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIGameServerToWorldModule.h"

class AFCGameServerToWorldModule : public AFIGameServerToWorldModule
{
public:
    AFCGameServerToWorldModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

    virtual void SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen);
    virtual AFINetClientModule* GetClusterClientModule();

protected:
    void OnSocketWSEvent(const NetEventType e, const AFGUID& xClientID, const int nServerID);

protected:
    void Register(const int nSeverID);
    void RefreshWorldInfo();
    void TransPBToProxy(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& var);

private:
    void SendOnline(const AFGUID& self);
    void SendOffline(const AFGUID& self);

private:
    //AFILogModule* m_pLogModule;
    AFIKernelModule* m_pKernelModule;
    AFIClassModule* m_pClassModule;
    AFIElementModule* m_pElementModule;
    AFINetClientModule* m_pNetClientModule;
    AFIGameServerNet_ServerModule* m_pGameServerNet_ServerModule;
};