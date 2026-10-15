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
    if (!load_config())
    {
        return false;
    }

    if (!load_scene())
    {
        return false;
    }

    return true;
}

bool physic_config_module_impl::shut() {
    return true;
}

bool physic_config_module_impl::get_physic_scene(const std::string& scene_id, physic_scene& scene) const {
    auto iter = scene_list_.find(scene_id);
    if (iter == scene_list_.end())
    {
        ARK_LOG_ERROR("do not have this scene scene_id={}", scene_id);
        return false;
    }

    scene = iter->second;
    return true;
}

const physic_scene_list& physic_config_module_impl::get_physic_scene_list() const {
    return scene_list_;
}

const physic_config& physic_config_module_impl::get_physic_config() const {
    return config_;
}

bool physic_config_module_impl::load_scene() {
    std::string file_path = get_plugin_manager()->GetResPath() + physic_scene_path;

    std::multimap<std::string, std::string> sub_path_list;
    if (!get_scene_file_list("", file_path, sub_path_list))
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
            scene.scene_id = scene_id;
            iter_scene = scene_list_.insert(std::make_pair(scene_id, scene)).first;
        }

        if (iter_scene == scene_list_.end())
        {
            ARK_LOG_ERROR("do not have this scene scene_id={}", scene_id);
            return false;
        }
        auto& scene = iter_scene->second;

        ///load file
        if (file_path.ends_with(physic_mesh_file_end))
        {
            if (!load_terrain(scene, file_path))
            {
                return false;
            }
        }
        else if (file_path.ends_with(physic_collider_file_end))
        {
            if (!load_collider(scene, file_path))
            {
                return false;
            }
        }
    }

    return true;
}

bool physic_config_module_impl::load_terrain(physic_scene& scene, const std::string& file_path) {
    scene.mesh_file.path = file_path;
    scene.mesh_file.cooked_path = gen_cooked_file_path(file_path);

    return true;
}

bool physic_config_module_impl::load_collider(physic_scene& scene, const std::string& file_path) {
    xml xml_doc(file_path);
    auto root = xml_doc.get_root_node();
    ARK_ASSERT_RET_VAL(root.is_valid(), false);

    for (auto node = root.find_node("node"); node.is_valid(); node.next_node())
    {
        collider* collider = nullptr;

        auto name = node.get_string("Collider");
        if (name == "BoxCollider")
        {
            auto box_collider = ARK_NEW box_collider();
            collider = box_collider;

            auto value = node.get_string("Position");
            ARK_ASSERT_RET_VAL(vec3_from_string(box_collider->position, value), false);

            value = node.get_string("Center");
            ARK_ASSERT_RET_VAL(vec3_from_string(box_collider->center, value), false);

            value = node.get_string("Size");
            ARK_ASSERT_RET_VAL(vec3_from_string(box_collider->size, value), false);
        }
        else if (name == "CapsuleCollider")
        {
            auto capsule_collider = ARK_NEW capsule_collider();
            collider = capsule_collider;

            auto value = node.get_string("Position");
            ARK_ASSERT_RET_VAL(vec3_from_string(capsule_collider->position, value), false);

            value = node.get_string("Center");
            ARK_ASSERT_RET_VAL(vec3_from_string(capsule_collider->center, value), false);

            capsule_collider->radius = misc::from_string<physx::PxReal>(node.get_string("Radius"));
            capsule_collider->height = misc::from_string<physx::PxReal>(node.get_string("Height"));
        }
        else if (name == "SphereCollider")
        {
            auto sphere_collider = ARK_NEW sphere_collider();
            collider = sphere_collider;

            auto value = node.get_string("Position");
            ARK_ASSERT_RET_VAL(vec3_from_string(sphere_collider->position, value), false);

            value = node.get_string("Center");
            ARK_ASSERT_RET_VAL(vec3_from_string(sphere_collider->center, value), false);

            sphere_collider->radius = misc::from_string<physx::PxReal>(node.get_string("Radius"));
        }
        else if (name == "MeshCollider")
        {
            auto mesh_collider = ARK_NEW mesh_collider();
            collider = mesh_collider;

            auto value = node.get_string("Vertices");
            std::vector<std::string> slices;
            string_utils::split(slices, value, ',');
            ARK_ASSERT_RET_VAL(slices.size() % 3 == 0, false);

            auto count = slices.size() / 3;
            ARK_ASSERT_RET_VAL(count > 0, false);

            ///vertices
            mesh_collider->vertices = ARK_NEW_ARRAY(physx::PxVec3, count);
            ARK_ASSERT_RET_VAL(mesh_collider->vertices != nullptr, false);

            for (size_t i = 0; i < count; i++)
            {
                mesh_collider->vertices[i] = get_vec3(slices[i * 3], slices[i * 3 + 1], slices[i * 3 + 2]);
            }
            mesh_collider->vertex_count = count;

            ///indices
            value = node.get_string("Triangles");
            slices.clear();
            string_utils::split(slices, value, ',');
            ARK_ASSERT_RET_VAL(slices.size() % 3 == 0, false);

            count = slices.size();
            mesh_collider->triangles = ARK_NEW_ARRAY(physx::PxU32, count);
            ARK_ASSERT_RET_VAL(mesh_collider->triangles != nullptr, false);

            for (size_t i = 0; i < count; i++)
            {
                mesh_collider->triangles[i] = misc::from_string<physx::PxU32>(slices[i].data());
            }
            mesh_collider->triangle_count = count;

            value = node.get_string("Position");
            ARK_ASSERT_RET_VAL(vec3_from_string(mesh_collider->position, value), false);

            if (misc::from_string<int32_t>(node.get_string("IsConvex")) > 0)
            {
                mesh_collider->convex = true;
            }
            else
            {
                mesh_collider->convex = false;
            }

            mesh_collider->mesh_file.cooked_path = gen_cooked_file_path(file_path);
        }
        else if (name == "TerrainCollider")
        {
            ///terrain collider position
            auto value = node.get_string("Position");
            ARK_ASSERT_RET_VAL(vec3_from_string(scene.position, value), false);
            continue;
        }

        if (collider == nullptr)
        {
            ARK_LOG_ERROR("no this type of collider collider={}", name);
            return false;
        }

        collider->trigger = misc::from_string<int32_t>(node.get_string("IsTrigger")) > 0 ? true : false;
        scene.colliders.push_back(collider);
    }

    return true;
}

