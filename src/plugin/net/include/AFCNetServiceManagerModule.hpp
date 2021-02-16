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
#include "base/AFPluginManager.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "utility/interface/AFITimerModule.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "net/interface/AFINetServerService.hpp"
#include "net/interface/AFINetClientService.hpp"

namespace ark {

class AFCNetServiceManagerModule final : public AFINetServiceManagerModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool PostInit() override;
    bool Update() override;
    bool Shut() override;

    bool CreateServer(const size_t silent_timeout, const AFIMsgHeader& csReqHeader, const AFIMsgHeader& ssReqHeader) override;

    std::shared_ptr<AFINetServerService> GetInterNetServer() override;
    std::shared_ptr<AFINetServerService> GetIntraNetServer() override;

    //int CreateClusterClients(AFHeaderLen head_len = AFHeaderLen::SS_HEAD_LENGTH) override;
    //int CreateClusterClient(AFHeaderLen head_len, bus_id_t bus_id, const std::string& url) override;

    //AFINetClientService* GetNetClientService(const ARK_APP_TYPE& app_type) override;
    //AFINetClientService* GetNetClientServiceByBusID(bus_id_t bus_id) override;

    bool AddNetConnectionBus(bus_id_t target_bus_id, std::shared_ptr<AFINet> net_ptr) override;
    bool RemoveNetConnectionBus(bus_id_t target_bus_id) override;
    std::shared_ptr<AFINet> GetNetConnectionBus(bus_id_t src_bus, bus_id_t target_bus) override;

    std::shared_ptr<AFINetClientService> GetClientService(const ARK_APP_TYPE app_type) override;

    conv_id_t GetSessionID(bus_id_t target_bus_id) override;
    bool RemoveSessionID(conv_id_t session_id) override;

    bool CreateClientService(const AFBusAddr& bus_addr, proto_type proto, const std::string& ip, uint16_t port,
        std::shared_ptr<const AFIMsgHeader> head = std::shared_ptr<const AFIMsgHeader>(new AFSSMsgHeader)) override;

protected:
    //ananas::Future<std::pair<bool, std::string>> RegisterToConsul(bus_id_t bus_id);
    //int DeregisterFromConsul(bus_id_t bus_id);

    void HealthCheck(uint64_t timer_id, const guid_t& entity_id);

    bool CheckConnectedTargetServer(const AFBusAddr& bus_addr);
    std::shared_ptr<AFConnectionData> GetTargetClientConnection(const AFBusAddr& bus_addr);

    void OnRegServerCallBack(const AFNetMsg* msg, conv_id_t session_id);

private:
    // bus_id -> net_server_service - support multi-server in the same app
    using ServerService = AFSmartPtrMap<bus_id_t, AFINetServerService>;
    ServerService server_services_;

    ServerService intra_services_;

    // target_app_type -> net_client_service
    // the NetClientService have target multi-client info.
    AFSmartPtrMap<ARK_APP_TYPE, AFINetClientService> client_services_;

    // All net relations, for finding AFINet
    // <self_bus, client_bus> -> AFINet
    AFSmartPtrMap<std::pair<bus_id_t, bus_id_t>, AFINet> net_bus_relations_;

    //bus id <--> session id
    std::map<bus_id_t, conv_id_t> bus_session_list_;

    AFIBusModule* m_pBusModule;
    AFITimerModule* m_pTimerModule;
};

} // namespace ark
