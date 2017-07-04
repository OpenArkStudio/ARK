// -------------------------------------------------------------------------
//    @FileName			:    NFCGameServerNet_ServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    NFCGameServerNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef NFC_GAMESERVER_SERVER_MODULE_H
#define NFC_GAMESERVER_SERVER_MODULE_H

//#include "GameServerPCH.h"
//#include "NW_Helper.h"
//  the cause of sock'libariy, thenfore "NFCNet.h" much be included first.
#include <memory>
#include "NFComm/NFMessageDefine/NFMsgDefine.h"
#include "SDK/Interface/NFINetModule.h"
#include "SDK/Interface/NFILogModule.h"
#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFIClassModule.h"
#include "SDK/Interface/NFISceneProcessModule.h"
#include "SDK/Interface/NFIGameLogicModule.h"
#include "SDK/Interface/NFIElementModule.h"
#include "SDK/Interface/NFIPluginManager.h"
#include "SDK/Interface/NFIUUIDModule.h"
#include "SDK/Interface/NFIGameServerToWorldModule.h"
#include "SDK/Interface/NFIGameServerNet_ServerModule.h"
#include "SDK/Interface/NFIGameServerNet_ServerModule.h"
////////////////////////////////////////////////////////////////////////////



class NFCGameServerNet_ServerModule
    : public NFIGameServerNet_ServerModule
{
public:
    NFCGameServerNet_ServerModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

    virtual void LogReceive(const char* str) {}
    virtual void LogSend(const char* str) {}
    virtual void SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const AFGUID& self);
    virtual void SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const AFGUID& self);
	virtual NFINetModule* GetNetModule();

    virtual bool AddPlayerGateInfo(const AFGUID& nRoleID, const AFGUID& nClientID, const int nGateID);
    virtual bool RemovePlayerGateInfo(const AFGUID& nRoleID);
    virtual NF_SHARE_PTR<GateBaseInfo> GetPlayerGateInfo(const AFGUID& nRoleID);

    virtual NF_SHARE_PTR<GateServerInfo> GetGateServerInfo(const int nGateID);
    virtual NF_SHARE_PTR<GateServerInfo> GetGateServerInfoBySockIndex(const int nSockIndex);

    virtual int OnPropertyEnter(const AFDataList& argVar, const AFGUID& self);
    virtual int OnRecordEnter(const AFDataList& argVar, const AFGUID& self);

    virtual int OnObjectListEnter(const AFDataList& self, const AFDataList& argVar);
    virtual int OnObjectListLeave(const AFDataList& self, const AFDataList& argVar);

protected:
    void OnSocketPSEvent(const int nSockIndex, const NF_NET_EVENT eEvent, const AFGUID& xClientID, const int nServerID);
    void OnClientDisconnect(const int nSockIndex);
    void OnClientConnected(const int nSockIndex);

protected:
    void OnProxyServerRegisteredProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnProxyServerUnRegisteredProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnRefreshProxyServerInfoProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

protected:
    void OnReqiureRoleListProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnCreateRoleGameProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnDeleteRoleGameProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnClienEnterGameProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnClienLeaveGameProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnClienSwapSceneProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);


    ///////////WORLD_START///////////////////////////////////////////////////////////////
    void OnTransWorld(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
	
protected:
    //网络同步
    int OnPropertyCommonEvent(const AFGUID& self, const std::string& strPropertyName, const AFDataList::TData& oldVar, const AFDataList::TData& newVar);
    int OnRecordCommonEvent(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFDataList::TData& oldVar, const AFDataList::TData& newVar);
    int OnClassCommonEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFDataList& var);

    int OnGroupEvent(const AFGUID& self, const std::string& strPropertyName, const AFDataList::TData& oldVar, const AFDataList::TData& newVar);
    int OnContainerEvent(const AFGUID& self, const std::string& strPropertyName, const AFDataList::TData& oldVar, const AFDataList::TData& newVar);

    int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFDataList& var);
    int OnSwapSceneResultEvent(const AFGUID& self, const int nEventID, const AFDataList& var);

    int GetBroadCastObject(const AFGUID& self, const std::string& strPropertyName, const bool bTable, AFDataList& valueObject);
    int GetBroadCastObject(const int nObjectContainerID, const int nGroupID, AFDataList& valueObject);

private:
    //<角色id,角色网关基础信息>//其实可以在object系统中被代替
    NFMapEx<AFGUID, GateBaseInfo> mRoleBaseData;
    //gateid,data
    NFMapEx<int, GateServerInfo> mProxyMap;

    //////////////////////////////////////////////////////////////////////////
    NFIUUIDModule* m_pUUIDModule;
    NFIKernelModule* m_pKernelModule;
    NFIClassModule* m_pClassModule;
    NFILogModule* m_pLogModule;
    NFISceneProcessModule* m_pSceneProcessModule;
    NFIElementModule* m_pElementModule;
	NFINetModule* m_pNetModule;
    //////////////////////////////////////////////////////////////////////////
    NFIGameServerToWorldModule* m_pGameServerToWorldModule;
};
#endif
