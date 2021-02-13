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

#include "plugin/physic/include/AFCPhysicModule.hpp"
#include "plugin/physic/include/AFPhysicDefine.hpp"
#include "plugin/physic/include/AFWaveFront.hpp"

namespace ark {

#define SAFE_RELEASE(x)                                                                                                \
    if (x)                                                                                                             \
    {                                                                                                                  \
        x->release();                                                                                                  \
        x = NULL;                                                                                                      \
    }

#define SCENE_WRITE_LOCK(x) physx::PxSceneWriteLock writeLock(*x);

bool AFCPhysicModule::Init()
{
    m_pPhysicConfigModule = FindModule<AFIPhysicConfigModule>();
    m_pTimerModule = FindModule<AFITimerModule>();
    m_pGUIDModule = FindModule<AFIGUIDModule>();

    return true;
}

bool AFCPhysicModule::Shut()
{
    cooked_scene_list_.clear();

    for (auto& iter : scene_list_)
    {
        SAFE_RELEASE(iter.second);
    }
    scene_list_.clear();

    PxCloseExtensions();

    SAFE_RELEASE(mCooking);
    SAFE_RELEASE(mPhysics);
    SAFE_RELEASE(mPvd);
    SAFE_RELEASE(mPVDTransport);
    SAFE_RELEASE(mCpuDispatcher);
    SAFE_RELEASE(mFoundation);

    return true;
}

bool AFCPhysicModule::PostInit()
{
    ///store physic config
    config_ = m_pPhysicConfigModule->GetPhysicConfig();
    if (config_.step_ <= 0.0001)
    {
        ARK_LOG_ERROR("physic simulate step <= 0");
        exit(0);
    }

    ///init physic
    if (!InitPhysic())
    {
        ARK_LOG_ERROR("physic init failed");
        exit(0);
    }

    ///cook mesh
    cooked_scene_list_.clear();
    auto& scene_list = m_pPhysicConfigModule->GetPhysicSceneList();
    for (auto& iter : scene_list)
    {
        auto& scene = iter.second;
        if (!CookSceneMesh(scene))
        {
            ARK_LOG_ERROR("cook physic scene failed scene id = {}", scene.scene_id_);
            exit(0);
        }
    }

    ///add simulate timer
    if (config_.bSimulate)
    {
        std::chrono::milliseconds interval((int)(config_.step_ * 1000));
        m_pTimerModule->AddForeverTimer(guid_t(), interval, this, &AFCPhysicModule::OnSimulate);
    }

    return true;
}

guid_t AFCPhysicModule::CreateScene(const std::string& scene_id)
{
    AFPhysicSceneCooked scene_cooked;
    if (!FindPhysicScene(scene_id, scene_cooked))
    {
        ARK_LOG_ERROR("create scene failed scene id = {}", scene_id);
        return NULL_GUID;
    }

    ///generate guid
    auto guid = m_pGUIDModule->CreateGUID();
    if (guid == NULL_GUID)
    {
        ARK_LOG_ERROR("create scene failed scene id = {}", scene_id);
        return NULL_GUID;
    }

    ///create physic scene
    physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
    sceneDesc.simulationEventCallback = &mSimulationEventCallback;
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = mCpuDispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

    auto px_scene = mPhysics->createScene(sceneDesc);
    if (px_scene == nullptr)
    {
        ARK_LOG_ERROR("create scene failed");
        return NULL_GUID;
    }

    if (!InitScene(px_scene, scene_cooked))
    {
        ARK_LOG_ERROR("init scene failed scene");
        SAFE_RELEASE(px_scene)
        return NULL_GUID;
    }

    ///for pvd
    ScenePVDSetting(px_scene);

    ///save to list
    px_scene->userData = (void*)guid_t(guid);
    scene_list_.insert(std::make_pair(guid, px_scene));
    return guid;
}

bool AFCPhysicModule::DestroyScene(const guid_t& scene_obj_id)
{
    ///check whether the particular scene exists
    auto px_scene = FindScene(scene_obj_id);
    if (px_scene == nullptr)
    {
        ARK_LOG_ERROR("scene does not exist");
        return false;
    }

    SAFE_RELEASE(px_scene);

    scene_list_.erase(scene_obj_id);
    return true;
}

bool AFCPhysicModule::RayCast(const guid_t& scene_obj_id, const AFPhysic3D& origin, const AFPhysic3D& direction,
    const float distance, AFPhysicRayHit& rayHit)
{
    auto px_origin = AFVector3DToPxVec3(origin);
    auto px_direction = AFVector3DToPxVec3(direction);

    physx::PxRaycastBuffer px_ray_hit;
    auto result = RayCast(scene_obj_id, px_origin, px_direction, distance, px_ray_hit);
    if (result)
    {
        auto hitActor = px_ray_hit.block.actor;
        if (hitActor && hitActor->userData)
        {
            rayHit.hit_obj_id = reinterpret_cast<guid_t>(hitActor->userData);
        }

        rayHit.is_hit = result;
        rayHit.hit_distance = px_ray_hit.block.distance;
        rayHit.hit_pos = PxVec3ToAFVector3D(px_ray_hit.block.position);
    }

    return result;
}

bool AFCPhysicModule::RayCast(const guid_t& scene_obj_id, const physx::PxVec3& origin, const physx::PxVec3& direction,
    const float distance, physx::PxRaycastBuffer& rayHit)
{
    auto px_scene = FindScene(scene_obj_id);
    if (px_scene == nullptr)
    {
        ARK_LOG_ERROR("this scene does not exist");
        return false;
    }

    auto normalDir = direction;
    if (!normalDir.isNormalized())
    {
        normalDir = normalDir.getNormalized();
    }

    return px_scene->raycast(origin, normalDir, distance, rayHit);
}

guid_t AFCPhysicModule::CreateDynamicBoxActor(
    const guid_t& scene_obj_id, const AFPhysic3D& position, const AFPhysic3D& size)
{
    auto px_scene = FindScene(scene_obj_id);
    if (px_scene == nullptr)
    {
        ARK_LOG_ERROR("this scene does not exist");
        return NULL_GUID;
    }

    auto px_size = AFVector3DToPxVec3(size);
    auto px_position = AFVector3DToPxVec3(position);

    physx::PxBoxGeometry geometry(px_size * 0.5F);
    auto transform = physx::PxTransform(px_position);
    return CreateDynamicActor(px_scene, transform, geometry);
}

guid_t AFCPhysicModule::CreateStaticBoxActor(
    const guid_t& scene_obj_id, const AFPhysic3D& position, const AFPhysic3D& size)
{
    auto px_scene = FindScene(scene_obj_id);
    if (px_scene == nullptr)
    {
        ARK_LOG_ERROR("this scene does not exist");
        return NULL_GUID;
    }

    auto px_size = AFVector3DToPxVec3(size);
    auto px_position = AFVector3DToPxVec3(position);

    physx::PxBoxGeometry geometry(px_size * 0.5F);
    auto transform = physx::PxTransform(px_position);
    return CreateStaticActor(px_scene, transform, geometry);
}

guid_t AFCPhysicModule::CreateDynamicCapsuleActor(
    const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius, const float height)
{
    auto px_scene = FindScene(scene_obj_id);
    if (px_scene == nullptr)
    {
        ARK_LOG_ERROR("this scene does not exist");
        return NULL_GUID;
    }

    physx::PxCapsuleGeometry geometry(radius, height * 0.5F);

    auto px_position = AFVector3DToPxVec3(position);
    auto transform = physx::PxTransform(px_position);
    QuatRoate(transform);
    return CreateDynamicActor(px_scene, transform, geometry);
}

guid_t AFCPhysicModule::CreateStaticCapsuleActor(
    const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius, const float height)
{
    auto px_scene = FindScene(scene_obj_id);
    if (px_scene == nullptr)
    {
        ARK_LOG_ERROR("this scene does not exist scene id={} index={}");
        return NULL_GUID;
    }

    physx::PxCapsuleGeometry geometry(radius, height * 0.5F);

    auto px_position = AFVector3DToPxVec3(position);
    auto transform = physx::PxTransform(px_position);
    QuatRoate(transform);
    return CreateStaticActor(px_scene, transform, geometry);
}

guid_t AFCPhysicModule::CreateDynamicSphereActor(
    const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius)
{
    auto px_scene = FindScene(scene_obj_id);
    if (px_scene == nullptr)
    {
        ARK_LOG_ERROR("this scene does not exist");
        return NULL_GUID;
    }

    physx::PxSphereGeometry geometry(radius);

    auto px_position = AFVector3DToPxVec3(position);
    auto transform = physx::PxTransform(px_position);
    return CreateDynamicActor(px_scene, transform, geometry);
}

guid_t AFCPhysicModule::CreateStaticSphereActor(
    const guid_t& scene_obj_id, const AFPhysic3D& position, const float radius)
{
    auto px_scene = FindScene(scene_obj_id);
    if (px_scene == nullptr)
    {
        ARK_LOG_ERROR("this scene does not exist");
        return NULL_GUID;
    }

    physx::PxSphereGeometry geometry(radius);

    auto px_position = AFVector3DToPxVec3(position);
    auto transform = physx::PxTransform(px_position);
    return CreateStaticActor(px_scene, transform, geometry);
}

guid_t AFCPhysicModule::CreateDynamicActor(
    physx::PxScene* px_scene, const physx::PxTransform& transform, const physx::PxGeometry& geometry)
{
    SCENE_WRITE_LOCK(px_scene);

    auto actor = CreateDynamic(transform, geometry, *mMaterial);
    if (actor == nullptr)
    {
        ARK_LOG_ERROR("create actor failed");
        return NULL_GUID;
    }

    px_scene->addActor(*actor);

    ///generate guid
    auto guid = m_pGUIDModule->CreateGUID();
    actor->userData = (void*)guid_t(guid);
    actor_list_.insert(std::make_pair(guid, actor));
    return guid;
}

guid_t AFCPhysicModule::CreateStaticActor(
    physx::PxScene* px_scene, const physx::PxTransform& transform, const physx::PxGeometry& geometry)
{
    SCENE_WRITE_LOCK(px_scene);

    auto actor = CreateStatic(transform, geometry, *mMaterial);
    if (actor == nullptr)
    {
        ARK_LOG_ERROR("create actor failed");
        return NULL_GUID;
    }

    px_scene->addActor(*actor);

    ///generate guid
    auto guid = m_pGUIDModule->CreateGUID();
    actor->userData = (void*)guid_t(guid);
    actor_list_.insert(std::make_pair(guid, actor));
    return guid;
}

bool AFCPhysicModule::UpdateActor(const guid_t& actor_obj_id, const AFPhysic3D& position)
{
    auto actor = FindActor(actor_obj_id);
    if (actor == nullptr)
    {
        ARK_LOG_ERROR("actor does not exist actor id = {}", actor_obj_id);
        return false;
    }

    auto rigid_dynamic = actor->is<physx::PxRigidDynamic>();
    if (rigid_dynamic == nullptr)
    {
        ARK_LOG_ERROR("actor can not be updated");
        return false;
    }

    auto px_scene = actor->getScene();
    if (!px_scene)
    {
        ARK_LOG_ERROR("scene is null");
        return false;
    }

    SCENE_WRITE_LOCK(px_scene);

    physx::PxTransform transform = rigid_dynamic->getGlobalPose();

    auto px_position = AFVector3DToPxVec3(position);
    transform.p = px_position;
    if (config_.bSimulate)
    {
        rigid_dynamic->setKinematicTarget(transform);
    }
    else
    {
        rigid_dynamic->setGlobalPose(transform);
    }

    return true;
}

bool AFCPhysicModule::DestroyActor(const guid_t& actor_obj_id)
{
    auto actor = FindActor(actor_obj_id);
    if (actor == nullptr)
    {
        ARK_LOG_ERROR("actor does not exist actor id = {}", actor_obj_id);
        return false;
    }

    auto px_scene = actor->getScene();
    if (!px_scene)
    {
        ARK_LOG_ERROR("scene is null");
        return false;
    }

    SCENE_WRITE_LOCK(px_scene);

    actor->release();

    actor_list_.erase(actor_obj_id);
    return true;
}

bool AFCPhysicModule::GetActorPosition(const guid_t& actor_obj_id, AFPhysic3D& position)
{
    auto actor = FindActor(actor_obj_id);
    if (actor == nullptr)
    {
        ARK_LOG_ERROR("actor does not exist actor id = {}", actor_obj_id);
        return false;
    }

    auto rigid_actor = actor->is<physx::PxRigidActor>();
    if (rigid_actor == nullptr)
    {
        ARK_LOG_ERROR("actor does not exist actor id = {}", actor_obj_id);
        return false;
    }

    position = PxVec3ToAFVector3D(rigid_actor->getGlobalPose().p);
    return true;
}

//bool AFCPhysicModule::AddChild(physx::PxActor* parentActor, const physx::PxTransform& localParent,
//    physx::PxActor* childActor, const physx::PxTransform& localChild)
//{
//    if (!parentActor || !childActor)
//    {
//        ARK_LOG_ERROR("actor is null");
//        return false;
//    }
//
//    if (parentActor->getScene() != childActor->getScene())
//    {
//        ARK_LOG_ERROR("actors are not in the same scene");
//        return false;
//    }
//
//    auto rigidParent = parentActor->is<physx::PxRigidActor>();
//    auto rigidChild = childActor->is<physx::PxRigidActor>();
//    if (!rigidParent || !rigidChild)
//    {
//        ARK_LOG_ERROR("actor is not rigid actor");
//        return false;
//    }
//
//    auto numShapesChild = rigidChild->getNbShapes();
//    auto shapesChild = ARK_NEW_ARRAY(physx::PxShape*, numShapesChild);
//    for (physx::PxU32 i = 0; i < numShapesChild; i++)
//    {
//        //
//    }
//    ARK_DELETE_ARRAY(physx::PxShape*, shapesChild);
//
//    return true;
//}
//
//bool AFCPhysicModule::RemoveChild(
//    physx::PxActor* parentActor, physx::PxActor* childActor, const physx::PxTransform& childTransform)
//{
//    return false;
//}

AFPhysic3D AFCPhysicModule::RayGetPoint(const AFPhysic3D& origin, const AFPhysic3D& direction, const float distance)
{
    auto px_origin = AFVector3DToPxVec3(origin);
    auto px_direction = AFVector3DToPxVec3(direction);

    auto normalDir = px_direction;
    if (!px_direction.isNormalized())
    {
        normalDir = px_direction.getNormalized();
    }

    auto destPoint = px_origin + normalDir * distance;
    return PxVec3ToAFVector3D(destPoint);
}

float AFCPhysicModule::GetHeight(const guid_t& scene_obj_id, const AFPhysic3D& position)
{
    float distance = 100.0F;

    ///firstly ray cast down
    auto px_position = AFVector3DToPxVec3(position);
    physx::PxVec3 dir(0.0F, -1.0F, 0.0F);
    physx::PxRaycastBuffer rayHit;
    auto result = RayCast(scene_obj_id, px_position, dir, distance, rayHit);
    if (result)
    {
        return rayHit.block.position.y;
    }

    ///then ray cast up
    dir.y = -dir.y;
    result = RayCast(scene_obj_id, px_position, dir, distance, rayHit);
    if (result)
    {
        return rayHit.block.position.y;
    }

    return position.y;
}

bool AFCPhysicModule::RegisterTriggerCallBack(TRIGGER_CALL_BACK_FUNCTION&& cb)
{
    mSimulationEventCallback.RegisterTriggerCallBack(std::forward<TRIGGER_CALL_BACK_FUNCTION>(cb));
    return true;
}

bool AFCPhysicModule::CookSceneMesh(const PhysicScene& scene)
{
    AFPhysicSceneCooked scene_cooked;

    ///cook terrain mesh
    auto terrain_mesh = CreateTriangleMesh(scene);
    if (terrain_mesh == nullptr)
    {
        ARK_LOG_ERROR("mesh is null");
        return false;
    }

    scene_cooked.terrain_mesh_ = terrain_mesh;

    for (auto& iter_collider : scene.collider_list_)
    {
        auto collider = iter_collider;
        if (collider->IsMeshCollider())
        {
            ///cook scene object mesh
            auto mesh_collider = dynamic_cast<MeshCollider*>(collider);
            if (!mesh_collider)
            {
                ARK_LOG_ERROR("mesh is null");
                return false;
            }

            if (mesh_collider->bConvex)
            {
                auto collider_mesh = CreateConvexMesh(mesh_collider);
                if (!collider_mesh)
                {
                    ARK_LOG_ERROR("mesh is null");
                    return false;
                }

                scene_cooked.collider_convex_list_.insert(std::make_pair(collider_mesh, mesh_collider->position_));
            }
            else
            {
                auto collider_mesh = CreateTriangleMesh(mesh_collider);
                if (!collider_mesh)
                {
                    ARK_LOG_ERROR("mesh is null");
                    return false;
                }

                scene_cooked.collider_triangle_list_.insert(std::make_pair(collider_mesh, mesh_collider->position_));
            }

            continue;
        }

        scene_cooked.collider_list_.push_back(collider);
    }

    scene_cooked.scene_ = &const_cast<PhysicScene&>(scene);
    cooked_scene_list_.insert(std::make_pair(scene_cooked.scene_->scene_id_, scene_cooked));

    ARK_LOG_DEBUG("scene cooked scene id = {}", scene.scene_id_);

    return true;
}

bool AFCPhysicModule::FindPhysicScene(const std::string& scene_id, AFPhysicSceneCooked& scene_cooked)
{
    auto iter = cooked_scene_list_.find(scene_id);
    if (iter == cooked_scene_list_.end())
    {
        return false;
    }

    scene_cooked = iter->second;
    return true;
}

bool AFCPhysicModule::InitPhysic()
{
    mDefaultAllocatorCallback = physx::PxDefaultAllocator();
    mDefaultErrorCallback = physx::PxDefaultErrorCallback();
    physx::PxAllocatorCallback* allocator = &mDefaultAllocatorCallback;
    mFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, *allocator, mDefaultErrorCallback);
    ARK_ASSERT_RET_VAL(mFoundation, false);

