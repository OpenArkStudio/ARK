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

#include "base/AFDefine.hpp"
#include "kernel/include/AFCKernelModule.hpp"
#include "kernel/include/AFCEntity.hpp"
#include "log/interface/AFILogModule.hpp"

namespace ark {

AFCKernelModule::AFCKernelModule()
{
    inner_nodes_.AddElement(AFEntityMetaBaseEntity::config_id(), ARK_NEW int32_t(0));
    inner_nodes_.AddElement(AFEntityMetaBaseEntity::class_name(), ARK_NEW int32_t(0));
    inner_nodes_.AddElement(AFEntityMetaBaseEntity::map_id(), ARK_NEW int32_t(0));
    inner_nodes_.AddElement(AFEntityMetaBaseEntity::map_inst_id(), ARK_NEW int32_t(0));
}

AFCKernelModule::~AFCKernelModule()
{
    objects_.clear();
}

bool AFCKernelModule::Init()
{
    delete_list_.clear();

    m_pMapModule = FindModule<AFIMapModule>();
    m_pClassModule = FindModule<AFIClassMetaModule>();
    m_pConfigModule = FindModule<AFIConfigModule>();
    m_pLogModule = FindModule<AFILogModule>();
    m_pGUIDModule = FindModule<AFIGUIDModule>();

    return true;
}

bool AFCKernelModule::Update()
{
    cur_exec_object_ = NULL_GUID;

    if (!delete_list_.empty())
    {
        for (auto it : delete_list_)
        {
            DestroyEntity(it);
        }

        delete_list_.clear();
    }

    for (auto iter : objects_)
    {
        auto pEntity = iter.second;
        if (pEntity == nullptr)
        {
            continue;
        }

        pEntity->Update();
    }

    return true;
}

bool AFCKernelModule::PreShut()
{
    return DestroyAll();
}

bool AFCKernelModule::CopyData(ARK_SHARE_PTR<AFIEntity> pEntity, const ID_TYPE config_id)
{
    if (pEntity == nullptr)
    {
        return false;
    }

    if (config_id > 0)
    {
        // static node manager must be not empty
        auto pStaticEntity = GetStaticEntity(config_id);
        auto pStaticNodeManager = GetNodeManager(pStaticEntity);
        if (pStaticNodeManager == nullptr || pStaticNodeManager->IsEmpty())
        {
            return false;
        }

        // node manager must be empty
        auto pNodeManager = GetNodeManager(pEntity);
        if (pNodeManager == nullptr || !pNodeManager->IsEmpty())
        {
            return false;
        }

        // copy data
        auto& data_list = pStaticNodeManager->GetDataList();
        for (auto iter : data_list)
        {
            auto pData = iter.second;
            pNodeManager->CreateData(pData);
        }

        return true;
    }

    return false;
}

ARK_SHARE_PTR<AFIEntity> AFCKernelModule::CreateEntity(const AFGUID& self, const int map_id, const int map_instance_id,
    const std::string& class_name, const ID_TYPE config_id, const AFIDataList& args)
{
    AFGUID object_id = self;

    auto pMapInfo = m_pMapModule->GetMapInfo(map_id);
    if (pMapInfo == nullptr)
    {
        ARK_LOG_ERROR("There is no scene, scene = {}", map_id);
        return nullptr;
    }

    if (!pMapInfo->ExistInstance(map_instance_id))
    {
        ARK_LOG_ERROR("There is no group, scene = {} group = {}", map_id, map_instance_id);
        return nullptr;
    }

    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    if (nullptr == pClassMeta)
    {
        ARK_LOG_ERROR("There is no class meta, name = {}", class_name);
        return nullptr;
    }

    // check args num
    size_t arg_count = args.GetCount();
    if (arg_count % 2 != 0)
    {
        ARK_LOG_ERROR("Args count is wrong, count = {}", arg_count);
        return nullptr;
    }

    if (object_id == NULL_GUID)
    {
        object_id = m_pGUIDModule->CreateGUID();
    }

    // Check if the entity exists
    if (GetEntity(object_id) != nullptr)
    {
        ARK_LOG_ERROR("The entity has existed, id = {}", object_id);
        return nullptr;
    }

    ARK_SHARE_PTR<AFIEntity> pEntity =
        std::make_shared<AFCEntity>(pClassMeta, object_id, config_id, map_id, map_instance_id);

    objects_.insert(object_id, pEntity);

    pMapInfo->AddEntityToInstance(
        map_instance_id, object_id, ((class_name == AFEntityMetaPlayer::self_name()) ? true : false));

    CopyData(pEntity, config_id);

    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_LOAD_DATA, args);

    for (size_t i = 0; i < arg_count; i += 2)
    {
        auto data_type = args.GetType(i);
        auto index = args.UInt(i + 1);
        switch (data_type)
        {
            case ark::ArkDataType::DT_BOOLEAN:
                pEntity->SetBool(index, args.Bool(i));
                break;
            case ark::ArkDataType::DT_INT32:
                pEntity->SetInt32(index, args.Int(i));
                break;
            case ark::ArkDataType::DT_UINT32:
                pEntity->SetUInt32(index, args.UInt(i));
                break;
            case ark::ArkDataType::DT_INT64:
                pEntity->SetInt64(index, args.Int64(i));
                break;
            case ark::ArkDataType::DT_UINT64:
                pEntity->SetUInt64(index, args.UInt64(i));
                break;
            case ark::ArkDataType::DT_FLOAT:
                pEntity->SetFloat(index, args.Float(i));
                break;
            case ark::ArkDataType::DT_DOUBLE:
                pEntity->SetDouble(index, args.Double(i));
                break;
            case ark::ArkDataType::DT_STRING:
                pEntity->SetString(index, args.String(i));
                break;
            default:
                break;
        }
    }

    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_LOAD_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_POST_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_DATA_FINISHED, args);

    return pEntity;
}

