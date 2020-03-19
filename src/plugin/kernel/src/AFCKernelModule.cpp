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
#include "kernel/include/AFCTable.hpp"
#include "kernel/include/AFCDataList.hpp"
#include "kernel/include/AFCContainer.hpp"

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

    auto container_func = std::bind(&AFCKernelModule::OnContainerCallBack, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    AddCommonContainerCallBack(std::move(container_func), 999999); // after other callbacks being done

    AddSyncCallBack();

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

    for (auto& iter : objects_)
    {
        auto pEntity = iter.second;
        if (pEntity == nullptr)
        {
            continue;
        }

        pEntity->Update();
    }

    AFClassCallBackManager::OnDelaySync();

    return true;
}

bool AFCKernelModule::PreShut()
{
    return DestroyAll();
}

bool AFCKernelModule::CopyData(std::shared_ptr<AFIEntity> pEntity, std::shared_ptr<AFIStaticEntity> pStaticEntity)
{
    if (pEntity == nullptr || nullptr == pStaticEntity)
    {
        return false;
    }

    // static node manager must be not empty
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
    for (auto& iter : data_list)
    {
        auto pData = iter.second;
        pNodeManager->CreateData(pData);
    }

    return true;
}

std::shared_ptr<AFIEntity> AFCKernelModule::CreateEntity(const AFGUID& self, const int map_id,
    const int map_instance_id, const std::string& class_name, const ID_TYPE config_id, const AFIDataList& args)
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

    std::shared_ptr<AFIStaticEntity> pStaticEntity = nullptr;
    if (config_id > 0)
    {
        auto pStaticEntity = GetStaticEntity(config_id);
        if (nullptr == pStaticEntity)
        {
            ARK_LOG_ERROR("There is no config, config_id = {}", config_id);
            return nullptr;
        }

        if (pStaticEntity->GetClassName() != class_name)
        {
            ARK_LOG_ERROR("Config class does not match entity class, config_id = {}", config_id);
            return nullptr;
        }
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

    std::shared_ptr<AFIEntity> pEntity =
        std::make_shared<AFCEntity>(pClassMeta, object_id, config_id, map_id, map_instance_id, args);

    objects_.insert(object_id, pEntity);

    if (class_name == AFEntityMetaPlayer::self_name())
    {
        pMapInfo->AddEntityToInstance(map_instance_id, object_id, true);
    }
    //else if (class_name == AFEntityMetaPlayer::self_name()) // to do : npc type for now we do not have
    //{
    //}

    CopyData(pEntity, pStaticEntity);

    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_LOAD_DATA, args);

    // original args here

    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_LOAD_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_POST_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_DATA_FINISHED, args);

    return pEntity;
}

