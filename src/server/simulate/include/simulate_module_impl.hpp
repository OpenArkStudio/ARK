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

#include "ark/physic/physic_module.hpp"
#include "ark/plugin_manager.hpp"
#include "simulate/interface/simulate_module.hpp"

namespace ark {

class simulate_module_impl final : public simulate_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;

    bool CreateScene(const std::string& scene_id, const int32_t index) override;
    bool DestroyScene(const std::string& scene_id, const int32_t index) override;

    bool CreateBox(const std::string& scene_id, const int32_t index, const simulate_net_id obj_id,
        const physic3_d& position, const physic3_d& size, bool bDynamic) override;
    bool CreateCapsule(const std::string& scene_id, const int32_t index, const simulate_net_id obj_id,
        const physic3_d& position, const float raius, const float height, bool bDynamic) override;
    bool CreateSphere(const std::string& scene_id, const int32_t index, const simulate_net_id obj_id,
        const physic3_d& position, const float raius, bool bDynamic) override;

    bool UpdateActor(const std::string& scene_id, const int32_t index, const simulate_net_id obj_id,
        const physic3_d& position) override;
    bool DestroyActor(const std::string& scene_id, const int32_t index, const simulate_net_id obj_id) override;

    bool RayCast(const std::string& scene_id, const int32_t index, const physic3_d& start, const physic3_d& direction,
        const float distance, simulate::ack_ray_cast& ackRayCast) override;

private:
    bool RegisterTriggerCallBack(simulate_trigger_cb_function&& cb) override;

    void InsertSceneObjActors(const guid_t& scene_obj_id, const std::string& scene_id, const int32_t index);
    void RemoveSceneObjActors(const std::string& scene_id, const int32_t index);
    simulate_scene* FindSceneObjActors(const std::string& scene_id, const int32_t index);

    simulate::simulate_vec3 AFVecToSimulateVec3(const physic3_d& vec3);

    void TriggerCallBack(const guid_t& scene_obj_id, const physic_trigger_pair& trigger_pair);

private:
    scene_index_scene_map scene_index_scene_list_;
    scene_index_list scene_index_pair_list_;

    simulate_trigger_cb_function trigger_call_back_function_{nullptr};

    physic_module* m_pPhysicModule{nullptr};
};

} // namespace ark
