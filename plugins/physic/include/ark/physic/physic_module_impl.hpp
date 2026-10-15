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

#include "ark/physic/physic_config_module.hpp"
#include "ark/physic/physic_define.hpp"
#include "ark/physic/physic_module.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/utility/guid_module.hpp"
#include "ark/utility/timer_module.hpp"

namespace ark {

class simulation_event_callback final : public physx::PxSimulationEventCallback
{
public:
    void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs,
        physx::PxU32 nbPairs) override {};

    void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override {
        for (physx::PxU32 i = 0; i < count; i++)
        {
            // ignore pairs when shapes have been deleted
            auto& onePair = pairs[i];
            if (onePair.flags &
                (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
            {
                continue;
            }

            auto actorTrigger = onePair.triggerActor;
            if (!actorTrigger || !actorTrigger->userData)
            {
                continue;
            }

            auto px_scene = actorTrigger->getScene();
            if (!px_scene || !px_scene->userData)
            {
                continue;
            }

            auto scene_obj_id = reinterpret_cast<guid_t>(px_scene->userData);

            physic_trigger_pair trigger_pair;
            trigger_pair.trigger_obj_id = reinterpret_cast<guid_t>(actorTrigger->userData);
            if (onePair.otherActor && onePair.otherActor->userData)
            {
                trigger_pair.other_obj_id = reinterpret_cast<guid_t>(onePair.otherActor->userData);
            }

            for (auto& iter : trigger_cb_list_)
            {
                iter(scene_obj_id, trigger_pair);
            }
        }
    }

    void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) override {}

    void onWake(physx::PxActor**, physx::PxU32) override {}

    void onSleep(physx::PxActor**, physx::PxU32) override {}

    void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) override {}

    void RegisterTriggerCallBack(trigger_call_back_function&& cb) {
        trigger_cb_list_.push_back(std::forward<trigger_call_back_function>(cb));
    }

private:
    using trigger_cb_list = std::list<trigger_call_back_function>;
    trigger_cb_list trigger_cb_list_;
};

class file_output_stream : public physx::PxOutputStream
{
public:
    file_output_stream(const std::string& name) {
        physic::OpenFile(&mFile_, name.c_str(), "wb");
    }

    virtual ~file_output_stream() {
        Release();
    }

    physx::PxU32 write(const void* src, physx::PxU32 count) override {
        return mFile_ ? physx::PxU32(fwrite(src, 1, count, mFile_)) : 0;
    }

    bool Valid() {
        return mFile_ != nullptr;
    }

    void Release() {
        if (mFile_)
        {
            fclose(mFile_);
            mFile_ = nullptr;
        }
    }

private:
    PxFileHandle mFile_{nullptr};
};

///physic scene
class physic_scene_cooked
{
public:
    ~physic_scene_cooked() {
        scene_ = nullptr;
        terrain_mesh_ = nullptr;
        collider_list_.clear();
        collider_triangle_list_.clear();
        collider_convex_list_.clear();
    }

    physic_scene* scene_{nullptr};
    physx::PxTriangleMesh* terrain_mesh_{nullptr};
    std::list<collider*> collider_list_;
    std::map<physx::PxTriangleMesh*, physx::PxVec3> collider_triangle_list_;
    std::map<physx::PxConvexMesh*, physx::PxVec3> collider_convex_list_;
};

class physic_module_impl final : public physic_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;
    bool shut() override;
    bool post_init() override;

    guid_t CreateScene(const std::string& scene_id) override;
    bool DestroyScene(const guid_t& scene_obj_id) override;

    bool RayCast(const guid_t& scene_obj_id, const physic3_d& origin, const physic3_d& direction, const float distance,
        physic_ray_hit& rayHit) override;

    guid_t CreateDynamicBoxActor(const guid_t& scene_obj_id, const physic3_d& position, const physic3_d& size) override;
    guid_t CreateStaticBoxActor(const guid_t& scene_obj_id, const physic3_d& position, const physic3_d& size) override;
    guid_t CreateDynamicCapsuleActor(
        const guid_t& scene_obj_id, const physic3_d& position, const float radius, const float height) override;
    guid_t CreateStaticCapsuleActor(
        const guid_t& scene_obj_id, const physic3_d& position, const float radius, const float height) override;
    guid_t CreateDynamicSphereActor(const guid_t& scene_obj_id, const physic3_d& position, const float radius) override;
    guid_t CreateStaticSphereActor(const guid_t& scene_obj_id, const physic3_d& position, const float radius) override;

    bool UpdateActor(const guid_t& actor_obj_id, const physic3_d& position) override;
    bool DestroyActor(const guid_t& actor_obj_id) override;
    bool GetActorPosition(const guid_t& actor_obj_id, physic3_d& position) override;

    //bool AddChild(physx::PxActor* parentActor, const physx::PxTransform& localParent, physx::PxActor* childActor,
    //    const physx::PxTransform& localChild) override;
    //bool RemoveChild(
    //    physx::PxActor* parentActor, physx::PxActor* childActor, const physx::PxTransform& childTransform) override;

    physic3_d RayGetPoint(const physic3_d& origin, const physic3_d& direction, const float distance) override;

    float GetHeight(const guid_t& scene_obj_id, const physic3_d& position) override;

