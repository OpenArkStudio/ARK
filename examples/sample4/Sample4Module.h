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
#include "base/random/AFRandomXoshiro.hpp"
#include "physic/interface/AFIPhysicModule.hpp"
#include "physic/interface/AFIPhysicConfigModule.hpp"

namespace ark {

using SampleActorDirectionList = std::vector<AFPhysic3D>;
struct SampleActorDirection
{
    int move_count{0};
    SampleActorDirectionList direction_list;
};
using MoveCountActorDirectionList = std::map<int, SampleActorDirection>;

struct SampleMoveStep
{
    float move_step{0.0F};
    MoveCountActorDirectionList count_actor_move_list;
};
using StepMoveCountActorMoveList = std::map<int, SampleMoveStep>;

class SampleScene
{
public:
    bool IsInRange(const AFPhysic3D& pos) const
    {
        if (pos.x < min_pos.x || pos.z < min_pos.z)
        {
            return false;
        }

        if (pos.x > max_pos.x || pos.z > max_pos.z)
        {
            return false;
        }

        return true;
    }

    std::string scene_id{""};
    AFPhysic3D min_pos;
    AFPhysic3D max_pos;

    guid_t scene_obj_id{0U};
    std::vector<guid_t> actor_list;
};

class Sample4Module final : public AFIModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit Sample4Module() = default;

    bool Init() override;
    bool PostInit() override;
    bool PreShut() override;
    bool Shut() override;

protected:
    void InitBase();
    void InitMove();
    void InitMoveStep(const int step);
    void InitActorDirection(const int move_count, SampleMoveStep& sample_move_step);
    void CreateScene();
    void CreateActor();
    void MoveActor();
    void MoveActor(const SampleScene& scene);
    void MoveActor(const SampleScene& scene, SampleActorDirection& actor_move, const float distance);
    AFPhysic3D RandomMoveDirection();
    void LogRayCast(const SampleScene& scene, const int hit, float distance, const AFPhysic3D& hit_pos);
    void ReRandomPosition(const SampleScene& scene);
    AFPhysic3D RandomPosition(const SampleScene& scene);
    void TestRaycast();

protected:
    PhysicConfig config;
    xoroshiro128_plus gen;

    std::vector<SampleScene> sample_scene_list;
    StepMoveCountActorMoveList sample_move_list;

protected:
    AFIPhysicModule* m_pPhysicModule;
    AFIPhysicConfigModule* m_pPhysicConfigModule;
};

} // namespace ark
