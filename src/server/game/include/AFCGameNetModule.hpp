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

#include "base/AFPluginManager.hpp"
#include "proto/AFProtoCPP.hpp"
#include "log/interface/AFILogModule.hpp"
#include "utility/interface/AFIGUIDModule.hpp"
#include "kernel/interface/AFIKernelModule.hpp"
#include "kernel/interface/AFIMapModule.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "bus/interface/AFIMsgModule.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "game/interface/AFISceneProcessModule.hpp"
#include "game/interface/AFIAccountModule.hpp"
#include "game/interface/AFIGameNetModule.hpp"

namespace ark {

class AFCGameNetModule final : public AFIGameNetModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool PostInit() override;
    //bool PreUpdate() override;

    virtual void SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const AFGUID& self);
    virtual void SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const AFGUID& self);
    virtual std::shared_ptr<AFINetServerService> GetNetServerService();

    virtual bool AddPlayerGateInfo(const AFGUID& nRoleID, const AFGUID& nClientID, const int nGateID);
    virtual bool RemovePlayerGateInfo(const AFGUID& nRoleID);
    virtual std::shared_ptr<GateBaseInfo> GetPlayerGateInfo(const AFGUID& nRoleID);

    virtual std::shared_ptr<GateServerInfo> GetGateServerInfo(const int nGateID);
    virtual std::shared_ptr<GateServerInfo> GetGateServerInfoByClientID(const AFGUID& nClientID);

    virtual int OnViewDataNodeEnter(const AFIDataList& argVar, const AFGUID& self);
    virtual int OnSelfDataNodeEnter(const AFGUID& self);
    virtual int OnViewDataTableEnter(const AFIDataList& argVar, const AFGUID& self);
    virtual int OnSelfDataTableEnter(const AFGUID& self);

    virtual int OnEntityListEnter(const AFIDataList& self, const AFIDataList& argVar);
    virtual int OnEntityListLeave(const AFIDataList& self, const AFIDataList& argVar);

    // void OnSocketEvent(const AFNetEvent* event);
    // void OnClientDisconnect(const AFGUID& xClientID);
    // void OnClientConnected(const AFGUID& xClientID);

    void OnRegisteredProxy(const AFNetMsg* msg, const int64_t session_id);
    void OnUnregisteredProxy(const AFNetMsg* msg, const int64_t session_id);
    void OnRefreshProxy(const AFNetMsg* msg, const int64_t session_id);
    void OnReqiureRoleList(const AFNetMsg* msg, const int64_t session_id);
    void OnCreateRole(const AFNetMsg* msg, const int64_t session_id);
    void OnDeleteRole(const AFNetMsg* msg, const int64_t session_id);
    void OnRecoverRole(const AFNetMsg* msg, const int64_t session_id);

    void OnEnterGame(const AFNetMsg* msg, const int64_t session_id);
    void OnLeaveGame(const AFNetMsg* msg, const int64_t session_id);
    void OnSwapScene(const AFNetMsg* msg, const int64_t session_id);

    ///////////WORLD_START///////////////////////////////////////////////////////////////
    void OnTransWorld(const AFNetMsg* msg);

    // Common Node & table & class event
    int OnCommonClassEvent(
        const AFGUID& self, const std::string& strClassName, const ArkEntityEvent eClassEvent, const AFIDataList& var);

    int OnMapInstanceEvent(const AFGUID& self, const std::string& name, const uint32_t index, const AFIData& oldVar,
        const AFIData& newVar);
    int OnContainerEvent(const AFGUID& self, const std::string& name, const uint32_t index, const AFIData& oldVar,
        const AFIData& newVar);

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
    //int StartClient();

    int CommonClassDestoryEvent(const AFGUID& self);

private:
    //<PlayerID, GateBaseInfo>//其实可以在object系统中被代替
    AFSmartPtrMap<AFGUID, GateBaseInfo> mRoleBaseData;
    // gate id,data
    AFSmartPtrMap<int, GateServerInfo> mProxyMap;
    //////////////////////////////////////////////////////////////////////////
    AFIGUIDModule* m_pUUIDModule;
    AFIKernelModule* m_pKernelModule;
    AFILogModule* m_pLogModule;
    AFIBusModule* m_pBusModule;
    AFINetServiceManagerModule* m_pNetServiceManagerModule;
    AFIMapModule* m_pMapModule;
    //////////////////////////////////////////////////////////////////////////
    AFISceneProcessModule* m_pSceneProcessModule;
    AFIAccountModule* m_AccountModule;
    AFIMsgModule* m_pMsgModule;

    std::shared_ptr<AFINetServerService> m_pNetServerService;
};

} // namespace ark