ARK_SHARE_PTR<AFIEntity> AFCKernelModule::CreateContainerEntity(
    const AFGUID& self, const uint32_t container_index, const std::string& class_name, const ID_TYPE config_id)
{
    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("There is no object, object = {}", self);
        return nullptr;
    }

    auto pContainer = pEntity->FindContainer(container_index);
    if (pContainer == nullptr)
    {
        ARK_LOG_ERROR("There is no container, container = {}", container_index);
        return nullptr;
    }

    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    if (nullptr == pClassMeta)
    {
        ARK_LOG_ERROR("There is no class meta, name = {}", class_name);
        return nullptr;
    }

    auto map_id = pEntity->GetMapID();
    auto pMapInfo = m_pMapModule->GetMapInfo(map_id);
    if (pMapInfo == nullptr)
    {
        ARK_LOG_ERROR("There is no scene, scene = {}", map_id);
        return nullptr;
    }

    auto map_instance_id = pEntity->GetMapEntityID();
    if (!pMapInfo->ExistInstance(map_instance_id))
    {
        ARK_LOG_ERROR("There is no group, scene = {} group = {}", map_id, map_instance_id);
        return nullptr;
    }

    AFGUID object_id = m_pGUIDModule->CreateGUID();

    // Check if the entity exists
    if (GetEntity(object_id) != nullptr)
    {
        ARK_LOG_ERROR("The entity has existed, id = {}", object_id);
        return nullptr;
    }

    ARK_SHARE_PTR<AFIEntity> pContainerEntity =
        std::make_shared<AFCEntity>(pClassMeta, object_id, config_id, map_id, map_instance_id);

    objects_.insert(object_id, pContainerEntity);

    CopyData(pContainerEntity, config_id);

    pContainer->Place(pContainerEntity);

    AFCDataList args;
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_LOAD_DATA, args);

    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_LOAD_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_POST_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_DATA_FINISHED, args);

    return pContainerEntity;
}

ARK_SHARE_PTR<AFIStaticEntity> AFCKernelModule::GetStaticEntity(const ID_TYPE config_id)
{
    return m_pConfigModule->FindStaticEntity(config_id);
}

ARK_SHARE_PTR<AFIEntity> AFCKernelModule::GetEntity(const AFGUID& self)
{
    return objects_.find_value(self);
}

bool AFCKernelModule::DestroyAll()
{
    for (auto iter : objects_)
    {
        delete_list_.push_back(iter.second->GetID());
    }

    // run another frame
    Update();

    return true;
}

bool AFCKernelModule::DestroyEntity(const AFGUID& self)
{
    if (self == cur_exec_object_ && self != NULL_GUID)
    {
        return DestroySelf(self);
    }

    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("Cannot find this object, self={}", NULL_GUID);
        return false;
    }

    auto pParentContainer = pEntity->GetParentContainer();
    if (pParentContainer)
    {
        // use container to destroy its entity
        return pParentContainer->Destroy(self);
    }
    else
    {
        return InnerDestroyEntity(pEntity);
    }
}

bool AFCKernelModule::DestroySelf(const AFGUID& self)
{
    delete_list_.push_back(self);
    return true;
}

bool AFCKernelModule::InnerDestroyEntity(ARK_SHARE_PTR<AFIEntity> pEntity)
{
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("Cannot find this object, self={}", NULL_GUID);
        return false;
    }

    auto& self = pEntity->GetID();
    int32_t map_id = pEntity->GetMapID();
    int32_t inst_id = pEntity->GetMapEntityID();

    ARK_SHARE_PTR<AFMapInfo> pMapInfo = m_pMapModule->GetMapInfo(map_id);
    if (pMapInfo != nullptr)
    {
        const std::string& class_name = pEntity->GetClassName();

        pMapInfo->RemoveEntityFromInstance(
            inst_id, self, ((class_name == AFEntityMetaPlayer::self_name()) ? true : false));

        DoEvent(self, class_name, ArkEntityEvent::ENTITY_EVT_PRE_DESTROY, AFCDataList());
        DoEvent(self, class_name, ArkEntityEvent::ENTITY_EVT_DESTROY, AFCDataList());

        return objects_.erase(self);
    }
    else
    {
        ARK_LOG_ERROR("Cannot find this map, object_id={} map={} inst={}", self, map_id, inst_id);
        return false;
    }
}

bool AFCKernelModule::AddEventCallBack(const AFGUID& self, const int nEventID, EVENT_PROCESS_FUNCTOR&& cb)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(self);
    ARK_ASSERT_RET_VAL(pEntity != nullptr, false);

    auto pEventManager = GetEventManager(pEntity);
    ARK_ASSERT_RET_VAL(pEventManager != nullptr, false);

    return pEventManager->AddEventCallBack(nEventID, std::forward<EVENT_PROCESS_FUNCTOR>(cb));
}

