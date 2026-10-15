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
#include "ark/net/net_service_manager_module.hpp"
#include "ark/plugin_manager.hpp"
#include "simulate/interface/simulate_module.hpp"
#include "simulate/interface/simulate_net_module.hpp"
#include "simulate_msg_define.hpp"

namespace ark {

class simulate_net_module_impl final : public simulate_net_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;
    bool post_init() override;

private:
    int StartServer();
    void OnRecvMsg(const net_msg* msg, conv_id_t session_id);

    physic3_d SimulateVec3ToAFVec(const simulate::simulate_vec3& vec3);

    void TriggerCallBack(const std::string& scene_id, const int32_t index, const simulate::trigger_pair& pair);

    void SendMsg(const std::string& scene_id, const int32_t index, const simulate::simulate_msg_type msg_id,
        const msgpack::sbuffer& buffer);
    void SendAckRayCast(const std::string& scene_id, const int32_t index, const simulate::ack_ray_cast& ackRayCast);
    void SendAckTrigger(const std::string& scene_id, const int32_t index, const simulate::ack_trigger& ackRayCast);

    void UpdateBusID(const std::string& scene_id, const int32_t index, const int32_t bus_id);
    bool get_bus_id(const std::string& scene_id, const int32_t index, int32_t& bus_id);

private:
    simulate_module* m_pSimulateModule{nullptr};

    using index_bus_map = std::map<int32_t, int32_t>;
    using scene_index_bus_map = std::unordered_map<std::string, index_bus_map>;
    scene_index_bus_map scene_index_bus_list_;

    bus_module* m_pBusModule{nullptr};
    net_service_manager_module* m_pNetServiceManagerModule{nullptr};
    std::shared_ptr<net_server_service> m_pNetServerService;
};

} // namespace ark