    ///create pvd for debug
    CreatePvdConnection();

    physx::PxTolerancesScale scale;

#if PX_SUPPORT_PVD
    bool recordMemoryAllocations = true;
#else
    bool recordMemoryAllocations = false;
#endif

    ///create physx
    mPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *mFoundation, scale, recordMemoryAllocations, mPvd);
    ARK_ASSERT_RET_VAL(mPhysics, false);

    ///register particles
    PxRegisterParticles(*mPhysics);

    bool result = PxInitExtensions(*mPhysics, mPvd);
    ARK_ASSERT_RET_VAL(result, false);

    ///create cooking
    physx::PxCookingParams params(scale);
    params.meshWeldTolerance = 0.001f;
    params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);
    params.buildGPUData = false;
    mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, params);
    ARK_ASSERT_RET_VAL(mCooking, false);

    ///create dispatcher
    mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(config_.dispatcher_treads_num_);
    ARK_ASSERT_RET_VAL(mCpuDispatcher, false);

    ///create default material
    mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.5f);

    mSimulationEventCallback = AFSimulationEventCallback();

    ///create plane
    mPlane = physx::PxPlane(0, 1, 0, 0);

    return true;
}

void AFCPhysicModule::CreatePvdConnection()
{
#if PX_SUPPORT_PVD
    mPVDTransport =
        physx::PxDefaultPvdSocketTransportCreate(config_.pvd_host.c_str(), config_.pvd_port, config_.pvd_timeout);
    if (mPVDTransport == nullptr)
    {
        ARK_LOG_ERROR("create pvd socket failed");
        return;
    }

    mPvd = physx::PxCreatePvd(*mFoundation);
    mPvd->connect(*mPVDTransport, physx::PxPvdInstrumentationFlag::eALL);
#endif
}