bool AFCKernelModule::AddClassCallBack(const std::string& class_name, CLASS_EVENT_FUNCTOR&& cb)
{
    return m_pClassModule->AddClassCallBack(class_name, std::forward<CLASS_EVENT_FUNCTOR>(cb));
}

bool AFCKernelModule::AddDataCallBack(
    const std::string& class_name, const std::string& name, DATA_NODE_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto index = pClassMeta->GetIndex(name);
    if (index == 0)
    {
        return false;
    }

    AddDataCallBack(class_name, index, std::forward<DATA_NODE_EVENT_FUNCTOR>(cb));

    return true;
}

bool AFCKernelModule::AddTableCallBack(
    const std::string& class_name, const std::string& name, DATA_TABLE_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto index = pClassMeta->GetIndex(name);
    if (index == 0)
    {
        return false;
    }

    AddTableCallBack(class_name, index, std::forward<DATA_TABLE_EVENT_FUNCTOR>(cb));

    return true;
}

bool AFCKernelModule::AddDataCallBack(const std::string& class_name, const uint32_t index, DATA_NODE_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pDataMeta = pClassMeta->FindDataMeta(index);
    ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->AddDataCallBack(index, std::forward<DATA_NODE_EVENT_FUNCTOR>(cb));

    return true;
}

bool AFCKernelModule::AddTableCallBack(
    const std::string& class_name, const uint32_t index, DATA_TABLE_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pTableMeta = pClassMeta->FindTableMeta(index);
    ARK_ASSERT_RET_VAL(pTableMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->AddTableCallBack(index, std::forward<DATA_TABLE_EVENT_FUNCTOR>(cb));

    return true;
}

bool AFCKernelModule::AddContainerCallBack(
    const std::string& class_name, const uint32_t index, CONTAINER_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pContainerMeta = pClassMeta->FindContainerMeta(index);
    ARK_ASSERT_RET_VAL(pContainerMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->AddContainerCallBack(index, std::forward<CONTAINER_EVENT_FUNCTOR>(cb));

    return true;
}

bool AFCKernelModule::AddCommonContainerCallBack(CONTAINER_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(AFEntityMetaPlayer::self_name());
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto& meta_list = pClassMeta->GetContainerMetaList();
    for (auto iter : meta_list)
    {
        auto pMeta = iter.second;
        if (!pMeta)
        {
            continue;
        }

        AddContainerCallBack(
            AFEntityMetaPlayer::self_name(), pMeta->GetIndex(), std::forward<CONTAINER_EVENT_FUNCTOR>(cb));
    }

    return true;
}

bool AFCKernelModule::AddCommonClassEvent(CLASS_EVENT_FUNCTOR&& cb)
{
    auto& class_meta_list = m_pClassModule->GetMetaList();
    for (auto iter : class_meta_list)
    {
        auto pClassMeta = iter.second;
        if (nullptr == pClassMeta)
        {
            continue;
        }

        if (!pClassMeta->IsEntityMeta())
        {
            continue;
        }

        AddClassCallBack(iter.first, std::forward<CLASS_EVENT_FUNCTOR>(cb));
    }

    return true;
}

bool AFCKernelModule::AddCommonNodeEvent(DATA_NODE_EVENT_FUNCTOR&& cb)
{
    auto& class_meta_list = m_pClassModule->GetMetaList();
    for (auto iter : class_meta_list)
    {
        auto pClassMeta = iter.second;
        if (nullptr == pClassMeta)
        {
            continue;
        }

        if (!pClassMeta->IsEntityMeta())
        {
            continue;
        }

        auto& data_meta_list = pClassMeta->GetDataMetaList();
        for (auto iter_data : data_meta_list)
        {
            AddDataCallBack(iter.first, iter_data.first, std::forward<DATA_NODE_EVENT_FUNCTOR>(cb));
        }
    }

    return true;
}

bool AFCKernelModule::AddCommonTableEvent(DATA_TABLE_EVENT_FUNCTOR&& cb)
{
    auto& class_meta_list = m_pClassModule->GetMetaList();
    for (auto iter : class_meta_list)
    {
        auto pClassMeta = iter.second;
        if (nullptr == pClassMeta)
        {
            continue;
        }

        if (!pClassMeta->IsEntityMeta())
        {
            continue;
        }

        auto& table_meta_list = pClassMeta->GetTableMetaList();
        for (auto iter_data : table_meta_list)
        {
            AddTableCallBack(iter.first, iter_data.first, std::forward<DATA_TABLE_EVENT_FUNCTOR>(cb));
        }
    }

    return true;
}

bool AFCKernelModule::DoEvent(
    const AFGUID& self, const std::string& class_name, ArkEntityEvent class_event, const AFIDataList& args)
{
    return m_pClassModule->DoEvent(self, class_name, class_event, args);
}

bool AFCKernelModule::DoEvent(const AFGUID& self, const int event_id, const AFIDataList& args)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(self);
    ARK_ASSERT_RET_VAL(pEntity != nullptr, false);

    auto pEventManager = GetEventManager(pEntity);
    ARK_ASSERT_RET_VAL(pEventManager != nullptr, false);

    return pEventManager->DoEvent(event_id, args);
}

bool AFCKernelModule::Exist(const AFGUID& self)
{
    return (objects_.find_value(self) != nullptr);
}

bool AFCKernelModule::LogSelfInfo(const AFGUID& id)
{
    return false;
}

int AFCKernelModule::LogObjectData(const AFGUID& guid)
{
    auto entity = GetEntity(guid);
    if (entity == nullptr)
    {
        return -1;
    }

    auto pNodeManager = GetNodeManager(entity);
    ARK_ASSERT_RET_VAL(pNodeManager != nullptr, -1);

    auto pTableManager = GetTableManager(entity);
    ARK_ASSERT_RET_VAL(pTableManager != nullptr, -1);

    auto& node_list = pNodeManager->GetDataList();
    for (auto iter : node_list)
    {
        auto pData = iter.second;
        if (!pData)
        {
            continue;
        }

        ARK_LOG_TRACE("Player[{}] Node[{}] Value[{}]", guid, pData->GetName(), pData->ToString());
    }

    auto& table_list = pTableManager->GetTableList();
    for (auto iter : table_list)
    {
        auto pTable = iter.second;
        if (!pTable)
        {
            continue;
        }

        for (auto pRow = pTable->First(); pRow != nullptr; pRow = pTable->Next())
        {
            auto pRowNodeManager = GetNodeManager(pRow);
            if (!pRowNodeManager)
            {
                continue;
            }

            auto& row_data_list = pRowNodeManager->GetDataList();
            for (auto iter_row : row_data_list)
            {
                auto pNode = iter_row.second;
                if (!pNode)
                {
                    continue;
                }

                ARK_LOG_TRACE("Player[{}] Table[{}] Row[{}] Col[{}] Value[{}]", guid, pTable->GetName(), pRow->GetRow(),
                    pNode->GetName(), pNode->ToString());
            }
        }
    }

    return 0;
}

bool AFCKernelModule::LogInfo(const AFGUID& id)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(id);
    if (pEntity != nullptr)
    {
        ARK_LOG_ERROR("Cannot find entity, id = {}", id);
        return false;
    }

    if (m_pMapModule->IsInMapInstance(id))
    {
        int map_id = pEntity->GetMapID();

        ARK_LOG_INFO("----------child object list-------- , id = {} mapid = {}", id, map_id);
        AFCDataList entity_list;
        int online_count = m_pMapModule->GetMapOnlineList(map_id, entity_list);
        for (int i = 0; i < online_count; ++i)
        {
            AFGUID target_entity_id = entity_list.Int64(i);
            ARK_LOG_INFO("id = {} mapid = {}", target_entity_id, map_id);
        }
    }
    else
    {
        ARK_LOG_INFO("---------print object start--------, id = {}", id);
        ARK_LOG_INFO("---------print object end--------, id = {}", id);
    }

    return true;
}

