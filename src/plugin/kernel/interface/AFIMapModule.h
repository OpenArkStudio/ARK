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
#include "base/AFMap.hpp"
#include "base/AFVector3D.hpp"
#include "kernel/interface/AFIDataList.hpp"
#include "interface/AFIModule.h"

namespace ark
{

    //Map instance
    class AFMapInstance
    {
    public:
        explicit AFMapInstance(int inst_id) :
            inst_id_(inst_id)
        {

        }

        AFMapEx<AFGUID, bool> player_entities_;
        AFMapEx<AFGUID, bool> other_entities_;
        int inst_id_;
    };

    //All instance in this map
    class AFMapInfo
    {
    public:
        explicit AFMapInfo() :
            instance_id_(0),
            width_(512)
        {
        }

        explicit AFMapInfo(int width) :
            instance_id_(0),
            width_(width)
        {
        }

        virtual ~AFMapInfo()
        {
            map_instances_.clear();
        }

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

        AFMapEx<int, AFMapInstance>& GetAllInstance()
        {
            return map_instances_;
        }

        bool AddInstance(int inst_id, ARK_SHARE_PTR<AFMapInstance> inst)
        {
            return map_instances_.insert(inst_id, inst).second;
        }

        bool RemoveInstance(int inst_id)
        {
            map_instances_.erase(inst_id);
            return true;
        }

        bool AddEntityToInstance(const int inst_id, const AFGUID& entity_id, bool is_player)
        {
            ARK_SHARE_PTR<AFMapInstance> pInfo = map_instances_.find_value(inst_id);
            if (pInfo == nullptr)
            {
                return false;
            }
            else
            {
                if (is_player)
                {
                    return pInfo->player_entities_.insert(entity_id, nullptr).second; //TODO:Map.second mean nothing
                }
                else
                {
                    return pInfo->other_entities_.insert(entity_id, nullptr).second; //TODO:Map.second mean nothing
                }
            }
        }

        bool RemoveEntityFromInstance(const int inst_id, const AFGUID& entity_id, bool is_player)
        {
            ARK_SHARE_PTR<AFMapInstance> pInfo = map_instances_.find_value(inst_id);
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
        int instance_id_;
        int map_id_;
        int width_; //will separate the grid
        AFMapEx<int, AFMapInstance> map_instances_;
    };

    class AFIMapModule : public AFIModule
    {
    public:
        virtual ARK_SHARE_PTR<AFMapInfo> GetMapInfo(const int map_id) = 0;

        virtual bool IsInMapInstance(const AFGUID& self) = 0;
        virtual bool ExistMap(const int map_id) = 0;

        virtual bool SwitchMap(const AFGUID& self, const int target_map, const int target_inst, const AFVector3D& pos, const float orient, const AFIDataList& args) = 0;

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
        virtual int GetEntityByDataNode(const int map_id, const std::string& data_node, const AFIDataList& args, AFIDataList& entities) = 0;
    };

}