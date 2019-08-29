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

ARK_SHARE_PTR<AFIEntity> AFCKernelModule::CreateEntity(const AFGUID& self, const int map_id, const int map_instance_id,
    const std::string& class_name, const ID_TYPE_ARG config_id, const AFIDataList& args)
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

    if (config_id > 0)
    {
        auto pStaticEntity = m_pConfigModule->FindStaticEntity(class_name, config_id);
        if (pStaticEntity != nullptr)
        {
            pEntity->InitData(pStaticEntity);
        }
    }

    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_LOAD_DATA, args);

    // TODO: set data
    for (size_t i = 0; i < arg_count; i += 2)
    {
        const std::string& data_name = args.String(i);
        // switch (args.GetType(i + 1))
        //{
        // default:
        //    break;
        //}
    }
    // for (size_t i = 0; (i + 1) < args.GetCount(); i += 2)
    //{
    //    const std::string& node_name = args.String(i);
    //    if (!inner_nodes_.ExistElement(node_name))
    //    {
    //        AFDataNode* pArgNode = pNodeManager->GetNode(node_name.c_str());
    //        if (pArgNode != nullptr)
    //        {
    //            args.ToAFIData(i + 1, pArgNode->value);
    //        }
    //    }
    //}

    // pEntity->SetString(AFEntityMetaBaseEntity::config_id(), config_index);
    // pEntity->SetString(AFEntityMetaBaseEntity::class_name(), class_name);
    // pEntity->SetNodeInt(AFEntityMetaBaseEntity::map_id(), map_id);
    // pEntity->SetNodeInt(AFEntityMetaBaseEntity::map_inst_id(), map_instance_id);

    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_LOAD_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_POST_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_DATA_FINISHED, args);

    return pEntity;
}

ARK_SHARE_PTR<AFIEntity> AFCKernelModule::CreateContainerEntity(
    const AFGUID& self, const std::string& container_name, const std::string& class_name, const ID_TYPE_ARG config_id)
{
    auto pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("There is no object, object = {}", self);
        return nullptr;
    }

    auto pContainer = pEntity->FindContainer(container_name);
    if (pContainer == nullptr)
    {
        ARK_LOG_ERROR("There is no container, container = {}", container_name);
        return nullptr;
    }

    auto map_id = pEntity->GetMapID();
    ARK_SHARE_PTR<AFMapInfo> pMapInfo = m_pMapModule->GetMapInfo(map_id);
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

    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    if (nullptr == pClassMeta)
    {
        ARK_LOG_ERROR("There is no class meta, name = {}", class_name);
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

    pMapInfo->AddEntityToInstance(
        map_instance_id, object_id, ((class_name == AFEntityMetaPlayer::self_name()) ? true : false));

    if (config_id > 0)
    {
        auto pStaticEntity = m_pConfigModule->FindStaticEntity(class_name, config_id);
        if (pStaticEntity != nullptr)
        {
            pContainerEntity->InitData(pStaticEntity);
        }
    }

    pContainer->Place(object_id);

    AFCDataList args;
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_LOAD_DATA, args);

    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_LOAD_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_POST_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_DATA_FINISHED, args);

    return pContainerEntity;
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

    ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("Cannot find this object, self={}", self);
        return false;
    }

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

bool AFCKernelModule::DestroySelf(const AFGUID& self)
{
    delete_list_.push_back(self);
    return true;
}

bool AFCKernelModule::AddEventCallBack(const AFGUID& self, const int nEventID, EVENT_PROCESS_FUNCTOR&& cb)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(self);
    ARK_ASSERT_RET_VAL(pEntity != nullptr, false);

    return pEntity->GetEventManager()->AddEventCallBack(nEventID, std::forward<EVENT_PROCESS_FUNCTOR>(cb));
}

bool AFCKernelModule::AddClassCallBack(const std::string& class_name, CLASS_EVENT_FUNCTOR&& cb)
{
    return m_pClassModule->AddClassCallBack(class_name, std::forward<CLASS_EVENT_FUNCTOR>(cb));
}

bool AFCKernelModule::AddDataCallBack(const std::string& class_name, const std::string& name, DATA_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto index = pClassMeta->GetIndex(name);
    if (index == 0)
    {
        return false;
    }

    AddDataCallBack(class_name, index, std::forward<DATA_EVENT_FUNCTOR>(cb));

    return true;
}

bool AFCKernelModule::AddTableCallBack(
    const std::string& class_name, const std::string& name, RECORD_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto index = pClassMeta->GetIndex(name);
    if (index == 0)
    {
        return false;
    }

    AddTableCallBack(class_name, index, std::forward<RECORD_EVENT_FUNCTOR>(cb));

    return true;
}

bool AFCKernelModule::AddDataCallBack(const std::string& class_name, const uint32_t index, DATA_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pDataMeta = pClassMeta->FindDataMeta(index);
    ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->AddDataCallBack(index, std::forward<DATA_EVENT_FUNCTOR>(cb));

    return true;
}

bool AFCKernelModule::AddTableCallBack(const std::string& class_name, const uint32_t index, RECORD_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pTableMeta = pClassMeta->FindTableMeta(index);
    ARK_ASSERT_RET_VAL(pTableMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->AddTableCallBack(index, std::forward<RECORD_EVENT_FUNCTOR>(cb));

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

    return pEntity->GetEventManager()->DoEvent(event_id, args);
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

bool AFCKernelModule::LogSelfInfo(const AFGUID& id)
{
    return false;
}

} // namespace ark