bool AFCPhysicModule::InitScene(physx::PxScene* px_scene, const AFPhysicSceneCooked& scene_cooked)
{
    auto& scene = *scene_cooked.scene_;
    if (!scene_cooked.terrain_mesh_)
    {
        ARK_LOG_ERROR("scene terrain mesh do not exist scene id = {}", scene.scene_id_);
        return false;
    }

    ///create plane
    auto plane = physx::PxCreatePlane(*mPhysics, mPlane, *mMaterial);
    px_scene->addActor(*plane);

    ///create terrain
    auto terrain_geometry = physx::PxTriangleMeshGeometry(scene_cooked.terrain_mesh_);
    auto terrain_transform = physx::PxTransform(physx::PxIdentity);
    terrain_transform.q = physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 1, 0)); ///different from unity
    auto terrain_actor = CreateStatic(terrain_transform, terrain_geometry, *mMaterial);
    px_scene->addActor(*terrain_actor);

    ///create actors
    for (const auto& iter : scene_cooked.collider_list_)
    {
        const auto collider = iter;
        auto actor = CreateStatic(collider);
        if (!actor)
        {
            ARK_LOG_ERROR("create actor failed scene id={} file={}", scene.scene_id_, scene.mesh_file_.path_);
            continue;
        }

        if (collider->bTrigger_)
        {
            physx::PxShape* shape = nullptr;
            actor->getShapes(&shape, 1);
            if (shape)
            {
                shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
                shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
            }
        }

        px_scene->addActor(*actor);
    }

    ///create triangle actors
    for (const auto& iter : scene_cooked.collider_triangle_list_)
    {
        auto mesh = iter.first;
        auto& position = iter.second;
        if (!mesh)
        {
            ARK_LOG_ERROR("create actor failed scene id={}", scene.scene_id_);
            continue;
        }

        auto geometry = physx::PxTriangleMeshGeometry(mesh);
        auto actor = CreateStatic(physx::PxTransform(position), geometry, *mMaterial);
        if (!actor)
        {
            ARK_LOG_ERROR("create actor failed scene id={} file={}", scene.scene_id_);
            continue;
        }

        px_scene->addActor(*actor);
    }

    ///create convex actors
    for (const auto& iter : scene_cooked.collider_convex_list_)
    {
        auto mesh = iter.first;
        auto& position = iter.second;
        if (!mesh)
        {
            ARK_LOG_ERROR("create actor failed scene id={}", scene.scene_id_);
            continue;
        }

        auto geometry = physx::PxConvexMeshGeometry(mesh);
        auto actor = CreateStatic(physx::PxTransform(position), geometry, *mMaterial);
        if (!actor)
        {
            ARK_LOG_ERROR("create actor failed scene id={} file={}", scene.scene_id_);
            continue;
        }

        px_scene->addActor(*actor);
    }

    return true;
}