private:
    ///inner functions
    bool RayCast(const guid_t& scene_obj_id, const physx::PxVec3& origin, const physx::PxVec3& direction,
        const float distance, physx::PxRaycastBuffer& rayHit);

    bool RegisterTriggerCallBack(trigger_call_back_function&& cb) override;

    bool CookSceneMesh(const physic_scene& scene);
    bool FindPhysicScene(const std::string& scene_id, physic_scene_cooked& scene_cooked);

    bool InitPhysic();
    void CreatePvdConnection();
    bool InitScene(physx::PxScene* px_scene, const physic_scene_cooked& scene_cooked);

    physx::PxRigidStatic* CreateStatic(collider* collider);
    physx::PxRigidStatic* CreateStatic(const physx::PxTransform& transform, const physx::PxGeometry& geometry,
        physx::PxMaterial& material, const physx::PxTransform& shapeOffset = physx::PxTransform(physx::PxIdentity));
    physx::PxRigidDynamic* CreateDynamic(const physx::PxTransform& transform, const physx::PxGeometry& geometry,
        physx::PxMaterial& material, float density = DEFAULT_DENSITY,
        const physx::PxTransform& shapeOffset = physx::PxTransform(physx::PxIdentity));

    guid_t CreateDynamicActor(
        physx::PxScene* px_scene, const physx::PxTransform& transform, const physx::PxGeometry& geometry);
    guid_t CreateStaticActor(
        physx::PxScene* px_scene, const physx::PxTransform& transform, const physx::PxGeometry& geometry);

    ///not for use
    physx::PxTriangleMesh* CreateTriangleMesh(const physic_mesh_file& mesh_file);

    ///create mesh then store its pointer for reuse
    physx::PxTriangleMesh* CreateTriangleMesh(const physic_scene& scene_config);
    physx::PxTriangleMesh* CreateTriangleMesh(mesh_collider* collider);
    physx::PxConvexMesh* CreateConvexMesh(mesh_collider* collider);

    void ScenePVDSetting(physx::PxScene* px_scene);
    void QuatRoate(physx::PxTransform& transform);

    void OnSimulate(uint64_t timer_id, const guid_t& guid);

    physx::PxScene* FindScene(const guid_t& scene_obj_id);
    physx::PxActor* FindActor(const guid_t& actor_obj_id);

    physx::PxVec3 AFVector3DToPxVec3(const physic3_d& vec);
    physic3_d PxVec3ToAFVector3D(const physx::PxVec3& vec);

protected:
    physx::PxDefaultAllocator mDefaultAllocatorCallback;
    physx::PxDefaultErrorCallback mDefaultErrorCallback;
    simulation_event_callback mSimulationEventCallback;
    physx::PxPlane mPlane;

    physx::PxMaterial* mMaterial{nullptr};

    physx::PxDefaultCpuDispatcher* mCpuDispatcher{nullptr};
    physx::PxFoundation* mFoundation{nullptr};
    physx::PxPhysics* mPhysics{nullptr};
    physx::PxCooking* mCooking{nullptr};
    physx::PxPvd* mPvd{nullptr};
    physx::PxPvdTransport* mPVDTransport{nullptr};

    ///scene list
    std::map<guid_t, physx::PxScene*> scene_list_;

    ///actor list
    std::map<guid_t, physx::PxActor*> actor_list_;

    ///cooked scene list
    using cooked_scene_list = std::unordered_map<std::string, physic_scene_cooked>;
    cooked_scene_list cooked_scene_list_;

    ///physic config
    physic_config config_;

    physic_config_module* m_pPhysicConfigModule{nullptr};
    timer_module* m_pTimerModule{nullptr};
    guid_module* m_pGUIDModule{nullptr};
};

} // namespace ark
