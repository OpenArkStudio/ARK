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
#include "ark/container/map.hpp"
#include "ark/net/net_client_service.hpp"
#include "ark/net/net_server_service.hpp"
#include "ark/net/net_service_manager_module.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/utility/timer_module.hpp"

namespace ark {

class net_service_manager_module_impl final : public net_service_manager_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;
    bool post_init() override;
    bool update() override;
    bool shut() override;

    bool create_server(
        const size_t silent_timeout, const msg_header& csReqHeader, const msg_header& ssReqHeader) override;

    std::shared_ptr<net_server_service> get_inter_net_server() override;
    std::shared_ptr<net_server_service> get_intra_net_server() override;

    //int CreateClusterClients(AFHeaderLen head_len = AFHeaderLen::SS_HEAD_LENGTH) override;
    //int CreateClusterClient(AFHeaderLen head_len, bus_id_t bus_id, const std::string& url) override;

    //AFINetClientService* GetNetClientService(const ARK_APP_TYPE& app_type) override;
    //AFINetClientService* GetNetClientServiceByBusID(bus_id_t bus_id) override;

    bool add_net_connection_bus(bus_id_t target_bus_id, std::shared_ptr<net> net_ptr) override;
    bool remove_net_connection_bus(bus_id_t target_bus_id) override;
    std::shared_ptr<net> get_net_connection_bus(bus_id_t src_bus, bus_id_t target_bus) override;

    std::shared_ptr<net_client_service> get_client_service(const app_type app_type) override;

    conv_id_t get_session_id(bus_id_t target_bus_id) override;
    bool remove_session_id(conv_id_t session_id) override;

    bool create_client_service(const bus_addr& bus_addr, proto_type proto, const std::string& ip, uint16_t port,
        std::shared_ptr<const msg_header> head = std::shared_ptr<const msg_header>(new ss_msg_header)) override;

protected:
    //ananas::Future<std::pair<bool, std::string>> RegisterToConsul(bus_id_t bus_id);
    //int DeregisterFromConsul(bus_id_t bus_id);

    void health_check(uint64_t timer_id, const guid_t& entity_id);

    bool check_connected_target_server(const bus_addr& bus_addr);
    std::shared_ptr<connection_data> get_target_client_connection(const bus_addr& bus_addr);

    void on_reg_server_call_back(const net_msg* msg, conv_id_t session_id);

private:
    // bus_id -> net_server_service - support multi-server in the same app
    using server_service = shared_object_map<bus_id_t, net_server_service>;
    server_service server_services_;

    server_service intra_services_;

    // target_app_type -> net_client_service
    // the NetClientService have target multi-client info.
    shared_object_map<app_type, net_client_service> client_services_;

    // All net relations, for finding AFINet
    // <self_bus, client_bus> -> AFINet
    shared_object_map<std::pair<bus_id_t, bus_id_t>, net> net_bus_relations_;

    //bus id <--> session id
    std::map<bus_id_t, conv_id_t> bus_session_list_;

    bus_module* m_pBusModule;
    timer_module* m_pTimerModule;
};

} // namespace ark