physx::PxRigidStatic* AFCPhysicModule::CreateStatic(Collider* collider)
{
    if (collider == nullptr)
    {
        ARK_LOG_ERROR("collider is null");
        return nullptr;
    }

    if (collider->IsBoxCollider())
    {
        auto real_collider = dynamic_cast<BoxCollider*>(collider);
        if (real_collider == nullptr)
        {
            ARK_LOG_ERROR("collider to BoxCollider failed");
            return nullptr;
        }

        auto half = real_collider->size_ * 0.5F;
        physx::PxBoxGeometry geometry(half);
        auto transform = physx::PxTransform(real_collider->position_);
        return CreateStatic(transform, geometry, *mMaterial);
    }
    else if (collider->IsCapsuleCollider())
    {
        auto real_collider = dynamic_cast<CapsuleCollider*>(collider);
        if (real_collider == nullptr)
        {
            ARK_LOG_ERROR("collider to CapsuleCollider failed");
            return nullptr;
        }

        auto transform = physx::PxTransform(real_collider->position_);
        QuatRoate(transform);

        physx::PxCapsuleGeometry geometry(real_collider->radius_, real_collider->height_ * 0.5F);
        return CreateStatic(transform, geometry, *mMaterial);
    }
    else if (collider->IsSphereCollider())
    {
        auto real_collider = dynamic_cast<SphereCollider*>(collider);
        if (real_collider == nullptr)
        {
            ARK_LOG_ERROR("collider to SphereCollider failed");
            return nullptr;
        }

        physx::PxSphereGeometry geometry(real_collider->radius_);
        auto transform = physx::PxTransform(real_collider->position_);
        return CreateStatic(transform, geometry, *mMaterial);
    }

    return nullptr;
}