//--------------entity to pb db data------------------
bool AFCKernelModule::EntityToDBData(const AFGUID& self, AFMsg::pb_db_entity& pb_data)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(self);
    return EntityToDBData(pEntity, pb_data);
}

bool AFCKernelModule::EntityToDBData(ARK_SHARE_PTR<AFIEntity> pEntity, AFMsg::pb_db_entity& pb_data)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr, false);

    auto pNodeManager = GetNodeManager(pEntity);
    ARK_ASSERT_RET_VAL(pNodeManager != nullptr, false);

    auto pTableManager = GetTableManager(pEntity);
    ARK_ASSERT_RET_VAL(pTableManager != nullptr, false);

    auto pContainerManager = GetContainerManager(pEntity);
    ARK_ASSERT_RET_VAL(pContainerManager != nullptr, false);

    pb_data.set_id(pEntity->GetID());
    pb_data.set_config_id(pEntity->GetConfigID());
    pb_data.set_map_id(pEntity->GetMapID());
    pb_data.set_map_inst_id(pEntity->GetMapEntityID());
    pb_data.set_class_name(pEntity->GetClassName());

    // node to db
    auto& node_list = pNodeManager->GetDataList();
    for (auto iter : node_list)
    {
        auto pNode = iter.second;
        if (!pNode)
        {
            continue;
        }

        if (!pNode->HaveMask(ArkNodeMask::PF_SAVE))
        {
            continue;
        }

        NodeToDBData(pNode, *pb_data.mutable_data());
    }

    // table to db
    auto& table_list = pTableManager->GetTableList();
    for (auto iter : table_list)
    {
        auto pTable = iter.second;
        if (!pTable)
        {
            continue;
        }

        if (!pTable->HaveMask(ArkTableNodeMask::PF_SAVE))
        {
            continue;
        }

        AFMsg::pb_db_table pb_table;
        if (!TableToDBData(pTable, pb_table))
        {
            continue;
        }

        pb_data.mutable_data()->mutable_datas_table()->insert({pTable->GetName(), pb_table});
    }

    // container to db
    auto& container_list = pContainerManager->GetContainerList();
    for (auto iter : container_list)
    {
        auto pContainer = iter.second;
        if (!pContainer)
        {
            continue;
        }

        AFMsg::pb_db_container pb_container;
        for (auto index = pContainer->First(); index > 0; index = pContainer->Next())
        {
            auto pSubEntity = pContainer->Find(index);
            if (!pSubEntity)
            {
                continue;
            }

            AFMsg::pb_db_entity pb_container_entity;
            if (!EntityToDBData(pSubEntity, pb_container_entity))
            {
                continue;
            }

            pb_container.mutable_datas_value()->insert({index, pb_container_entity});
        }

        if (pb_container.datas_value_size() > 0)
        {
            pb_data.mutable_data()->mutable_datas_entity()->insert({pContainer->GetName(), pb_container});
        }
    }

    return true;
}