bool physic_config_module_impl::get_scene_file_list(
    const std::string& father_path, const std::string& path, std::multimap<std::string, std::string>& sub_path_list) {
#ifdef ARK_PLATFORM_WIN
    std::string search_path = path + "/*.*";
    intptr_t handle;
    _finddata_t find_data;
    handle = _findfirst(search_path.c_str(), &find_data);

    if (handle == -1)
    {
        ARK_LOG_ERROR("can not open path={}", path);
        return false;
    }

    do
    {
        if (find_data.attrib & _A_SUBDIR)
        {
            /// is sub dir
            if (strcmp(find_data.name, ".") == 0 || strcmp(find_data.name, "..") == 0)
            {
                continue;
            }

            auto new_dir = path + physic_slash + find_data.name;
            if (!get_scene_file_list(path, new_dir, sub_path_list))
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
                ARK_LOG_ERROR("not in a scene path={} file = {}", path, find_data.name);
                return false;
            }

            ///cut to get scene_id
            auto off = path.find(father_path);
            auto scene_id = path.substr(off + father_path.length() + 1, path.length());
            auto file_path = path + physic_slash + find_data.name;
            sub_path_list.insert(std::make_pair(scene_id, file_path));
        }
    } while (_findnext(handle, &find_data) == 0);

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

            string new_dir = path + physic_slash + entry->d_name;
            if (!get_scene_file_list(path, new_dir, sub_path_list))
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
            auto file_path = path + physic_slash + entry->d_name;
            sub_path_list.insert(std::make_pair(scene_id, file_path));
        }
    }

    closedir(dir);
    return true;
#endif

    return false;
}

bool physic_config_module_impl::load_config() {
    std::string file_path = get_plugin_manager()->GetResPath() + physic_config_path;

    xml xml_doc(file_path);
    auto root = xml_doc.get_root_node();
    ARK_ASSERT_RET_VAL(root.is_valid(), false);

    for (auto node = root.find_node("node"); node.is_valid(); node.next_node())
    {
        config_.simulate = (misc::from_string<int32_t>(node.get_string("SimulateOn")) > 0);

        config_.dispatcher_threads = misc::from_string<physx::PxU32>(node.get_string("DispatcherThreadsNum"));

        auto frequency = misc::from_string<physx::PxU32>(node.get_string("Frequency"));
        ARK_ASSERT_RET_VAL(frequency > 0, false);
        config_.step = 1.0F / frequency;

        config_.pvd_host = node.get_string("PVDHost");
        config_.pvd_port = misc::from_string<int32_t>(node.get_string("PVDPort"));
        config_.pvd_timeout = misc::from_string<uint32_t>(node.get_string("PVDTimeOut"));
    }

    return true;
}

bool physic_config_module_impl::vec3_from_string(physx::PxVec3& vec3, const std::string& value) {
    std::vector<std::string> slices;
    string_utils::split(slices, value, ',');

    ARK_ASSERT_RET_VAL(slices.size() == 3, false);

    vec3 = get_vec3(slices[0], slices[1], slices[2]);

    return true;
}

physx::PxVec3 physic_config_module_impl::get_vec3(
    const AFSlice& slice_x, const AFSlice& slice_y, const AFSlice& slice_z) {
    return physx::PxVec3(misc::from_string<physx::PxReal>(slice_x.data()),
        misc::from_string<physx::PxReal>(slice_y.data()), misc::from_string<physx::PxReal>(slice_z.data()));
}

std::string physic_config_module_impl::gen_cooked_file_path(const std::string& file_path) {
    auto file_path_cooked = file_path;
    string_utils::replace(file_path_cooked, physic_scene_path.c_str(), physic_cooked_path.c_str());

    ///create file in path
    create_file_path(file_path_cooked);

    return file_path_cooked;
}

void physic_config_module_impl::create_file_path(const std::string& file_path) {
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
