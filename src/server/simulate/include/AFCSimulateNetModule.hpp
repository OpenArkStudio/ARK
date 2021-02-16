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

#include "base/AFPluginManager.hpp"
#include "simulate/interface/AFISimulateNetModule.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "simulate/interface/AFISimulateModule.hpp"
#include "AFSimulateMsgDefine.hpp"

namespace ark {

class AFCSimulateNetModule final : public AFISimulateNetModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool PostInit() override;

private:
    int StartServer();
    void OnRecvMsg(const AFNetMsg* msg, conv_id_t session_id);

    AFPhysic3D SimulateVec3ToAFVec(const simulate::SimulateVec3& vec3);

    void TriggerCallBack(const std::string& scene_id, const int32_t index, const simulate::TriggerPair& pair);

    void SendMsg(const std::string& scene_id, const int32_t index, const simulate::SimulateMsgType msg_id,
        const msgpack::sbuffer& buffer);
    void SendAckRayCast(const std::string& scene_id, const int32_t index, const simulate::AckRayCast& ackRayCast);
    void SendAckTrigger(const std::string& scene_id, const int32_t index, const simulate::AckTrigger& ackRayCast);

    void UpdateBusID(const std::string& scene_id, const int32_t index, const int32_t bus_id);
    bool GetBusID(const std::string& scene_id, const int32_t index, int32_t& bus_id);

private:
    AFISimulateModule* m_pSimulateModule{nullptr};

    using IndexBusMap = std::map<int32_t, int32_t>;
    using SceneIndexBusMap = std::unordered_map<std::string, IndexBusMap>;
    SceneIndexBusMap scene_index_bus_list_;

    AFIBusModule* m_pBusModule{nullptr};
    AFINetServiceManagerModule* m_pNetServiceManagerModule{nullptr};
    std::shared_ptr<AFINetServerService> m_pNetServerService;
};

} // namespace ark