ARK_SHARE_PTR<AFIEntity> AFCKernelModule::CreateEntity(const AFMsg::pb_db_entity& pb_data)
{
    auto entity_id = pb_data.id();
    auto pEntity = GetEntity(entity_id);
    if (pEntity != nullptr)
    {
        ARK_LOG_ERROR("entity already exists, object = {}", entity_id);
        return nullptr;
    }

    const std::string& class_name = pb_data.class_name();
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    if (nullptr == pClassMeta)
    {
        ARK_LOG_ERROR("There is no class meta, name = {}", class_name);
        return nullptr;
    }

    auto map_id = pb_data.map_id();
    auto map_inst_id = pb_data.map_inst_id();

    pEntity = std::make_shared<AFCEntity>(pClassMeta, entity_id, NULL_INT, map_id, map_inst_id);

    objects_.insert(entity_id, pEntity);

    // init data
    auto& pb_db_entity_data = pb_data.data();

    DBDataToNode(pEntity, pb_db_entity_data);

    //array data(todo : whether we need this?)
    //for (auto iter : pb_db_entity_data.datas_bool())
    //{
    //    pEntity->SetBool(iter.first, iter.second);
    //}

    // table data
    for (auto iter : pb_db_entity_data.datas_table())
    {
        DBDataToTable(pEntity, iter.first, iter.second);
    }

    // container data
    for (auto iter : pb_db_entity_data.datas_entity())
    {
        DBDataToContainer(pEntity, iter.first, iter.second);
    }

    // pMapInfo->AddEntityToInstance(map_inst_id, entity_id, true);

    // todo : add new event?
    AFCDataList args;
    DoEvent(entity_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_LOAD_DATA, args);

    DoEvent(entity_id, class_name, ArkEntityEvent::ENTITY_EVT_LOAD_DATA, args);
    DoEvent(entity_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_EFFECT_DATA, args);
    DoEvent(entity_id, class_name, ArkEntityEvent::ENTITY_EVT_EFFECT_DATA, args);
    DoEvent(entity_id, class_name, ArkEntityEvent::ENTITY_EVT_POST_EFFECT_DATA, args);
    DoEvent(entity_id, class_name, ArkEntityEvent::ENTITY_EVT_DATA_FINISHED, args);

    return pEntity;
}

bool AFCKernelModule::SendCustomMessage(const AFGUID& target, const uint32_t msg_id, const AFIDataList& args)
{
    ARK_ASSERT_RET_VAL(Exist(target) && msg_id > 0, false);

    AFMsg::pb_custom_message custom_message;
    custom_message.set_message_id(msg_id);

    size_t count = args.GetCount();
    for (size_t i = 0; i < count; i++)
    {
        auto data_type = args.GetType(i);
        switch (data_type)
        {
            case ark::ArkDataType::DT_BOOLEAN:
                custom_message.add_data_list()->set_bool_value(args.Bool(i));
                break;
            case ark::ArkDataType::DT_INT32:
                custom_message.add_data_list()->set_int_value(args.Int(i));
                break;
            case ark::ArkDataType::DT_UINT32:
                custom_message.add_data_list()->set_uint_value(args.UInt(i));
                break;
            case ark::ArkDataType::DT_INT64:
                custom_message.add_data_list()->set_int64_value(args.Int64(i));
                break;
            case ark::ArkDataType::DT_UINT64:
                custom_message.add_data_list()->set_uint64_value(args.UInt64(i));
                break;
            case ark::ArkDataType::DT_FLOAT:
                custom_message.add_data_list()->set_float_value(args.Float(i));
                break;
            case ark::ArkDataType::DT_DOUBLE:
                custom_message.add_data_list()->set_double_value(args.Double(i));
                break;
            case ark::ArkDataType::DT_STRING:
                custom_message.add_data_list()->set_str_value(args.String(i));
                break;
            default:
                break;
        }
    }

    // send message

    return true;
}

// pb table to entity table
bool AFCKernelModule::DBDataToTable(
    ARK_SHARE_PTR<AFIEntity> pEntityData, const std::string& name, const AFMsg::pb_db_table& pb_table)
{
    ARK_ASSERT_RET_VAL(pEntityData != nullptr, false);

    auto pTable = pEntityData->FindTable(name);
    ARK_ASSERT_RET_VAL(pTable != nullptr, false);

    for (auto iter : pb_table.datas_value())
    {
        auto row_index = iter.first;
        if (row_index == NULL_INT)
        {
            continue;
        }

        auto& pb_db_entity_data = iter.second;
        auto pRow = pTable->AddRow(row_index);
        if (pRow == nullptr)
        {
            continue;
        }

        DBDataToNode(pRow, pb_db_entity_data);
    }

    return true;
}

