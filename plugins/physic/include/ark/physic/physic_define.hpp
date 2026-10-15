#pragma once

#include <functional>
#include <list>
#include <string>

#include "ark/macros.hpp"
#include "ark/math/vector3d.hpp"
#include "ark/platform.hpp"
#include "physX/include/PxPhysicsAPI.h"

namespace ark {

const static std::string PHYSIC_SCENE_PATH = "physic/scene";
const static std::string PHYSIC_CONFIG_PATH = "physic/PhysicConfig.xml";
const static std::string PHYSIC_SLASH = "/";
const static std::string PHYSIC_MESH_FILE_END = ".obj";
const static std::string PHYSIC_COLLIDER_FILE_END = ".xml";
const static std::string PHYSIC_COOKED_PATH = "physic/cache";
const static physx::PxVec3 ZERO_VEC3 = physx::PxVec3();
const static physx::PxReal ZERO_REAL = 0.0F;
const static physx::PxU8 ZERO_U8 = 0U;
const static physx::PxReal DEFAULT_DENSITY = 10.0F;

using physx_net_id = uint64_t;
using physx_scene_id = std::string;
using physx_scene_index = int32_t;

namespace physic {

inline physx::PxI32 OpenFile(FILE** file, const char* name, const char* mode) {
#ifdef ARK_PLATFORM_WIN
    static const physx::PxU32 MAX_LEN = 300;
    char buf[MAX_LEN + 1];

    physx::PxU32 i;
    for (i = 0; i < MAX_LEN && name[i]; i++)
    {
        buf[i] = name[i] == '/' ? '\\' : name[i];
    }
    buf[i] = 0;

    return i == MAX_LEN ? -1 : ::fopen_s(file, buf, mode);
#elif defined(ARK_PLATFORM_LINUX) || defined(ARK_PLATFORM_DARWIN)
    FILE* fp = ::fopen(name, mode);
    if (fp)
    {
        *file = fp;
        return 0;
    }
    return -1;
#endif

    return -1;
};

} // namespace physic

struct physic_config
{
    bool bSimulate{false};
    physx::PxU32 dispatcher_treads_num_{0};
    physx::PxReal step_{0.0333F};
    std::string pvd_host{""};
    int32_t pvd_port{0};
    uint32_t pvd_timeout{0};
};

struct physic_mesh_file
{
    std::string path_{""};
    std::string path_cooked_{""};
};

class collider
{
public:
    virtual ~collider() = default;

    virtual bool IsBoxCollider() const {
        return false;
    }

    virtual bool IsCapsuleCollider() const {
        return false;
    }

    virtual bool IsSphereCollider() const {
        return false;
    }

    virtual bool IsMeshCollider() const {
        return false;
    }

    virtual bool IsTerrainCollider() const {
        return false;
    }

    bool IsTrigger() const {
        return bTrigger_;
    }

public:
    bool bTrigger_{false};
};

class box_collider : public collider
{
public:
    bool IsBoxCollider() const override {
        return true;
    }

public:
    physx::PxVec3 position_{ZERO_VEC3};
    physx::PxVec3 center_{ZERO_VEC3};
    physx::PxVec3 size_{ZERO_VEC3};
};

class capsule_collider : public collider
{
public:
    bool IsCapsuleCollider() const override {
        return true;
    }

public:
    physx::PxVec3 position_{ZERO_VEC3};
    physx::PxVec3 center_{ZERO_VEC3};
    physx::PxReal radius_{ZERO_REAL};
    physx::PxReal height_{ZERO_REAL};
};

class sphere_collider : public collider
{
public:
    bool IsSphereCollider() const override {
        return true;
    }

public:
    physx::PxVec3 position_{ZERO_VEC3};
    physx::PxVec3 center_{ZERO_VEC3};
    physx::PxReal radius_{ZERO_REAL};
};

class mesh_collider : public collider
{
public:
    ~mesh_collider() {
        ARK_DELETE_ARRAY(physx::PxVec3, vertices_);
        ARK_DELETE_ARRAY(physx::PxU32, triangles_);
    }

    bool IsMeshCollider() const override {
        return true;
    }

public:
    bool bConvex{false};
    physx::PxVec3 position_{ZERO_VEC3};
    uint32_t vertices_num_{0};
    uint32_t triangles_num_{0};
    physx::PxVec3* vertices_{nullptr};
    physx::PxU32* triangles_{nullptr};

    physic_mesh_file mesh_file_;
};

class physic_scene
{
public:
    ~physic_scene() {
        for (auto iter : collider_list_)
        {
            if (iter)
            {
                ARK_DELETE(iter);
            }
        }
    }

public:
    std::string scene_id_{""};
    physic_mesh_file mesh_file_;

    physx::PxVec3 position_{ZERO_VEC3};

    using collider_list = std::list<collider*>;
    collider_list collider_list_;
};

using physic_scene_list = std::unordered_map<std::string, physic_scene>;

} // namespace ark
