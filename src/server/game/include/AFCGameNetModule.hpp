/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once

#include "proto/AFProtoCPP.hpp"
#include "log/interface/AFILogModule.hpp"
#include "utility/interface/AFIGUIDModule.hpp"
#include "kernel/interface/AFIKernelModule.hpp"
#include "kernel/interface/AFIMetaClassModule.hpp"
#include "kernel/interface/AFIConfigModule.hpp"
#include "kernel/interface/AFIMapModule.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "bus/interface/AFIMsgModule.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "game/interface/AFISceneProcessModule.hpp"
#include "game/interface/AFIAccountModule.hpp"
#include "game/interface/AFIGameNetModule.hpp"

namespace ark {

class AFCGameNetModule : public AFIGameNetModule
{
public:
    bool Init() override;
    bool PostInit() override;
    bool PreUpdate() override;

    virtual void SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const AFGUID& self);
    virtual void SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const AFGUID& self);
    virtual AFINetServerService* GetNetServerService();

    virtual bool AddPlayerGateInfo(const AFGUID& nRoleID, const AFGUID& nClientID, const int nGateID);
    virtual bool RemovePlayerGateInfo(const AFGUID& nRoleID);
    virtual ARK_SHARE_PTR<GateBaseInfo> GetPlayerGateInfo(const AFGUID& nRoleID);

    virtual ARK_SHARE_PTR<GateServerInfo> GetGateServerInfo(const int nGateID);
    virtual ARK_SHARE_PTR<GateServerInfo> GetGateServerInfoByClientID(const AFGUID& nClientID);

    virtual int OnViewDataNodeEnter(const AFIDataList& argVar, const AFGUID& self);
    virtual int OnSelfDataNodeEnter(const AFGUID& self);
    virtual int OnViewDataTableEnter(const AFIDataList& argVar, const AFGUID& self);
    virtual int OnSelfDataTableEnter(const AFGUID& self);

    virtual int OnEntityListEnter(const AFIDataList& self, const AFIDataList& argVar);
    virtual int OnEntityListLeave(const AFIDataList& self, const AFIDataList& argVar);

    // void OnSocketEvent(const AFNetEvent* event);
    // void OnClientDisconnect(const AFGUID& xClientID);
    // void OnClientConnected(const AFGUID& xClientID);

    void OnProxyServerRegisteredProcess(const AFNetMsg* msg, const int64_t session_id);
    void OnProxyServerUnRegisteredProcess(const AFNetMsg* msg, const int64_t session_id);
    void OnRefreshProxyServerInfoProcess(const AFNetMsg* msg, const int64_t session_id);
    void OnReqiureRoleListProcess(const AFNetMsg* msg, const int64_t session_id);
    void OnCreateRoleGameProcess(const AFNetMsg* msg, const int64_t session_id);
    void OnDeleteRoleGameProcess(const AFNetMsg* msg, const int64_t session_id);

    void OnClienEnterGameProcess(const AFNetMsg* msg, const int64_t session_id);
    void OnClientLeaveGameProcess(const AFNetMsg* msg, const int64_t session_id);
    void OnClienSwapSceneProcess(const AFNetMsg* msg, const int64_t session_id);

    ///////////WORLD_START///////////////////////////////////////////////////////////////
    void OnTransWorld(const AFNetMsg* msg);

    // Common Node & table & class event
    int OnCommonDataNodeEvent(
        const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar);
    int OnCommonDataTableEvent(
        const AFGUID& self, const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar);
    int OnCommonClassEvent(
        const AFGUID& self, const std::string& strClassName, const ArkEntityEvent eClassEvent, const AFIDataList& var);

    int OnMapInstanceEvent(
        const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar);
    int OnContainerEvent(
        const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar);

    int OnEntityEvent(
        const AFGUID& self, const std::string& strClassName, const ArkEntityEvent eClassEvent, const AFIDataList& var);
    int OnSwapSceneResultEvent(const AFGUID& self, const int nEventID, const AFIDataList& var);

    int GetNodeBroadcastEntityList(const AFGUID& self, const std::string& strPropertyName, AFIDataList& valueObject);
    int GetTableBroadcastEntityList(const AFGUID& self, const std::string& strPropertyName, AFIDataList& valueObject);
    int GetBroadcastEntityList(const int nObjectContainerID, const int nGroupID, AFIDataList& valueObject);

    bool ProcessLeaveGroup(const AFGUID& self, int nSceneID, int nOldGroupID);
    bool ProcessEnterGroup(const AFGUID& self, int nSceneID, int nNewGroupID);

protected:
    int StartServer();
    int StartClient();

    void CommonDataTableAddEvent(
        const AFGUID& self, const std::string& strTableName, int nRow, int nCol, const AFCDataList& valueBroadCaseList);
    void CommonDataTableDeleteEvent(
        const AFGUID& self, const std::string& strTableName, int nRow, const AFCDataList& valueBroadCaseList);
    void CommonDataTableSwapEvent(const AFGUID& self, const std::string& strTableName, int nRow, int target_row,
        const AFCDataList& valueBroadCaseList);
    void CommonDataTableUpdateEvent(const AFGUID& self, const std::string& strTableName, int nRow, int nCol,
        const AFIData& newVar, const AFCDataList& valueBroadCaseList);

    int CommonClassDestoryEvent(const AFGUID& self);

private:
    //<角色id,角色网关基础信息>//其实可以在object系统中被代替
    AFMapEx<AFGUID, GateBaseInfo> mRoleBaseData;
    // gate id,data
    AFMapEx<int, GateServerInfo> mProxyMap;
    //////////////////////////////////////////////////////////////////////////
    AFIGUIDModule* m_pUUIDModule;
    AFIKernelModule* m_pKernelModule;
    AFIMetaClassModule* m_pClassModule;
    AFILogModule* m_pLogModule;
    AFIConfigModule* m_pConfigModule;
    AFIBusModule* m_pBusModule;
    AFINetServiceManagerModule* m_pNetServiceManagerModule;
    AFIMapModule* m_pMapModule;
    //////////////////////////////////////////////////////////////////////////
    AFISceneProcessModule* m_pSceneProcessModule;
    AFIAccountModule* m_AccountModule;

    AFINetServerService* m_pNetServerService;
    AFIMsgModule* m_pMsgModule;
};

} // namespace ark
