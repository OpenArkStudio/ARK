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

#include "ark/define.hpp"
#include "ark/misc.hpp"
#include "ark/time/date_time.hpp"
#include "sample4_module.hpp"

#define USE_CAPSULE 0

#if USE_CAPSULE
const float HALF_HEIGHT = 88.0F;
const float RADIUS = 36.0F;
const float RAYCAST_RADUIS = 36.01F;
#else
const ark::physic3_d BOX_SIZE = ark::physic3_d(100.0F, 100.0F, 100.0F);
const float RADIUS = 70.711;
const float RAYCAST_RADUIS = RADIUS;
#endif

const int ACOTR_COUNT = 100;
const int ACTOR_X_COUNT = 10;
const int ACOTR_Z_COUNT = 10;
const int RANDOM_MAX = 1000000;
const int STEP_BASE = 100;
const int COUNT_BASE = 10000;
const int MOVE_STEP[4] = {5 * STEP_BASE, 10 * STEP_BASE, 50 * STEP_BASE, 100 * STEP_BASE};
const int MOVE_COUNT[3] = {1 * COUNT_BASE, 5 * COUNT_BASE, 10 * COUNT_BASE}; //wan

namespace ark {

bool sample4_module::init() {
    std::cout << GET_CLASS_NAME(sample4_module) << ", Init" << std::endl;
    return true;
}

bool sample4_module::post_init() {
    std::cout << GET_CLASS_NAME(sample4_module) << ", PostInit" << std::endl;

    m_pPhysicModule = find_module<physic_module>();
    m_pPhysicConfigModule = find_module<physic_config_module>();

    ARK_LOG_INFO("Init finished...");

    ///init base
    InitBase();
    std::cout << GET_CLASS_NAME(sample4_module) << "init base success" << std::endl;

    InitMove();
    std::cout << GET_CLASS_NAME(sample4_module) << "init move success" << std::endl;

    CreateScene();
    std::cout << GET_CLASS_NAME(sample4_module) << "create scene success" << std::endl;

    //TestRaycast();

    CreateActor();

    ///move actor
    MoveActor();
    std::cout << GET_CLASS_NAME(sample4_module) << "move actor success" << std::endl;

    return true;
}

bool sample4_module::pre_shut() {
    std::cout << GET_CLASS_NAME(sample4_module) << ", PreShut" << std::endl;

    return true;
}

bool sample4_module::shut() {
    std::cout << GET_CLASS_NAME(sample4_module) << ", Shut" << std::endl;
    return true;
}

void sample4_module::InitBase() {
    ///random init
    gen = xoroshiro128_plus();
    gen.init_seed(0x12345678);

    ///get config
    config = m_pPhysicConfigModule->GetPhysicConfig();
}

void sample4_module::InitMove() {
    ///random direction
    auto step_count = sizeof(MOVE_STEP) / sizeof(int);
    for (size_t i = 0; i < step_count; i++)
    {
        ///ray cast distances
        auto step = MOVE_STEP[i];
        InitMoveStep(step);
    }
}

void sample4_module::InitMoveStep(const int step) {
    sample_move_step sample_move_step;
    sample_move_step.move_step = float(step);

    auto move_count = sizeof(MOVE_COUNT) / sizeof(int);
    for (size_t i = 0; i < move_count; i++)
    {
        ///move counts of each actor
        auto one_move_count = MOVE_COUNT[i];
        InitActorDirection(one_move_count, sample_move_step);
    }

    sample_move_list.insert(std::make_pair(step, sample_move_step));
}

void sample4_module::InitActorDirection(const int move_count, sample_move_step& sample_move_step) {
    sample_actor_direction actor_direction;
    actor_direction.move_count = move_count;
    for (int i = 0; i < ACOTR_COUNT; i++)
    {
        ///actor move directions
        physic3_d direction = RandomMoveDirection();
        actor_direction.direction_list.push_back(direction);
    }

    sample_move_step.count_actor_move_list.insert(std::make_pair(actor_direction.move_count, actor_direction));
}

void sample4_module::CreateScene() {
    ///first scene for test
    auto& scene_list = m_pPhysicConfigModule->GetPhysicSceneList();
    for (auto& iter : scene_list)
    {
        sample_scene sample_scene;

        auto& scene_id = iter.first;
        if (scene_id == "wfcy")
        {
            sample_scene.min_pos = physic3_d(-9372.780273, 700, -16475.042969);
            sample_scene.max_pos = physic3_d(12115.969727, 700, 9105.064453);
        }
        else if (scene_id == "fjcz")
        {
            sample_scene.min_pos = physic3_d(-6977.476563, 700, -4477.612305);
            sample_scene.max_pos = physic3_d(10624.397461, 700, 12644.917969);
        }
        else
        {
            continue;
        }

        ///create a scene
        sample_scene.scene_obj_id = m_pPhysicModule->CreateScene(scene_id);
        if (sample_scene.scene_obj_id == NULL_GUID)
        {
            ARK_ASSERT_RET_NONE("scene create failed");
            exit(0);
        }

        sample_scene.scene_id = scene_id;
        sample_scene_list.push_back(sample_scene);
    }
}

void sample4_module::CreateActor() {
    for (auto& iter : sample_scene_list)
    {
        auto& sample_scene = iter;
        auto scene_obj_id = sample_scene.scene_obj_id;

        //auto interval_x = (sample_scene.max_pos.x - sample_scene.min_pos.x) / ACTOR_X_COUNT;
        //auto interval_z = (sample_scene.max_pos.z - sample_scene.min_pos.z) / ACOTR_Z_COUNT;
        for (int i = 0; i < ACOTR_COUNT; i++)
        {
            //auto index_x = i / ACTOR_X_COUNT;
            //auto index_z = i % ACOTR_Z_COUNT;

            //pos.x = sample_scene.min_pos.x + (index_x + 0.5) * interval_x;
            //pos.y = sample_scene.min_pos.y;
            //pos.z = sample_scene.min_pos.z + (index_z + 0.5) * interval_z;

            //physx::PxRaycastBuffer rayHit;
            //if (m_pPhysicModule->RayCast(px_scene, pos, AFPhysic3D(0, -1, 0), 1000, rayHit))
            //{
            //    pos.y = rayHit.block.position.y;
            //}

            auto pos = RandomPosition(sample_scene);

#if USE_CAPSULE
            auto actor = m_pPhysicModule->CreateDynamicCapsuleActor(px_scene, pos, RADIUS, HALF_HEIGHT * 2);
#else
            auto actor = m_pPhysicModule->CreateDynamicBoxActor(scene_obj_id, pos, BOX_SIZE);
#endif

            if (actor != NULL_GUID)
            {
                sample_scene.actor_list.push_back(actor);
                ARK_LOG_INFO("actor index = {} created at position x = {} y = {} z= {}", i, pos.x, pos.y, pos.z);
            }
            else
            {
                ARK_LOG_INFO("actor created faile index = ", i);
                exit(0);
            }
        }
    }

#if USE_CAPSULE
    ARK_LOG_INFO("create capsule actors success");
    std::cout << GET_CLASS_NAME(sample4_module) << "create capsule actors success" << std::endl;
#else
    ARK_LOG_INFO("create box actors success");
    std::cout << GET_CLASS_NAME(sample4_module) << "create box actors success" << std::endl;
#endif
}

void sample4_module::MoveActor() {
    for (auto& iter : sample_scene_list)
    {
        auto& sample_scene = iter;
        MoveActor(sample_scene);
    }
}

void sample4_module::MoveActor(const sample_scene& scene) {
    for (auto& iter_move : sample_move_list)
    {
        auto& move_step = iter_move.second;
        for (auto& iter_actor_move : move_step.count_actor_move_list)
        {
            auto& actor_move = iter_actor_move.second;
            MoveActor(scene, actor_move, move_step.move_step);
        }
    }
}

void sample4_module::MoveActor(const sample_scene& scene, sample_actor_direction& actor_move, const float distance) {
    auto scene_obj_id = scene.scene_obj_id;
    auto& actor_list = scene.actor_list;
    auto actor_count = actor_list.size();

    auto move_count = actor_move.move_count;
    auto& actor_direction_list = actor_move.direction_list;

    //ARK_LOG_INFO(
    //    "--------------actor move scene id = {} actor count = {} move count = {} move distance = {}--------------",
    //    scene.scene_id, actor_list.size(), move_count, distance);

    //std::cout << "--------------actor move scene id = " << scene.scene_id << " actor count = " << actor_list.size()
    //          << " move count = " << move_count << " move distance = " << distance << "--------------" << std::endl;

    auto distance2 = distance * 2;
    int hit = 0;
    int missed = 0;

    date_time now;
    for (int i = 0; i < move_count; i++)
    {
        for (size_t j = 0; j < actor_count; j++)
        {
            auto actor = actor_list[j];
            physic3_d pos;
            if (!m_pPhysicModule->GetActorPosition(actor, pos))
            {
                ARK_LOG_ERROR("get actor position failed actor id = {}", actor);
                continue;
            }

            auto& direction = actor_direction_list[j];

            physic3_d origin;
            origin.x = pos.x + direction.x * RAYCAST_RADUIS;
            origin.z = pos.z + direction.z * RAYCAST_RADUIS;
            origin.y = pos.y;

            pos.x += (direction.x * distance);
            pos.z += (direction.z * distance);

            ///is out of range
            if (scene.IsInRange(pos))
            {
                physic_ray_hit rayHit;
                auto result = m_pPhysicModule->RayCast(scene_obj_id, origin, direction, distance, rayHit);
                if (result)
                {
                    pos = rayHit.hit_pos;

                    ///a new direction
                    direction = RandomMoveDirection();

                    hit += 1;
                    //LogRayCast(scene, hit, actor, rayHit.block.distance, rayHit.block.actor, pos);
                }
                else
                {
                    missed += 1;
                }
            }
            else
            {
                ///reverse direction
                direction.x = -direction.x;
                direction.z = -direction.z;

                pos.x += (direction.x * distance2);
                pos.z += (direction.z * distance2);

                physic_ray_hit rayHit;
                auto result = m_pPhysicModule->RayCast(scene_obj_id, origin, direction, distance, rayHit);
                if (result)
                {
                    pos = rayHit.hit_pos;

                    ///a new direction
                    direction = RandomMoveDirection();

                    hit += 1;
                    //LogRayCast(scene, hit, actor, rayHit.block.distance, rayHit.block.actor, pos);
                }
                else
                {
                    missed += 1;
                }
            }

            auto result = m_pPhysicModule->UpdateActor(actor, pos);
            if (!result)
            {
                ARK_LOG_INFO("actor move to origin failed");
            }

            //ARK_LOG_INFO("actor index = {} move to position x = {} y = {} z = {}", j, pos.x, pos.y, pos.z);
        }
    }

    auto elapsed = now.get_elapsed();
    auto time = elapsed / 1000.0F;
    auto average_time = float(elapsed) / float(actor_count * move_count) * 1000.0F;
    auto total_count = actor_count * move_count / COUNT_BASE;
    auto move_count_base = move_count / COUNT_BASE;
    ARK_LOG_INFO("scene id = {} ray cast distance = {} total count = {}(w) actor count = {} move count = {}(w) total "
                 "time = {}(s) per time = "
                 "{}(us) hit = "
                 "{} missed = {}",
        scene.scene_id, distance, total_count, actor_count, move_count_base, time, average_time, hit, missed);

    std::cout << "scene id = " << scene.scene_id << " ray cast distance = " << distance
              << " total count = " << total_count << "(w) actor count = " << actor_count
              << " move count = " << move_count_base << "(w) total time = " << time << "(s) per time = " << average_time
              << "(us)"
              << " hit = " << hit << " missed = " << missed << std::endl;

    ///reset actor position
    ReRandomPosition(scene);
}

physic3_d sample4_module::RandomMoveDirection() {
    auto random = rand() % RANDOM_MAX;
    auto dx = float(random) / float(RANDOM_MAX);
    physic3_d direction(dx, 0, std::sqrt(1 - dx * dx));

    return direction;
}

void sample4_module::LogRayCast(const sample_scene& scene, const int hit, float distance, const physic3_d& hit_pos) {
    ARK_LOG_INFO("scene id = {} hit index = {} hit distance = {} pos x = {} pos y = {} pos z = {}", scene.scene_id, hit,
        distance, hit_pos.x, hit_pos.y, hit_pos.z);
}

void sample4_module::ReRandomPosition(const sample_scene& scene) {
    auto& actor_list = scene.actor_list;
    for (auto iter : actor_list)
    {
        auto actor = iter;
        auto pos = RandomPosition(scene);
        m_pPhysicModule->UpdateActor(actor, pos);
    }
}

physic3_d sample4_module::RandomPosition(const sample_scene& scene) {
    physic3_d pos;

    float mul = 1.0F;
    if (scene.scene_id == "wfcy")
    {
        mul = 0.5F;
    }

    auto width_x = (int)(scene.max_pos.x - scene.min_pos.x);
    auto width_z = (int)(scene.max_pos.z - scene.min_pos.z);
    pos.x = (float)(rand() % width_x + scene.min_pos.x) * mul;
    pos.y = scene.min_pos.y;
    pos.z = (float)(rand() % width_z + scene.min_pos.z) * mul;

    return pos;
}

void sample4_module::TestRaycast() {
    for (auto& iter : sample_scene_list)
    {
        auto& sample_scene = iter;
        auto scene_obj_id = sample_scene.scene_obj_id;
        if (sample_scene.scene_id == "wfcy")
        {
            //sample_scene.min_pos = AFPhysic3D(-9372.780273, 700, -16475.042969);
            //sample_scene.max_pos = AFPhysic3D(12115.969727, 700, 9105.064453);

            auto pos1 = physic3_d(-9000, 700, -16000);
            auto actor1 = m_pPhysicModule->CreateDynamicBoxActor(scene_obj_id, pos1, physic3_d(100.0F, 100.0F, 100.0F));

            auto pos2 = physic3_d(-8800, 700, -16000);
            auto actor2 = m_pPhysicModule->CreateDynamicBoxActor(scene_obj_id, pos2, physic3_d(100.0F, 100.0F, 100.0F));

            auto direction = physic3_d(1, 0, 0);
            physic3_d origin;
            origin.x = pos1.x + direction.x * 70.711F;
            origin.z = pos1.z;
            origin.y = pos1.y;

            float rayDist = 500;
            physic_ray_hit rayHit;
            bool bResult = m_pPhysicModule->RayCast(scene_obj_id, origin, direction, rayDist, rayHit);
            if (bResult)
            {
                auto hitPos = rayHit.hit_pos;
                std::cout << "ray cast distance = " << rayDist << " hit pos = (" << hitPos.x << ", " << hitPos.y << ", "
                          << hitPos.z << ") hit distance = " << rayHit.hit_distance << std::endl;
            }
            else
            {
                std::cout << "ray cast distance = " << rayDist << " did not hit " << std::endl;
            }

            rayDist = 5;
            physic_ray_hit rayHit2;
            bResult = m_pPhysicModule->RayCast(scene_obj_id, origin, direction, rayDist, rayHit2);
            if (bResult)
            {
                auto hitPos = rayHit2.hit_pos;
                std::cout << "ray cast distance = " << rayDist << " hit pos = (" << hitPos.x << ", " << hitPos.y << ", "
                          << hitPos.z << ") hit distance = " << rayHit.hit_distance << std::endl;
            }
            else
            {
                std::cout << "ray cast distance = " << rayDist << " did not hit " << std::endl;
            }
        }
    }
}

} // namespace ark
