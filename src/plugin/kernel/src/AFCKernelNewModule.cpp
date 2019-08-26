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
#include "kernel/include/AFCKernelNewModule.hpp"
#include "kernel/include/AFCObject.hpp"

namespace ark {

AFCKernelNewModule::AFCKernelNewModule()
{
    inner_nodes_.AddElement(AFEntityMetaBaseEntity::config_id(), ARK_NEW int32_t(0));
    inner_nodes_.AddElement(AFEntityMetaBaseEntity::class_name(), ARK_NEW int32_t(0));
    inner_nodes_.AddElement(AFEntityMetaBaseEntity::map_id(), ARK_NEW int32_t(0));
    inner_nodes_.AddElement(AFEntityMetaBaseEntity::map_inst_id(), ARK_NEW int32_t(0));
}

AFCKernelNewModule::~AFCKernelNewModule()
{
    objects_.clear();
}

bool AFCKernelNewModule::Init()
{
    delete_list_.clear();

    m_pMapModule = FindModule<AFIMapModule>();
    m_pClassModule = FindModule<AFIClassNewMetaModule>();
    m_pConfigModule = FindModule<AFIConfigNewModule>();
    m_pLogModule = FindModule<AFILogModule>();
    m_pGUIDModule = FindModule<AFIGUIDModule>();

    return true;
}

bool AFCKernelNewModule::Update()
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
        auto pObject = iter.second;
        if (pObject == nullptr)
        {
            continue;
        }

        pObject->Update();
    }

    return true;
}

bool AFCKernelNewModule::PreShut()
{
    return DestroyAll();
}

ARK_SHARE_PTR<AFIObject> AFCKernelNewModule::CreateObject(const AFGUID& self, const int map_id,
    const int map_instance_id, const std::string& class_name, const uint64_t config_id, const AFIDataList& args)
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
    if (FindObject(object_id) != nullptr)
    {
        ARK_LOG_ERROR("The entity has existed, id = {}", object_id);
        return nullptr;
    }

    ARK_SHARE_PTR<AFIObject> pObject =
        std::make_shared<AFCObject>(pClassMeta, object_id, config_id, map_id, map_instance_id);

    objects_.insert(object_id, pObject);

    pMapInfo->AddEntityToInstance(
        map_instance_id, object_id, ((class_name == AFEntityMetaPlayer::self_name()) ? true : false));

    if (config_id > 0)
    {
        auto pStaticObject = m_pConfigModule->FindStaticObject(class_name, config_id);
        if (pStaticObject != nullptr)
        {
            pObject->InitData(pStaticObject);
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

    // pObject->SetString(AFEntityMetaBaseEntity::config_id(), config_index);
    // pObject->SetString(AFEntityMetaBaseEntity::class_name(), class_name);
    // pObject->SetNodeInt(AFEntityMetaBaseEntity::map_id(), map_id);
    // pObject->SetNodeInt(AFEntityMetaBaseEntity::map_inst_id(), map_instance_id);

    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_LOAD_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_PRE_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_POST_EFFECT_DATA, args);
    DoEvent(object_id, class_name, ArkEntityEvent::ENTITY_EVT_DATA_FINISHED, args);

    return pObject;
}

