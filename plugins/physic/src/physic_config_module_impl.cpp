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

#include "ark/physic/physic_config_module_impl.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/xml.hpp"

#ifdef ARK_PLATFORM_WIN
#include <direct.h>
#include <io.h>
#elif defined(ARK_PLATFORM_LINUX) || defined(ARK_PLATFORM_DARWIN)
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>
#endif

namespace ark {

bool physic_config_module_impl::init() {
    if (!LoadConfig())
    {
        return false;
    }

    if (!LoadScene())
    {
        return false;
    }

    return true;
}

bool physic_config_module_impl::shut() {
    return true;
}

bool physic_config_module_impl::GetPhysicScene(const std::string& scene_id, physic_scene& scene) const {
    auto iter = scene_list_.find(scene_id);
    if (iter == scene_list_.end())
    {
        ARK_LOG_ERROR("do not have this scene scene_id={}", scene_id);
        return false;
    }

    scene = iter->second;
    return true;
}

const physic_scene_list& physic_config_module_impl::GetPhysicSceneList() const {
    return scene_list_;
}

const physic_config& physic_config_module_impl::GetPhysicConfig() const {
    return config_;
}

bool physic_config_module_impl::LoadScene() {
    std::string file_path = get_plugin_manager()->GetResPath() + PHYSIC_SCENE_PATH;

    std::multimap<std::string, std::string> sub_path_list;
    if (!GetSceneFileList("", file_path, sub_path_list))
    {
        return false;
    }

    ///read scene
    for (auto& iter : sub_path_list)
    {
        auto scene_id = iter.first;
        auto file_path = iter.second;

        ///find scene
        auto iter_scene = scene_list_.find(scene_id);
        if (iter_scene == scene_list_.end())
        {
            physic_scene scene;
            scene.scene_id_ = scene_id;
            iter_scene = scene_list_.insert(std::make_pair(scene_id, scene)).first;
        }

        if (iter_scene == scene_list_.end())
        {
            ARK_LOG_ERROR("do not have this scene scene_id={}", scene_id);
            return false;
        }
        auto& scene = iter_scene->second;

        ///load file
        if (string_utils::EndsWith(file_path, PHYSIC_MESH_FILE_END))
        {
            if (!LoadTerrain(scene, file_path))
            {
                return false;
            }
        }
        else if (string_utils::EndsWith(file_path, PHYSIC_COLLIDER_FILE_END))
        {
            if (!LoadCollider(scene, file_path))
            {
                return false;
            }
        }
    }

    return true;
}

bool physic_config_module_impl::LoadTerrain(physic_scene& scene, const std::string& file_path) {
    scene.mesh_file_.path_ = file_path;
    scene.mesh_file_.path_cooked_ = GenCookedFilePath(file_path);

    return true;
}

bool physic_config_module_impl::LoadCollider(physic_scene& scene, const std::string& file_path) {
    xml xml_doc(file_path);
    auto root = xml_doc.GetRootNode();
    ARK_ASSERT_RET_VAL(root.IsValid(), false);

    for (auto node = root.FindNode("node"); node.IsValid(); node.NextNode())
    {
        collider* collider = nullptr;

        auto name = node.GetString("Collider");
        if (name == "BoxCollider")
        {
            auto box_collider = ARK_NEW box_collider();
            collider = box_collider;

            auto value = node.GetString("Position");
            ARK_ASSERT_RET_VAL(Vec3FromString(box_collider->position_, value), false);

            value = node.GetString("Center");
            ARK_ASSERT_RET_VAL(Vec3FromString(box_collider->center_, value), false);

            value = node.GetString("Size");
            ARK_ASSERT_RET_VAL(Vec3FromString(box_collider->size_, value), false);
        }
        else if (name == "CapsuleCollider")
        {
            auto capsule_collider = ARK_NEW capsule_collider();
            collider = capsule_collider;

            auto value = node.GetString("Position");
            ARK_ASSERT_RET_VAL(Vec3FromString(capsule_collider->position_, value), false);

            value = node.GetString("Center");
            ARK_ASSERT_RET_VAL(Vec3FromString(capsule_collider->center_, value), false);

            capsule_collider->radius_ = misc::FromString<physx::PxReal>(node.GetString("Radius"));
            capsule_collider->height_ = misc::FromString<physx::PxReal>(node.GetString("Height"));
        }
        else if (name == "SphereCollider")
        {
            auto sphere_collider = ARK_NEW sphere_collider();
            collider = sphere_collider;

            auto value = node.GetString("Position");
            ARK_ASSERT_RET_VAL(Vec3FromString(sphere_collider->position_, value), false);

            value = node.GetString("Center");
            ARK_ASSERT_RET_VAL(Vec3FromString(sphere_collider->center_, value), false);

            sphere_collider->radius_ = misc::FromString<physx::PxReal>(node.GetString("Radius"));
        }
        else if (name == "MeshCollider")
        {
            auto mesh_collider = ARK_NEW mesh_collider();
            collider = mesh_collider;

            auto value = node.GetString("Vertices");
            std::vector<std::string> slices;
            string_utils::split(slices, value, ',');
            ARK_ASSERT_RET_VAL(slices.size() % 3 == 0, false);

            auto count = slices.size() / 3;
            ARK_ASSERT_RET_VAL(count > 0, false);

            ///vertices
            mesh_collider->vertices_ = ARK_NEW_ARRAY(physx::PxVec3, count);
            ARK_ASSERT_RET_VAL(mesh_collider->vertices_ != nullptr, false);

            for (size_t i = 0; i < count; i++)
            {
                mesh_collider->vertices_[i] = GetVec3(slices[i * 3], slices[i * 3 + 1], slices[i * 3 + 2]);
            }
            mesh_collider->vertices_num_ = count;

            ///indices
            value = node.GetString("Triangles");
            slices.clear();
            string_utils::split(slices, value, ',');
            ARK_ASSERT_RET_VAL(slices.size() % 3 == 0, false);

            count = slices.size();
            mesh_collider->triangles_ = ARK_NEW_ARRAY(physx::PxU32, count);
            ARK_ASSERT_RET_VAL(mesh_collider->triangles_ != nullptr, false);

            for (size_t i = 0; i < count; i++)
            {
                mesh_collider->triangles_[i] = misc::FromString<physx::PxU32>(slices[i].data());
            }
            mesh_collider->triangles_num_ = count;

            value = node.GetString("Position");
            ARK_ASSERT_RET_VAL(Vec3FromString(mesh_collider->position_, value), false);

            if (misc::FromString<int32_t>(node.GetString("IsConvex")) > 0)
            {
                mesh_collider->bConvex = true;
            }
            else
            {
                mesh_collider->bConvex = false;
            }

            mesh_collider->mesh_file_.path_cooked_ = GenCookedFilePath(file_path);
        }
        else if (name == "TerrainCollider")
        {
            ///terrain collider position
            auto value = node.GetString("Position");
            ARK_ASSERT_RET_VAL(Vec3FromString(scene.position_, value), false);
            continue;
        }

        if (collider == nullptr)
        {
            ARK_LOG_ERROR("no this type of collider collider={}", name);
            return false;
        }

        collider->bTrigger_ = misc::FromString<int32_t>(node.GetString("IsTrigger")) > 0 ? true : false;
        scene.collider_list_.push_back(collider);
    }

    return true;
}

bool physic_config_module_impl::GetSceneFileList(
    const std::string& father_path, const std::string& path, std::multimap<std::string, std::string>& sub_path_list) {
#ifdef ARK_PLATFORM_WIN
    std::string strPath = path + "/*.*";
    intptr_t handle;
    _finddata_t findData;
    handle = _findfirst(strPath.c_str(), &findData);

    if (handle == -1)
    {
        ARK_LOG_ERROR("can not open path={}", path);
        return false;
    }

    do
    {
        if (findData.attrib & _A_SUBDIR)
        {
            /// is sub dir
            if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
            {
                continue;
            }

            auto new_dir = path + PHYSIC_SLASH + findData.name;
            if (!GetSceneFileList(path, new_dir, sub_path_list))
            {
                ARK_LOG_ERROR("read path failed path={}", new_dir);
                return false;
            }
        }
        else
        {
            /// not sub dir
            if (father_path.empty())
            {
                ARK_LOG_ERROR("not in a scene path={} file = {}", path, findData.name);
                return false;
            }

            ///cut to get scene_id
            auto off = path.find(father_path);
            auto scene_id = path.substr(off + father_path.length() + 1, path.length());
            auto file_path = path + PHYSIC_SLASH + findData.name;
            sub_path_list.insert(std::make_pair(scene_id, file_path));
        }
    } while (_findnext(handle, &findData) == 0);

    _findclose(handle);
    return true;

#elif defined(ARK_PLATFORM_LINUX) || defined(ARK_PLATFORM_DARWIN)
    DIR* dir = opendir(path.c_str());
    if (dir == NULL)
    {
        ARK_LOG_ERROR("can not open path={}", path);
        return false;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            ///is sub dir
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }

            string new_dir = path + PHYSIC_SLASH + entry->d_name;
            if (!GetSceneFileList(path, new_dir, sub_path_list))
            {
                ARK_LOG_ERROR("read path failed path={}", new_dir);
                return false;
            }
        }
        else
        {
            /// not sub dir
            if (father_path.empty())
            {
                ARK_LOG_ERROR("not in a scene path={} file = {}", path, entry->d_name);
                return false;
            }

            ///cut to get scene_id
            auto off = path.find(father_path);
            auto scene_id = path.substr(off + father_path.length() + 1, path.length());
            auto file_path = path + PHYSIC_SLASH + entry->d_name;
            sub_path_list.insert(std::make_pair(scene_id, file_path));
        }
    }

    closedir(dir);
    return true;
