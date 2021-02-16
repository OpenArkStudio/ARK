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

#include "base/container/AFMap.hpp"
#include "base/container/AFCConsistentHash.hpp"
#include "base/AFPluginManager.hpp"
#include "net/interface/AFINetClientService.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "bus/interface/AFIMsgModule.hpp"

namespace ark {

class AFCNetClientService final : public AFINetClientService
{
public:
    enum EConstDefine
    {
        EConstDefine_DefaultWeight = 255,
    };

    AFCNetClientService(AFPluginManager* p);

    bool StartClient(std::shared_ptr<const AFIMsgHeader> head, bus_id_t target_bus_id, const AFEndpoint& endpoint) override;
    void Update() override;
    //void Shutdown() override;

    bool RegMsgCallback(msg_id_t msg_id, const NET_MSG_FUNCTOR&& cb) override;
    bool RegForwardMsgCallback(const NET_MSG_FUNCTOR&& cb) override;
    bool RegNetEventCallback(const NET_EVENT_FUNCTOR&& cb) override;

    std::shared_ptr<AFConnectionData> GetConnectionInfo(bus_id_t bus_id) override;
    //AFMapEx<int, AFConnectionData>& GetServerList() override;

    std::shared_ptr<AFConnectionData> GetSuitableConnect(const std::string& key) override;

    void AddAccountBusID(const std::string& account, bus_id_t bus_id) override;
    void RemoveAccountBusID(const std::string& account) override;
    int GetAccountBusID(const std::string& account) override;

    void AddActorBusID(const guid_t& actor, const bus_id_t bus_id) override;
    void RemoveActorBusID(const guid_t& actor) override;
    int GetActorBusID(const guid_t& actor) override;

protected:
    void ProcessUpdate();
    void ProcessAddConnection();

    std::shared_ptr<AFINet> CreateNet(const proto_type proto);

    //void RegisterToServer(conv_id_t session_id, bus_id_t bus_id);
    int OnConnect(const AFNetEvent* event);
    int OnDisconnect(const AFNetEvent* event);

    void OnNetMsg(const AFNetMsg* msg, conv_id_t session_id);
    void OnNetEvent(const AFNetEvent* event);

    void KeepReport(std::shared_ptr<AFConnectionData>& connection_data)
    {
        // TODO
    }

    void LogServerInfo(const std::string& strServerInfo)
    {
        // TODO
    }

    void LogServerInfo();
    void KeepAlive(std::shared_ptr<AFConnectionData> pServerData);

    //////////////////////////////////////////////////////////////////////////
    //bool GetServerMachineData(const std::string& strServerID, AFCMachineNode& xMachineData);
    //void AddServerWeightData(std::shared_ptr<AFConnectionData>& xInfo);

    //void RemoveServerWeightData(std::shared_ptr<AFConnectionData>& xInfo);
    //////////////////////////////////////////////////////////////////////////
    bool AddServerNode(std::shared_ptr<AFConnectionData> data);
    bool GetServerNode(const std::string& hash_key, AFVNode& vnode);
    void RemoveServerNode(std::shared_ptr<AFConnectionData> data);

    void RegToServer(const AFIMsgHeader& head, bus_id_t target_bus_id);
    void SendReport(std::shared_ptr<AFConnectionData> pServerData);

private:
    AFPluginManager* m_pPluginManager;
    AFINetServiceManagerModule* m_pNetServiceManagerModule;
    AFIBusModule* m_pBusModule;
    AFIMsgModule* m_pMsgModule;

    // Connected connections(may the ConnectState is different)
    AFSmartPtrMap<bus_id_t, AFConnectionData> real_connections_;

    std::map<guid_t, bus_id_t> actor_bus_map_;                  // actor id bus id
    std::unordered_map<std::string, bus_id_t> account_bus_map_; // account bus id

    // the new consistent hash map
    AFConsistentHashmapType consistent_hashmap_;

    std::list<AFConnectionData> tmp_connections_;

    std::map<msg_id_t, NET_MSG_FUNCTOR> net_msg_callbacks_;
    std::list<NET_EVENT_FUNCTOR> net_event_callbacks_;

    // [NOT USE now] - forward to other processes
    std::list<NET_MSG_FUNCTOR> net_msg_forward_callbacks_;
};

} // namespace ark
