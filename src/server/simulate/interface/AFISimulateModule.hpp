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

#include "interface/AFIModule.hpp"
#include "simulate/include/AFSimulateDefine.hpp"
#include "simulate/include/AFSimulateMsgDefine.hpp"
#include "physic/interface/AFIPhysicModule.hpp"

namespace ark {

class AFISimulateModule : public AFIModule
{
public:
    virtual bool CreateScene(const std::string& scene_id, const int32_t index) = 0;
    virtual bool DestroyScene(const std::string& scene_id, const int32_t index) = 0;

    virtual bool CreateBox(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id,
        const AFPhysic3D& position, const AFPhysic3D& size, bool bDynamic) = 0;
    virtual bool CreateCapsule(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id,
        const AFPhysic3D& position, const float raius, const float height, bool bDynamic) = 0;
    virtual bool CreateSphere(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id,
        const AFPhysic3D& position, const float raius, bool bDynamic) = 0;

    virtual bool UpdateActor(
        const std::string& scene_id, const int32_t index, const SimulateNetID obj_id, const AFPhysic3D& position) = 0;
    virtual bool DestroyActor(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id) = 0;

    virtual bool RayCast(const std::string& scene_id, const int32_t index, const AFPhysic3D& start,
        const AFPhysic3D& direction, const float distance, simulate::AckRayCast& ackRayCast) = 0;

    template<typename BaseType>
    bool RegisterTriggerCallBack(BaseType* pBase,
        void (BaseType::*handler)(const std::string& scene_id, const int32_t index, const simulate::TriggerPair& pair))
    {
        auto functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        return RegisterTriggerCallBack(std::move(functor));
    }

private:
    virtual bool RegisterTriggerCallBack(SIMULATE_TRIGGER_CB_FUNCTION&& cb) = 0;
};

} // namespace ark