physx::PxRigidStatic* AFCPhysicModule::CreateStatic(const physx::PxTransform& transform,
    const physx::PxGeometry& geometry, physx::PxMaterial& material, const physx::PxTransform& shapeOffset)
{
    if (!transform.isValid() || !shapeOffset.isValid())
    {
        ARK_LOG_ERROR("transform invalid");
        return nullptr;
    }

    auto shape = mPhysics->createShape(geometry, material, true);
    if (!shape)
    {
        ARK_LOG_ERROR("createa shape failed");
        return nullptr;
    }

    shape->setLocalPose(shapeOffset);

    auto actor = mPhysics->createRigidStatic(transform);
    if (actor)
    {
        actor->attachShape(*shape);
    }

    shape->release();
    return actor;
}

physx::PxRigidDynamic* AFCPhysicModule::CreateDynamic(const physx::PxTransform& transform,
    const physx::PxGeometry& geometry, physx::PxMaterial& material, float density,
    const physx::PxTransform& shapeOffset)
{
    if (!transform.isValid() || !shapeOffset.isValid())
    {
        ARK_LOG_ERROR("transform invalid");
        return nullptr;
    }

    auto shape = mPhysics->createShape(geometry, material, true);
    if (!shape)
    {
        ARK_LOG_ERROR("createa shape failed");
        return nullptr;
    }

    shape->setLocalPose(shapeOffset);

    auto actor = mPhysics->createRigidDynamic(transform);
    if (actor)
    {
        actor->attachShape(*shape);
        physx::PxRigidBodyExt::updateMassAndInertia(*actor, density);

        if (config_.bSimulate)
        {
            actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
        }
    }

    shape->release();
    return actor;
}

