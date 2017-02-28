// -------------------------------------------------------------------------
//    @FileName			:    NFCLoginNet_ServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    NFCLoginNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef NFC_LOGINNET_SERVER_MODULE_H
#define NFC_LOGINNET_SERVER_MODULE_H

//  the cause of sock'libariy, thenfore "NFCNet.h" much be included first.
#include "NFComm/NFCore/NFMap.h"
#include "NFComm/NFMessageDefine/NFMsgDefine.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFILoginNet_ServerModule.h"
#include "NFComm/NFPluginModule/NFILoginLogicModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFPluginModule/NFINetModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILoginToMasterModule.h"
#include "NFComm/NFPluginModule/NFIUUIDModule.h"

#define NET_MSG_PROCESS(xNFMsg, msg) \
    NFGUID nPlayerID; \
    xNFMsg xMsg; \
    if (!ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID)) \
    { \
        return 0; \
    } \
    \
    NFIActorMessage xActorMsg; \
    xActorMsg.eType = NFIActorMessage::EACTOR_NET_MSG; \
    xActorMsg.nSubMsgID = nMsgID; \
    xMsg.SerializeToString(&xActorMsg.data); \
    pPluginManager->GetFramework().Send(xActorMsg, pPluginManager->GetAddress(), pPluginManager->GetAddress());

class NFCLoginNet_ServerModule
    : public NFILoginNet_ServerModule

{
public:
    NFCLoginNet_ServerModule(NFIPluginManager* p)
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

    virtual int OnSelectWorldResultsProcess(const int nWorldID, const NFGUID xSenderID, const int nLoginID, const std::string& strAccount, const std::string& strWorldIP, const int nWorldPort, const std::string& strKey);

protected:
    void OnSocketClientEvent(const int nSockIndex, const NF_NET_EVENT eEvent, const NFGUID& xClientID, const int nServerID);

protected:
    void OnClientDisconnect(const int nAddress, const NFGUID& xClientID);
    void OnClientConnected(const int nAddress, const NFGUID& xClientID);

    //登入
    void OnLoginProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);

    //选择大世界
    void OnSelectWorldProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);

    //申请查看世界列表
    void OnViewWorldProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);

	void OnHeartBeat(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);
	void OnLogOut(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);
	void InvalidMessage(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID);

protected:

protected:
    void SynWorldToClient(const int nFD);

    NFMapEx<NFGUID, SessionData> mmClientSessionData;

private:

    NFILoginToMasterModule* m_pLoginToMasterModule;
    NFIClassModule* m_pClassModule;
    NFIElementModule* m_pElementModule;
    NFIKernelModule* m_pKernelModule;
    NFILogModule* m_pLogModule;
    NFILoginLogicModule* m_pLoginLogicModule;
    NFIUUIDModule* m_pUUIDModule;
	NFINetModule* m_pNetModule;
};

#endif