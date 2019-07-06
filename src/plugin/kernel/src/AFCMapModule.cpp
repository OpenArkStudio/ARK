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

#include "interface/AFIPluginManager.h"
#include "kernel/include/AFDataNode.hpp"
#include "kernel/include/AFCMapModule.h"

namespace ark
{

bool AFCMapModule::Init()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();

    return true;
}

bool AFCMapModule::Shut()
{
    map_infos_.clear();
    return true;
}

ARK_SHARE_PTR<ark::AFMapInfo> AFCMapModule::GetMapInfo(const int map_id)
{
    return map_infos_.find_value(map_id);
}

bool AFCMapModule::IsInMapInstance(const AFGUID &self)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);
    if (pEntity != nullptr)
    {
        return (pEntity->GetNodeInt(AFEntityMetaBaseEntity::map_inst_id()) < 0);
    }
    else
    {
        ARK_LOG_ERROR("Cannot find entity, id = {}", self);
        return false;
    }
}

bool AFCMapModule::ExistMap(const int map_id)
{
    ARK_SHARE_PTR<AFMapInfo> pMapInfo = map_infos_.find_value(map_id);
    return (pMapInfo != nullptr);
}

bool AFCMapModule::SwitchMap(
    const AFGUID &self, const int target_map, const int target_inst, const AFVector3D &pos, const float fOrient, const AFIDataList &args)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("Cannot find entity, id = {}", self);
        return false;
    }

    int32_t old_map = pEntity->GetNodeInt(AFEntityMetaBaseEntity::map_id());
    int32_t old_inst = pEntity->GetNodeInt(AFEntityMetaBaseEntity::map_inst_id());

    ARK_SHARE_PTR<AFMapInfo> pOldMapInfo = map_infos_.find_value(old_map);
    ARK_SHARE_PTR<AFMapInfo> pNewMapInfo = map_infos_.find_value(target_map);

    if (nullptr == pOldMapInfo)
    {
        ARK_LOG_ERROR("cannot find this map, id={}", old_map);
        return false;
    }

    if (nullptr == pNewMapInfo)
    {
        ARK_LOG_ERROR("cannot tind this map, id = {}", target_map);
        return false;
    }

    if (!pNewMapInfo->ExistInstance(target_inst))
    {
        ARK_LOG_ERROR("Cannot find this map instance, map={} inst={}", target_map, target_inst);
        return false;
    }

    pOldMapInfo->RemoveEntityFromInstance(old_inst, self, true);

    if (target_map != old_map)
    {
        pEntity->SetNodeInt(AFEntityMetaBaseEntity::map_id(), 0);
        pEntity->SetNodeInt(AFEntityMetaBaseEntity::map_id(), target_map);
    }

    pEntity->SetNodeInt(AFEntityMetaBaseEntity::map_inst_id(), target_inst);

    pEntity->SetNodeDouble("X", pos.x);
    pEntity->SetNodeDouble("Y", pos.y);
    pEntity->SetNodeDouble("Z", pos.z);

    return pNewMapInfo->AddEntityToInstance(target_inst, self, true);
}

bool AFCMapModule::CreateMap(const int map_id)
{
    ARK_SHARE_PTR<AFMapInfo> pMapInfo = map_infos_.find_value(map_id);
    if (pMapInfo != nullptr)
    {
        return false;
    }

    pMapInfo = std::make_shared<AFMapInfo>();
    if (pMapInfo == nullptr)
    {
        return false;
    }

    map_infos_.insert(map_id, pMapInfo);

    // create group 0
    ARK_SHARE_PTR<AFMapInstance> pMapInst = std::make_shared<AFMapInstance>(0);
    if (nullptr == pMapInst)
    {
        return false;
    }

    pMapInfo->AddInstance(0, pMapInst);
    ARK_LOG_INFO("Create scene success, map={} inst=0", map_id);
    return true;
}

bool AFCMapModule::DestroyMap(const int map_id)
{
    return map_infos_.erase(map_id);
}

int AFCMapModule::GetOnlineCount()
{
    int online_count = 0;
    for (auto iter : map_infos_)
    {
        auto pMapInfo = iter.second;
        auto &all_instances = pMapInfo->GetAllInstance();
        for (auto it : all_instances)
        {
            online_count += it.second->player_entities_.size();
        }
    }

    return online_count;
}

int AFCMapModule::GetMaxOnlineCount()
{
    // test count 5000
    // and it should be define in a config file
    return 10000;
}

int AFCMapModule::GetMapOnlineCount(const int map_id)
{
    int online_count = 0;
    ARK_SHARE_PTR<AFMapInfo> pMapInfo = map_infos_.find_value(map_id);
    if (pMapInfo == nullptr)
    {
        return online_count;
    }

    auto all_instances = pMapInfo->GetAllInstance();
    for (auto iter : all_instances)
    {
        online_count += iter.second->player_entities_.size();
    }

    return online_count;
}

