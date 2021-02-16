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

#include "base/container/AFList.hpp"
#include "base/container/AFMap.hpp"
#include "base/math/AFVector3D.hpp"
#include "interface/AFIModule.hpp"
#include "kernel/interface/AFIDataList.hpp"

namespace ark {

// Map instance
class AFMapInstance
{
public:
    explicit AFMapInstance(int inst_id)
        : inst_id_(inst_id)
    {
    }

    AFSmartPtrMap<guid_t, bool> player_entities_;
    AFSmartPtrMap<guid_t, bool> other_entities_;
    int inst_id_;
};

// All instance in this map
class AFMapInfo
{
public:
    AFMapInfo() = default;

    AFMapInfo(int width)
        : width_(width)
    {
    }

    virtual ~AFMapInfo() = default;

    int CreateInstanceID()
    {
        return ++instance_id_;
    }

    int GetWidth()
    {
        return width_;
    }

    std::shared_ptr<AFMapInstance> GetInstance(int inst_id)
    {
        return map_instances_.find_value(inst_id);
    }

    bool ExistInstance(int inst_id)
    {
        return (map_instances_.find_value(inst_id) != nullptr);
    }

    AFSmartPtrMap<int, AFMapInstance>& GetAllInstance()
    {
        return map_instances_;
    }

    bool AddInstance(int inst_id, std::shared_ptr<AFMapInstance> inst)
    {
        return map_instances_.insert(inst_id, inst).second;
    }

    bool RemoveInstance(int inst_id)
    {
        map_instances_.erase(inst_id);
        return true;
    }

    bool AddEntityToInstance(const int inst_id, const guid_t& entity_id, bool is_player)
    {
        std::shared_ptr<AFMapInstance> pInfo = map_instances_.find_value(inst_id);
        if (pInfo == nullptr)
        {
            return false;
        }
        else
        {
            if (is_player)
            {
                return pInfo->player_entities_.insert(entity_id, nullptr).second; // TODO:Map.second mean nothing
            }
            else
            {
                return pInfo->other_entities_.insert(entity_id, nullptr).second; // TODO:Map.second mean nothing
            }
        }
    }

    bool RemoveEntityFromInstance(const int inst_id, const guid_t& entity_id, bool is_player)
    {
        std::shared_ptr<AFMapInstance> pInfo = map_instances_.find_value(inst_id);
        if (nullptr == pInfo)
        {
            return false;
        }
        else
        {
            if (is_player)
            {
                return pInfo->player_entities_.erase(entity_id);
            }
            else
            {
                return pInfo->other_entities_.erase(entity_id);
            }
        }
    }

private:
    int instance_id_{0};
    //int map_id_{0};
    int width_{512}; // will separate the grid
    AFSmartPtrMap<int, AFMapInstance> map_instances_;
};

class AFIMapModule : public AFIModule
{
public:
    virtual std::shared_ptr<AFMapInfo> GetMapInfo(const int map_id) = 0;

    virtual bool IsInMapInstance(const guid_t& self) = 0;
    virtual bool ExistMap(const int map_id) = 0;

    virtual bool SwitchMap(const guid_t& self, const int target_map, const int target_inst, const AFVector3D& pos,
        const float orient, const AFIDataList& args) = 0;

    virtual bool CreateMap(const int map_id) = 0;
    virtual bool DestroyMap(const int map_id) = 0;

    virtual int GetOnlineCount() = 0;
    virtual int GetMaxOnlineCount() = 0;
    virtual int GetMapOnlineCount(const int map_id) = 0;
    virtual int GetMapOnlineCount(const int map_id, const int inst_id) = 0;
    virtual int GetMapOnlineList(const int map_id, AFIDataList& args) = 0;

    virtual int CreateMapInstance(const int map_id) = 0;
    virtual bool ReleaseMapInstance(const int map_id, const int inst_id) = 0;
    virtual bool ExitMapInstance(const int map_id, const int inst_id) = 0;

    virtual bool GetInstEntityList(const int map_id, const int inst_id, AFIDataList& list) = 0;
    virtual int GetEntityByDataNode(
        const int map_id, const std::string& data_node, const AFIDataList& args, AFIDataList& entities) = 0;
};

} // namespace ark