#endif

    return false;
}

bool physic_config_module_impl::LoadConfig() {
    std::string file_path = get_plugin_manager()->GetResPath() + PHYSIC_CONFIG_PATH;

    xml xml_doc(file_path);
    auto root = xml_doc.GetRootNode();
    ARK_ASSERT_RET_VAL(root.IsValid(), false);

    for (auto node = root.FindNode("node"); node.IsValid(); node.NextNode())
    {
        config_.bSimulate = (misc::FromString<int32_t>(node.GetString("SimulateOn")) > 0);

        config_.dispatcher_treads_num_ = misc::FromString<physx::PxU32>(node.GetString("DispatcherThreadsNum"));

        auto frequency = misc::FromString<physx::PxU32>(node.GetString("Frequency"));
        ARK_ASSERT_RET_VAL(frequency > 0, false);
        config_.step_ = 1.0F / frequency;

        config_.pvd_host = node.GetString("PVDHost");
        config_.pvd_port = misc::FromString<int32_t>(node.GetString("PVDPort"));
        config_.pvd_timeout = misc::FromString<uint32_t>(node.GetString("PVDTimeOut"));
    }

    return true;
}

bool physic_config_module_impl::Vec3FromString(physx::PxVec3& vec3, const std::string& value) {
    std::vector<std::string> slices;
    string_utils::split(slices, value, ',');

    ARK_ASSERT_RET_VAL(slices.size() == 3, false);

    vec3 = GetVec3(slices[0], slices[1], slices[2]);

    return true;
}

