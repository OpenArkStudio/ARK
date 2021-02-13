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

namespace ark {

///3d position
class AFBaseVec3D
{
public:
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};

    AFBaseVec3D() = default;

    AFBaseVec3D(float a, float b, float c)
        : x(a)
        , y(b)
        , z(c)
    {
    }

    AFBaseVec3D(const AFBaseVec3D& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
    }

    const AFBaseVec3D& operator=(const AFBaseVec3D& rhs)
    {
        if (this != &rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
        }
        return *this;
    }
};
using AFPhysic3D = AFBaseVec3D;

///physic ray hit
struct AFPhysicRayHit
{
    bool is_hit{false};
    float hit_distance{0.0F};
    guid_t hit_obj_id{0U};
    AFPhysic3D hit_pos;
};

///physic trigger pair
struct AFPhysicTriggerPair
{
    guid_t trigger_obj_id{0U};
    guid_t other_obj_id{0U};
};

///call back function
using TRIGGER_CALL_BACK_FUNCTION = std::function<void(const guid_t&, const AFPhysicTriggerPair&)>;

class AFIPhysicModule : public AFIModule
{
public:
    ///create a new scene
    virtual guid_t CreateScene(const std::string& scene_id) = 0;

    ///destroy a scene
    virtual bool DestroyScene(const guid_t& scene_obj_id) = 0;

    ///raycast
    virtual bool RayCast(const guid_t& scene_obj_id, const AFPhysic3D& origin, const AFPhysic3D& direction,
        const float distance, AFPhysicRayHit& rayHit) = 0;

    ///create a box actor
    virtual guid_t CreateDynamicBoxActor(
        const guid_t& scene_obj_id, const AFPhysic3D& position, const AFPhysic3D& size) = 0;
    virtual guid_t CreateStaticBoxActor(
        const guid_t& scene_obj_id, const AFPhysic3D& position, const AFPhysic3D& size) = 0;

    ///create a capsule actor
    virtual guid_t CreateDynamicCapsuleActor(
        const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius, const float height) = 0;
    virtual guid_t CreateStaticCapsuleActor(
        const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius, const float height) = 0;

    ///create a sphere actor
    virtual guid_t CreateDynamicSphereActor(
        const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius) = 0;
    virtual guid_t CreateStaticSphereActor(
        const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius) = 0;

    ///update actor
    virtual bool UpdateActor(const guid_t& actor_obj_id, const AFPhysic3D& position) = 0;

    ///destroy an actor
    virtual bool DestroyActor(const guid_t& actor_obj_id) = 0;

    ///get actor position
    virtual bool GetActorPosition(const guid_t& actor_obj_id, AFPhysic3D& position) = 0;

    ///add a child actor
    //virtual bool AddChild(physx::PxActor* parentActor, const physx::PxTransform& localParent,
    //    physx::PxActor* childActor, const physx::PxTransform& localChild) = 0;

    ///remove a child actor
    //virtual bool RemoveChild(
    //    physx::PxActor* parentActor, physx::PxActor* childActor, const physx::PxTransform& childTransform) = 0;

    ///register trigger call back
    template<typename BaseType>
    bool RegisterTriggerCallBack(BaseType* pBase, void (BaseType::*handler)(const guid_t&, const AFPhysicTriggerPair&))
    {
        auto functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2);
        return RegisterTriggerCallBack(std::move(functor));
    }

    ///ray get point
    virtual AFPhysic3D RayGetPoint(const AFPhysic3D& origin, const AFPhysic3D& direction, const float distance) = 0;

    ///get height
    virtual float GetHeight(const guid_t& scene_obj_id, const AFPhysic3D& position) = 0;

private:
    virtual bool RegisterTriggerCallBack(TRIGGER_CALL_BACK_FUNCTION&& cb) = 0;
};

} // namespace ark