bool AFCKernelModule::DBDataToContainer(
    ARK_SHARE_PTR<AFIEntity> pEntity, const std::string& name, const AFMsg::pb_db_container& pb_data)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr, false);

    for (auto iter : pb_data.datas_value())
    {
        auto pContainer = pEntity->FindContainer(iter.first);
        if (nullptr == pContainer)
        {
            continue;
        }

        auto pContainerEntity = CreateEntity(iter.second);
        if (nullptr == pContainerEntity)
        {
            continue;
        }

        pContainer->Place(pContainerEntity);
    }

    return true;
}

int AFCKernelModule::OnContainerCallBack(const AFGUID& self, const uint32_t index, const ArkContainerOpType op_type,
    const uint32_t src_index, const uint32_t dest_index)
{
    if (op_type == ArkContainerOpType::OP_DESTROY)
    {
        // destroy entity
        auto pEntity = GetEntity(self);
        ARK_ASSERT_RET_VAL(pEntity != nullptr, 0);

        auto pContainer = pEntity->FindContainer(index);
        ARK_ASSERT_RET_VAL(pContainer != nullptr, 0);

        auto pContainerEntity = pContainer->Find(src_index);
        ARK_ASSERT_RET_VAL(pContainerEntity != nullptr, 0);

        InnerDestroyEntity(pContainerEntity);
    }

    return 0;
}

template<typename T>
bool AFCKernelModule::DBDataToNode(T pData, const AFMsg::pb_db_entity_data& pb_db_entity_data)
{
    //bool data
    for (auto iter : pb_db_entity_data.datas_bool())
    {
        pData->SetBool(iter.first, iter.second);
    }

    //int32 data
    for (auto iter : pb_db_entity_data.datas_int32())
    {
        pData->SetInt32(iter.first, iter.second);
    }

    //uint32 data
    for (auto iter : pb_db_entity_data.datas_uint32())
    {
        pData->SetUInt32(iter.first, iter.second);
    }

    //int64 data
    for (auto iter : pb_db_entity_data.datas_int64())
    {
        pData->SetInt64(iter.first, iter.second);
    }

    //uint64 data
    for (auto iter : pb_db_entity_data.datas_uint64())
    {
        pData->SetUInt64(iter.first, iter.second);
    }

    //float data
    for (auto iter : pb_db_entity_data.datas_float())
    {
        pData->SetFloat(iter.first, iter.second);
    }

    //double data
    for (auto iter : pb_db_entity_data.datas_double())
    {
        pData->SetDouble(iter.first, iter.second);
    }

    //string data
    for (auto iter : pb_db_entity_data.datas_string())
    {
        pData->SetString(iter.first, iter.second);
    }

    return true;
}

//-----------------------------
bool AFCKernelModule::NodeToDBData(AFINode* pNode, AFMsg::pb_db_entity_data& pb_data)
{
    ARK_ASSERT_RET_VAL(pNode != nullptr, false);

    auto& name = pNode->GetName();
    switch (pNode->GetType())
    {
        case ArkDataType::DT_BOOLEAN:
            pb_data.mutable_datas_bool()->insert({name, pNode->GetBool()});
            break;
        case ArkDataType::DT_INT32:
            pb_data.mutable_datas_int32()->insert({name, pNode->GetInt32()});
            break;
        case ArkDataType::DT_UINT32:
            pb_data.mutable_datas_uint32()->insert({name, pNode->GetUInt32()});
            break;
        case ArkDataType::DT_INT64:
            pb_data.mutable_datas_int64()->insert({name, pNode->GetInt64()});
            break;
        case ArkDataType::DT_UINT64:
            pb_data.mutable_datas_uint64()->insert({name, pNode->GetUInt64()});
            break;
        case ArkDataType::DT_FLOAT:
            pb_data.mutable_datas_float()->insert({name, pNode->GetFloat()});
            break;
        case ArkDataType::DT_DOUBLE:
            pb_data.mutable_datas_double()->insert({name, pNode->GetDouble()});
            break;
        case ArkDataType::DT_STRING:
            pb_data.mutable_datas_string()->insert({name, pNode->GetString()});
            break;
        default:
            ARK_ASSERT_RET_VAL(0, false);
            break;
    }

    return true;
}

bool AFCKernelModule::TableToDBData(AFITable* pTable, AFMsg::pb_db_table& pb_data)
{
    ARK_ASSERT_RET_VAL(pTable != nullptr, false);

    for (auto pRow = pTable->First(); pRow != nullptr; pRow = pTable->Next())
    {
        auto pNodeManager = GetNodeManager(pRow);
        if (!pNodeManager)
        {
            continue;
        }

        AFMsg::pb_db_entity_data row_data;

        auto& data_list = pNodeManager->GetDataList();
        for (auto iter : data_list)
        {
            NodeToDBData(iter.second, row_data);
        }

        pb_data.mutable_datas_value()->insert({pRow->GetRow(), row_data});
    }

    return true;
}

