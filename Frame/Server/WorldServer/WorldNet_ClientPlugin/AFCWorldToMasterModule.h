// -------------------------------------------------------------------------
//    @FileName         :    AFCLoginNet_ServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCLoginNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef AFC_WORLDNET_CLIENT_MODULE_H
#define AFC_WORLDNET_CLIENT_MODULE_H

//  the cause of sock'libariy, thenfore "AFCNet.h" much be included first.

#include "SDK/Proto/AFMsgDefine.h"
#include "SDK/Interface/AFIWorldToMasterModule.h"
#include "SDK/Interface/AFIWorldLogicModule.h"
#include "SDK/Interface/AFINetServerModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIWorldNet_ServerModule.h"
#include "SDK/Interface/AFINetClientModule.hpp"

class AFCWorldToMasterModule
    : public AFIWorldToMasterModule
{
public:
    AFCWorldToMasterModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool BeforeShut();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

protected:

    void OnSocketMSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID);

    //连接丢失,删2层(连接对象，帐号对象)
    void OnClientDisconnect(const AFGUID& xClientID);
    //有连接
    void OnClientConnected(const AFGUID& xClientID);

    virtual void LogServerInfo(const std::string& strServerInfo);


    void Register(const int nServerID);
    void RefreshWorldInfo();

    void OnSelectServerProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnKickClientProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    void InvalidMessage(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
private:

    AFILogModule* m_pLogModule;
    AFIElementModule* m_pElementModule;
    AFIClassModule* m_pClassModule;
    AFIWorldLogicModule* m_pWorldLogicModule;
    AFIWorldNet_ServerModule* m_pWorldNet_ServerModule;
    AFINetClientModule* m_pNetClientModule;
};

#endif