// -------------------------------------------------------------------------
//    @FileName         :    AFCLoginNet_ServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCLoginNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef AFC_LOGINNET_SERVER_MODULE_H
#define AFC_LOGINNET_SERVER_MODULE_H

//  the cause of sock'libariy, thenfore "AFCNet.h" much be included first.
#include "SDK/Base/AFMap.h"
#include "SDK/Proto/AFMsgDefine.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFILoginNet_ServerModule.h"
#include "SDK/Interface/AFILoginLogicModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFINetServerModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFILoginToMasterModule.h"
#include "SDK/Interface/AFIGUIDModule.h"

#define NET_MSG_PROCESS(xNFMsg, msg) \
    AFGUID nPlayerID; \
    xNFMsg xMsg; \
    if (!ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID)) \
    { \
        return 0; \
    } \
    \
    AFIActorMessage xActorMsg; \
    xActorMsg.eType = AFIActorMessage::EACTOR_NET_MSG; \
    xActorMsg.nSubMsgID = nMsgID; \
    xMsg.SerializeToString(&xActorMsg.data); \
    pPluginManager->GetFramework().Send(xActorMsg, pPluginManager->GetAddress(), pPluginManager->GetAddress());

class AFCLoginNet_ServerModule
    : public AFILoginNet_ServerModule

{
public:
    AFCLoginNet_ServerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();


    virtual bool BeforeShut();
    virtual bool AfterInit();

    virtual void LogReceive(const char* str) {}
    virtual void LogSend(const char* str) {}

    virtual int OnSelectWorldResultsProcess(const int nWorldID, const AFGUID xSenderID, const int nLoginID, const std::string& strAccount, const std::string& strWorldIP, const int nWorldPort, const std::string& strKey);

protected:
    void OnSocketClientEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID);

protected:
    void OnClientDisconnect(const AFGUID& xClientID);
    void OnClientConnected(const AFGUID& xClientID);

    //登入
    void OnLoginProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //选择大世界
    void OnSelectWorldProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //申请查看世界列表
    void OnViewWorldProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    void OnHeartBeat(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnLogOut(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void InvalidMessage(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

protected:

protected:
    void SynWorldToClient(const AFGUID& xClientID);

    AFMapEx<AFGUID, SessionData> mmClientSessionData;

private:

    AFILoginToMasterModule* m_pLoginToMasterModule;
    AFIClassModule* m_pClassModule;
    AFIElementModule* m_pElementModule;
    AFIKernelModule* m_pKernelModule;
    AFILogModule* m_pLogModule;
    AFILoginLogicModule* m_pLoginLogicModule;
    AFIGUIDModule* m_pUUIDModule;
    AFINetServerModule* m_pNetModule;
};

#endif