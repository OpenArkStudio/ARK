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

#include "simulate/include/AFCSimulateModule.hpp"
#include "base/AFPluginManager.hpp"

namespace ark {
//method realization

#define GET_AND_CHECK_OBJ_EXIST(scene_id, index)                                                                       \
    auto simulate_scene = FindSceneObjActors(scene_id, index);                                                         \
    if (!simulate_scene)                                                                                               \
    {                                                                                                                  \
        ARK_LOG_ERROR("scene has not been created scene id = {} index = {} obj id = {}", scene_id, index, obj_id);     \
        return false;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    if (simulate_scene->Exist(obj_id))                                                                                 \
    {                                                                                                                  \
        ARK_LOG_ERROR("has created this obj scene id = {} index = {} obj id = {}", scene_id, index, obj_id);           \
        return false;                                                                                                  \
    }

bool AFCSimulateModule::Init()
{
    m_pPhysicModule = FindModule<AFIPhysicModule>();

    m_pPhysicModule->RegisterTriggerCallBack(this, &AFCSimulateModule::TriggerCallBack);

    return true;
}

bool AFCSimulateModule::CreateScene(const std::string& scene_id, const int32_t index)
{
    auto simulate_scene = FindSceneObjActors(scene_id, index);
    if (simulate_scene)
    {
        ARK_LOG_ERROR("has created this scene id = {} index = {}", scene_id, index);
        return false;
    }

    auto scene_obj_id = m_pPhysicModule->CreateScene(scene_id);
    if (scene_obj_id == NULL_GUID)
    {
        ARK_LOG_ERROR("create scene failed scene id = {} index = {}", scene_id, index);
        return false;
    }

    InsertSceneObjActors(scene_obj_id, scene_id, index);
    return true;
}

bool AFCSimulateModule::DestroyScene(const std::string& scene_id, const int32_t index)
{
    auto simulate_scene = FindSceneObjActors(scene_id, index);
    if (!simulate_scene)
    {
        ARK_LOG_ERROR("does not have this scene id = {} index = {}", scene_id, index);
        return false;
    }

    if (!m_pPhysicModule->DestroyScene(simulate_scene->GetSceneObjID()))
    {
        ARK_LOG_ERROR("destroy scene failed scene id = {} index = {}", scene_id, index);
        return false;
    }

    RemoveSceneObjActors(scene_id, index);
    return true;
}

bool AFCSimulateModule::CreateBox(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id,
    const AFPhysic3D& position, const AFPhysic3D& size, bool bDynamic)
{
    GET_AND_CHECK_OBJ_EXIST(scene_id, index);

    guid_t actor_obj_id;
    if (bDynamic)
    {
        actor_obj_id = m_pPhysicModule->CreateDynamicBoxActor(simulate_scene->GetSceneObjID(), position, size);
    }
    else
    {
        actor_obj_id = m_pPhysicModule->CreateStaticBoxActor(simulate_scene->GetSceneObjID(), position, size);
    }

    if (actor_obj_id == NULL_GUID)
    {
        ARK_LOG_ERROR("create actor failed scene id = {} index = {} obj id = {}", scene_id, index, obj_id);
        return false;
    }

    simulate_scene->InsertObjActor(obj_id, actor_obj_id);
    return true;
}

bool AFCSimulateModule::CreateCapsule(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id,
    const AFPhysic3D& position, const float raius, const float height, bool bDynamic)
{
    GET_AND_CHECK_OBJ_EXIST(scene_id, index);

    guid_t actor_obj_id;
    if (bDynamic)
    {
        actor_obj_id =
            m_pPhysicModule->CreateDynamicCapsuleActor(simulate_scene->GetSceneObjID(), position, raius, height);
    }
    else
    {
        actor_obj_id =
            m_pPhysicModule->CreateStaticCapsuleActor(simulate_scene->GetSceneObjID(), position, raius, height);
    }

    if (actor_obj_id == NULL_GUID)
    {
        ARK_LOG_ERROR("create actor failed scene id = {} index = {} obj id = {}", scene_id, index, obj_id);
        return false;
    }

    simulate_scene->InsertObjActor(obj_id, actor_obj_id);
    return true;
}

bool AFCSimulateModule::CreateSphere(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id,
    const AFPhysic3D& position, const float raius, bool bDynamic)
{
    GET_AND_CHECK_OBJ_EXIST(scene_id, index);

    guid_t actor_obj_id;
    if (bDynamic)
    {
        actor_obj_id = m_pPhysicModule->CreateDynamicSphereActor(simulate_scene->GetSceneObjID(), position, raius);
    }
    else
    {
        actor_obj_id = m_pPhysicModule->CreateStaticSphereActor(simulate_scene->GetSceneObjID(), position, raius);
    }

    if (actor_obj_id == NULL_GUID)
    {
        ARK_LOG_ERROR("create actor failed scene id = {} index = {} obj id = {}", scene_id, index, obj_id);
        return false;
    }

    simulate_scene->InsertObjActor(obj_id, actor_obj_id);
    return true;
}

bool AFCSimulateModule::UpdateActor(
    const std::string& scene_id, const int32_t index, const SimulateNetID obj_id, const AFPhysic3D& position)
{
    auto simulate_scene = FindSceneObjActors(scene_id, index);
    if (!simulate_scene)
    {
        ARK_LOG_ERROR("scene has not been created scene id = {} index = {} obj id = {}", scene_id, index, obj_id);
        return false;
    }

    auto actor_obj_id = simulate_scene->FindActor(obj_id);
    if (actor_obj_id == NULL_GUID)
    {
        ARK_LOG_ERROR("does not have this obj scene id = {} index = {} obj id = {}", scene_id, index, obj_id);
        return false;
    }

    if (!m_pPhysicModule->UpdateActor(actor_obj_id, position))
    {
        ARK_LOG_ERROR("update acotr failed scene id = {} index = {} obj id = {}", scene_id, index, obj_id);
        return false;
    }

    return true;
}

bool AFCSimulateModule::DestroyActor(const std::string& scene_id, const int32_t index, const SimulateNetID obj_id)
{
    auto simulate_scene = FindSceneObjActors(scene_id, index);
    if (!simulate_scene)
    {
        ARK_LOG_ERROR("scene has not been created scene id = {} index = {} obj id = {}", scene_id, index, obj_id);
        return false;
    }

    auto actor_obj_id = simulate_scene->FindActor(obj_id);
    if (actor_obj_id == NULL_GUID)
    {
        ARK_LOG_ERROR("does not have this obj scene id = {} index = {} obj id = {}", scene_id, index, obj_id);
        return false;
    }

    if (!m_pPhysicModule->DestroyActor(actor_obj_id))
    {
        ARK_LOG_ERROR("destroy acotr failed scene id = {} index = {} obj id = {}", scene_id, index, obj_id);
        return false;
    }

    simulate_scene->RemoveObj(obj_id);
    return true;
}

bool AFCSimulateModule::RayCast(const std::string& scene_id, const int32_t index, const AFPhysic3D& start,
    const AFPhysic3D& direction, const float distance, simulate::AckRayCast& ackRayCast)
{
    auto simulate_scene = FindSceneObjActors(scene_id, index);
    if (!simulate_scene)
    {
        ARK_LOG_ERROR("has created this scene id = {} index = {}", scene_id, index);
        return false;
    }

    AFPhysicRayHit rayHit;
    if (!m_pPhysicModule->RayCast(simulate_scene->GetSceneObjID(), start, direction, distance, rayHit))
    {
        ARK_LOG_ERROR("ray cast failed scene id = {} index = {}", scene_id, index);
        return false;
    }

    ackRayCast.scene_id = scene_id;
    ackRayCast.index = index;
    ackRayCast.is_hit = rayHit.is_hit;

    auto actor_obj_id = rayHit.hit_obj_id;
    auto obj_id = simulate_scene->FindObj(actor_obj_id);
    ackRayCast.hit_obj_id = obj_id;
    ackRayCast.hit_pos = AFVecToSimulateVec3(rayHit.hit_pos);

    return true;
}

bool AFCSimulateModule::RegisterTriggerCallBack(SIMULATE_TRIGGER_CB_FUNCTION&& cb)
{
    trigger_call_back_function_ = cb;
    return true;
}

void AFCSimulateModule::InsertSceneObjActors(
    const guid_t& scene_obj_id, const std::string& scene_id, const int32_t index)
{
    auto iter = scene_index_scene_list_.find(scene_id);
    if (iter == scene_index_scene_list_.end())
    {
        IndexSceneMap obj_actors_list;
        iter = scene_index_scene_list_.insert(std::make_pair(scene_id, obj_actors_list)).first;
        if (iter == scene_index_scene_list_.end())
        {
            return;
        }
    }

    auto& index_obj_list = iter->second;
    auto iter_index = index_obj_list.find(index);
    if (iter_index != index_obj_list.end())
    {
        return;
    }

    AFSimulateScene simulate_scene(scene_obj_id);
    index_obj_list.insert(std::make_pair(index, simulate_scene));

    scene_index_pair_list_.insert(std::make_pair(scene_obj_id, SceneIndexPair(scene_id, index)));
}

void AFCSimulateModule::RemoveSceneObjActors(const std::string& scene_id, const int32_t index)
{
    auto iter = scene_index_scene_list_.find(scene_id);
    if (iter == scene_index_scene_list_.end())
    {
        return;
    }

    auto& index_obj_list = iter->second;
    auto iter_index = index_obj_list.find(index);
    if (iter_index == index_obj_list.end())
    {
        return;
    }

    auto scene_obj_id = iter_index->second.GetSceneObjID();
    scene_index_pair_list_.erase(scene_obj_id);

    index_obj_list.erase(iter_index);
}

AFSimulateScene* AFCSimulateModule::FindSceneObjActors(const std::string& scene_id, const int32_t index)
{
    auto iter = scene_index_scene_list_.find(scene_id);
    if (iter == scene_index_scene_list_.end())
    {
        return nullptr;
    }

    auto& index_obj_list = iter->second;
    auto iter_index = index_obj_list.find(index);
    if (iter_index == index_obj_list.end())
    {
        return nullptr;
    }

    return &iter_index->second;
}

simulate::SimulateVec3 AFCSimulateModule::AFVecToSimulateVec3(const AFPhysic3D& vec3)
{
    return simulate::SimulateVec3(vec3.x, vec3.y, vec3.z);
}

void AFCSimulateModule::TriggerCallBack(const guid_t& scene_obj_id, const AFPhysicTriggerPair& trigger_pair)
{
    auto iter = scene_index_pair_list_.find(scene_obj_id);
    if (iter == scene_index_pair_list_.end())
    {
        ARK_LOG_ERROR("does not find scene");
        return;
    }

    auto& scene_pair = iter->second;
    auto& scene_id = scene_pair.first;
    auto index = scene_pair.second;

    auto simulate_scene = FindSceneObjActors(scene_id, index);
    if (!simulate_scene)
    {
        ARK_LOG_ERROR("does not find scene");
        return;
    }

    if (trigger_call_back_function_)
    {
        simulate::TriggerPair trigger_pair;
        trigger_pair.trigger_obj_id = simulate_scene->FindObj(trigger_pair.trigger_obj_id);
        trigger_pair.other_obj_id = simulate_scene->FindObj(trigger_pair.other_obj_id);

        trigger_call_back_function_(scene_id, index, trigger_pair);
    }
}

} // namespace ark
