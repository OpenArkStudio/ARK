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

#include "SDK/Core/AFDefine.hpp"
#include "SDK/Core/AFGUID.hpp"
#include "SDK/Core/AFCEntity.h"
#include "SDK/Core/AFDataNode.hpp"
#include "SDK/Core/AFDataTable.h"
#include "Common/AFDataDefine.hpp"
#include "AFCKernelModule.h"

namespace ark
{

    AFCKernelModule::AFCKernelModule()
    {
        mInnerProperty.AddElement(IObject::ConfigID(), ARK_NEW int32_t(0));
        mInnerProperty.AddElement(IObject::ClassName(), ARK_NEW int32_t(0));
        mInnerProperty.AddElement(IObject::MapID(), ARK_NEW int32_t(0));
        mInnerProperty.AddElement(IObject::InstanceID(), ARK_NEW int32_t(0));
    }

    AFCKernelModule::~AFCKernelModule()
    {
        for (size_t i = 0; i < mInnerProperty.GetCount(); ++i)
        {
            int32_t* pInnerProperty = mInnerProperty[i];
            ARK_DELETE(pInnerProperty);
        }

        entities_.ClearAll();
    }

    bool AFCKernelModule::Init()
    {
        delete_list_.clear();

        m_pMapModule = pPluginManager->FindModule<AFIMapModule>();
        m_pClassModule = pPluginManager->FindModule<AFIMetaClassModule>();
        m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();
        m_pGUIDModule = pPluginManager->FindModule<AFIGUIDModule>();

        return true;
    }

    bool AFCKernelModule::Update()
    {
        cur_exec_entity_ = NULL_GUID;

        if (!delete_list_.empty())
        {
            for (auto it : delete_list_)
            {
                DestroyEntity(it);
            }

            delete_list_.clear();
        }

        for (ARK_SHARE_PTR<AFIEntity> pEntity = entities_.First(); pEntity != nullptr; pEntity = entities_.Next())
        {
            cur_exec_entity_ = pEntity->Self();
            pEntity->Update();
            cur_exec_entity_ = NULL_GUID;
        }

        return true;
    }

    bool AFCKernelModule::PreShut()
    {
        return DestroyAll();
    }

