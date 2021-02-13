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
#include "plugin/physic/interface/AFIPhysicModule.hpp"
#include "plugin/physic/interface/AFIPhysicConfigModule.hpp"
#include "plugin/utility/interface/AFITimerModule.hpp"
#include "plugin/utility/interface/AFIGUIDModule.hpp"
#include "plugin/physic/include/AFPhysicDefine.hpp"

namespace ark {

class AFSimulationEventCallback final : public physx::PxSimulationEventCallback
{
public:
    void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs,
        physx::PxU32 nbPairs) override{};

    void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override
    {
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

            AFPhysicTriggerPair trigger_pair;
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

    void RegisterTriggerCallBack(TRIGGER_CALL_BACK_FUNCTION&& cb)
    {
        trigger_cb_list_.push_back(std::forward<TRIGGER_CALL_BACK_FUNCTION>(cb));
    }

private:
    using TriggerCBList = std::list<TRIGGER_CALL_BACK_FUNCTION>;
    TriggerCBList trigger_cb_list_;
};

class AFFileOutputStream : public physx::PxOutputStream
{
public:
    AFFileOutputStream(const std::string& name)
    {
        physic::OpenFile(&mFile_, name.c_str(), "wb");
    }

    virtual ~AFFileOutputStream()
    {
        Release();
    }

    physx::PxU32 write(const void* src, physx::PxU32 count) override
    {
        return mFile_ ? physx::PxU32(fwrite(src, 1, count, mFile_)) : 0;
    }

    bool Valid()
    {
        return mFile_ != nullptr;
    }

    void Release()
    {
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
class AFPhysicSceneCooked
{
public:
    ~AFPhysicSceneCooked()
    {
        scene_ = nullptr;
        terrain_mesh_ = nullptr;
        collider_list_.clear();
        collider_triangle_list_.clear();
        collider_convex_list_.clear();
    }

    PhysicScene* scene_{nullptr};
    physx::PxTriangleMesh* terrain_mesh_{nullptr};
    std::list<Collider*> collider_list_;
    std::map<physx::PxTriangleMesh*, physx::PxVec3> collider_triangle_list_;
    std::map<physx::PxConvexMesh*, physx::PxVec3> collider_convex_list_;
};

class AFCPhysicModule final : public AFIPhysicModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool Shut() override;
    bool PostInit() override;

    guid_t CreateScene(const std::string& scene_id) override;
    bool DestroyScene(const guid_t& scene_obj_id) override;

    bool RayCast(const guid_t& scene_obj_id, const AFPhysic3D& origin, const AFPhysic3D& direction,
        const float distance, AFPhysicRayHit& rayHit) override;

    guid_t CreateDynamicBoxActor(
        const guid_t& scene_obj_id, const AFPhysic3D& position, const AFPhysic3D& size) override;
    guid_t CreateStaticBoxActor(
        const guid_t& scene_obj_id, const AFPhysic3D& position, const AFPhysic3D& size) override;
    guid_t CreateDynamicCapsuleActor(
        const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius, const float height) override;
    guid_t CreateStaticCapsuleActor(
        const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius, const float height) override;
    guid_t CreateDynamicSphereActor(
        const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius) override;
    guid_t CreateStaticSphereActor(const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius) override;

    bool UpdateActor(const guid_t& actor_obj_id, const AFPhysic3D& position) override;
    bool DestroyActor(const guid_t& actor_obj_id) override;
    bool GetActorPosition(const guid_t& actor_obj_id, AFPhysic3D& position) override;

    //bool AddChild(physx::PxActor* parentActor, const physx::PxTransform& localParent, physx::PxActor* childActor,
    //    const physx::PxTransform& localChild) override;
    //bool RemoveChild(
    //    physx::PxActor* parentActor, physx::PxActor* childActor, const physx::PxTransform& childTransform) override;

    AFPhysic3D RayGetPoint(const AFPhysic3D& origin, const AFPhysic3D& direction, const float distance) override;

    float GetHeight(const guid_t& scene_obj_id, const AFPhysic3D& position) override;

private:
    ///inner functions
    bool RayCast(const guid_t& scene_obj_id, const physx::PxVec3& origin, const physx::PxVec3& direction,
        const float distance, physx::PxRaycastBuffer& rayHit);

    bool RegisterTriggerCallBack(TRIGGER_CALL_BACK_FUNCTION&& cb) override;

    bool CookSceneMesh(const PhysicScene& scene);
    bool FindPhysicScene(const std::string& scene_id, AFPhysicSceneCooked& scene_cooked);

    bool InitPhysic();
    void CreatePvdConnection();
    bool InitScene(physx::PxScene* px_scene, const AFPhysicSceneCooked& scene_cooked);

    physx::PxRigidStatic* CreateStatic(Collider* collider);
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
    physx::PxTriangleMesh* CreateTriangleMesh(const PhysicMeshFile& mesh_file);

    ///create mesh then store its pointer for reuse
    physx::PxTriangleMesh* CreateTriangleMesh(const PhysicScene& scene_config);
    physx::PxTriangleMesh* CreateTriangleMesh(MeshCollider* collider);
    physx::PxConvexMesh* CreateConvexMesh(MeshCollider* collider);

    void ScenePVDSetting(physx::PxScene* px_scene);
    void QuatRoate(physx::PxTransform& transform);

    void OnSimulate(uint64_t timer_id, const guid_t& guid);

    physx::PxScene* FindScene(const guid_t& scene_obj_id);
    physx::PxActor* FindActor(const guid_t& actor_obj_id);

    physx::PxVec3 AFVector3DToPxVec3(const AFPhysic3D& vec);
    AFPhysic3D PxVec3ToAFVector3D(const physx::PxVec3& vec);

protected:
    physx::PxDefaultAllocator mDefaultAllocatorCallback;
    physx::PxDefaultErrorCallback mDefaultErrorCallback;
    AFSimulationEventCallback mSimulationEventCallback;
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
    using CookedSceneList = std::unordered_map<std::string, AFPhysicSceneCooked>;
    CookedSceneList cooked_scene_list_;

    ///physic config
    PhysicConfig config_;

    AFIPhysicConfigModule* m_pPhysicConfigModule{nullptr};
    AFITimerModule* m_pTimerModule{nullptr};
    AFIGUIDModule* m_pGUIDModule{nullptr};
};

} // namespace ark