int AFCMapModule::GetMapOnlineCount(const int map_id, const int map_instance_id)
{
    int online_count = 0;

    ARK_SHARE_PTR<AFMapInfo> pMapInfo = map_infos_.find_value(map_id);
    if (pMapInfo == nullptr)
    {
        return online_count;
    }

    ARK_SHARE_PTR<AFMapInstance> pMapInst = pMapInfo->GetInstance(map_instance_id);
    if (pMapInst == nullptr)
    {
        return online_count;
    }

    online_count = pMapInst->player_entities_.size();
    return online_count;
}

int AFCMapModule::GetMapOnlineList(const int map_id, AFIDataList &list)
{
    ARK_SHARE_PTR<AFMapInfo> pMapInfo = map_infos_.find_value(map_id);
    if (pMapInfo == nullptr)
    {
        return 0;
    }

    auto all_instances = pMapInfo->GetAllInstance();
    for (auto iter : all_instances)
    {
        auto pMapInstance = iter.second;
        for (auto it : pMapInstance->player_entities_)
        {
            list.AddInt64(it.first);
        }
    }

    return list.GetCount();
}

int AFCMapModule::CreateMapInstance(const int map_id)
{
    ARK_SHARE_PTR<AFMapInfo> pMapInfo = map_infos_.find_value(map_id);
    if (pMapInfo == nullptr)
    {
        return -1;
    }

    int new_inst_id = pMapInfo->CreateInstanceID();

    if (pMapInfo->ExistInstance(new_inst_id))
    {
        return -1;
    }

    ARK_SHARE_PTR<AFMapInstance> pMapInstance = std::make_shared<AFMapInstance>(new_inst_id);
    if (pMapInstance == nullptr)
    {
        return -1;
    }

    pMapInfo->AddInstance(new_inst_id, pMapInstance);
    return new_inst_id;
}

bool AFCMapModule::ReleaseMapInstance(const int map_id, const int inst_id)
{
    ARK_SHARE_PTR<AFMapInfo> pMapInfo = map_infos_.find_value(map_id);
    if (nullptr == pMapInfo)
    {
        return false;
    }

    if (!pMapInfo->ExistInstance(inst_id))
    {
        return false;
    }

    AFCDataList listObject;

    if (GetInstEntityList(map_id, inst_id, listObject))
    {
        for (size_t i = 0; i < listObject.GetCount(); ++i)
        {
            AFGUID ident = listObject.Int64(i);

            if (!m_pKernelModule->DestroyEntity(ident))
            {
                ARK_LOG_ERROR("Destory entity failed, id  = {}, please check", ident);
            }
        }
    }

    pMapInfo->RemoveInstance(inst_id);

    return true;
}

bool AFCMapModule::ExitMapInstance(const int map_id, const int inst_id)
{
    ARK_SHARE_PTR<AFMapInfo> pMapInfo = map_infos_.find_value(map_id);
    if (pMapInfo == nullptr)
    {
        return false;
    }

    ARK_SHARE_PTR<AFMapInstance> pMapInstance = pMapInfo->GetInstance(inst_id);
    return (pMapInstance != nullptr);
}

bool AFCMapModule::GetInstEntityList(const int map_id, const int inst_id, AFIDataList &list)
{
    ARK_SHARE_PTR<AFMapInfo> pMapInfo = map_infos_.find_value(map_id);
    if (pMapInfo == nullptr)
    {
        return false;
    }

    ARK_SHARE_PTR<AFMapInstance> pMapInstance = pMapInfo->GetInstance(inst_id);
    if (pMapInstance == nullptr)
    {
        return false;
    }

    for (auto iter : pMapInstance->player_entities_)
    {
        list.AddInt64(iter.first);
    }

    for (auto iter : pMapInstance->other_entities_)
    {
        list.AddInt64(iter.first);
    }

    return true;
}

int AFCMapModule::GetEntityByDataNode(const int map_id, const std::string &name, const AFIDataList &value_args, AFIDataList &list)
{
    AFCDataList varObjectList;
    GetMapOnlineList(map_id, varObjectList);
    size_t entity_count = varObjectList.GetCount();
    for (size_t i = 0; i < entity_count; ++i)
    {
        AFGUID ident = varObjectList.Int64(i);
        ARK_SHARE_PTR<AFIEntity> pEntity = m_pKernelModule->GetEntity(ident);
        if (pEntity == nullptr)
        {
            continue;
        }

        ARK_SHARE_PTR<AFIDataNodeManager> &pNodeManager = pEntity->GetNodeManager();
        if (pNodeManager == nullptr)
        {
            continue;
        }

        AFDataNode *pDataNode = pNodeManager->GetNode(name.c_str());
        if (pDataNode != nullptr && value_args.Equal(0, pDataNode->value))
        {
            list.AddInt64(ident);
        }
    }

    return list.GetCount();
}

} // namespace ark