ARK_SHARE_PTR<AFIObject> AFCKernelNewModule::CreateContainerObject(
    const AFGUID& self, const std::string& container_name, const std::string& class_name, const uint64_t config_id)
{
    auto pObject = FindObject(self);
    if (pObject == nullptr)
    {
        ARK_LOG_ERROR("There is no object, object = {}", self);
        return nullptr;
    }

    auto pContainer = pObject->FindContainer(container_name);
    if (pContainer == nullptr)
    {
        ARK_LOG_ERROR("There is no container, container = {}", container_name);
        return nullptr;
    }

    auto map_id = pObject->GetMapID();
    ARK_SHARE_PTR<AFMapInfo> pMapInfo = m_pMapModule->GetMapInfo(map_id);
    if (pMapInfo == nullptr)
    {
        ARK_LOG_ERROR("There is no scene, scene = {}", map_id);
        return nullptr;
    }

    auto map_instance_id = pObject->GetMapObjectID();
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
    if (FindObject(object_id) != nullptr)
    {
        ARK_LOG_ERROR("The entity has existed, id = {}", object_id);
        return nullptr;
    }

    ARK_SHARE_PTR<AFIObject> pContainerObject =
        std::make_shared<AFCObject>(pClassMeta, object_id, config_id, map_id, map_instance_id);

    objects_.insert(object_id, pContainerObject);

    pMapInfo->AddEntityToInstance(
        map_instance_id, object_id, ((class_name == AFEntityMetaPlayer::self_name()) ? true : false));

    if (config_id > 0)
    {
        auto pStaticObject = m_pConfigModule->FindStaticObject(class_name, config_id);
        if (pStaticObject != nullptr)
        {
            pContainerObject->InitData(pStaticObject);
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

    return pContainerObject;
}

ARK_SHARE_PTR<AFIObject> AFCKernelNewModule::FindObject(const AFGUID& self)
{
    return objects_.find_value(self);
}

bool AFCKernelNewModule::DestroyAll()
{
    for (auto iter : objects_)
    {
        delete_list_.push_back(iter.second->GetID());
    }

    // run another frame
    Update();

    return true;
}

bool AFCKernelNewModule::DestroyEntity(const AFGUID& self)
{
    if (self == cur_exec_object_ && self != NULL_GUID)
    {
        return DestroySelf(self);
    }

    ARK_SHARE_PTR<AFIObject> pObject = FindObject(self);
    if (pObject == nullptr)
    {
        ARK_LOG_ERROR("Cannot find this object, self={}", self);
        return false;
    }

    int32_t map_id = pObject->GetMapID();
    int32_t inst_id = pObject->GetMapObjectID();

    ARK_SHARE_PTR<AFMapInfo> pMapInfo = m_pMapModule->GetMapInfo(map_id);
    if (pMapInfo != nullptr)
    {
        const std::string& class_name = pObject->GetClassName();

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

bool AFCKernelNewModule::DestroySelf(const AFGUID& self)
{
    delete_list_.push_back(self);
    return true;
}

bool AFCKernelNewModule::AddEventCallBack(const AFGUID& self, const int nEventID, EVENT_PROCESS_FUNCTOR&& cb)
{
    ARK_SHARE_PTR<AFIObject> pObject = FindObject(self);
    ARK_ASSERT_RET_VAL(pObject != nullptr, false);

    return pObject->GetEventManager()->AddEventCallBack(nEventID, std::forward<EVENT_PROCESS_FUNCTOR>(cb));
}

bool AFCKernelNewModule::AddClassCallBack(const std::string& class_name, CLASS_EVENT_FUNCTOR&& cb)
{
    return m_pClassModule->AddClassCallBack(class_name, std::forward<CLASS_EVENT_FUNCTOR>(cb));
}

bool AFCKernelNewModule::AddDataCallBack(
    const std::string& class_name, const std::string& name, DATA_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pDataMeta = pClassMeta->FindDataMeta(name);
    ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->AddDataCallBack(name, std::forward<DATA_EVENT_FUNCTOR>(cb));

    return true;
}

bool AFCKernelNewModule::AddRecordCallBack(
    const std::string& class_name, const std::string& name, RECORD_EVENT_FUNCTOR&& cb)
{
    auto pClassMeta = m_pClassModule->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pRecordMeta = pClassMeta->FindRecordMeta(name);
    ARK_ASSERT_RET_VAL(pRecordMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->AddRecordCallBack(name, std::forward<RECORD_EVENT_FUNCTOR>(cb));

    return true;
}

bool AFCKernelNewModule::DoEvent(
    const AFGUID& self, const std::string& class_name, ArkEntityEvent class_event, const AFIDataList& args)
{
    return m_pClassModule->DoEvent(self, class_name, class_event, args);
}

bool AFCKernelNewModule::DoEvent(const AFGUID& self, const int event_id, const AFIDataList& args)
{
    ARK_SHARE_PTR<AFIObject> pObject = FindObject(self);
    ARK_ASSERT_RET_VAL(pObject != nullptr, false);

    return pObject->GetEventManager()->DoEvent(event_id, args);
}

bool AFCKernelNewModule::LogInfo(const AFGUID& id)
{
    ARK_SHARE_PTR<AFIObject> pObject = FindObject(id);
    if (pObject != nullptr)
    {
        ARK_LOG_ERROR("Cannot find entity, id = {}", id);
        return false;
    }

    if (m_pMapModule->IsInMapInstance(id))
    {
        int map_id = pObject->GetMapID();

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

bool AFCKernelNewModule::LogSelfInfo(const AFGUID& id)
{
    return false;
}

} // namespace ark