physx::PxTriangleMesh* AFCPhysicModule::CreateTriangleMesh(const PhysicMeshFile& mesh_file)
{
    physx::PxTriangleMesh* mesh = nullptr;

    physx::PxDefaultFileInputData file(mesh_file.path_cooked_.c_str());
    if (file.isValid())
    {
        mesh = mPhysics->createTriangleMesh(file);
    }

    if (!mesh)
    {
        ///need cook
        AFWaveFrontObj wfo;
        if (wfo.LoadObj(mesh_file.path_.c_str(), false) == 0)
        {
            ARK_LOG_ERROR("load file failed file={}", mesh_file.path_);
            return nullptr;
        }

        physx::PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = wfo.mVertexCount;
        meshDesc.triangles.count = wfo.mTriCount;
        meshDesc.points.stride = sizeof(float) * 3;
        meshDesc.triangles.stride = sizeof(int) * 3;
        meshDesc.points.data = wfo.mVertices;
        meshDesc.triangles.data = wfo.mIndices;

        AFFileOutputStream out_file(mesh_file.path_cooked_.c_str());
        if (!out_file.Valid())
        {
            ARK_LOG_ERROR("load file failed file={}", mesh_file.path_cooked_);
            return nullptr;
        }

        bool bResult = mCooking->cookTriangleMesh(meshDesc, out_file);
        if (bResult)
        {
            out_file.Release();

            physx::PxDefaultFileInputData cooked_file(mesh_file.path_cooked_.c_str());
            if (!cooked_file.isValid())
            {
                ARK_LOG_ERROR("load file failed file={}", mesh_file.path_cooked_);
                return nullptr;
            }

            mesh = mPhysics->createTriangleMesh(cooked_file);
        }
    }

    return mesh;
}

