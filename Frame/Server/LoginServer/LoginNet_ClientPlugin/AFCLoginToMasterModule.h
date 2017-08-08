// -------------------------------------------------------------------------
//    @FileName         :    AFCLoginNet_ServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCLoginNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef AFC_LOGINNET_CLIENT_MODULE_H
#define AFC_LOGINNET_CLIENT_MODULE_H

//  the cause of sock'libariy, thenfore "AFCNet.h" much be included first.

#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFILoginLogicModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFINetServerModule.h"
#include "SDK/Interface/AFILoginNet_ServerModule.h"
#include "SDK/Interface/AFILoginToMasterModule.h"

class AFCLoginToMasterModule
    : public AFILoginToMasterModule
{
public:
    AFCLoginToMasterModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }


    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();
    virtual bool BeforeShut();

    virtual void LogReceive(const char* str) {}
    virtual void LogSend(const char* str) {}

    virtual AFINetClientModule* GetClusterModule();
    virtual AFMapEx<int, NFMsg::ServerInfoReport>& GetWorldMap();

protected:
    void OnSocketMSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID);

protected:

    //////////////////////////////////////////////////////////////////////////
    void OnSelectServerResultProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnWorldInfoProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //////////////////////////////////////////////////////////////////////////
    void Register(const int nServerID);

private:
    AFMapEx<int, NFMsg::ServerInfoReport> mWorldMap;

    AFILoginLogicModule* m_pLoginLogicModule;
    AFILoginNet_ServerModule* m_pLoginNet_ServerModule;
    AFIElementModule* m_pElementModule;
    AFIKernelModule* m_pKernelModule;
    AFIClassModule* m_pClassModule;
    AFILogModule* m_pLogModule;
    AFINetClientModule* m_pNetClientModule;
};

#endif