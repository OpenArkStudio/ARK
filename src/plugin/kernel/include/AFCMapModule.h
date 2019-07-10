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

#include "log/interface/AFILogModule.h"
#include "kernel/interface/AFIEntity.h"
#include "kernel/interface/AFIMapModule.h"
#include "kernel/interface/AFIKernelModule.h"

namespace ark {

class AFCMapModule : public AFIMapModule
{
public:
    bool Init() override;
    bool Shut() override;

    ARK_SHARE_PTR<AFMapInfo> GetMapInfo(const int map_id) override;

    bool IsInMapInstance(const AFGUID &self) override;
    bool ExistMap(const int map_id) override;

    bool SwitchMap(const AFGUID &self, const int target_map, const int target_inst, const AFVector3D &pos, const float orient,
        const AFIDataList &args) override;

    bool CreateMap(const int map_id) override;
    bool DestroyMap(const int map_id) override;

    int GetOnlineCount() override;
    int GetMaxOnlineCount() override;
    int GetMapOnlineCount(const int map_id) override;
    int GetMapOnlineCount(const int map_id, const int inst_id) override;
    int GetMapOnlineList(const int map_id, AFIDataList &list) override;

    int CreateMapInstance(const int map_id) override;
    bool ReleaseMapInstance(const int map_id, const int inst_id) override;
    bool ExitMapInstance(const int map_id, const int inst_id) override;

    bool GetInstEntityList(const int map_id, const int inst_id, AFIDataList &list) override;
    int GetEntityByDataNode(const int map_id, const std::string &name, const AFIDataList &value_args, AFIDataList &list) override;

private:
    AFIKernelModule *m_pKernelModule;
    AFILogModule *m_pLogModule;

    AFMapEx<int, AFMapInfo> map_infos_;
};

} // namespace ark