physx::PxTriangleMesh* AFCPhysicModule::CreateTriangleMesh(const PhysicScene& scene_config)
{
    auto& mesh_file = scene_config.mesh_file_;

    AFWaveFrontObj wfo;
    if (wfo.LoadObj(mesh_file.path_.c_str(), false) == 0)
    {
        ARK_LOG_ERROR("load file failed file={}", mesh_file.path_);
        return nullptr;
    }

    physx::PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = wfo.mVertexCount;
    meshDesc.triangles.count = wfo.mTriCount;
    meshDesc.points.stride = sizeof(float) * 3;
    meshDesc.triangles.stride = sizeof(int) * 3;
    meshDesc.points.data = wfo.mVertices;
    meshDesc.triangles.data = wfo.mIndices;

    physx::PxTriangleMeshCookingResult::Enum result{physx::PxTriangleMeshCookingResult::eFAILURE};
    auto mesh = mCooking->createTriangleMesh(meshDesc, mPhysics->getPhysicsInsertionCallback(), &result);
    if (result != physx::PxTriangleMeshCookingResult::eSUCCESS)
    {
        ARK_LOG_WARN("triangle mesh is too big scene ={}", mesh_file.path_);
    }

    return mesh;
}

physx::PxTriangleMesh* AFCPhysicModule::CreateTriangleMesh(MeshCollider* collider)
{
    if (collider->vertices_num_ == 0 || !collider->vertices_ || collider->triangles_num_ == 0 || !collider->triangles_)
    {
        ARK_LOG_ERROR("mesh collider do not have vertices");
        return nullptr;
    }

    physx::PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = collider->vertices_num_;
    meshDesc.triangles.count = collider->triangles_num_ / 3;
    meshDesc.points.stride = sizeof(float) * 3;
    meshDesc.triangles.stride = sizeof(int) * 3;
    meshDesc.points.data = collider->vertices_;
    meshDesc.triangles.data = collider->triangles_;

    auto mesh = mCooking->createTriangleMesh(meshDesc, mPhysics->getPhysicsInsertionCallback());
    return mesh;
}

