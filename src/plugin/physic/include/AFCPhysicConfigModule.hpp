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
#include "plugin/physic/interface/AFIPhysicConfigModule.hpp"
#include "base/AFSlice.hpp"

namespace ark {

class AFCPhysicConfigModule final : public AFIPhysicConfigModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool Shut() override;

    bool GetPhysicScene(const std::string& scene_id, PhysicScene& scene) const override;
    const PhysicSceneList& GetPhysicSceneList() const override;
    const PhysicConfig& GetPhysicConfig() const override;

private:
    bool LoadScene();
    bool LoadTerrain(PhysicScene& scene, const std::string& file_path);
    bool LoadCollider(PhysicScene& scene, const std::string& file_path);
    bool GetSceneFileList(const std::string& father_path, const std::string& path,
        std::multimap<std::string, std::string>& sub_path_list);
    bool LoadConfig();

    bool Vec3FromString(physx::PxVec3& vec3, const std::string& value);
    physx::PxVec3 GetVec3(const AFSlice& slice_x, const AFSlice& slice_y, const AFSlice& slice_z);
    std::string GenCookedFilePath(const std::string& file_path);
    void CreateFilePath(const std::string& file_path);

protected:
    PhysicSceneList scene_list_;
    PhysicConfig config_;
};

} // namespace ark