physx::PxVec3 physic_config_module_impl::GetVec3(
    const AFSlice& slice_x, const AFSlice& slice_y, const AFSlice& slice_z) {
    return physx::PxVec3(misc::FromString<physx::PxReal>(slice_x.data()),
        misc::FromString<physx::PxReal>(slice_y.data()), misc::FromString<physx::PxReal>(slice_z.data()));
}

std::string physic_config_module_impl::GenCookedFilePath(const std::string& file_path) {
    auto file_path_cooked = file_path;
    string_utils::replace(file_path_cooked, PHYSIC_SCENE_PATH.c_str(), PHYSIC_COOKED_PATH.c_str());

    ///create file in path
    CreateFilePath(file_path_cooked);

    return file_path_cooked;
}

void physic_config_module_impl::CreateFilePath(const std::string& file_path) {
#ifdef ARK_PLATFORM_WIN
    int len = file_path.length();
    char temp_path[256] = {0};
    for (int i = 0; i < len; i++)
    {
        temp_path[i] = file_path[i];
        if (temp_path[i] != '/')
        {
            continue;
        }

        if (_access(temp_path, 0) != -1)
        {
            continue;
        }

        int ret = _mkdir(temp_path);
        if (ret == -1)
        {
            return;
        }
    }
#elif defined(ARK_PLATFORM_LINUX) || defined(ARK_PLATFORM_DARWIN)
    int len = file_path.length();
    char temp_path[256] = {0};
    for (int i = 0; i < len; i++)
    {
        temp_path[i] = file_path[i];
        if (temp_path[i] != '/')
        {
            continue;
        }

        if (access(temp_path, F_OK) != -1)
        {
            continue;
        }

        mkdir(temp_path, 0755);
    }
#endif
}

} // namespace ark
