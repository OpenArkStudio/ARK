/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
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

#include "Common/AFDataDefine.hpp"
#include "SDK/Core/AFDataNode.hpp"
#include "AFCMapModule.h"

namespace ark
{

    AFCMapModule::~AFCMapModule()
    {
        map_infos_.ClearAll();
    }

    bool AFCMapModule::Init()
    {
        m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();

        return true;
    }

    ARK_SHARE_PTR<ark::AFMapInfo>& AFCMapModule::GetMapInfo(const int map_id)
    {
        return map_infos_.GetElement(map_id);
    }

    bool AFCMapModule::IsInMapInstance(const AFGUID& self)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = m_pKernelModule->GetEntity(self);
        if (pEntity != nullptr)
        {
            return (pEntity->GetNodeInt(IObject::InstanceID()) < 0);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCMapModule::ExistMap(const int map_id)
    {
        ARK_SHARE_PTR<AFMapInfo>& pMapInfo = map_infos_.GetElement(map_id);
        return (pMapInfo != nullptr);
    }

    bool AFCMapModule::SwitchMap(const AFGUID& self, const int target_map, const int target_inst, const Point3D& pos, const float fOrient, const AFIDataList& args)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = m_pKernelModule->GetEntity(self);
        if (pEntity == nullptr)
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }

        int32_t old_map = pEntity->GetNodeInt(IObject::MapID());
        int32_t old_inst = pEntity->GetNodeInt(IObject::InstanceID());

        ARK_SHARE_PTR<AFMapInfo>& pOldMapInfo = map_infos_.GetElement(old_map);
        ARK_SHARE_PTR<AFMapInfo>& pNewMapInfo = map_infos_.GetElement(target_map);

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

        if (pNewMapInfo->GetElement(target_inst) == nullptr)
        {
            ARK_LOG_ERROR("Cannot find this map instance, map={} inst={}", target_map, target_inst);
            return false;
        }

        pOldMapInfo->RemoveEntityFromInstance(old_inst, self, true);

        if (target_map != old_map)
        {
            pEntity->SetNodeInt(IObject::MapID(), 0);
            pEntity->SetNodeInt(IObject::InstanceID(), target_map);
        }

        pEntity->SetNodeInt(IObject::MapID(), target_inst);

        pEntity->SetNodeDouble("X", pos.x);
        pEntity->SetNodeDouble("Y", pos.y);
        pEntity->SetNodeDouble("Z", pos.z);

