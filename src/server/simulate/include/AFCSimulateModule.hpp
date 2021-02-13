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
#include "physic/interface/AFIPhysicModule.hpp"
#include "simulate/interface/AFISimulateModule.hpp"

namespace ark {

class AFCSimulateModule final : public AFISimulateModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;

    bool CreateScene(const std::string& scene_id, const int32_t index) override;
    bool DestroyScene(const std::string& scene_id, const int32_t index) override;

    bool CreateBox(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id,
        const AFPhysic3D& position, const AFPhysic3D& size, bool bDynamic) override;
    bool CreateCapsule(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id,
        const AFPhysic3D& position, const float raius, const float height, bool bDynamic) override;
    bool CreateSphere(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id,
        const AFPhysic3D& position, const float raius, bool bDynamic) override;

    bool UpdateActor(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id,
        const AFPhysic3D& position) override;
    bool DestroyActor(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id) override;

    bool RayCast(const std::string& scene_id, const int32_t index, const AFPhysic3D& start, const AFPhysic3D& direction,
        const float distance, simulate::AckRayCast& ackRayCast) override;

private:
    bool RegisterTriggerCallBack(SIMULATE_TRIGGER_CB_FUNCTION&& cb) override;

    void InsertSceneObjActors(const guid_t& scene_obj_id, const std::string& scene_id, const int32_t index);
    void RemoveSceneObjActors(const std::string& scene_id, const int32_t index);
    AFSimulateScene* FindSceneObjActors(const std::string& scene_id, const int32_t index);

    simulate::SimulateVec3 AFVecToSimulateVec3(const AFPhysic3D& vec3);

    void TriggerCallBack(const guid_t& scene_obj_id, const AFPhysicTriggerPair& trigger_pair);

private:
    SceneIndexSceneMap scene_index_scene_list_;
    SceneIndexList scene_index_pair_list_;

    SIMULATE_TRIGGER_CB_FUNCTION trigger_call_back_function_{nullptr};

    AFIPhysicModule* m_pPhysicModule{nullptr};
};

} // namespace ark