//----------entity to pb client data---------------
bool AFCKernelModule::NodeToPBData(AFINode* pNode, AFMsg::pb_entity_data* pb_data)
{
    ARK_ASSERT_RET_VAL(pNode != nullptr && pb_data != nullptr, false);

    auto index = pNode->GetIndex();
    switch (pNode->GetType())
    {
        case ArkDataType::DT_BOOLEAN:
            pb_data->mutable_datas_bool()->insert({index, pNode->GetBool()});
            break;
        case ArkDataType::DT_INT32:
            pb_data->mutable_datas_int32()->insert({index, pNode->GetInt32()});
            break;
        case ArkDataType::DT_UINT32:
            pb_data->mutable_datas_uint32()->insert({index, pNode->GetUInt32()});
            break;
        case ArkDataType::DT_INT64:
            pb_data->mutable_datas_int64()->insert({index, pNode->GetInt64()});
            break;
        case ArkDataType::DT_UINT64:
            pb_data->mutable_datas_uint64()->insert({index, pNode->GetUInt64()});
            break;
        case ArkDataType::DT_FLOAT:
            pb_data->mutable_datas_float()->insert({index, pNode->GetFloat()});
            break;
        case ArkDataType::DT_DOUBLE:
            pb_data->mutable_datas_double()->insert({index, pNode->GetDouble()});
            break;
        case ArkDataType::DT_STRING:
            pb_data->mutable_datas_string()->insert({index, pNode->GetString()});
            break;
        default:
            ARK_ASSERT_RET_VAL(0, false);
            break;
    }

    return true;
}

bool AFCKernelModule::NodeToPBData(const uint32_t index, const AFIData& data, AFMsg::pb_entity_data* pb_data)
{
    ARK_ASSERT_RET_VAL(index > 0 && pb_data != nullptr, false);

    switch (data.GetType())
    {
        case ArkDataType::DT_BOOLEAN:
            pb_data->mutable_datas_bool()->insert({index, data.GetBool()});
            break;
        case ArkDataType::DT_INT32:
            pb_data->mutable_datas_int32()->insert({index, data.GetInt()});
            break;
        case ArkDataType::DT_UINT32:
            pb_data->mutable_datas_uint32()->insert({index, data.GetUInt()});
            break;
        case ArkDataType::DT_INT64:
            pb_data->mutable_datas_int64()->insert({index, data.GetInt64()});
            break;
        case ArkDataType::DT_UINT64:
            pb_data->mutable_datas_uint64()->insert({index, data.GetUInt64()});
            break;
        case ArkDataType::DT_FLOAT:
            pb_data->mutable_datas_float()->insert({index, data.GetFloat()});
            break;
        case ArkDataType::DT_DOUBLE:
            pb_data->mutable_datas_double()->insert({index, data.GetDouble()});
            break;
        case ArkDataType::DT_STRING:
            pb_data->mutable_datas_string()->insert({index, data.GetString()});
            break;
        default:
            ARK_ASSERT_RET_VAL(0, false);
            break;
    }

    return true;
}

bool AFCKernelModule::TableToPBData(AFITable* pTable, const uint32_t index, AFMsg::pb_table* pb_data)
{
    ARK_ASSERT_RET_VAL(pTable != nullptr && index > 0 && pb_data != nullptr, false);

    for (AFIRow* pRow = pTable->First(); pRow != nullptr; pRow = pTable->Next())
    {
        AFMsg::pb_entity_data row_data;
        if (!RowToPBData(pRow, pRow->GetRow(), &row_data))
        {
            continue;
        }

        pb_data->mutable_datas_value()->insert({index, row_data});
    }

    return true;
}

bool AFCKernelModule::RowToPBData(AFIRow* pRow, const uint32_t index, AFMsg::pb_entity_data* pb_data)
{
    ARK_ASSERT_RET_VAL(pRow != nullptr && index > 0 && pb_data != nullptr, false);

    auto pNodeManager = GetNodeManager(pRow);
    if (!pNodeManager)
    {
        return false;
    }

    auto& data_list = pNodeManager->GetDataList();
    for (auto iter : data_list)
    {
        NodeToPBData(iter.second, pb_data);
    }

    return true;
}

bool AFCKernelModule::TableRowDataToPBData(
    const uint32_t index, uint32_t row, const uint32_t col, const AFIData& data, AFMsg::pb_entity_data* pb_data)
{
    ARK_ASSERT_RET_VAL(index > 0 && row > 0 && col > 0 && pb_data != nullptr, false);

    AFMsg::pb_entity_data row_data;
    if (!NodeToPBData(col, data, &row_data))
    {
        return false;
    }

    AFMsg::pb_table table_data;
    table_data.mutable_datas_value()->insert({row, row_data});

    pb_data->mutable_datas_table()->insert({index, table_data});

    return true;
}

//node all to pb data
bool AFCKernelModule::NodeAllToPBData(ARK_SHARE_PTR<AFIEntity> pEntity, AFMsg::pb_entity_data* pb_data)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    auto pNodeManager = GetNodeManager(pEntity);
    ARK_ASSERT_RET_VAL(pNodeManager != nullptr, false);

    auto& data_list = pNodeManager->GetDataList();
    for (auto iter : data_list)
    {
        NodeToPBData(iter.second, pb_data);
    }

    return true;
}

//table all to pb data
bool AFCKernelModule::TableAllToPBData(ARK_SHARE_PTR<AFIEntity> pEntity, AFMsg::pb_entity_data* pb_data)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    auto pTableManager = GetTableManager(pEntity);
    ARK_ASSERT_RET_VAL(pTableManager != nullptr, false);

    auto& data_list = pTableManager->GetTableList();
    for (auto iter : data_list)
    {
        auto pTable = iter.second;
        if (!pTable)
        {
            continue;
        }
        const auto index = pTable->GetIndex();

        AFMsg::pb_table table_data;
        if (!TableToPBData(pTable, index, &table_data))
        {
            continue;
        }

        pb_data->mutable_datas_table()->insert({index, table_data});
    }

    return true;
}