        return pNewMapInfo->AddEntityToInstance(target_inst, self, true);
    }

    bool AFCMapModule::CreateMap(const int map_id)
    {
        ARK_SHARE_PTR<AFMapInfo>& pMapInfo = map_infos_.GetElement(map_id);
        if (pMapInfo != nullptr)
        {
            return false;
        }

        pMapInfo = std::make_shared<AFMapInfo>();
        if (pMapInfo == nullptr)
        {
            return false;
        }

        map_infos_.AddElement(map_id, pMapInfo);

        //create group 0
        ARK_SHARE_PTR<AFMapInstance> pGroupInfo = std::make_shared<AFMapInstance>(0);
        if (nullptr == pGroupInfo)
        {
            return false;
        }

        pMapInfo->AddElement(0, pGroupInfo);
        ARK_LOG_INFO("Create scene success, map={} inst=0", map_id);
        return true;
    }

    bool AFCMapModule::DestroyMap(const int map_id)
    {
        return map_infos_.RemoveElement(map_id);
    }

    int AFCMapModule::GetOnlineCount()
    {
        int online_count = 0;
        for (ARK_SHARE_PTR<AFMapInfo>& pMapInfo = map_infos_.First(); pMapInfo != nullptr; pMapInfo = map_infos_.Next())
        {
            for (ARK_SHARE_PTR<AFMapInstance>& pMapInst = pMapInfo->First(); pMapInst != nullptr; pMapInst = pMapInfo->Next())
            {
                online_count += pMapInst->mxPlayerList.GetCount();
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
        ARK_SHARE_PTR<AFMapInfo>& pMapInfo = map_infos_.GetElement(map_id);
        if (pMapInfo == nullptr)
        {
            return online_count;
        }

        for (ARK_SHARE_PTR<AFMapInstance>& pGroupInfo = pMapInfo->First(); pGroupInfo != nullptr; pGroupInfo = pMapInfo->Next())
        {
            online_count += pGroupInfo->mxPlayerList.GetCount();
        }

        return online_count;
    }

    int AFCMapModule::GetMapOnlineCount(const int map_id, const int map_instance_id)
    {
        int online_count = 0;

        ARK_SHARE_PTR<AFMapInfo>& pMapInfo = map_infos_.GetElement(map_id);
        if (pMapInfo == nullptr)
        {
            return online_count;
        }

        ARK_SHARE_PTR<AFMapInstance>& pGroupInfo = pMapInfo->GetElement(map_instance_id);
        if (pGroupInfo == nullptr)
        {
            return online_count;
        }

        online_count = pGroupInfo->mxPlayerList.GetCount();
        return online_count;
    }

    int AFCMapModule::GetMapOnlineList(const int map_id, AFIDataList& list)
    {
        ARK_SHARE_PTR<AFMapInfo>& pMapInfo = map_infos_.GetElement(map_id);
        if (pMapInfo == nullptr)
        {
            return 0;
        }

        for (ARK_SHARE_PTR<AFMapInstance>& pMapInstance = pMapInfo->First(); pMapInstance != nullptr; pMapInstance = pMapInfo->Next())
        {
            AFGUID entity_id;
            for (pMapInstance->mxPlayerList.First(entity_id); !entity_id.IsNULL(); pMapInstance->mxPlayerList.Next(entity_id))
            {
                list.AddObject(entity_id);
                entity_id = NULL_GUID;
            }
        }

        return list.GetCount();
    }

    int AFCMapModule::CreateMapInstance(const int map_id)
    {
        ARK_SHARE_PTR<AFMapInfo>& pMapInfo = map_infos_.GetElement(map_id);
        if (pMapInfo == nullptr)
        {
            return -1;
        }

        int new_inst_id = pMapInfo->CreateInstanceID();

        if (nullptr != pMapInfo->GetElement(new_inst_id))
        {
            return -1;
        }

        ARK_SHARE_PTR<AFMapInstance> pMapInstance = std::make_shared<AFMapInstance>(new_inst_id);
        if (pMapInstance == nullptr)
        {
            return -1;
        }

        pMapInfo->AddElement(new_inst_id, pMapInstance);
        return new_inst_id;
    }

    bool AFCMapModule::ReleaseMapInstance(const int map_id, const int inst_id)
    {
        ARK_SHARE_PTR<AFMapInfo>& pMapInfo = map_infos_.GetElement(map_id);

        if (nullptr == pMapInfo)
        {
            return false;
        }

        if (nullptr == pMapInfo->GetElement(inst_id))
        {
            return false;
        }

        AFCDataList listObject;

        if (GetInstEntityList(map_id, inst_id, listObject))
        {
            for (size_t i = 0; i < listObject.GetCount(); ++i)
            {
                AFGUID ident = listObject.Object(i);

                if (!m_pKernelModule->DestroyEntity(ident))
                {
                    ARK_LOG_ERROR("Destory entity failed, id  = {}, pls check", ident.ToString().c_str());
                }
            }
        }

        pMapInfo->RemoveElement(inst_id);

        return true;
    }

    bool AFCMapModule::ExitMapInstance(const int map_id, const int inst_id)
    {
        ARK_SHARE_PTR<AFMapInfo>& pMapInfo = map_infos_.GetElement(map_id);
        if (pMapInfo == nullptr)
        {
            return false;
        }

        ARK_SHARE_PTR<AFMapInstance>& pMapInstance = pMapInfo->GetElement(inst_id);
        return (pMapInstance != nullptr);
    }

    bool AFCMapModule::GetInstEntityList(const int map_id, const int inst_id, AFIDataList& list)
    {
        ARK_SHARE_PTR<AFMapInfo> pMapInfo = map_infos_.GetElement(map_id);
        if (pMapInfo == nullptr)
        {
            return false;
        }

        ARK_SHARE_PTR<AFMapInstance>& pMapInstance = pMapInfo->GetElement(inst_id);
        if (pMapInstance == nullptr)
        {
            return false;
        }

        AFGUID entity_id = NULL_GUID;
        for (pMapInstance->mxPlayerList.First(entity_id); !entity_id.IsNULL(); pMapInstance->mxPlayerList.Next(entity_id))
        {
            list.AddObject(entity_id);
            entity_id = NULL_GUID;
        }

        entity_id = NULL_GUID;
        for (pMapInstance->mxOtherList.First(entity_id); !entity_id.IsNULL(); pMapInstance->mxOtherList.Next(entity_id))
        {
            list.AddObject(entity_id);
            entity_id = NULL_GUID;
        }

        return true;
    }

    int AFCMapModule::GetEntityByDataNode(const int map_id, const std::string& name, const AFIDataList& value_args, AFIDataList& list)
    {
        AFCDataList varObjectList;
        GetMapOnlineList(map_id, varObjectList);
        size_t entity_count = varObjectList.GetCount();
        for (size_t i = 0; i < entity_count; ++i)
        {
            AFGUID ident = varObjectList.Object(i);
            ARK_SHARE_PTR<AFIEntity>& pEntity = m_pKernelModule->GetEntity(ident);
            if (pEntity == nullptr)
            {
                continue;
            }

            ARK_SHARE_PTR<AFIDataNodeManager>& pNodeManager = pEntity->GetNodeManager();
            if (pNodeManager == nullptr)
            {
                continue;
            }

            AFDataNode* pDataNode = pNodeManager->GetNode(name.c_str());
            if (pDataNode != nullptr && value_args.Equal(0, pDataNode->value))
            {
                list.AddObject(ident);
            }
        }

        return list.GetCount();
    }

}