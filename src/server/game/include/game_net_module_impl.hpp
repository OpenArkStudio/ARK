/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "ark/bus/bus_module.hpp"
#include "ark/bus/msg_module.hpp"
#include "ark/kernel/kernel_module.hpp"
#include "ark/kernel/map_module.hpp"
#include "ark/net/net_service_manager_module.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/proto.hpp"
#include "ark/utility/guid_module.hpp"
#include "game/interface/game_net_module.hpp"

namespace ark {

class game_net_module_impl final : public game_net_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;
    bool post_init() override;
    //bool PreUpdate() override;

    void SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const guid_t& self) override;
    void SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const guid_t& self) override;
    std::shared_ptr<net_server_service> GetNetServerService() override;

    bool AddPlayerGateInfo(const guid_t& nRoleID, const guid_t& nClientID, const int nGateID) override;
    bool RemovePlayerGateInfo(const guid_t& nRoleID) override;
    //std::shared_ptr<GateBaseInfo> GetPlayerGateInfo(const guid_t& nRoleID) override;

    //virtual std::shared_ptr<GateServerInfo> GetGateServerInfo(const int nGateID);
    //virtual std::shared_ptr<GateServerInfo> GetGateServerInfoByClientID(const guid_t& nClientID);

    virtual int OnViewDataNodeEnter(const data_list& argVar, const guid_t& self);
    virtual int OnSelfDataNodeEnter(const guid_t& self);
    virtual int OnViewDataTableEnter(const data_list& argVar, const guid_t& self);
    virtual int OnSelfDataTableEnter(const guid_t& self);

    virtual int OnEntityListEnter(const data_list& self, const data_list& argVar);
    virtual int OnEntityListLeave(const data_list& self, const data_list& argVar);

    // void OnSocketEvent(const AFNetEvent* event);
    // void OnClientDisconnect(const guid_t& xClientID);
    // void OnClientConnected(const guid_t& xClientID);

    // Common Node & table & class event
    int OnCommonClassEvent(
        const guid_t& self, const std::string& strClassName, const entity_event eClassEvent, const data_list& var);

    //int OnMapInstanceEvent(const guid_t& self, const std::string& name, const uint32_t index, const AFIData& oldVar,
    //    const AFIData& newVar);
    //int OnContainerEvent(const guid_t& self, const std::string& name, const uint32_t index, const AFIData& oldVar,
    //    const AFIData& newVar);

    int OnLeaveScene(const guid_t& self, const int map_id, const int map_inst_id);
    int OnEnterScene(const guid_t& self, const int map_id, const int map_inst_id);

    int OnEntityEvent(
        const guid_t& self, const std::string& strClassName, const entity_event eClassEvent, const data_list& var);

    int GetBroadcastEntityList(const int nObjectContainerID, const int nGroupID, data_list& valueObject);

    bool ProcessLeaveGroup(const guid_t& self, int nSceneID, int nOldGroupID);
    bool ProcessEnterGroup(const guid_t& self, int nSceneID, int nNewGroupID);

protected:
    int StartServer();
    //int StartClient();

    int CommonClassDestoryEvent(const guid_t& self);

private:
    //<PlayerID, GateBaseInfo>//其实可以在object系统中被代替
    // AFSmartPtrMap<guid_t, GateBaseInfo> mRoleBaseData;
    // gate id,data
    // AFSmartPtrMap<int, GateServerInfo> mProxyMap;
    //////////////////////////////////////////////////////////////////////////
    guid_module* m_pUUIDModule;
    kernel_module* m_pKernelModule;
    bus_module* m_pBusModule;
    net_service_manager_module* m_pNetServiceManagerModule;
    map_module* m_pMapModule;

    msg_module* m_pMsgModule;

    std::shared_ptr<net_server_service> m_pNetServerService;
};

} // namespace ark
