// -------------------------------------------------------------------------
//    @FileName			:    AFCGameServerNet_ServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    AFCGameServerNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef AFC_GAMESERVER_SERVER_MODULE_H
#define AFC_GAMESERVER_SERVER_MODULE_H

//#include "GameServerPCH.h"
//#include "NW_Helper.h"
//  the cause of sock'libariy, thenfore "AFCNet.h" much be included first.
#include <memory>
#include "SDK/Proto/NFMsgDefine.h"
#include "SDK/Interface/AFINetModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFISceneProcessModule.h"
#include "SDK/Interface/AFIGameLogicModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFIUUIDModule.h"
#include "SDK/Interface/AFIGameServerToWorldModule.h"
#include "SDK/Interface/AFIGameServerNet_ServerModule.h"
#include "SDK/Interface/AFIGameServerNet_ServerModule.h"
////////////////////////////////////////////////////////////////////////////



class AFCGameServerNet_ServerModule
    : public AFIGameServerNet_ServerModule
{
public:
    AFCGameServerNet_ServerModule(AFIPluginManager* p)
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
	virtual AFINetModule* GetNetModule();

    virtual bool AddPlayerGateInfo(const AFGUID& nRoleID, const AFGUID& nClientID, const int nGateID);
    virtual bool RemovePlayerGateInfo(const AFGUID& nRoleID);
    virtual NF_SHARE_PTR<GateBaseInfo> GetPlayerGateInfo(const AFGUID& nRoleID);

    virtual NF_SHARE_PTR<GateServerInfo> GetGateServerInfo(const int nGateID);
    virtual NF_SHARE_PTR<GateServerInfo> GetGateServerInfoBySockIndex(const int nSockIndex);

    virtual int OnPropertyEnter(const AFIDataList& argVar, const AFGUID& self);
    virtual int OnRecordEnter(const AFIDataList& argVar, const AFGUID& self);

    virtual int OnObjectListEnter(const AFIDataList& self, const AFIDataList& argVar);
    virtual int OnObjectListLeave(const AFIDataList& self, const AFIDataList& argVar);

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
    int OnPropertyCommonEvent(const AFGUID& self, const std::string& strPropertyName, const AFIDataList::TData& oldVar, const AFIDataList::TData& newVar);
    int OnRecordCommonEvent(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFIDataList::TData& oldVar, const AFIDataList::TData& newVar);
    int OnClassCommonEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& var);

    int OnGroupEvent(const AFGUID& self, const std::string& strPropertyName, const AFIDataList::TData& oldVar, const AFIDataList::TData& newVar);
    int OnContainerEvent(const AFGUID& self, const std::string& strPropertyName, const AFIDataList::TData& oldVar, const AFIDataList::TData& newVar);

    int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& var);
    int OnSwapSceneResultEvent(const AFGUID& self, const int nEventID, const AFIDataList& var);

    int GetBroadCastObject(const AFGUID& self, const std::string& strPropertyName, const bool bTable, AFIDataList& valueObject);
    int GetBroadCastObject(const int nObjectContainerID, const int nGroupID, AFIDataList& valueObject);

private:
    //<角色id,角色网关基础信息>//其实可以在object系统中被代替
    AFMapEx<AFGUID, GateBaseInfo> mRoleBaseData;
    //gateid,data
    AFMapEx<int, GateServerInfo> mProxyMap;

    //////////////////////////////////////////////////////////////////////////
    AFIUUIDModule* m_pUUIDModule;
    AFIKernelModule* m_pKernelModule;
    AFIClassModule* m_pClassModule;
    AFILogModule* m_pLogModule;
    AFISceneProcessModule* m_pSceneProcessModule;
    AFIElementModule* m_pElementModule;
	AFINetModule* m_pNetModule;
    //////////////////////////////////////////////////////////////////////////
    AFIGameServerToWorldModule* m_pGameServerToWorldModule;
};
#endif
