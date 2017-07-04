// -------------------------------------------------------------------------
//    @FileName			:    NFCLoginNet_ServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    NFCLoginNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef NFC_LOGINNET_CLIENT_MODULE_H
#define NFC_LOGINNET_CLIENT_MODULE_H

//  the cause of sock'libariy, thenfore "NFCNet.h" much be included first.

#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFILoginLogicModule.h"
#include "SDK/Interface/NFILogModule.h"
#include "SDK/Interface/NFIClassModule.h"
#include "SDK/Interface/NFIElementModule.h"
#include "SDK/Interface/NFINetModule.h"
#include "SDK/Interface/NFILoginNet_ServerModule.h"
#include "SDK/Interface/NFILoginToMasterModule.h"

class NFCLoginToMasterModule
    : public NFILoginToMasterModule
{
public:
    NFCLoginToMasterModule(NFIPluginManager* p)
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

	virtual NFINetClientModule* GetClusterModule();
    virtual NFMapEx<int, NFMsg::ServerInfoReport>& GetWorldMap();

protected:
    void OnSocketMSEvent(const int nSockIndex, const NF_NET_EVENT eEvent, const AFGUID& xClientID, const int nServerID);

protected:

    //////////////////////////////////////////////////////////////////////////
	void OnSelectServerResultProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
	void OnWorldInfoProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //////////////////////////////////////////////////////////////////////////
    void Register(const int nServerID);

private:
    NFMapEx<int, NFMsg::ServerInfoReport> mWorldMap;

    NFILoginLogicModule* m_pLoginLogicModule;
    NFILoginNet_ServerModule* m_pLoginNet_ServerModule;
    NFIElementModule* m_pElementModule;
    NFIKernelModule* m_pKernelModule;
    NFIClassModule* m_pClassModule;
    NFILogModule* m_pLogModule;
	NFINetClientModule* m_pNetClientModule;
};

#endif