std::shared_ptr<AFIEntity> AFCKernelModule::CreateContainerEntity(
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

    std::shared_ptr<AFIStaticEntity> pStaticEntity = nullptr;
    if (config_id > 0)
    {
        auto pStaticEntity = GetStaticEntity(config_id);
        if (nullptr == pStaticEntity)
        {
            ARK_LOG_ERROR("There is no config, config_id = {}", config_id);
            return nullptr;
        }

        if (pStaticEntity->GetClassName() != class_name)
        {
            ARK_LOG_ERROR("Config class does not match entity class, config_id = {}", config_id);
            return nullptr;
        }
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

    std::shared_ptr<AFIEntity> pContainerEntity =
        std::make_shared<AFCEntity>(pClassMeta, object_id, config_id, map_id, map_instance_id, AFCDataList());

    objects_.insert(object_id, pContainerEntity);

    CopyData(pContainerEntity, pStaticEntity);

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

std::shared_ptr<AFIStaticEntity> AFCKernelModule::GetStaticEntity(const ID_TYPE config_id)
{
    return m_pConfigModule->FindStaticEntity(config_id);
}

std::shared_ptr<AFIEntity> AFCKernelModule::GetEntity(const AFGUID& self)
{
    return objects_.find_value(self);
}

bool AFCKernelModule::DestroyAll()
{
    for (auto& iter : objects_)
    {
        auto& pEntity = iter.second;
        if (pEntity->GetParentContainer() != nullptr)
        {
            continue;
        }

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

bool AFCKernelModule::InnerDestroyEntity(std::shared_ptr<AFIEntity> pEntity)
{
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("Cannot find this object, self={}", NULL_GUID);
        return false;
    }

    auto& self = pEntity->GetID();
    int32_t map_id = pEntity->GetMapID();
    int32_t inst_id = pEntity->GetMapEntityID();

    std::shared_ptr<AFMapInfo> pMapInfo = m_pMapModule->GetMapInfo(map_id);
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
    std::shared_ptr<AFIEntity> pEntity = GetEntity(self);
    ARK_ASSERT_RET_VAL(pEntity != nullptr, false);

    auto pEventManager = GetEventManager(pEntity);
    ARK_ASSERT_RET_VAL(pEventManager != nullptr, false);

    return pEventManager->AddEventCallBack(nEventID, std::forward<EVENT_PROCESS_FUNCTOR>(cb));
}

bool AFCKernelModule::AddClassCallBack(const std::string& class_name, CLASS_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    return m_pClassModule->AddClassCallBack(class_name, std::forward<CLASS_EVENT_FUNCTOR>(cb), prio);
}

bool AFCKernelModule::AddNodeCallBack(
    const std::string& class_name, const std::string& name, DATA_NODE_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto index = pClassMeta->GetIndex(name);
    if (index == 0)
    {
        return false;
    }

    AddNodeCallBack(class_name, index, std::forward<DATA_NODE_EVENT_FUNCTOR>(cb), prio);

    return true;
}

bool AFCKernelModule::AddTableCallBack(
    const std::string& class_name, const std::string& name, DATA_TABLE_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto index = pClassMeta->GetIndex(name);
    if (index == 0)
    {
        return false;
    }

    AddTableCallBack(class_name, index, std::forward<DATA_TABLE_EVENT_FUNCTOR>(cb), prio);

    return true;
}

bool AFCKernelModule::AddNodeCallBack(
    const std::string& class_name, const uint32_t index, DATA_NODE_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pDataMeta = pClassMeta->FindDataMeta(index);
    ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->AddDataCallBack(index, std::forward<DATA_NODE_EVENT_FUNCTOR>(cb), prio);

    return true;
}

bool AFCKernelModule::AddTableCallBack(
    const std::string& class_name, const uint32_t index, DATA_TABLE_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pTableMeta = pClassMeta->FindTableMeta(index);
    ARK_ASSERT_RET_VAL(pTableMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->AddTableCallBack(index, std::forward<DATA_TABLE_EVENT_FUNCTOR>(cb), prio);

    return true;
}

bool AFCKernelModule::AddContainerCallBack(
    const std::string& class_name, const uint32_t index, CONTAINER_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pContainerMeta = pClassMeta->FindContainerMeta(index);
    ARK_ASSERT_RET_VAL(pContainerMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->AddContainerCallBack(index, std::forward<CONTAINER_EVENT_FUNCTOR>(cb), prio);

    return true;
}

bool AFCKernelModule::AddCommonContainerCallBack(CONTAINER_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto pClassMeta = m_pClassModule->FindMeta(AFEntityMetaPlayer::self_name());
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto& meta_list = pClassMeta->GetContainerMetaList();
    for (auto& iter : meta_list)
    {
        auto pMeta = iter.second;
        if (!pMeta)
        {
            continue;
        }

        auto call_back = cb;
        AddContainerCallBack(AFEntityMetaPlayer::self_name(), pMeta->GetIndex(), std::move(call_back), prio);
    }

    return true;
}

bool AFCKernelModule::AddCommonClassEvent(CLASS_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto& class_meta_list = m_pClassModule->GetMetaList();
    for (auto& iter : class_meta_list)
    {
        auto pClassMeta = iter.second;
        if (nullptr == pClassMeta)
        {
            continue;
        }

        auto call_back = cb;
        AddClassCallBack(iter.first, std::move(call_back), prio);
    }

    return true;
}

bool AFCKernelModule::AddLeaveSceneEvent(const std::string& class_name, SCENE_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    return pCallBack->AddLeaveSceneEvent(std::forward<SCENE_EVENT_FUNCTOR>(cb), prio);
}

bool AFCKernelModule::AddEnterSceneEvent(const std::string& class_name, SCENE_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    return pCallBack->AddEnterSceneEvent(std::forward<SCENE_EVENT_FUNCTOR>(cb), prio);
}

bool AFCKernelModule::AddMoveEvent(const std::string& class_name, MOVE_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    return pCallBack->AddMoveEvent(std::forward<MOVE_EVENT_FUNCTOR>(cb), prio);
}

void AFCKernelModule::AddSyncCallBack()
{
    // node sync call back
    auto node_func = std::bind(&AFCKernelModule::OnSyncNode, this, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4);

    AFClassCallBackManager::AddNodeSyncCallBack(ArkDataMask::PF_SYNC_VIEW, std::move(node_func));
    AFClassCallBackManager::AddNodeSyncCallBack(ArkDataMask::PF_SYNC_SELF, std::move(node_func));

    // table sync call back
    auto table_func = std::bind(&AFCKernelModule::OnSyncTable, this, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4);

    AFClassCallBackManager::AddTableSyncCallBack(ArkDataMask::PF_SYNC_VIEW, std::move(table_func));
    AFClassCallBackManager::AddTableSyncCallBack(ArkDataMask::PF_SYNC_SELF, std::move(table_func));

    // container sync call back
    auto container_func =
        std::bind(&AFCKernelModule::OnSyncContainer, this, std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

    AFClassCallBackManager::AddContainerSyncCallBack(ArkDataMask::PF_SYNC_VIEW, std::move(container_func));
    AFClassCallBackManager::AddContainerSyncCallBack(ArkDataMask::PF_SYNC_SELF, std::move(container_func));

    // data delay call back
    auto delay_func = std::bind(
        &AFCKernelModule::OnDelaySyncData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    AFClassCallBackManager::AddDelaySyncCallBack(ArkDataMask::PF_SYNC_VIEW, std::move(delay_func));
    AFClassCallBackManager::AddDelaySyncCallBack(ArkDataMask::PF_SYNC_SELF, std::move(delay_func));

    // send msg functor
    auto view_func = std::bind(&AFCKernelModule::SendToView, this, std::placeholders::_1, std::placeholders::_2);
    sync_functors.insert(std::make_pair(ArkDataMask::PF_SYNC_VIEW, std::forward<SYNC_FUNCTOR>(view_func)));

    auto self_func = std::bind(&AFCKernelModule::SendToSelf, this, std::placeholders::_1, std::placeholders::_2);
    sync_functors.insert(std::make_pair(ArkDataMask::PF_SYNC_SELF, std::forward<SYNC_FUNCTOR>(self_func)));
}

int AFCKernelModule::OnSyncNode(
    const AFGUID& self, const uint32_t index, const ArkDataMask mask_value, const AFIData& data)
{
    // find parent entity
    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync node failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_entity pb_data;
    auto entity_id = self;
    auto pb_entity = pb_data.mutable_data();

    auto pParentContainer = pEntity->GetParentContainer();
    if (pParentContainer != nullptr)
    {
        if (!TryAddContainerPBEntity(pParentContainer, self, *pb_data.mutable_data(), entity_id, pb_entity))
        {
            ARK_LOG_ERROR("sync node failed container entity do no exist, self={}", self);
            return -1;
        }
    }

    if (!NodeToPBData(index, data, pb_entity))
    {
        ARK_LOG_ERROR("node to pb failed, self={}, index={}", self, index);
        return -1;
    }

    pb_data.set_id(entity_id);
    if (SendSyncMsg(entity_id, mask_value, pb_data) < 0)
    {
        ARK_LOG_ERROR("send sync msg failed, self={}, index={}", entity_id, index);
        return -1;
    }

    return 0;
}

int AFCKernelModule::OnSyncTable(
    const AFGUID& self, const TABLE_EVENT_DATA& event, const ArkDataMask mask_value, const AFIData& data)
{
    ArkTableOpType op_type = static_cast<ArkTableOpType>(event.op_type_);
    switch (op_type)
    {
        case ArkTableOpType::TABLE_ADD:
        {
            OnSyncTableAdd(self, event, mask_value);
        }
        break;
        case ArkTableOpType::TABLE_DELETE:
        {
            OnSyncTableDelete(self, event, mask_value);
        }
        break;
        case ArkTableOpType::TABLE_SWAP:
            // do not have yet
            break;
        case ArkTableOpType::TABLE_UPDATE:
        {
            OnSyncTableUpdate(self, event, mask_value, data);
        }
        break;
        case ArkTableOpType::TABLE_COVERAGE:
            // will do something
            break;
        default:
            break;
    }

    return 0;
}

int AFCKernelModule::OnSyncContainer(const AFGUID& self, const uint32_t index, const ArkDataMask mask,
    const ArkContainerOpType op_type, uint32_t src_index, uint32_t dest_index)
{
    switch (op_type)
    {
        case ArkContainerOpType::OP_PLACE:
        {
            OnSyncContainerPlace(self, index, mask, src_index);
        }
        break;
        case ArkContainerOpType::OP_REMOVE:
        {
            OnSyncContainerRemove(self, index, mask, src_index);
        }
        break;
        case ArkContainerOpType::OP_DESTROY:
        {
            OnSyncContainerDestroy(self, index, mask, src_index);
        }
        break;
        case ArkContainerOpType::OP_SWAP:
        {
            OnSyncContainerSwap(self, index, mask, src_index, dest_index);
        }
        break;
        default:
            break;
    }
    return 0;
}

int AFCKernelModule::OnDelaySyncData(const AFGUID& self, const ArkDataMask mask_value, const AFDelaySyncData& data)
{
    if (data.node_list_.size() == 0 && data.table_list_.size() == 0)
    {
        return 0;
    }

    // find parent entity
    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync delay data failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_delay_entity pb_data;
    auto entity_id = self;
    auto pb_entity = pb_data.mutable_data();

    auto pParentContainer = pEntity->GetParentContainer();
    if (pParentContainer != nullptr)
    {
        if (!TryAddContainerPBEntity(pParentContainer, self, *pb_data.mutable_data(), entity_id, pb_entity))
        {
            ARK_LOG_ERROR("sync delay data failed container entity do no exist, self={}", self);
            return -1;
        }
    }

    // node to pb
    for (auto& iter : data.node_list_)
    {
        auto pNode = iter;
        if (!NodeToPBData(pNode, pb_entity))
        {
            continue;
        }
    }

    // table to pb
    for (auto& iter : data.table_list_)
    {
        auto table_index = iter.first;
        auto& table = iter.second;
        DelayTableToPB(table, table_index, pb_data, *pb_entity);
    }

    // container to pb
    for (auto& iter : data.container_list_)
    {
        auto container_index = iter.first;
        auto& container = iter.second;
        DelayContainerToPB(pEntity, container, container_index, pb_data);
    }

    pb_data.set_id(entity_id);

    if (SendSyncMsg(entity_id, mask_value, pb_data) < 0)
    {
        ARK_LOG_ERROR("send sync msg failed, self={}", self);
        return -1;
    }

    return 0;
}

int AFCKernelModule::OnSyncTableAdd(const AFGUID& self, const TABLE_EVENT_DATA& event, const ArkDataMask mask_value)
{
    // find parent entity
    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync table failed entity do no exist, self={}", self);
        return -1;
    }

    auto pTable = pEntity->FindTable(event.table_index_);
    if (nullptr == pTable)
    {
        ARK_LOG_ERROR("sync table failed table do no exist, self={}, table={}", self, event.table_name_);
        return -1;
    }

    auto pRow = pTable->FindRow(event.row_);
    if (nullptr == pRow)
    {
        ARK_LOG_ERROR(
            "sync table failed table row do no exist, self={}, table={}, row={}", self, event.table_name_, event.row_);
        return -1;
    }

    AFMsg::pb_entity_table_add pb_data;
    auto entity_id = self;
    auto pb_entity = pb_data.mutable_data();

    auto pParentContainer = pEntity->GetParentContainer();
    if (pParentContainer != nullptr)
    {
        if (!TryAddContainerPBEntity(pParentContainer, self, *pb_data.mutable_data(), entity_id, pb_entity))
        {
            ARK_LOG_ERROR("sync node failed container entity do no exist, self={}", self);
            return -1;
        }
    }

    AFMsg::pb_entity_data pb_row;
    if (!RowToPBData(pRow, event.row_, &pb_row))
    {
        ARK_LOG_ERROR(
            "sync table failed table row do no exist, self={}, table={}, row={}", self, event.table_name_, event.row_);
        return -1;
    }

    AFMsg::pb_table pb_table;
    pb_table.mutable_datas_value()->insert({event.row_, pb_row});
    pb_entity->mutable_datas_table()->insert({event.table_index_, pb_table});

    pb_data.set_id(entity_id);
    SendSyncMsg(entity_id, mask_value, pb_data);

    return 0;
}

int AFCKernelModule::OnSyncTableDelete(const AFGUID& self, const TABLE_EVENT_DATA& event, const ArkDataMask mask_value)
{
    // find parent entity
    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync table failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_entity_table_delete pb_data;
    auto entity_id = self;
    auto pb_entity = pb_data.mutable_data();

    auto pParentContainer = pEntity->GetParentContainer();
    if (pParentContainer != nullptr)
    {
        if (!TryAddContainerPBEntity(pParentContainer, self, *pb_data.mutable_data(), entity_id, pb_entity))
        {
            ARK_LOG_ERROR("sync node failed container entity do no exist, self={}", self);
            return -1;
        }
    }

    AFMsg::pb_entity_data pb_row;
    AFMsg::pb_table pb_table;
    pb_table.mutable_datas_value()->insert({event.row_, pb_row});
    pb_entity->mutable_datas_table()->insert({event.table_index_, pb_table});

    SendSyncMsg(entity_id, mask_value, pb_data);
    return 0;
}

int AFCKernelModule::OnSyncTableUpdate(
    const AFGUID& self, const TABLE_EVENT_DATA& event, const ArkDataMask mask_value, const AFIData& data)
{
    // find parent entity
    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync table failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_entity_table_update pb_data;
    auto entity_id = self;
    auto pb_entity = pb_data.mutable_data();

    auto pParentContainer = pEntity->GetParentContainer();
    if (pParentContainer != nullptr)
    {
        if (!TryAddContainerPBEntity(pParentContainer, self, *pb_data.mutable_data(), entity_id, pb_entity))
        {
            ARK_LOG_ERROR("sync node failed container entity do no exist, self={}", self);
            return -1;
        }
    }

    AFMsg::pb_entity_data pb_row;
    if (!NodeToPBData(event.data_index_, data, &pb_row))
    {
        ARK_LOG_ERROR(
            "sync table failed table node do no exist, self={}, table={}, row={}", self, event.table_name_, event.row_);
        return -1;
    }

    AFMsg::pb_table pb_table;
    pb_table.mutable_datas_value()->insert({event.row_, pb_row});
    pb_entity->mutable_datas_table()->insert({event.table_index_, pb_table});

    pb_data.set_id(entity_id);
    SendSyncMsg(entity_id, mask_value, pb_data);
    return 0;
}

int AFCKernelModule::OnSyncContainerPlace(
    const AFGUID& self, const uint32_t index, const ArkDataMask mask, uint32_t src_index)
{
    // find parent entity
    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync container failed entity do no exist, self={}", self);
        return -1;
    }

    auto pContainer = pEntity->FindContainer(index);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync container failed container do no exist, self={}, container={}", self, index);
        return -1;
    }

    auto pContainerEntity = pContainer->Find(src_index);
    if (pContainerEntity == nullptr)
    {
        ARK_LOG_ERROR("sync container failed container entity do no exist, self={}, container={}, entity={}", self,
            index, src_index);
        return -1;
    }

    if (pContainerEntity->IsSent())
    {
        AFMsg::pb_container_place pb_data;
        pb_data.set_id(self);
        pb_data.set_index(index);
        pb_data.set_entity_index(src_index);
        pb_data.set_entity_id(pContainerEntity->GetID());

        SendSyncMsg(self, mask, pb_data);
    }
    else
    {
        AFMsg::pb_container_create pb_data;
        if (!EntityToPBData(pContainerEntity, pb_data.mutable_data()))
        {
            ARK_LOG_ERROR("sync container failed container entity to pb failed, self={}, container={}, entity={}", self,
                index, src_index);
            return -1;
        }

        pb_data.set_id(self);
        pb_data.set_index(index);
        pb_data.set_entity_index(src_index);

        SendSyncMsg(self, mask, pb_data);
    }

    return 0;
}

int AFCKernelModule::OnSyncContainerRemove(
    const AFGUID& self, const uint32_t index, const ArkDataMask mask, uint32_t src_index)
{
    // find parent entity
    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync container failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_container_remove pb_data;
    pb_data.set_id(self);
    pb_data.set_index(index);
    pb_data.set_entity_index(src_index);

    SendSyncMsg(self, mask, pb_data);

    return 0;
}

int AFCKernelModule::OnSyncContainerDestroy(
    const AFGUID& self, const uint32_t index, const ArkDataMask mask, uint32_t src_index)
{
    // find parent entity
    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync container failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_container_destroy pb_data;
    pb_data.set_id(self);
    pb_data.set_index(index);
    pb_data.set_entity_index(src_index);

    SendSyncMsg(self, mask, pb_data);

    return 0;
}

int AFCKernelModule::OnSyncContainerSwap(
    const AFGUID& self, const uint32_t index, const ArkDataMask mask, uint32_t src_index, uint32_t dest_index)
{
    // find parent entity
    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync container failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_container_swap pb_data;
    pb_data.set_id(self);
    pb_data.set_index(index);
    pb_data.set_src_index(src_index);
    pb_data.set_dest_index(dest_index);

    SendSyncMsg(self, mask, pb_data);

    return 0;
}

bool AFCKernelModule::DelayTableToPB(
    const AFDelaySyncTable& table, const uint32_t index, AFMsg::pb_delay_entity& data, AFMsg::pb_entity_data& pb_entity)
{
    AFMsg::pb_table pb_table;
    AFMsg::delay_clear_row clear_row_list;
    for (auto& iter : table.row_list_)
    {
        AFMsg::pb_entity_data pb_row;

        auto row_index = iter.first;
        auto& row_data = iter.second;
        for (auto& iter_row : row_data.node_list_)
        {
            auto pNode = iter_row;
            if (NodeToPBData(pNode, &pb_row))
            {
                pb_table.mutable_datas_value()->insert({row_index, pb_row});
            }
        }

        if (row_data.need_clear_)
        {
            clear_row_list.add_row_list(row_index);
        }
    }

    if (table.need_clear_)
    {
        data.add_clear_tables(index);
    }

    if (clear_row_list.row_list_size() > 0)
    {
        data.mutable_clear_rows()->insert({index, clear_row_list});
    }

    pb_entity.mutable_datas_table()->insert({index, pb_table});

    return true;
}

bool AFCKernelModule::DelayContainerToPB(std::shared_ptr<AFIEntity> pEntity, const AFDelaySyncContainer& container,
    const uint32_t index, AFMsg::pb_delay_entity& data)
{
    AFMsg::delay_container pb_delay_container;
    AFMsg::pb_container pb_conatiner;
    for (auto& iter : container.index_list_)
    {
        auto entity_index = iter;
        auto pContaier = pEntity->FindContainer(index);
        if (pContaier == nullptr)
        {
            continue;
        }

        auto pContainerEntity = pContaier->Find(entity_index);
        if (pContainerEntity == nullptr)
        {
            pb_delay_container.mutable_entity_list()->insert({entity_index, NULL_GUID});
        }
        else if (pEntity->IsSent())
        {
            pb_delay_container.mutable_entity_list()->insert({entity_index, pContainerEntity->GetID()});
        }
        else
        {
            AFMsg::pb_entity pb_container_entity;
            if (!EntityToPBData(pContainerEntity, &pb_container_entity))
            {
                continue;
            }

            pb_conatiner.mutable_datas_value()->insert({entity_index, pb_container_entity});
        }
    }

    if (pb_conatiner.datas_value_size() > 0)
    {
        data.mutable_data()->mutable_datas_container()->insert({index, pb_conatiner});
    }
    data.mutable_container_entity()->insert({index, pb_delay_container});

    AFMsg::delay_container pb_destroy_entity;
    for (auto& iter : container.destroy_list_)
    {
        auto entity_index = iter;
        pb_destroy_entity.mutable_entity_list()->insert({entity_index, NULL_GUID});
    }

    data.mutable_destroy_entity()->insert({index, pb_destroy_entity});

    return true;
}

bool AFCKernelModule::TryAddContainerPBEntity(std::shared_ptr<AFIContainer> pContainer, const AFGUID& self,
    AFMsg::pb_entity_data& pb_entity_data, AFGUID& parent_id, AFMsg::pb_entity_data*& pb_container_entity)
{
    parent_id = pContainer->GetParentID();
    auto pParentEntity = GetEntity(parent_id);
    if (pParentEntity == nullptr)
    {
        ARK_LOG_ERROR("parent entity do no exist, parent={}", parent_id);
        return false;
    }

    auto self_index = pContainer->Find(self);
    if (self_index == 0)
    {
        ARK_LOG_ERROR("entity is not in container, self={}", self);
        return false;
    }

    AFMsg::pb_container pb_container;
    auto result_container = pb_entity_data.mutable_datas_container()->insert({pContainer->GetIndex(), pb_container});
    if (!result_container.second)
    {
        ARK_LOG_ERROR("entity insert container failed, self={} container index = {}", self, pContainer->GetIndex());
        return false;
    }
    auto& container = result_container.first->second;

    AFMsg::pb_entity pb_entity;
    auto result_entity = container.mutable_datas_value()->insert({self_index, pb_entity});
    if (!result_entity.second)
    {
        ARK_LOG_ERROR("container insert entity failed, self={} container index = {}", self, pContainer->GetIndex());
        return false;
    }

    auto& entity = result_entity.first->second;
    pb_container_entity = entity.mutable_data();
    return true;
}

int AFCKernelModule::SendSyncMsg(const AFGUID& self, const ArkDataMask mask_value, const google::protobuf::Message& msg)
{
    auto iter = sync_functors.find(mask_value);
    if (iter == sync_functors.end())
    {
        return -1;
    }

    iter->second(self, msg);

    return 0;
}

int AFCKernelModule::SendToView(const AFGUID& self, const google::protobuf::Message& msg)
{
    auto pEntity = GetEntity(self);
    if (nullptr == pEntity)
    {
        return -1;
    }

    auto map_id = pEntity->GetMapID();
    auto inst_id = pEntity->GetMapEntityID();

    AFCDataList map_inst_entity_list;
    m_pMapModule->GetInstEntityList(map_id, inst_id, map_inst_entity_list);

    for (size_t i = 0; i < map_inst_entity_list.GetCount(); i++)
    {
        auto pViewEntity = GetEntity(map_inst_entity_list.Int64(i));
        if (pViewEntity == nullptr)
        {
            continue;
        }

        const std::string& strObjClassName = pViewEntity->GetClassName();
        if (AFEntityMetaPlayer::self_name() == strObjClassName)
        {
            SendToSelf(pViewEntity->GetID(), msg);
        }
    }

    return 0;
}

int AFCKernelModule::SendToSelf(const AFGUID& self, const google::protobuf::Message& msg)
{
    //SendMsgPBToGate(AFMsg::EGMI_ACK_NODE_DATA, entity, ident);
    return 0;
}

bool AFCKernelModule::DoEvent(
    const AFGUID& self, const std::string& class_name, ArkEntityEvent class_event, const AFIDataList& args)
{
    return m_pClassModule->DoClassEvent(self, class_name, class_event, args);
}

bool AFCKernelModule::DoEvent(const AFGUID& self, const int event_id, const AFIDataList& args)
{
    std::shared_ptr<AFIEntity> pEntity = GetEntity(self);
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
    for (auto& iter : node_list)
    {
        auto pData = iter.second;
        if (!pData)
        {
            continue;
        }

        ARK_LOG_TRACE("Player[{}] Node[{}] Value[{}]", guid, pData->GetName(), pData->ToString());
    }

    auto& table_list = pTableManager->GetTableList();
    for (auto& iter : table_list)
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
            for (auto& iter_row : row_data_list)
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
    std::shared_ptr<AFIEntity> pEntity = GetEntity(id);
    if (pEntity == nullptr)
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
    std::shared_ptr<AFIEntity> pEntity = GetEntity(self);
    return EntityToDBData(pEntity, pb_data);
}

bool AFCKernelModule::EntityToDBData(std::shared_ptr<AFIEntity> pEntity, AFMsg::pb_db_entity& pb_data)
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
    for (auto& iter : node_list)
    {
        auto pNode = iter.second;
        if (!pNode)
        {
            continue;
        }

        if (!pNode->HaveMask(ArkDataMask::PF_SAVE))
        {
            continue;
        }

        NodeToDBData(pNode, *pb_data.mutable_data());
    }

    // table to db
    auto& table_list = pTableManager->GetTableList();
    for (auto& iter : table_list)
    {
        auto pTable = iter.second;
        if (!pTable)
        {
            continue;
        }

        if (!pTable->HaveMask(ArkDataMask::PF_SAVE))
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
    for (auto& iter : container_list)
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

std::shared_ptr<AFIEntity> AFCKernelModule::CreateEntity(const AFMsg::pb_db_entity& pb_data)
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
    auto pMapInfo = m_pMapModule->GetMapInfo(map_id);
    if (pMapInfo == nullptr)
    {
        ARK_LOG_ERROR("There is no scene, scene = {}", map_id);
        return nullptr;
    }

    auto pCEntity = std::make_shared<AFCEntity>(pClassMeta, entity_id, NULL_INT, map_id, map_inst_id, AFCDataList());
    pEntity = std::static_pointer_cast<AFIEntity>(pCEntity);

    objects_.insert(entity_id, pEntity);

    if (class_name == AFEntityMetaPlayer::self_name())
    {
        pMapInfo->AddEntityToInstance(map_inst_id, entity_id, true);
    }
    //else if (class_name == AFEntityMetaPlayer::self_name()) // to do : npc type for now we do not have
    //{
    //}

    // init data
    auto& pb_db_entity_data = pb_data.data();

    // node data
    auto pNodeManager = pCEntity->GetNodeManager();
    if (nullptr != pNodeManager)
    {
        DBDataToNode(pNodeManager, pb_db_entity_data);
    }

    // table data
    auto pTableManager = pCEntity->GetTableManager();
    if (nullptr != pTableManager)
    {
        for (auto& iter : pb_db_entity_data.datas_table())
        {
            DBDataToTable(pEntity, iter.first, iter.second);
        }
    }

    // container data
    for (auto& iter : pb_db_entity_data.datas_entity())
    {
        DBDataToContainer(pEntity, iter.first, iter.second);
    }

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
    std::shared_ptr<AFIEntity> pEntity, const std::string& name, const AFMsg::pb_db_table& pb_table)
{
    auto pTable = pEntity->FindTable(name);
    ARK_ASSERT_RET_VAL(pTable != nullptr, false);

    auto pCTable = dynamic_cast<AFCTable*>(pTable);
    ARK_ASSERT_RET_VAL(pCTable != nullptr, false);

    for (auto& iter : pb_table.datas_value())
    {
        auto row_index = iter.first;
        if (row_index == NULL_INT)
        {
            continue;
        }

        auto& pb_db_entity_data = iter.second;
        auto pRow = pCTable->CreateRow(row_index, AFCDataList());
        if (pRow == nullptr)
        {
            continue;
        }

        auto pNodeManager = GetNodeManager(pRow);
        if (pNodeManager == nullptr)
        {
            continue;
        }

        DBDataToNode(pNodeManager, pb_db_entity_data);
    }

    return true;
}

bool AFCKernelModule::DBDataToContainer(
    std::shared_ptr<AFIEntity> pEntity, const std::string& name, const AFMsg::pb_db_container& pb_data)
{
    auto pContainer = pEntity->FindContainer(name);
    if (nullptr == pContainer)
    {
        return false;
    }

    auto pCContainer = std::dynamic_pointer_cast<AFCContainer>(pContainer);
    if (nullptr == pCContainer)
    {
        return false;
    }

    for (auto& iter : pb_data.datas_value())
    {
        auto pContainerEntity = CreateEntity(iter.second);
        if (nullptr == pContainerEntity)
        {
            continue;
        }

        pCContainer->InitEntityList(iter.first, pContainerEntity);
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

bool AFCKernelModule::DBDataToNode(
    std::shared_ptr<AFNodeManager> pNodeManager, const AFMsg::pb_db_entity_data& pb_db_entity_data)
{
    //bool data
    for (auto& iter : pb_db_entity_data.datas_bool())
    {
        auto pNode = pNodeManager->CreateData(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->SetBool(iter.second);
    }

    //int32 data
    for (auto& iter : pb_db_entity_data.datas_int32())
    {
        auto pNode = pNodeManager->CreateData(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->SetInt32(iter.second);
    }

    //uint32 data
    for (auto& iter : pb_db_entity_data.datas_uint32())
    {
        auto pNode = pNodeManager->CreateData(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->SetUInt32(iter.second);
    }

    //int64 data
    for (auto& iter : pb_db_entity_data.datas_int64())
    {
        auto pNode = pNodeManager->CreateData(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->SetInt64(iter.second);
    }

    //uint64 data
    for (auto& iter : pb_db_entity_data.datas_uint64())
    {
        auto pNode = pNodeManager->CreateData(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->SetUInt64(iter.second);
    }

    //float data
    for (auto& iter : pb_db_entity_data.datas_float())
    {
        auto pNode = pNodeManager->CreateData(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->SetFloat(iter.second);
    }

    //double data
    for (auto& iter : pb_db_entity_data.datas_double())
    {
        auto pNode = pNodeManager->CreateData(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->SetDouble(iter.second);
    }

    //string data
    for (auto& iter : pb_db_entity_data.datas_string())
    {
        auto pNode = pNodeManager->CreateData(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->SetString(iter.second);
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
        for (auto& iter : data_list)
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

bool AFCKernelModule::ContainerToPBData(std::shared_ptr<AFIContainer> pContainer, AFMsg::pb_container* pb_data)
{
    ARK_ASSERT_RET_VAL(pContainer != nullptr && pb_data != nullptr, false);

    for (auto index = pContainer->First(); index > 0; index = pContainer->Next())
    {
        auto pEntity = pContainer->Find(index);
        if (nullptr == pEntity)
        {
            continue;
        }

        AFMsg::pb_entity pb_entity;
        if (!EntityToPBData(pEntity, &pb_entity))
        {
            continue;
        }

        pb_data->mutable_datas_value()->insert({index, pb_entity});
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
    for (auto& iter : data_list)
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

bool AFCKernelModule::EntityToPBData(std::shared_ptr<AFIEntity> pEntity, AFMsg::pb_entity* pb_data)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    pb_data->set_id(pEntity->GetID());

    EntityNodeToPBData(pEntity, pb_data->mutable_data());

    EntityTableToPBData(pEntity, pb_data->mutable_data());

    EntityContainerToPBData(pEntity, pb_data->mutable_data());

    return true;
}

bool AFCKernelModule::EntityToPBDataByMask(
    std::shared_ptr<AFIEntity> pEntity, ArkMaskType mask, AFMsg::pb_entity* pb_data)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    pb_data->set_id(pEntity->GetID());

    EntityNodeToPBData(pEntity, pb_data->mutable_data(), mask);

    EntityTableToPBData(pEntity, pb_data->mutable_data(), mask);

    EntityContainerToPBData(pEntity, pb_data->mutable_data(), mask);

    return true;
}

bool AFCKernelModule::EntityContainerToPBData(
    std::shared_ptr<AFIEntity> pEntity, AFMsg::pb_entity_data* pb_data, const ArkMaskType mask)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    auto pContainerManager = GetContainerManager(pEntity);
    ARK_ASSERT_RET_VAL(pContainerManager != nullptr, false);

    auto& container_list = pContainerManager->GetContainerList();
    for (auto& iter : container_list)
    {
        auto pContainer = iter.second;
        if (!pContainer)
        {
            continue;
        }

        if (!mask.none())
        {
            auto result = (pContainer->GetMask() & mask);
            if (!result.any())
            {
                continue;
            }
        }

        AFMsg::pb_container pb_container;
        if (!ContainerToPBData(pContainer, &pb_container))
        {
            continue;
        }

        pb_data->mutable_datas_container()->insert({iter.first, pb_container});
    }

    return true;
}

//node all to pb data
bool AFCKernelModule::EntityNodeToPBData(
    std::shared_ptr<AFIEntity> pEntity, AFMsg::pb_entity_data* pb_data, const ArkMaskType mask /* = 0*/)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    auto pNodeManager = GetNodeManager(pEntity);
    ARK_ASSERT_RET_VAL(pNodeManager != nullptr, false);

    auto& data_list = pNodeManager->GetDataList();
    for (auto& iter : data_list)
    {
        auto pNode = iter.second;
        if (!pNode)
        {
            continue;
        }

        if (!mask.none())
        {
            auto result = (pNode->GetMask() & mask);
            if (!result.any())
            {
                continue;
            }
        }

        NodeToPBData(pNode, pb_data);
    }

    return true;
}

bool AFCKernelModule::EntityTableToPBData(
    std::shared_ptr<AFIEntity> pEntity, AFMsg::pb_entity_data* pb_data, const ArkMaskType mask /* = 0*/)
{
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    auto pTableManager = GetTableManager(pEntity);
    ARK_ASSERT_RET_VAL(pTableManager != nullptr, false);

    auto& data_list = pTableManager->GetTableList();
    for (auto& iter : data_list)
    {
        auto pTable = iter.second;
        if (!pTable)
        {
            continue;
        }

        if (!mask.none())
        {
            auto result = (pTable->GetMask() & mask);
            if (!result.any())
            {
                continue;
            }
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
std::shared_ptr<AFNodeManager> AFCKernelModule::GetNodeManager(std::shared_ptr<AFIStaticEntity> pStaticEntity) const
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

std::shared_ptr<AFNodeManager> AFCKernelModule::GetNodeManager(std::shared_ptr<AFIEntity> pEntity) const
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

std::shared_ptr<AFNodeManager> AFCKernelModule::GetNodeManager(AFIRow* pRow) const
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

std::shared_ptr<AFTableManager> AFCKernelModule::GetTableManager(std::shared_ptr<AFIEntity> pEntity) const
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

std::shared_ptr<AFIContainerManager> AFCKernelModule::GetContainerManager(std::shared_ptr<AFIEntity> pEntity) const
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

std::shared_ptr<AFIEventManager> AFCKernelModule::GetEventManager(std::shared_ptr<AFIEntity> pEntity) const
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
