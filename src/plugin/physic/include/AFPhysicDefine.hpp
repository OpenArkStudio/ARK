#pragma once

#include "base/AFMacros.hpp"
#include "base/AFPlatform.hpp"
#include "base/math/AFVector3D.hpp"
#include "physX/include/PxPhysicsAPI.h"
#include <string>
#include <list>
#include <functional>

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

using PhysxNetID = uint64_t;
using PhysxSceneID = std::string;
using PhysxSceneIndex = int32_t;

namespace physic {

inline physx::PxI32 OpenFile(FILE** file, const char* name, const char* mode)
{
#ifdef ARK_PLATFORM_WIN
    static const physx::PxU32 MAX_LEN = 300;
    char buf[MAX_LEN + 1];

    physx::PxU32 i;
    for (i = 0; i < MAX_LEN && name[i]; i++)
        buf[i] = name[i] == '/' ? '\\' : name[i];
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

struct PhysicConfig
{
    bool bSimulate{false};
    physx::PxU32 dispatcher_treads_num_{0};
    physx::PxReal step_{0.0333F};
    std::string pvd_host{""};
    int32_t pvd_port{0};
    uint32_t pvd_timeout{0};
};

struct PhysicMeshFile
{
    std::string path_{""};
    std::string path_cooked_{""};
};

class Collider
{
public:
    virtual ~Collider() = default;

    virtual bool IsBoxCollider() const
    {
        return false;
    }

    virtual bool IsCapsuleCollider() const
    {
        return false;
    }

    virtual bool IsSphereCollider() const
    {
        return false;
    }

    virtual bool IsMeshCollider() const
    {
        return false;
    }

    virtual bool IsTerrainCollider() const
    {
        return false;
    }

    bool IsTrigger() const
    {
        return bTrigger_;
    }

public:
    bool bTrigger_{false};
};

class BoxCollider : public Collider
{
public:
    bool IsBoxCollider() const override
    {
        return true;
    }

public:
    physx::PxVec3 position_{ZERO_VEC3};
    physx::PxVec3 center_{ZERO_VEC3};
    physx::PxVec3 size_{ZERO_VEC3};
};

class CapsuleCollider : public Collider
{
public:
    bool IsCapsuleCollider() const override
    {
        return true;
    }

public:
    physx::PxVec3 position_{ZERO_VEC3};
    physx::PxVec3 center_{ZERO_VEC3};
    physx::PxReal radius_{ZERO_REAL};
    physx::PxReal height_{ZERO_REAL};
};

class SphereCollider : public Collider
{
public:
    bool IsSphereCollider() const override
    {
        return true;
    }

public:
    physx::PxVec3 position_{ZERO_VEC3};
    physx::PxVec3 center_{ZERO_VEC3};
    physx::PxReal radius_{ZERO_REAL};
};

class MeshCollider : public Collider
{
public:
    ~MeshCollider()
    {
        ARK_DELETE_ARRAY(physx::PxVec3, vertices_);
        ARK_DELETE_ARRAY(physx::PxU32, triangles_);
    }

    bool IsMeshCollider() const override
    {
        return true;
    }

public:
    bool bConvex{false};
    physx::PxVec3 position_{ZERO_VEC3};
    uint32_t vertices_num_{0};
    uint32_t triangles_num_{0};
    physx::PxVec3* vertices_{nullptr};
    physx::PxU32* triangles_{nullptr};

    PhysicMeshFile mesh_file_;
};

class PhysicScene
{
public:
    ~PhysicScene()
    {
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
    PhysicMeshFile mesh_file_;

    physx::PxVec3 position_{ZERO_VEC3};

    using ColliderList = std::list<Collider*>;
    ColliderList collider_list_;
};
using PhysicSceneList = std::unordered_map<std::string, PhysicScene>;

} // namespace ark