    bool AFCKernelModule::FindHeartBeat(const AFGUID& self, const std::string& name)
    {
        ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(self);

        if (pEntity != nullptr)
        {
            return pEntity->CheckHeartBeatExist(name);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::RemoveHeartBeat(const AFGUID& self, const std::string& name)
    {
        ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(self);

        if (pEntity != nullptr)
        {
            return pEntity->RemoveHeartBeat(name);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    ARK_SHARE_PTR<AFIEntity> AFCKernelModule::CreateEntity(const AFGUID& self, const int map_id, const int map_instance_id, const std::string& class_name, const std::string& config_index, const AFIDataList& args)
    {
        AFGUID entity_id = self;

        ARK_SHARE_PTR<AFMapInfo>& pMapInfo = m_pMapModule->GetMapInfo(map_id);
        if (pMapInfo == nullptr)
        {
            ARK_LOG_ERROR("There is no scene, scene = {}", map_id);
            return nullptr;
        }

        if (pMapInfo->GetElement(map_instance_id) == nullptr)
        {
            ARK_LOG_ERROR("There is no group, scene = {} group = {}", map_id, map_instance_id);
            return nullptr;
        }

        if (entity_id.IsNULL())
        {
            entity_id = m_pGUIDModule->CreateGUID();
        }

        //Check if the entity exists
        if (GetEntity(entity_id) != nullptr)
        {
            ARK_LOG_ERROR("The entity has existed, id = {}", entity_id.ToString());
            return nullptr;
        }

        ARK_SHARE_PTR<AFIEntity> pEntity = std::make_shared<AFCEntity>(entity_id);
        entities_.AddElement(entity_id, pEntity);
        pMapInfo->AddEntityToInstance(map_instance_id, entity_id, ((class_name == Player::ThisName()) ? true : false));

        ARK_SHARE_PTR<AFIDataNodeManager>& pNodeManager = pEntity->GetNodeManager();
        ARK_SHARE_PTR<AFIDataTableManager>& pTableManager = pEntity->GetTableManager();
        m_pClassModule->InitDataNodeManager(class_name, pNodeManager);
        m_pClassModule->InitDataTableManager(class_name, pTableManager);

        ARK_SHARE_PTR<AFIDataNodeManager> pConfigNodeManager = m_pConfigModule->GetNodeManager(config_index);

        if (pConfigNodeManager != nullptr)
        {
            size_t configNodeCount = pConfigNodeManager->GetNodeCount();

            for (size_t i = 0; i < configNodeCount; ++i)
            {
                AFDataNode* pConfigNode = pConfigNodeManager->GetNodeByIndex(i);
                if (pConfigNode != nullptr && pConfigNode->Changed())
                {
                    pNodeManager->SetNode(pConfigNode->GetName(), pConfigNode->GetValue());
                }
            }
        }

        DoEvent(entity_id, class_name, ENTITY_EVT_PRE_LOAD_DATA, args);

        for (size_t i = 0; (i + 1) < args.GetCount(); i += 2)
        {
            const std::string& strDataNodeName = args.String(i);
            if (!mInnerProperty.ExistElement(strDataNodeName))
            {
                AFDataNode* pArgNode = pNodeManager->GetNode(strDataNodeName.c_str());
                if (pArgNode != nullptr)
                {
                    args.ToAFIData(i + 1, pArgNode->value);
                }
            }
        }

        pEntity->SetNodeString(IObject::ConfigID(), config_index);
        pEntity->SetNodeString(IObject::ClassName(), class_name);
        pEntity->SetNodeInt(IObject::MapID(), map_id);
        pEntity->SetNodeInt(IObject::InstanceID(), map_instance_id);

        DoEvent(entity_id, class_name, ENTITY_EVT_LOAD_DATA, args);
        DoEvent(entity_id, class_name, ENTITY_EVT_PRE_EFFECT_DATA, args);
        DoEvent(entity_id, class_name, ENTITY_EVT_EFFECT_DATA, args);
        DoEvent(entity_id, class_name, ENTITY_EVT_POST_EFFECT_DATA, args);
        DoEvent(entity_id, class_name, ENTITY_EVT_DATA_FINISHED, args);

        return pEntity;
    }

    ARK_SHARE_PTR<AFIEntity>& AFCKernelModule::GetEntity(const AFGUID& self)
    {
        return entities_.GetElement(self);
    }

    bool AFCKernelModule::DestroyAll()
    {
        for (ARK_SHARE_PTR<AFIEntity> pEntity = entities_.First(); pEntity != nullptr; pEntity = entities_.Next())
        {
            delete_list_.push_back(pEntity->Self());
        }

        //run another frame
        Update();

        common_class_callbacks_.clear();
        common_data_node_callbacks_.clear();
        common_data_table_callbacks_.clear();

        return true;
    }

    bool AFCKernelModule::DestroyEntity(const AFGUID& self)
    {
        if (self == cur_exec_entity_ && !self.IsNULL())
        {
            return DestroySelf(self);
        }

        int32_t map_id = GetNodeInt(self, IObject::MapID());
        int32_t inst_id = GetNodeInt(self, IObject::InstanceID());

        ARK_SHARE_PTR<AFMapInfo>& pMapInfo = m_pMapModule->GetMapInfo(map_id);
        if (pMapInfo != nullptr)
        {
            const std::string& class_name = GetNodeString(self, IObject::ClassName());

            pMapInfo->RemoveEntityFromInstance(inst_id, self, ((class_name == Player::ThisName()) ? true : false));

            DoEvent(self, class_name, ENTITY_EVT_PRE_DESTROY, AFCDataList());
            DoEvent(self, class_name, ENTITY_EVT_DESTROY, AFCDataList());

            return entities_.RemoveElement(self);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find this map, entity_id={} map={} inst={}", self.ToString(), map_id, inst_id);
            return false;
        }
    }

    bool AFCKernelModule::FindNode(const AFGUID& self, const std::string& name)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->CheckNodeExist(name);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetNodeBool(const AFGUID& self, const std::string& name, const bool value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->SetNodeBool(name, value);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetNodeInt(const AFGUID& self, const std::string& name, const int32_t value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->SetNodeInt(name, value);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetNodeInt64(const AFGUID& self, const std::string& name, const int64_t value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->SetNodeInt64(name, value);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetNodeFloat(const AFGUID& self, const std::string& name, const float value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->SetNodeFloat(name, value);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetNodeDouble(const AFGUID& self, const std::string& name, const double value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->SetNodeDouble(name, value);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetNodeString(const AFGUID& self, const std::string& name, const std::string& value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->SetNodeString(name, value);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetNodeObject(const AFGUID& self, const std::string& name, const AFGUID& value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->SetNodeObject(name, value);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::GetNodeBool(const AFGUID& self, const std::string& name)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetNodeBool(name);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_BOOLEAN;
        }
    }

    int32_t AFCKernelModule::GetNodeInt(const AFGUID& self, const std::string& name)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetNodeInt(name);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_INT;
        }
    }

    int64_t AFCKernelModule::GetNodeInt64(const AFGUID& self, const std::string& name)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetNodeInt64(name);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_INT64;
        }
    }

    float AFCKernelModule::GetNodeFloat(const AFGUID& self, const std::string& name)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetNodeFloat(name);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_FLOAT;
        }
    }

    double AFCKernelModule::GetNodeDouble(const AFGUID& self, const std::string& name)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetNodeDouble(name);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_DOUBLE;
        }
    }

    const char* AFCKernelModule::GetNodeString(const AFGUID& self, const std::string& name)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetNodeString(name);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return nullptr;
        }
    }

    const AFGUID AFCKernelModule::GetNodeObject(const AFGUID& self, const std::string& name)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetNodeObject(name);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_GUID;
        }
    }

    AFDataTable* AFCKernelModule::FindTable(const AFGUID& self, const std::string& name)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetTableManager()->GetTable(name.c_str());
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return nullptr;
        }
    }

    bool AFCKernelModule::ClearTable(const AFGUID& self, const std::string& name)
    {
        AFDataTable* pTable = FindTable(self, name);
        if (pTable != nullptr)
        {
            pTable->Clear();
            return true;
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetTableBool(const AFGUID& self, const std::string& name, const int row, const int col, const bool value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            if (!pEntity->SetTableBool(name, row, col, value))
            {
                ARK_LOG_ERROR("error for row or col, id = {}", self.ToString());
                return false;
            }

            return true;
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetTableInt(const AFGUID& self, const std::string& name, const int row, const int col, const int32_t value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            if (!pEntity->SetTableInt(name, row, col, value))
            {
                ARK_LOG_ERROR("error for row or col, id = {}", self.ToString());
                return false;
            }

            return true;
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col, const int64_t value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            if (!pEntity->SetTableInt64(name, row, col, value))
            {
                ARK_LOG_ERROR("error for row or col, id = {}", self.ToString());
                return false;
            }

            return true;
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col, const float value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            if (!pEntity->SetTableFloat(name, row, col, value))
            {
                ARK_LOG_ERROR("error for row or col, id = {}", self.ToString());
                return false;
            }

            return true;
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col, const double value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            if (!pEntity->SetTableDouble(name, row, col, value))
            {
                ARK_LOG_ERROR("error for row or col, id = {}", self.ToString());
                return false;
            }

            return true;
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetTableString(const AFGUID& self, const std::string& name, const int row, const int col, const std::string& value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            if (!pEntity->SetTableString(name, row, col, value))
            {
                ARK_LOG_ERROR("error for row or col, id = {}", self.ToString());
                return false;
            }

            return true;
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::SetTableObject(const AFGUID& self, const std::string& name, const int row, const int col, const AFGUID& value)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            if (!pEntity->SetTableObject(name, row, col, value))
            {
                ARK_LOG_ERROR("error for row or col, id = {}", self.ToString());
                return false;
            }

            return true;
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return false;
        }
    }

    bool AFCKernelModule::GetTableBool(const AFGUID& self, const std::string& name, const int row, const int col)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetTableBool(name, row, col);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_BOOLEAN;
        }
    }

    int32_t AFCKernelModule::GetTableInt(const AFGUID& self, const std::string& name, const int row, const int col)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetTableInt(name, row, col);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_INT;
        }
    }

    int64_t AFCKernelModule::GetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetTableInt64(name, row, col);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_INT64;
        }
    }

    float AFCKernelModule::GetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetTableFloat(name, row, col);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_FLOAT;
        }
    }

    double AFCKernelModule::GetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetTableDouble(name, row, col);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_DOUBLE;
        }
    }

    const char* AFCKernelModule::GetTableString(const AFGUID& self, const std::string& name, const int row, const int col)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetTableString(name, row, col);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_STR.c_str();
        }
    }

    const AFGUID AFCKernelModule::GetTableObject(const AFGUID& self, const std::string& name, const int row, const int col)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        if (pEntity != nullptr)
        {
            return pEntity->GetTableObject(name, row, col);
        }
        else
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
            return NULL_GUID;
        }
    }

    bool AFCKernelModule::DestroySelf(const AFGUID& self)
    {
        delete_list_.push_back(self);
        return true;
    }

    bool AFCKernelModule::RegCommonClassEvent(const CLASS_EVENT_FUNCTOR_PTR& cb)
    {
        for (ARK_SHARE_PTR<AFIMetaClass>& pClass = m_pClassModule->First(); pClass != nullptr; pClass = m_pClassModule->Next())
        {
            AddClassCallBack(pClass->GetClassName(), cb);
        }

        return true;
    }

    bool AFCKernelModule::RegCommonDataNodeEvent(const DATA_NODE_EVENT_FUNCTOR_PTR& cb)
    {
        for (ARK_SHARE_PTR<AFIMetaClass> pClass = m_pClassModule->First(); pClass != nullptr; pClass = m_pClassModule->Next())
        {
            pClass->AddCommonNodeCallback(cb);
        }

        return true;
    }

    bool AFCKernelModule::RegCommonDataTableEvent(const DATA_TABLE_EVENT_FUNCTOR_PTR& cb)
    {
        for (ARK_SHARE_PTR<AFIMetaClass> pClass = m_pClassModule->First(); pClass != nullptr; pClass = m_pClassModule->Next())
        {
            pClass->AddCommonTableCallback(cb);
        }

        return true;
    }

    bool AFCKernelModule::AddEventCallBack(const AFGUID& self, const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        return ((pEntity != nullptr) ? pEntity->GetEventManager()->AddEventCallBack(nEventID, cb) : false);
    }

    bool AFCKernelModule::AddClassCallBack(const std::string& class_name, const CLASS_EVENT_FUNCTOR_PTR& cb)
    {
        return m_pClassModule->AddClassCallBack(class_name, cb);
    }

    bool AFCKernelModule::DoEvent(const AFGUID& self, const std::string& class_name, ARK_ENTITY_EVENT class_event, const AFIDataList& args)
    {
        return m_pClassModule->DoEvent(self, class_name, class_event, args);
    }

    bool AFCKernelModule::DoEvent(const AFGUID& self, const int event_id, const AFIDataList& args)
    {
        ARK_SHARE_PTR<AFIEntity>& pEntity = GetEntity(self);
        return ((pEntity != nullptr) ? pEntity->GetEventManager()->DoEvent(event_id, args) : false);
    }

    bool AFCKernelModule::LogInfo(const AFGUID& id)
    {
        ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(id);
        if (pEntity != nullptr)
        {
            ARK_LOG_ERROR("Cannot find entity, id = {}", id.ToString());
            return false;
        }

        if (m_pMapModule->IsInMapInstance(id))
        {
            int map_id = GetNodeInt(id, IObject::MapID());

            ARK_LOG_INFO("----------child object list-------- , id = {} mapid = {}", id.ToString(), map_id);
            AFCDataList entity_list;
            int online_count = m_pMapModule->GetMapOnlineList(map_id, entity_list);
            for (int i = 0; i < online_count; ++i)
            {
                AFGUID target_entity_id = entity_list.Object(i);
                ARK_LOG_INFO("id = {} mapid = {}", target_entity_id.ToString(), map_id);
            }
        }
        else
        {
            ARK_LOG_INFO("---------printf object start--------, id = {}", id.ToString());
            ARK_LOG_INFO("---------printf object end--------", id.ToString());
        }

        return true;
    }

    bool AFCKernelModule::LogSelfInfo(const AFGUID& id)
    {
        return false;
    }

}