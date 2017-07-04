// -------------------------------------------------------------------------
//    @FileName			:    NFCLoginNet_ServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    NFCLoginNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef NFC_WORLDNET_CLIENT_MODULE_H
#define NFC_WORLDNET_CLIENT_MODULE_H

//  the cause of sock'libariy, thenfore "NFCNet.h" much be included first.

#include "NFComm/NFMessageDefine/NFMsgDefine.h"
#include "SDK/Interface/NFIWorldToMasterModule.h"
#include "SDK/Interface/NFIWorldLogicModule.h"
#include "SDK/Interface/NFINetModule.h"
#include "SDK/Interface/NFIClassModule.h"
#include "SDK/Interface/NFIElementModule.h"
#include "SDK/Interface/NFILogModule.h"
#include "SDK/Interface/NFIWorldNet_ServerModule.h"
#include "SDK/Interface/NFINetClientModule.hpp"

class NFCWorldToMasterModule
    : public NFIWorldToMasterModule
{
public:
    NFCWorldToMasterModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool BeforeShut();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

protected:

    void OnSocketMSEvent(const int nSockIndex, const NF_NET_EVENT eEvent, const AFGUID& xClientID, const int nServerID);

    //连接丢失,删2层(连接对象，帐号对象)
    void OnClientDisconnect(const int nAddress);
    //有连接
    void OnClientConnected(const int nAddress);

    virtual void LogServerInfo(const std::string& strServerInfo);


    void Register(const int nServerID);
    void RefreshWorldInfo();

    void OnSelectServerProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnKickClientProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

	void InvalidMessage(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
private:

    NFILogModule* m_pLogModule;
    NFIElementModule* m_pElementModule;
    NFIClassModule* m_pClassModule;
    NFIWorldLogicModule* m_pWorldLogicModule;
    NFIWorldNet_ServerModule* m_pWorldNet_ServerModule;
	NFINetClientModule* m_pNetClientModule;
};

#endif