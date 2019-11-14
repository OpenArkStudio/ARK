/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "base/AFList.hpp"
#include "base/AFPluginManager.hpp"
#include "kernel/interface/AFIKernelModule.hpp"
#include "kernel/interface/AFIMapModule.hpp"
#include "log/interface/AFILogModule.hpp"
#include "utility/interface/AFIGUIDModule.hpp"
#include "game/interface/AFISceneProcessModule.hpp"
#include "game/interface/AFIAttributeModule.hpp"
#include "game/interface/AFIGameNetModule.hpp"

namespace ark {

class AFCSceneProcessModule final : public AFISceneProcessModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool PostInit() override;

    E_SCENE_TYPE GetMapInstanceType(const int map_id) override;
    bool IsInstanceMap(const int map_id) override;
    bool ApplyMapInstance(const int map_id, int& inst_id) override;
    bool ExitMapInstance(const int map_id, const int inst_id) override;

protected:
    int CreateMapInstance(const int map_id);
    bool CreateMapEntities(const int map_id, const int inst_id);
    bool LoadMapResource(const int map_id);

    int OnObjectClassEvent(
        const AFGUID& self, const std::string& strClassName, const ArkEntityEvent eClassEvent, const AFIDataList& var);
    int OnEnterSceneEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);
    int OnLeaveSceneEvent(const AFGUID& object, const int nEventID, const AFIDataList& var);

    void OnClientSwapMapProcess(const AFNetMsg* msg);

private:
    AFIKernelModule* m_pKernelModule;
    AFIMapModule* m_pMapModule;
    AFILogModule* m_pLogModule;
    AFIGUIDModule* m_pGUIDModule;
    AFIGameNetModule* m_pGameNetModule;
    //////////////////////////////////////////////////////////////////////////
    struct MapSeedResource
    {
        std::string strSeedID;
        std::string strConfigID;
        float fSeedX;
        float fSeedY;
        float fSeedZ;
    };

    // SceneID,(SeedID,SeedData)
    AFSmartPtrMap<int, AFSmartPtrMap<std::string, MapSeedResource>> map_res_;
};

} // namespace ark