physx::PxConvexMesh* AFCPhysicModule::CreateConvexMesh(MeshCollider* collider)
{
    if (collider->vertices_num_ == 0 || !collider->vertices_)
    {
        ARK_LOG_ERROR("mesh collider do not have vertices");
        return nullptr;
    }

    physx::PxConvexMeshDesc meshDesc;
    meshDesc.points.count = collider->vertices_num_;
    meshDesc.points.stride = sizeof(float) * 3;
    meshDesc.points.data = collider->vertices_;
    meshDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

    auto mesh = mCooking->createConvexMesh(meshDesc, mPhysics->getPhysicsInsertionCallback());
    return mesh;
}

void AFCPhysicModule::ScenePVDSetting(physx::PxScene* px_scene)
{
    auto pvd_client = px_scene->getScenePvdClient();
    if (pvd_client)
    {
        pvd_client->setScenePvdFlags(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS |
                                     physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES |
                                     physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS);

        pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);

        pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

void AFCPhysicModule::QuatRoate(physx::PxTransform& transform)
{
    auto plane_transform = physx::PxTransformFromPlaneEquation(mPlane);
    transform.q = plane_transform.q;
}

void AFCPhysicModule::OnSimulate(uint64_t timer_id, const guid_t& guid)
{
    for (auto& iter : scene_list_)
    {
        auto px_scene = iter.second;
        if (!px_scene)
        {
            continue;
        }

        physx::PxSceneWriteLock writeLockSimulate(*px_scene);
        px_scene->simulate(config_.step_);

        physx::PxSceneWriteLock writeLockFetch(*px_scene);
        px_scene->fetchResults(true);
    }
}

physx::PxScene* AFCPhysicModule::FindScene(const guid_t& scene_obj_id)
{
    auto iter = scene_list_.find(scene_obj_id);
    if (iter == scene_list_.end())
    {
        return nullptr;
    }

    return iter->second;
}

physx::PxActor* AFCPhysicModule::FindActor(const guid_t& actor_obj_id)
{
    auto iter = actor_list_.find(actor_obj_id);
    if (iter == actor_list_.end())
    {
        return nullptr;
    }

    return iter->second;
}

physx::PxVec3 AFCPhysicModule::AFVector3DToPxVec3(const AFPhysic3D& vec)
{
    return physx::PxVec3(vec.x, vec.y, vec.z);
}

AFPhysic3D AFCPhysicModule::PxVec3ToAFVector3D(const physx::PxVec3& vec)
{
    return AFPhysic3D(vec.x, vec.y, vec.z);
}

} // namespace ark