bool AFCKernelModule::EntityToPBData(ARK_SHARE_PTR<AFIEntity> pEntity, AFMsg::pb_entity* pb_data)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    pb_data->set_id(pEntity->GetID());

    NodeAllToPBData(pEntity, pb_data->mutable_data());

    TableAllToPBData(pEntity, pb_data->mutable_data());

    return true;
}

bool AFCKernelModule::EntityToPBDataByMask(
    ARK_SHARE_PTR<AFIEntity> pEntity, ArkMaskType mask, AFMsg::pb_entity* pb_data)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    pb_data->set_id(pEntity->GetID());

    NodeToPBDataByMask(pEntity, mask, pb_data->mutable_data());

    TableToPBDataByMask(pEntity, mask, pb_data->mutable_data());

    return true;
}

//node all to pb data
bool AFCKernelModule::NodeToPBDataByMask(
    ARK_SHARE_PTR<AFIEntity> pEntity, const ArkMaskType mask, AFMsg::pb_entity_data* pb_data)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    auto pNodeManager = GetNodeManager(pEntity);
    ARK_ASSERT_RET_VAL(pNodeManager != nullptr, false);

    auto& data_list = pNodeManager->GetDataList();
    for (auto iter : data_list)
    {
        auto pNode = iter.second;
        if (!pNode)
        {
            continue;
        }

        auto result = (pNode->GetMask() & mask);
        if (!result.any())
        {
            continue;
        }

        NodeToPBData(pNode, pb_data);
    }

    return true;
}

bool AFCKernelModule::TableToPBDataByMask(
    ARK_SHARE_PTR<AFIEntity> pEntity, const ArkMaskType mask, AFMsg::pb_entity_data* pb_data)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    auto pTableManager = GetTableManager(pEntity);
    ARK_ASSERT_RET_VAL(pTableManager != nullptr, false);

    auto& data_list = pTableManager->GetTableList();
    for (auto iter : data_list)
    {
        auto pTable = iter.second;
        if (!pTable)
        {
            continue;
        }

        auto result = (pTable->GetMask() & mask);
        if (!result.any())
        {
            continue;
        }

        const auto index = pTable->GetIndex();

        AFMsg::pb_table table_data;
        if (!TableToPBData(pTable, index, &table_data))
        {
            continue;
        }

        pb_data->mutable_datas_table()->insert({index, table_data});
    }

    return true;
}

// -----------get entity manager--------------
ARK_SHARE_PTR<AFNodeManager> AFCKernelModule::GetNodeManager(ARK_SHARE_PTR<AFIStaticEntity> pStaticEntity) const
{
    if (pStaticEntity == nullptr)
    {
        return nullptr;
    }

    auto pCStaticEntity = std::dynamic_pointer_cast<AFCStaticEntity>(pStaticEntity);
    if (pCStaticEntity == nullptr)
    {
        return nullptr;
    }

    return pCStaticEntity->GetNodeManager();
}

ARK_SHARE_PTR<AFNodeManager> AFCKernelModule::GetNodeManager(ARK_SHARE_PTR<AFIEntity> pEntity) const
{
    if (pEntity == nullptr)
    {
        return nullptr;
    }

    auto pCEnity = std::dynamic_pointer_cast<AFCEntity>(pEntity);
    if (pCEnity == nullptr)
    {
        return nullptr;
    }

    return pCEnity->GetNodeManager();
}

ARK_SHARE_PTR<AFNodeManager> AFCKernelModule::GetNodeManager(AFIRow* pRow) const
{
    if (pRow == nullptr)
    {
        return nullptr;
    }

    auto pCRow = dynamic_cast<AFCRow*>(pRow);
    if (pCRow == nullptr)
    {
        return nullptr;
    }

    return pCRow->GetNodeManager();
}

ARK_SHARE_PTR<AFTableManager> AFCKernelModule::GetTableManager(ARK_SHARE_PTR<AFIEntity> pEntity) const
{
    if (pEntity == nullptr)
    {
        return nullptr;
    }

    auto pCEnity = std::dynamic_pointer_cast<AFCEntity>(pEntity);
    if (pCEnity == nullptr)
    {
        return nullptr;
    }

    return pCEnity->GetTableManager();
}

ARK_SHARE_PTR<AFIContainerManager> AFCKernelModule::GetContainerManager(ARK_SHARE_PTR<AFIEntity> pEntity) const
{
    if (pEntity == nullptr)
    {
        return nullptr;
    }

    auto pCEnity = std::dynamic_pointer_cast<AFCEntity>(pEntity);
    if (pCEnity == nullptr)
    {
        return nullptr;
    }

    return pCEnity->GetContainerManager();
}

ARK_SHARE_PTR<AFIEventManager> AFCKernelModule::GetEventManager(ARK_SHARE_PTR<AFIEntity> pEntity) const
{
    if (pEntity == nullptr)
    {
        return nullptr;
    }

    auto pCEnity = std::dynamic_pointer_cast<AFCEntity>(pEntity);
    if (pCEnity == nullptr)
    {
        return nullptr;
    }

    return pCEnity->GetEventManager();
}

} // namespace ark
