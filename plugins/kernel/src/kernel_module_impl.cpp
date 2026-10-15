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

#include "ark/define.hpp"
#include "ark/kernel/container_impl.hpp"
#include "ark/kernel/data_list_impl.hpp"
#include "ark/kernel/entity_impl.hpp"
#include "ark/kernel/kernel_module_impl.hpp"
#include "ark/kernel/table_impl.hpp"

namespace ark {

kernel_module_impl::kernel_module_impl() {
    inner_nodes_.add_element(AFEntityMetaBaseEntity::config_id(), ARK_NEW int32_t(0));
    inner_nodes_.add_element(AFEntityMetaBaseEntity::class_name(), ARK_NEW int32_t(0));
    inner_nodes_.add_element(AFEntityMetaBaseEntity::map_id(), ARK_NEW int32_t(0));
    inner_nodes_.add_element(AFEntityMetaBaseEntity::map_inst_id(), ARK_NEW int32_t(0));
}

kernel_module_impl::~kernel_module_impl() {
    objects_.clear();
}

bool kernel_module_impl::init() {
    delete_list_.clear();

    m_pMapModule = find_module<map_module>();
    m_pClassModule = find_module<class_meta_module>();
    m_pConfigModule = find_module<config_module>();
    m_pGUIDModule = find_module<guid_module>();

    auto container_func = std::bind(&kernel_module_impl::on_container_call_back, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    add_common_container_call_back(std::move(container_func), 999999); // after other callbacks being done

    add_sync_call_back();

    return true;
}

bool kernel_module_impl::update() {
    cur_exec_object_ = NULL_GUID;

    if (!delete_list_.empty())
    {
        for (auto it : delete_list_)
        {
            destroy_entity(it);
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

        pEntity->update();
    }

    class_call_back_manager::on_delay_sync();

    return true;
}

bool kernel_module_impl::pre_shut() {
    return destroy_all();
}

bool kernel_module_impl::copy_data(std::shared_ptr<entity> pEntity, std::shared_ptr<static_entity> pStaticEntity) {
    if (pEntity == nullptr || nullptr == pStaticEntity)
    {
        return false;
    }

    // static node manager must be not empty
    auto pStaticNodeManager = get_node_manager(pStaticEntity);
    if (pStaticNodeManager == nullptr || pStaticNodeManager->is_empty())
    {
        return false;
    }

    // node manager must be empty
    auto pNodeManager = get_node_manager(pEntity);
    if (pNodeManager == nullptr || !pNodeManager->is_empty())
    {
        return false;
    }

    // copy data
    auto& data_list = pStaticNodeManager->get_data_list();
    for (auto& iter : data_list)
    {
        auto pData = iter.second;
        pNodeManager->create_data(pData);
    }

    return true;
}

std::shared_ptr<entity> kernel_module_impl::create_entity(const guid_t& self, const int map_id,
    const int map_instance_id, const std::string& class_name, const id_type config_id, const data_list& args) {
    guid_t object_id = self;

    auto pMapInfo = m_pMapModule->get_map_info(map_id);
    if (pMapInfo == nullptr)
    {
        ARK_LOG_ERROR("There is no scene, scene = {}", map_id);
        return nullptr;
    }

    if (!pMapInfo->exist_instance(map_instance_id))
    {
        ARK_LOG_ERROR("There is no group, scene = {} group = {}", map_id, map_instance_id);
        return nullptr;
    }

    auto pClassMeta = m_pClassModule->find_meta(class_name);
    if (nullptr == pClassMeta)
    {
        ARK_LOG_ERROR("There is no class meta, name = {}", class_name);
        return nullptr;
    }

    std::shared_ptr<static_entity> pStaticEntity = nullptr;
    if (config_id > 0)
    {
        auto pStaticEntity = get_static_entity(config_id);
        if (nullptr == pStaticEntity)
        {
            ARK_LOG_ERROR("There is no config, config_id = {}", config_id);
            return nullptr;
        }

        if (pStaticEntity->get_class_name() != class_name)
        {
            ARK_LOG_ERROR("Config class does not match entity class, config_id = {}", config_id);
            return nullptr;
        }
    }

    // check args num
    size_t arg_count = args.get_count();
    if (arg_count % 2 != 0)
    {
        ARK_LOG_ERROR("Args count is wrong, count = {}", arg_count);
        return nullptr;
    }

    if (object_id == NULL_GUID)
    {
        object_id = m_pGUIDModule->create_guid();
    }

    // Check if the entity exists
    if (get_entity(object_id) != nullptr)
    {
        ARK_LOG_ERROR("The entity has existed, id = {}", object_id);
        return nullptr;
    }

    std::shared_ptr<entity> pEntity =
        std::make_shared<entity_impl>(pClassMeta, object_id, config_id, map_id, map_instance_id, args);

    objects_.insert(object_id, pEntity);

    if (class_name == AFEntityMetaPlayer::self_name())
    {
        pMapInfo->add_entity_to_instance(map_instance_id, object_id, true);
    }
    //else if (class_name == AFEntityMetaPlayer::self_name()) // to do : npc type for now we do not have
    //{
    //}

    copy_data(pEntity, pStaticEntity);

    do_event(object_id, class_name, entity_event::ENTITY_EVT_PRE_LOAD_DATA, args);

    // original args here

    do_event(object_id, class_name, entity_event::ENTITY_EVT_LOAD_DATA, args);
    do_event(object_id, class_name, entity_event::ENTITY_EVT_PRE_EFFECT_DATA, args);
    do_event(object_id, class_name, entity_event::ENTITY_EVT_EFFECT_DATA, args);
    do_event(object_id, class_name, entity_event::ENTITY_EVT_POST_EFFECT_DATA, args);
    do_event(object_id, class_name, entity_event::ENTITY_EVT_DATA_FINISHED, args);

    return pEntity;
}

std::shared_ptr<entity> kernel_module_impl::create_container_entity(
    const guid_t& self, const uint32_t container_index, const std::string& class_name, const id_type config_id) {
    auto pEntity = get_entity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("There is no object, object = {}", self);
        return nullptr;
    }

    auto pContainer = pEntity->find_container(container_index);
    if (pContainer == nullptr)
    {
        ARK_LOG_ERROR("There is no container, container = {}", container_index);
        return nullptr;
    }

    auto pClassMeta = m_pClassModule->find_meta(class_name);
    if (nullptr == pClassMeta)
    {
        ARK_LOG_ERROR("There is no class meta, name = {}", class_name);
        return nullptr;
    }

    std::shared_ptr<static_entity> pStaticEntity = nullptr;
    if (config_id > 0)
    {
        auto pStaticEntity = get_static_entity(config_id);
        if (nullptr == pStaticEntity)
        {
            ARK_LOG_ERROR("There is no config, config_id = {}", config_id);
            return nullptr;
        }

        if (pStaticEntity->get_class_name() != class_name)
        {
            ARK_LOG_ERROR("Config class does not match entity class, config_id = {}", config_id);
            return nullptr;
        }
    }

    auto map_id = pEntity->get_map_id();
    auto pMapInfo = m_pMapModule->get_map_info(map_id);
    if (pMapInfo == nullptr)
    {
        ARK_LOG_ERROR("There is no scene, scene = {}", map_id);
        return nullptr;
    }

    auto map_instance_id = pEntity->get_map_entity_id();
    if (!pMapInfo->exist_instance(map_instance_id))
    {
        ARK_LOG_ERROR("There is no group, scene = {} group = {}", map_id, map_instance_id);
        return nullptr;
    }

    guid_t object_id = m_pGUIDModule->create_guid();

    // Check if the entity exists
    if (get_entity(object_id) != nullptr)
    {
        ARK_LOG_ERROR("The entity has existed, id = {}", object_id);
        return nullptr;
    }

    std::shared_ptr<entity> pContainerEntity =
        std::make_shared<entity_impl>(pClassMeta, object_id, config_id, map_id, map_instance_id, data_list_impl());

    objects_.insert(object_id, pContainerEntity);

    copy_data(pContainerEntity, pStaticEntity);

    pContainer->place(pContainerEntity);

    data_list_impl args;
    do_event(object_id, class_name, entity_event::ENTITY_EVT_PRE_LOAD_DATA, args);

    do_event(object_id, class_name, entity_event::ENTITY_EVT_LOAD_DATA, args);
    do_event(object_id, class_name, entity_event::ENTITY_EVT_PRE_EFFECT_DATA, args);
    do_event(object_id, class_name, entity_event::ENTITY_EVT_EFFECT_DATA, args);
    do_event(object_id, class_name, entity_event::ENTITY_EVT_POST_EFFECT_DATA, args);
    do_event(object_id, class_name, entity_event::ENTITY_EVT_DATA_FINISHED, args);

    return pContainerEntity;
}

std::shared_ptr<static_entity> kernel_module_impl::get_static_entity(const id_type config_id) {
    return m_pConfigModule->find_static_entity(config_id);
}

std::shared_ptr<entity> kernel_module_impl::get_entity(const guid_t& self) {
    return objects_.find_value(self);
}

bool kernel_module_impl::destroy_all() {
    for (auto& iter : objects_)
    {
        auto& pEntity = iter.second;
        if (pEntity->get_parent_container() != nullptr)
        {
            continue;
        }

        delete_list_.push_back(iter.second->get_id());
    }

    // run another frame
    update();

    return true;
}

bool kernel_module_impl::destroy_entity(const guid_t& self) {
    if (self == cur_exec_object_ && self != NULL_GUID)
    {
        return destroy_self(self);
    }

    auto pEntity = get_entity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("Cannot find this object, self={}", NULL_GUID);
        return false;
    }

    auto pParentContainer = pEntity->get_parent_container();
    if (pParentContainer)
    {
        // use container to destroy its entity
        return pParentContainer->destroy(self);
    }
    else
    {
        return inner_destroy_entity(pEntity);
    }
}

bool kernel_module_impl::destroy_self(const guid_t& self) {
    delete_list_.push_back(self);
    return true;
}

bool kernel_module_impl::inner_destroy_entity(std::shared_ptr<entity> pEntity) {
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("Cannot find this object, self={}", NULL_GUID);
        return false;
    }

    auto& self = pEntity->get_id();
    int32_t map_id = pEntity->get_map_id();
    int32_t inst_id = pEntity->get_map_entity_id();

    std::shared_ptr<map_info> pMapInfo = m_pMapModule->get_map_info(map_id);
    if (pMapInfo != nullptr)
    {
        const std::string& class_name = pEntity->get_class_name();

        pMapInfo->remove_entity_from_instance(
            inst_id, self, ((class_name == AFEntityMetaPlayer::self_name()) ? true : false));

        do_event(self, class_name, entity_event::ENTITY_EVT_PRE_DESTROY, data_list_impl());
        do_event(self, class_name, entity_event::ENTITY_EVT_DESTROY, data_list_impl());

        return objects_.erase(self);
    }
    else
    {
        ARK_LOG_ERROR("Cannot find this map, object_id={} map={} inst={}", self, map_id, inst_id);
        return false;
    }
}

bool kernel_module_impl::add_event_call_back(const guid_t& self, const int nEventID, event_process_functor&& cb) {
    std::shared_ptr<entity> pEntity = get_entity(self);
    ARK_ASSERT_RET_VAL(pEntity != nullptr, false);

    auto pEventManager = get_event_manager(pEntity);
    ARK_ASSERT_RET_VAL(pEventManager != nullptr, false);

    return pEventManager->add_event_call_back(nEventID, std::move(cb));
}

bool kernel_module_impl::add_class_call_back(
    const std::string& class_name, class_event_functor&& cb, const int32_t prio) {
    return m_pClassModule->add_class_call_back(class_name, std::move(cb), prio);
}

bool kernel_module_impl::add_node_call_back(
    const std::string& class_name, const std::string& name, data_node_event_functor&& cb, const int32_t prio) {
    auto pClassMeta = m_pClassModule->find_meta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto index = pClassMeta->get_index(name);
    if (index == 0)
    {
        return false;
    }

    add_node_call_back(class_name, index, std::move(cb), prio);

    return true;
}

bool kernel_module_impl::add_table_call_back(
    const std::string& class_name, const std::string& name, data_table_event_functor&& cb, const int32_t prio) {
    auto pClassMeta = m_pClassModule->find_meta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto index = pClassMeta->get_index(name);
    if (index == 0)
    {
        return false;
    }

    add_table_call_back(class_name, index, std::move(cb), prio);

    return true;
}

bool kernel_module_impl::add_node_call_back(
    const std::string& class_name, const uint32_t index, data_node_event_functor&& cb, const int32_t prio) {
    auto pClassMeta = m_pClassModule->find_meta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pDataMeta = pClassMeta->find_data_meta(index);
    ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

    auto pCallBack = pClassMeta->get_class_call_back_manager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->add_data_call_back(index, std::move(cb), prio);

    return true;
}

bool kernel_module_impl::add_table_call_back(
    const std::string& class_name, const uint32_t index, data_table_event_functor&& cb, const int32_t prio) {
    auto pClassMeta = m_pClassModule->find_meta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pTableMeta = pClassMeta->find_table_meta(index);
    ARK_ASSERT_RET_VAL(pTableMeta != nullptr, false);

    auto pCallBack = pClassMeta->get_class_call_back_manager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->add_table_call_back(index, std::move(cb), prio);

    return true;
}

bool kernel_module_impl::add_container_call_back(
    const std::string& class_name, const uint32_t index, container_event_functor&& cb, const int32_t prio) {
    auto pClassMeta = m_pClassModule->find_meta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pContainerMeta = pClassMeta->find_container_meta(index);
    ARK_ASSERT_RET_VAL(pContainerMeta != nullptr, false);

    auto pCallBack = pClassMeta->get_class_call_back_manager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    pCallBack->add_container_call_back(index, std::move(cb), prio);

    return true;
}

bool kernel_module_impl::add_common_container_call_back(container_event_functor&& cb, const int32_t prio) {
    auto pClassMeta = m_pClassModule->find_meta(AFEntityMetaPlayer::self_name());
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto& meta_list = pClassMeta->get_container_meta_list();
    for (auto& iter : meta_list)
    {
        auto pMeta = iter.second;
        if (!pMeta)
        {
            continue;
        }

        add_container_call_back(AFEntityMetaPlayer::self_name(), pMeta->get_index(), std::move(cb), prio);
    }

    return true;
}

bool kernel_module_impl::add_common_class_event(class_event_functor&& cb, const int32_t prio) {
    auto& class_meta_list = m_pClassModule->get_meta_list();
    for (auto& iter : class_meta_list)
    {
        auto pClassMeta = iter.second;
        if (nullptr == pClassMeta)
        {
            continue;
        }

        add_class_call_back(iter.first, std::move(cb), prio);
    }

    return true;
}

bool kernel_module_impl::add_leave_scene_event(
    const std::string& class_name, scene_event_functor&& cb, const int32_t prio) {
    auto pClassMeta = m_pClassModule->find_meta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pCallBack = pClassMeta->get_class_call_back_manager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    return pCallBack->add_leave_scene_event(std::move(cb), prio);
}

bool kernel_module_impl::add_enter_scene_event(
    const std::string& class_name, scene_event_functor&& cb, const int32_t prio) {
    auto pClassMeta = m_pClassModule->find_meta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pCallBack = pClassMeta->get_class_call_back_manager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    return pCallBack->add_enter_scene_event(std::move(cb), prio);
}

bool kernel_module_impl::add_move_event(const std::string& class_name, move_event_functor&& cb, const int32_t prio) {
    auto pClassMeta = m_pClassModule->find_meta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pCallBack = pClassMeta->get_class_call_back_manager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    return pCallBack->add_move_event(std::move(cb), prio);
}

void kernel_module_impl::add_sync_call_back() {
    // node sync call back
    auto node_func = std::bind(&kernel_module_impl::on_sync_node, this, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4);

    class_call_back_manager::add_node_sync_call_back(data_mask::PF_SYNC_VIEW, std::move(node_func));
    class_call_back_manager::add_node_sync_call_back(data_mask::PF_SYNC_SELF, std::move(node_func));

    // table sync call back
    auto table_func = std::bind(&kernel_module_impl::on_sync_table, this, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4);

    class_call_back_manager::add_table_sync_call_back(data_mask::PF_SYNC_VIEW, std::move(table_func));
    class_call_back_manager::add_table_sync_call_back(data_mask::PF_SYNC_SELF, std::move(table_func));

    // container sync call back
    auto container_func =
        std::bind(&kernel_module_impl::on_sync_container, this, std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

    class_call_back_manager::add_container_sync_call_back(data_mask::PF_SYNC_VIEW, std::move(container_func));
    class_call_back_manager::add_container_sync_call_back(data_mask::PF_SYNC_SELF, std::move(container_func));

    // data delay call back
    auto delay_func = std::bind(&kernel_module_impl::on_delay_sync_data, this, std::placeholders::_1,
        std::placeholders::_2, std::placeholders::_3);

    class_call_back_manager::add_delay_sync_call_back(data_mask::PF_SYNC_VIEW, std::move(delay_func));
    class_call_back_manager::add_delay_sync_call_back(data_mask::PF_SYNC_SELF, std::move(delay_func));

    // send msg functor
    auto view_func = std::bind(&kernel_module_impl::send_to_view, this, std::placeholders::_1, std::placeholders::_2);
    sync_functors.insert(std::make_pair(data_mask::PF_SYNC_VIEW, std::forward<sync_functor>(view_func)));

    auto self_func = std::bind(&kernel_module_impl::send_to_self, this, std::placeholders::_1, std::placeholders::_2);
    sync_functors.insert(std::make_pair(data_mask::PF_SYNC_SELF, std::forward<sync_functor>(self_func)));
}

int kernel_module_impl::on_sync_node(
    const guid_t& self, const uint32_t index, const data_mask mask_value, const data& data) {
    // find parent entity
    auto pEntity = get_entity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync node failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_entity pb_data;
    auto entity_id = self;
    auto pb_entity = pb_data.mutable_data();

    auto pParentContainer = pEntity->get_parent_container();
    if (pParentContainer != nullptr)
    {
        if (!try_add_container_pb_entity(pParentContainer, self, *pb_data.mutable_data(), entity_id, pb_entity))
        {
            ARK_LOG_ERROR("sync node failed container entity do no exist, self={}", self);
            return -1;
        }
    }

    if (!node_to_pb_data(index, data, pb_entity))
    {
        ARK_LOG_ERROR("node to pb failed, self={}, index={}", self, index);
        return -1;
    }

    pb_data.set_id(entity_id);
    if (send_sync_msg(entity_id, mask_value, pb_data) < 0)
    {
        ARK_LOG_ERROR("send sync msg failed, self={}, index={}", entity_id, index);
        return -1;
    }

    return 0;
}

int kernel_module_impl::on_sync_table(
    const guid_t& self, const table_event_data& event, const data_mask mask_value, const data& data) {
    table_op_type op_type = static_cast<table_op_type>(event.op_type_);
    switch (op_type)
    {
    case table_op_type::TABLE_ADD:
    {
        on_sync_table_add(self, event, mask_value);
    }
    break;
    case table_op_type::TABLE_DELETE:
    {
        on_sync_table_delete(self, event, mask_value);
    }
    break;
    case table_op_type::TABLE_SWAP:
        // do not have yet
        break;
    case table_op_type::TABLE_UPDATE:
    {
        on_sync_table_update(self, event, mask_value, data);
    }
    break;
    case table_op_type::TABLE_COVERAGE:
        // will do something
        break;
    default:
        break;
    }

    return 0;
}

int kernel_module_impl::on_sync_container(const guid_t& self, const uint32_t index, const data_mask mask,
    const container_op_type op_type, uint32_t src_index, uint32_t dest_index) {
    switch (op_type)
    {
    case container_op_type::OP_PLACE:
    {
        on_sync_container_place(self, index, mask, src_index);
    }
    break;
    case container_op_type::OP_REMOVE:
    {
        on_sync_container_remove(self, index, mask, src_index);
    }
    break;
    case container_op_type::OP_DESTROY:
    {
        on_sync_container_destroy(self, index, mask, src_index);
    }
    break;
    case container_op_type::OP_SWAP:
    {
        on_sync_container_swap(self, index, mask, src_index, dest_index);
    }
    break;
    default:
        break;
    }
    return 0;
}

int kernel_module_impl::on_delay_sync_data(
    const guid_t& self, const data_mask mask_value, const delay_sync_data& data) {
    if (data.node_list_.size() == 0 && data.table_list_.size() == 0)
    {
        return 0;
    }

    // find parent entity
    auto pEntity = get_entity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync delay data failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_delay_entity pb_data;
    auto entity_id = self;
    auto pb_entity = pb_data.mutable_data();

    auto pParentContainer = pEntity->get_parent_container();
    if (pParentContainer != nullptr)
    {
        if (!try_add_container_pb_entity(pParentContainer, self, *pb_data.mutable_data(), entity_id, pb_entity))
        {
            ARK_LOG_ERROR("sync delay data failed container entity do no exist, self={}", self);
            return -1;
        }
    }

    // node to pb
    for (auto& iter : data.node_list_)
    {
        auto pNode = iter;
        if (!node_to_pb_data(pNode, pb_entity))
        {
            continue;
        }
    }

    // table to pb
    for (auto& iter : data.table_list_)
    {
        auto table_index = iter.first;
        auto& table = iter.second;
        delay_table_to_pb(table, table_index, pb_data, *pb_entity);
    }

    // container to pb
    for (auto& iter : data.container_list_)
    {
        auto container_index = iter.first;
        auto& container = iter.second;
        delay_container_to_pb(pEntity, container, container_index, pb_data);
    }

    pb_data.set_id(entity_id);

    if (send_sync_msg(entity_id, mask_value, pb_data) < 0)
    {
        ARK_LOG_ERROR("send sync msg failed, self={}", self);
        return -1;
    }

    return 0;
}

int kernel_module_impl::on_sync_table_add(
    const guid_t& self, const table_event_data& event, const data_mask mask_value) {
    // find parent entity
    auto pEntity = get_entity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync table failed entity do no exist, self={}", self);
        return -1;
    }

    auto pTable = pEntity->find_table(event.table_index_);
    if (nullptr == pTable)
    {
        ARK_LOG_ERROR("sync table failed table do no exist, self={}, table={}", self, event.table_name_);
        return -1;
    }

    auto pRow = pTable->find_row(event.row_);
    if (nullptr == pRow)
    {
        ARK_LOG_ERROR(
            "sync table failed table row do no exist, self={}, table={}, row={}", self, event.table_name_, event.row_);
        return -1;
    }

    AFMsg::pb_entity_table_add pb_data;
    auto entity_id = self;
    auto pb_entity = pb_data.mutable_data();

    auto pParentContainer = pEntity->get_parent_container();
    if (pParentContainer != nullptr)
    {
        if (!try_add_container_pb_entity(pParentContainer, self, *pb_data.mutable_data(), entity_id, pb_entity))
        {
            ARK_LOG_ERROR("sync node failed container entity do no exist, self={}", self);
            return -1;
        }
    }

    AFMsg::pb_entity_data pb_row;
    if (!row_to_pb_data(pRow, event.row_, &pb_row))
    {
        ARK_LOG_ERROR(
            "sync table failed table row do no exist, self={}, table={}, row={}", self, event.table_name_, event.row_);
        return -1;
    }

    AFMsg::pb_table pb_table;
    pb_table.mutable_datas_value()->insert({event.row_, pb_row});
    pb_entity->mutable_datas_table()->insert({event.table_index_, pb_table});

    pb_data.set_id(entity_id);
    send_sync_msg(entity_id, mask_value, pb_data);

    return 0;
}

int kernel_module_impl::on_sync_table_delete(
    const guid_t& self, const table_event_data& event, const data_mask mask_value) {
    // find parent entity
    auto pEntity = get_entity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync table failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_entity_table_delete pb_data;
    auto entity_id = self;
    auto pb_entity = pb_data.mutable_data();

    auto pParentContainer = pEntity->get_parent_container();
    if (pParentContainer != nullptr)
    {
        if (!try_add_container_pb_entity(pParentContainer, self, *pb_data.mutable_data(), entity_id, pb_entity))
        {
            ARK_LOG_ERROR("sync node failed container entity do no exist, self={}", self);
            return -1;
        }
    }

    AFMsg::pb_entity_data pb_row;
    AFMsg::pb_table pb_table;
    pb_table.mutable_datas_value()->insert({event.row_, pb_row});
    pb_entity->mutable_datas_table()->insert({event.table_index_, pb_table});

    send_sync_msg(entity_id, mask_value, pb_data);
    return 0;
}

int kernel_module_impl::on_sync_table_update(
    const guid_t& self, const table_event_data& event, const data_mask mask_value, const data& data) {
    // find parent entity
    auto pEntity = get_entity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync table failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_entity_table_update pb_data;
    auto entity_id = self;
    auto pb_entity = pb_data.mutable_data();

    auto pParentContainer = pEntity->get_parent_container();
    if (pParentContainer != nullptr)
    {
        if (!try_add_container_pb_entity(pParentContainer, self, *pb_data.mutable_data(), entity_id, pb_entity))
        {
            ARK_LOG_ERROR("sync node failed container entity do no exist, self={}", self);
            return -1;
        }
    }

    AFMsg::pb_entity_data pb_row;
    if (!node_to_pb_data(event.data_index_, data, &pb_row))
    {
        ARK_LOG_ERROR(
            "sync table failed table node do no exist, self={}, table={}, row={}", self, event.table_name_, event.row_);
        return -1;
    }

    AFMsg::pb_table pb_table;
    pb_table.mutable_datas_value()->insert({event.row_, pb_row});
    pb_entity->mutable_datas_table()->insert({event.table_index_, pb_table});

    pb_data.set_id(entity_id);
    send_sync_msg(entity_id, mask_value, pb_data);
    return 0;
}

int kernel_module_impl::on_sync_container_place(
    const guid_t& self, const uint32_t index, const data_mask mask, uint32_t src_index) {
    // find parent entity
    auto pEntity = get_entity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync container failed entity do no exist, self={}", self);
        return -1;
    }

    auto pContainer = pEntity->find_container(index);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync container failed container do no exist, self={}, container={}", self, index);
        return -1;
    }

    auto pContainerEntity = pContainer->find(src_index);
    if (pContainerEntity == nullptr)
    {
        ARK_LOG_ERROR("sync container failed container entity do no exist, self={}, container={}, entity={}", self,
            index, src_index);
        return -1;
    }

    if (pContainerEntity->is_sent())
    {
        AFMsg::pb_container_place pb_data;
        pb_data.set_id(self);
        pb_data.set_index(index);
        pb_data.set_entity_index(src_index);
        pb_data.set_entity_id(pContainerEntity->get_id());

        send_sync_msg(self, mask, pb_data);
    }
    else
    {
        AFMsg::pb_container_create pb_data;
        if (!entity_to_pb_data(pContainerEntity, pb_data.mutable_data()))
        {
            ARK_LOG_ERROR("sync container failed container entity to pb failed, self={}, container={}, entity={}", self,
                index, src_index);
            return -1;
        }

        pb_data.set_id(self);
        pb_data.set_index(index);
        pb_data.set_entity_index(src_index);

        send_sync_msg(self, mask, pb_data);
    }

    return 0;
}

int kernel_module_impl::on_sync_container_remove(
    const guid_t& self, const uint32_t index, const data_mask mask, uint32_t src_index) {
    // find parent entity
    auto pEntity = get_entity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync container failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_container_remove pb_data;
    pb_data.set_id(self);
    pb_data.set_index(index);
    pb_data.set_entity_index(src_index);

    send_sync_msg(self, mask, pb_data);

    return 0;
}

int kernel_module_impl::on_sync_container_destroy(
    const guid_t& self, const uint32_t index, const data_mask mask, uint32_t src_index) {
    // find parent entity
    auto pEntity = get_entity(self);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("sync container failed entity do no exist, self={}", self);
        return -1;
    }

    AFMsg::pb_container_destroy pb_data;
    pb_data.set_id(self);
    pb_data.set_index(index);
    pb_data.set_entity_index(src_index);

    send_sync_msg(self, mask, pb_data);

    return 0;
}

int kernel_module_impl::on_sync_container_swap(
    const guid_t& self, const uint32_t index, const data_mask mask, uint32_t src_index, uint32_t dest_index) {
    // find parent entity
    auto pEntity = get_entity(self);
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

    send_sync_msg(self, mask, pb_data);

    return 0;
}

bool kernel_module_impl::delay_table_to_pb(const delay_sync_table& table, const uint32_t index,
    AFMsg::pb_delay_entity& data, AFMsg::pb_entity_data& pb_entity) {
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
            if (node_to_pb_data(pNode, &pb_row))
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

bool kernel_module_impl::delay_container_to_pb(std::shared_ptr<entity> pEntity, const delay_sync_container& container,
    const uint32_t index, AFMsg::pb_delay_entity& data) {
    AFMsg::delay_container pb_delay_container;
    AFMsg::pb_container pb_conatiner;
    for (auto& iter : container.index_list_)
    {
        auto entity_index = iter;
        auto pContaier = pEntity->find_container(index);
        if (pContaier == nullptr)
        {
            continue;
        }

        auto pContainerEntity = pContaier->find(entity_index);
        if (pContainerEntity == nullptr)
        {
            pb_delay_container.mutable_entity_list()->insert({entity_index, NULL_GUID});
        }
        else if (pEntity->is_sent())
        {
            pb_delay_container.mutable_entity_list()->insert({entity_index, pContainerEntity->get_id()});
        }
        else
        {
            AFMsg::pb_entity pb_container_entity;
            if (!entity_to_pb_data(pContainerEntity, &pb_container_entity))
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

bool kernel_module_impl::try_add_container_pb_entity(std::shared_ptr<container> pContainer, const guid_t& self,
    AFMsg::pb_entity_data& pb_entity_data, guid_t& parent_id, AFMsg::pb_entity_data*& pb_container_entity) {
    parent_id = pContainer->get_parent_id();
    auto pParentEntity = get_entity(parent_id);
    if (pParentEntity == nullptr)
    {
        ARK_LOG_ERROR("parent entity do no exist, parent={}", parent_id);
        return false;
    }

    auto self_index = pContainer->find(self);
    if (self_index == 0)
    {
        ARK_LOG_ERROR("entity is not in container, self={}", self);
        return false;
    }

    AFMsg::pb_container pb_container;
    auto result_container = pb_entity_data.mutable_datas_container()->insert({pContainer->get_index(), pb_container});
    if (!result_container.second)
    {
        ARK_LOG_ERROR("entity insert container failed, self={} container index = {}", self, pContainer->get_index());
        return false;
    }
    auto& container = result_container.first->second;

    AFMsg::pb_entity pb_entity;
    auto result_entity = container.mutable_datas_value()->insert({self_index, pb_entity});
    if (!result_entity.second)
    {
        ARK_LOG_ERROR("container insert entity failed, self={} container index = {}", self, pContainer->get_index());
        return false;
    }

    auto& entity = result_entity.first->second;
    pb_container_entity = entity.mutable_data();
    return true;
}

int kernel_module_impl::send_sync_msg(
    const guid_t& self, const data_mask mask_value, const google::protobuf::Message& msg) {
    auto iter = sync_functors.find(mask_value);
    if (iter == sync_functors.end())
    {
        return -1;
    }

    iter->second(self, msg);

    return 0;
}

int kernel_module_impl::send_to_view(const guid_t& self, const google::protobuf::Message& msg) {
    auto pEntity = get_entity(self);
    if (nullptr == pEntity)
    {
        return -1;
    }

    auto map_id = pEntity->get_map_id();
    auto inst_id = pEntity->get_map_entity_id();

    data_list_impl map_inst_entity_list;
    m_pMapModule->get_inst_entity_list(map_id, inst_id, map_inst_entity_list);

    for (size_t i = 0; i < map_inst_entity_list.get_count(); i++)
    {
        auto pViewEntity = get_entity(map_inst_entity_list.int64(i));
        if (pViewEntity == nullptr)
        {
            continue;
        }

        const std::string& strObjClassName = pViewEntity->get_class_name();
        if (AFEntityMetaPlayer::self_name() == strObjClassName)
        {
            send_to_self(pViewEntity->get_id(), msg);
        }
    }

    return 0;
}

int kernel_module_impl::send_to_self(const guid_t& self, const google::protobuf::Message& msg) {
    //SendMsgPBToGate(AFMsg::EGMI_ACK_NODE_DATA, entity, ident);
    return 0;
}

bool kernel_module_impl::do_event(
    const guid_t& self, const std::string& class_name, entity_event class_event, const data_list& args) {
    return m_pClassModule->do_class_event(self, class_name, class_event, args);
}

bool kernel_module_impl::do_event(const guid_t& self, const int event_id, const data_list& args) {
    std::shared_ptr<entity> pEntity = get_entity(self);
    ARK_ASSERT_RET_VAL(pEntity != nullptr, false);

    auto pEventManager = get_event_manager(pEntity);
    ARK_ASSERT_RET_VAL(pEventManager != nullptr, false);

    return pEventManager->do_event(event_id, args);
}

bool kernel_module_impl::exist(const guid_t& self) {
    return (objects_.find_value(self) != nullptr);
}

bool kernel_module_impl::log_self_info(const guid_t& id) {
    return false;
}

int kernel_module_impl::log_object_data(const guid_t& guid) {
    auto entity = get_entity(guid);
    if (entity == nullptr)
    {
        return -1;
    }

    auto pNodeManager = get_node_manager(entity);
    ARK_ASSERT_RET_VAL(pNodeManager != nullptr, -1);

    auto pTableManager = get_table_manager(entity);
    ARK_ASSERT_RET_VAL(pTableManager != nullptr, -1);

    auto& node_list = pNodeManager->get_data_list();
    for (auto& iter : node_list)
    {
        auto pData = iter.second;
        if (!pData)
        {
            continue;
        }

        ARK_LOG_TRACE("Player[{}] Node[{}] Value[{}]", guid, pData->get_name(), pData->to_string());
    }

    auto& table_list = pTableManager->get_table_list();
    for (auto& iter : table_list)
    {
        auto pTable = iter.second;
        if (!pTable)
        {
            continue;
        }

        for (auto pRow = pTable->first(); pRow != nullptr; pRow = pTable->next())
        {
            auto pRowNodeManager = get_node_manager(pRow);
            if (!pRowNodeManager)
            {
                continue;
            }

            auto& row_data_list = pRowNodeManager->get_data_list();
            for (auto& iter_row : row_data_list)
            {
                auto pNode = iter_row.second;
                if (!pNode)
                {
                    continue;
                }

                ARK_LOG_TRACE("Player[{}] Table[{}] Row[{}] Col[{}] Value[{}]", guid, pTable->get_name(),
                    pRow->get_row(), pNode->get_name(), pNode->to_string());
            }
        }
    }

    return 0;
}

bool kernel_module_impl::log_info(const guid_t& id) {
    std::shared_ptr<entity> pEntity = get_entity(id);
    if (pEntity == nullptr)
    {
        ARK_LOG_ERROR("Cannot find entity, id = {}", id);
        return false;
    }

    if (m_pMapModule->is_in_map_instance(id))
    {
        int map_id = pEntity->get_map_id();

        ARK_LOG_INFO("----------child object list-------- , id = {} mapid = {}", id, map_id);
        data_list_impl entity_list;
        int online_count = m_pMapModule->get_map_online_list(map_id, entity_list);
        for (int i = 0; i < online_count; ++i)
        {
            guid_t target_entity_id = entity_list.int64(i);
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
bool kernel_module_impl::entity_to_db_data(const guid_t& self, AFMsg::pb_db_entity& pb_data) {
    std::shared_ptr<entity> pEntity = get_entity(self);
    return entity_to_db_data(pEntity, pb_data);
}

bool kernel_module_impl::entity_to_db_data(std::shared_ptr<entity> pEntity, AFMsg::pb_db_entity& pb_data) {
    ARK_ASSERT_RET_VAL(pEntity != nullptr, false);

    auto pNodeManager = get_node_manager(pEntity);
    ARK_ASSERT_RET_VAL(pNodeManager != nullptr, false);

    auto pTableManager = get_table_manager(pEntity);
    ARK_ASSERT_RET_VAL(pTableManager != nullptr, false);

    auto pContainerManager = get_container_manager(pEntity);
    ARK_ASSERT_RET_VAL(pContainerManager != nullptr, false);

    pb_data.set_id(pEntity->get_id());
    pb_data.set_config_id(pEntity->get_config_id());
    pb_data.set_map_id(pEntity->get_map_id());
    pb_data.set_map_inst_id(pEntity->get_map_entity_id());
    pb_data.set_class_name(pEntity->get_class_name());

    // node to db
    auto& node_list = pNodeManager->get_data_list();
    for (auto& iter : node_list)
    {
        auto pNode = iter.second;
        if (!pNode)
        {
            continue;
        }

        if (!pNode->have_mask(data_mask::PF_SAVE))
        {
            continue;
        }

        node_to_db_data(pNode, *pb_data.mutable_data());
    }

    // table to db
    auto& table_list = pTableManager->get_table_list();
    for (auto& iter : table_list)
    {
        auto pTable = iter.second;
        if (!pTable)
        {
            continue;
        }

        if (!pTable->have_mask(data_mask::PF_SAVE))
        {
            continue;
        }

        AFMsg::pb_db_table pb_table;
        if (!table_to_db_data(pTable, pb_table))
        {
            continue;
        }

        pb_data.mutable_data()->mutable_datas_table()->insert({pTable->get_name(), pb_table});
    }

    // container to db
    auto& container_list = pContainerManager->get_container_list();
    for (auto& iter : container_list)
    {
        auto pContainer = iter.second;
        if (!pContainer)
        {
            continue;
        }

        AFMsg::pb_db_container pb_container;
        for (auto index = pContainer->first(); index > 0; index = pContainer->next())
        {
            auto pSubEntity = pContainer->find(index);
            if (!pSubEntity)
            {
                continue;
            }

            AFMsg::pb_db_entity pb_container_entity;
            if (!entity_to_db_data(pSubEntity, pb_container_entity))
            {
                continue;
            }

            pb_container.mutable_datas_value()->insert({index, pb_container_entity});
        }

        if (pb_container.datas_value_size() > 0)
        {
            pb_data.mutable_data()->mutable_datas_entity()->insert({pContainer->get_name(), pb_container});
        }
    }

    return true;
}

std::shared_ptr<entity> kernel_module_impl::create_entity(const AFMsg::pb_db_entity& pb_data) {
    auto entity_id = pb_data.id();
    auto pEntity = get_entity(entity_id);
    if (pEntity != nullptr)
    {
        ARK_LOG_ERROR("entity already exists, object = {}", entity_id);
        return nullptr;
    }

    const std::string& class_name = pb_data.class_name();
    auto pClassMeta = m_pClassModule->find_meta(class_name);
    if (nullptr == pClassMeta)
    {
        ARK_LOG_ERROR("There is no class meta, name = {}", class_name);
        return nullptr;
    }

    auto map_id = pb_data.map_id();
    auto map_inst_id = pb_data.map_inst_id();
    auto pMapInfo = m_pMapModule->get_map_info(map_id);
    if (pMapInfo == nullptr)
    {
        ARK_LOG_ERROR("There is no scene, scene = {}", map_id);
        return nullptr;
    }

    auto pCEntity =
        std::make_shared<entity_impl>(pClassMeta, entity_id, NULL_INT, map_id, map_inst_id, data_list_impl());
    pEntity = std::static_pointer_cast<entity>(pCEntity);

    objects_.insert(entity_id, pEntity);

    if (class_name == AFEntityMetaPlayer::self_name())
    {
        pMapInfo->add_entity_to_instance(map_inst_id, entity_id, true);
    }
    //else if (class_name == AFEntityMetaPlayer::self_name()) // to do : npc type for now we do not have
    //{
    //}

    // init data
    auto& pb_db_entity_data = pb_data.data();

    // node data
    auto pNodeManager = pCEntity->get_node_manager();
    if (nullptr != pNodeManager)
    {
        db_data_to_node(pNodeManager, pb_db_entity_data);
    }

    // table data
    auto pTableManager = pCEntity->get_table_manager();
    if (nullptr != pTableManager)
    {
        for (auto& iter : pb_db_entity_data.datas_table())
        {
            db_data_to_table(pEntity, iter.first, iter.second);
        }
    }

    // container data
    for (auto& iter : pb_db_entity_data.datas_entity())
    {
        db_data_to_container(pEntity, iter.first, iter.second);
    }

    // todo : add new event?
    data_list_impl args;
    do_event(entity_id, class_name, entity_event::ENTITY_EVT_PRE_LOAD_DATA, args);

    do_event(entity_id, class_name, entity_event::ENTITY_EVT_LOAD_DATA, args);
    do_event(entity_id, class_name, entity_event::ENTITY_EVT_PRE_EFFECT_DATA, args);
    do_event(entity_id, class_name, entity_event::ENTITY_EVT_EFFECT_DATA, args);
    do_event(entity_id, class_name, entity_event::ENTITY_EVT_POST_EFFECT_DATA, args);
    do_event(entity_id, class_name, entity_event::ENTITY_EVT_DATA_FINISHED, args);

    return pEntity;
}

bool kernel_module_impl::send_custom_message(const guid_t& target, const uint32_t msg_id, const data_list& args) {
    ARK_ASSERT_RET_VAL(exist(target) && msg_id > 0, false);

    AFMsg::pb_custom_message custom_message;
    custom_message.set_message_id(msg_id);

    size_t count = args.get_count();
    for (size_t i = 0; i < count; i++)
    {
        auto data_type = args.get_type(i);
        switch (data_type)
        {
        case ark::data_type::DT_BOOLEAN:
            custom_message.add_data_list()->set_bool_value(args.get_bool(i));
            break;
        case ark::data_type::DT_INT32:
            custom_message.add_data_list()->set_int_value(args.get_int(i));
            break;
        case ark::data_type::DT_UINT32:
            custom_message.add_data_list()->set_uint_value(args.u_int(i));
            break;
        case ark::data_type::DT_INT64:
            custom_message.add_data_list()->set_int64_value(args.int64(i));
            break;
        case ark::data_type::DT_UINT64:
            custom_message.add_data_list()->set_uint64_value(args.u_int64(i));
            break;
        case ark::data_type::DT_FLOAT:
            custom_message.add_data_list()->set_float_value(args.get_float(i));
            break;
        case ark::data_type::DT_DOUBLE:
            custom_message.add_data_list()->set_double_value(args.get_double(i));
            break;
        case ark::data_type::DT_STRING:
            custom_message.add_data_list()->set_str_value(args.string(i));
            break;
        default:
            break;
        }
    }

    // send message

    return true;
}

// pb table to entity table
bool kernel_module_impl::db_data_to_table(
    std::shared_ptr<entity> pEntity, const std::string& name, const AFMsg::pb_db_table& pb_table) {
    auto pTable = pEntity->find_table(name);
    ARK_ASSERT_RET_VAL(pTable != nullptr, false);

    auto pCTable = dynamic_cast<table_impl*>(pTable);
    ARK_ASSERT_RET_VAL(pCTable != nullptr, false);

    for (auto& iter : pb_table.datas_value())
    {
        auto row_index = iter.first;
        if (row_index == NULL_INT)
        {
            continue;
        }

        auto& pb_db_entity_data = iter.second;
        auto pRow = pCTable->create_row(row_index, data_list_impl());
        if (pRow == nullptr)
        {
            continue;
        }

        auto pNodeManager = get_node_manager(pRow);
        if (pNodeManager == nullptr)
        {
            continue;
        }

        db_data_to_node(pNodeManager, pb_db_entity_data);
    }

    return true;
}

bool kernel_module_impl::db_data_to_container(
    std::shared_ptr<entity> pEntity, const std::string& name, const AFMsg::pb_db_container& pb_data) {
    auto pContainer = pEntity->find_container(name);
    if (nullptr == pContainer)
    {
        return false;
    }

    auto pCContainer = std::dynamic_pointer_cast<container_impl>(pContainer);
    if (nullptr == pCContainer)
    {
        return false;
    }

    for (auto& iter : pb_data.datas_value())
    {
        auto pContainerEntity = create_entity(iter.second);
        if (nullptr == pContainerEntity)
        {
            continue;
        }

        pCContainer->init_entity_list(iter.first, pContainerEntity);
    }

    return true;
}

int kernel_module_impl::on_container_call_back(const guid_t& self, const uint32_t index,
    const container_op_type op_type, const uint32_t src_index, const uint32_t dest_index) {
    if (op_type == container_op_type::OP_DESTROY)
    {
        // destroy entity
        auto pEntity = get_entity(self);
        ARK_ASSERT_RET_VAL(pEntity != nullptr, 0);

        auto pContainer = pEntity->find_container(index);
        ARK_ASSERT_RET_VAL(pContainer != nullptr, 0);

        auto pContainerEntity = pContainer->find(src_index);
        ARK_ASSERT_RET_VAL(pContainerEntity != nullptr, 0);

        inner_destroy_entity(pContainerEntity);
    }

    return 0;
}

bool kernel_module_impl::db_data_to_node(
    std::shared_ptr<node_manager> pNodeManager, const AFMsg::pb_db_entity_data& pb_db_entity_data) {
    //bool data
    for (auto& iter : pb_db_entity_data.datas_bool())
    {
        auto pNode = pNodeManager->create_data(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->set_bool(iter.second);
    }

    //int32 data
    for (auto& iter : pb_db_entity_data.datas_int32())
    {
        auto pNode = pNodeManager->create_data(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->set_int32(iter.second);
    }

    //uint32 data
    for (auto& iter : pb_db_entity_data.datas_uint32())
    {
        auto pNode = pNodeManager->create_data(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->set_u_int32(iter.second);
    }

    //int64 data
    for (auto& iter : pb_db_entity_data.datas_int64())
    {
        auto pNode = pNodeManager->create_data(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->set_int64(iter.second);
    }

    //uint64 data
    for (auto& iter : pb_db_entity_data.datas_uint64())
    {
        auto pNode = pNodeManager->create_data(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->set_u_int64(iter.second);
    }

    //float data
    for (auto& iter : pb_db_entity_data.datas_float())
    {
        auto pNode = pNodeManager->create_data(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->set_float(iter.second);
    }

    //double data
    for (auto& iter : pb_db_entity_data.datas_double())
    {
        auto pNode = pNodeManager->create_data(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->set_double(iter.second);
    }

    //string data
    for (auto& iter : pb_db_entity_data.datas_string())
    {
        auto pNode = pNodeManager->create_data(iter.first);
        if (pNode == nullptr)
        {
            continue;
        }

        pNode->set_string(iter.second);
    }

    return true;
}

//-----------------------------
bool kernel_module_impl::node_to_db_data(node* pNode, AFMsg::pb_db_entity_data& pb_data) {
    ARK_ASSERT_RET_VAL(pNode != nullptr, false);

    auto& name = pNode->get_name();
    switch (pNode->get_type())
    {
    case data_type::DT_BOOLEAN:
        pb_data.mutable_datas_bool()->insert({name, pNode->get_bool()});
        break;
    case data_type::DT_INT32:
        pb_data.mutable_datas_int32()->insert({name, pNode->get_int32()});
        break;
    case data_type::DT_UINT32:
        pb_data.mutable_datas_uint32()->insert({name, pNode->get_u_int32()});
        break;
    case data_type::DT_INT64:
        pb_data.mutable_datas_int64()->insert({name, pNode->get_int64()});
        break;
    case data_type::DT_UINT64:
        pb_data.mutable_datas_uint64()->insert({name, pNode->get_u_int64()});
        break;
    case data_type::DT_FLOAT:
        pb_data.mutable_datas_float()->insert({name, pNode->get_float()});
        break;
    case data_type::DT_DOUBLE:
        pb_data.mutable_datas_double()->insert({name, pNode->get_double()});
        break;
    case data_type::DT_STRING:
        pb_data.mutable_datas_string()->insert({name, pNode->get_string()});
        break;
    default:
        ARK_ASSERT_RET_VAL(0, false);
        break;
    }

    return true;
}

bool kernel_module_impl::table_to_db_data(table* pTable, AFMsg::pb_db_table& pb_data) {
    ARK_ASSERT_RET_VAL(pTable != nullptr, false);

    for (auto pRow = pTable->first(); pRow != nullptr; pRow = pTable->next())
    {
        auto pNodeManager = get_node_manager(pRow);
        if (!pNodeManager)
        {
            continue;
        }

        AFMsg::pb_db_entity_data row_data;

        auto& data_list = pNodeManager->get_data_list();
        for (auto& iter : data_list)
        {
            node_to_db_data(iter.second, row_data);
        }

        pb_data.mutable_datas_value()->insert({pRow->get_row(), row_data});
    }

    return true;
}

//----------entity to pb client data---------------
bool kernel_module_impl::node_to_pb_data(node* pNode, AFMsg::pb_entity_data* pb_data) {
    ARK_ASSERT_RET_VAL(pNode != nullptr && pb_data != nullptr, false);

    auto index = pNode->get_index();
    switch (pNode->get_type())
    {
    case data_type::DT_BOOLEAN:
        pb_data->mutable_datas_bool()->insert({index, pNode->get_bool()});
        break;
    case data_type::DT_INT32:
        pb_data->mutable_datas_int32()->insert({index, pNode->get_int32()});
        break;
    case data_type::DT_UINT32:
        pb_data->mutable_datas_uint32()->insert({index, pNode->get_u_int32()});
        break;
    case data_type::DT_INT64:
        pb_data->mutable_datas_int64()->insert({index, pNode->get_int64()});
        break;
    case data_type::DT_UINT64:
        pb_data->mutable_datas_uint64()->insert({index, pNode->get_u_int64()});
        break;
    case data_type::DT_FLOAT:
        pb_data->mutable_datas_float()->insert({index, pNode->get_float()});
        break;
    case data_type::DT_DOUBLE:
        pb_data->mutable_datas_double()->insert({index, pNode->get_double()});
        break;
    case data_type::DT_STRING:
        pb_data->mutable_datas_string()->insert({index, pNode->get_string()});
        break;
    default:
        ARK_ASSERT_RET_VAL(0, false);
        break;
    }

    return true;
}

bool kernel_module_impl::node_to_pb_data(const uint32_t index, const data& data, AFMsg::pb_entity_data* pb_data) {
    ARK_ASSERT_RET_VAL(index > 0 && pb_data != nullptr, false);

    switch (data.get_type())
    {
    case data_type::DT_BOOLEAN:
        pb_data->mutable_datas_bool()->insert({index, data.get_bool()});
        break;
    case data_type::DT_INT32:
        pb_data->mutable_datas_int32()->insert({index, data.get_int()});
        break;
    case data_type::DT_UINT32:
        pb_data->mutable_datas_uint32()->insert({index, data.get_u_int()});
        break;
    case data_type::DT_INT64:
        pb_data->mutable_datas_int64()->insert({index, data.get_int64()});
        break;
    case data_type::DT_UINT64:
        pb_data->mutable_datas_uint64()->insert({index, data.get_u_int64()});
        break;
    case data_type::DT_FLOAT:
        pb_data->mutable_datas_float()->insert({index, data.get_float()});
        break;
    case data_type::DT_DOUBLE:
        pb_data->mutable_datas_double()->insert({index, data.get_double()});
        break;
    case data_type::DT_STRING:
        pb_data->mutable_datas_string()->insert({index, data.get_string()});
        break;
    default:
        ARK_ASSERT_RET_VAL(0, false);
        break;
    }

    return true;
}

bool kernel_module_impl::table_to_pb_data(table* pTable, const uint32_t index, AFMsg::pb_table* pb_data) {
    ARK_ASSERT_RET_VAL(pTable != nullptr && index > 0 && pb_data != nullptr, false);

    for (row* pRow = pTable->first(); pRow != nullptr; pRow = pTable->next())
    {
        AFMsg::pb_entity_data row_data;
        if (!row_to_pb_data(pRow, pRow->get_row(), &row_data))
        {
            continue;
        }

        pb_data->mutable_datas_value()->insert({index, row_data});
    }

    return true;
}

bool kernel_module_impl::container_to_pb_data(std::shared_ptr<container> pContainer, AFMsg::pb_container* pb_data) {
    ARK_ASSERT_RET_VAL(pContainer != nullptr && pb_data != nullptr, false);

    for (auto index = pContainer->first(); index > 0; index = pContainer->next())
    {
        auto pEntity = pContainer->find(index);
        if (nullptr == pEntity)
        {
            continue;
        }

        AFMsg::pb_entity pb_entity;
        if (!entity_to_pb_data(pEntity, &pb_entity))
        {
            continue;
        }

        pb_data->mutable_datas_value()->insert({index, pb_entity});
    }

    return true;
}

bool kernel_module_impl::row_to_pb_data(row* pRow, const uint32_t index, AFMsg::pb_entity_data* pb_data) {
    ARK_ASSERT_RET_VAL(pRow != nullptr && index > 0 && pb_data != nullptr, false);

    auto pNodeManager = get_node_manager(pRow);
    if (!pNodeManager)
    {
        return false;
    }

    auto& data_list = pNodeManager->get_data_list();
    for (auto& iter : data_list)
    {
        node_to_pb_data(iter.second, pb_data);
    }

    return true;
}

bool kernel_module_impl::table_row_data_to_pb_data(
    const uint32_t index, uint32_t row, const uint32_t col, const data& data, AFMsg::pb_entity_data* pb_data) {
    ARK_ASSERT_RET_VAL(index > 0 && row > 0 && col > 0 && pb_data != nullptr, false);

    AFMsg::pb_entity_data row_data;
    if (!node_to_pb_data(col, data, &row_data))
    {
        return false;
    }

    AFMsg::pb_table table_data;
    table_data.mutable_datas_value()->insert({row, row_data});

    pb_data->mutable_datas_table()->insert({index, table_data});

    return true;
}

bool kernel_module_impl::entity_to_pb_data(std::shared_ptr<entity> pEntity, AFMsg::pb_entity* pb_data) {
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    pb_data->set_id(pEntity->get_id());

    entity_node_to_pb_data(pEntity, pb_data->mutable_data());

    entity_table_to_pb_data(pEntity, pb_data->mutable_data());

    entity_container_to_pb_data(pEntity, pb_data->mutable_data());

    return true;
}

bool kernel_module_impl::entity_to_pb_data_by_mask(
    std::shared_ptr<entity> pEntity, mask_type mask, AFMsg::pb_entity* pb_data) {
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    pb_data->set_id(pEntity->get_id());

    entity_node_to_pb_data(pEntity, pb_data->mutable_data(), mask);

    entity_table_to_pb_data(pEntity, pb_data->mutable_data(), mask);

    entity_container_to_pb_data(pEntity, pb_data->mutable_data(), mask);

    return true;
}

bool kernel_module_impl::entity_container_to_pb_data(
    std::shared_ptr<entity> pEntity, AFMsg::pb_entity_data* pb_data, const mask_type mask) {
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    auto pContainerManager = get_container_manager(pEntity);
    ARK_ASSERT_RET_VAL(pContainerManager != nullptr, false);

    auto& container_list = pContainerManager->get_container_list();
    for (auto& iter : container_list)
    {
        auto pContainer = iter.second;
        if (!pContainer)
        {
            continue;
        }

        if (!mask.none())
        {
            auto result = (pContainer->get_mask() & mask);
            if (!result.any())
            {
                continue;
            }
        }

        AFMsg::pb_container pb_container;
        if (!container_to_pb_data(pContainer, &pb_container))
        {
            continue;
        }

        pb_data->mutable_datas_container()->insert({iter.first, pb_container});
    }

    return true;
}

//node all to pb data
bool kernel_module_impl::entity_node_to_pb_data(
    std::shared_ptr<entity> pEntity, AFMsg::pb_entity_data* pb_data, const mask_type mask /* = 0*/) {
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    auto pNodeManager = get_node_manager(pEntity);
    ARK_ASSERT_RET_VAL(pNodeManager != nullptr, false);

    auto& data_list = pNodeManager->get_data_list();
    for (auto& iter : data_list)
    {
        auto pNode = iter.second;
        if (!pNode)
        {
            continue;
        }

        if (!mask.none())
        {
            auto result = (pNode->get_mask() & mask);
            if (!result.any())
            {
                continue;
            }
        }

        node_to_pb_data(pNode, pb_data);
    }

    return true;
}

bool kernel_module_impl::entity_table_to_pb_data(
    std::shared_ptr<entity> pEntity, AFMsg::pb_entity_data* pb_data, const mask_type mask /* = 0*/) {
    ARK_ASSERT_RET_VAL(pEntity != nullptr && pb_data != nullptr, false);

    auto pTableManager = get_table_manager(pEntity);
    ARK_ASSERT_RET_VAL(pTableManager != nullptr, false);

    auto& data_list = pTableManager->get_table_list();
    for (auto& iter : data_list)
    {
        auto pTable = iter.second;
        if (!pTable)
        {
            continue;
        }

        if (!mask.none())
        {
            auto result = (pTable->get_mask() & mask);
            if (!result.any())
            {
                continue;
            }
        }

        const auto index = pTable->get_index();

        AFMsg::pb_table table_data;
        if (!table_to_pb_data(pTable, index, &table_data))
        {
            continue;
        }

        pb_data->mutable_datas_table()->insert({index, table_data});
    }

    return true;
}

// -----------get entity manager--------------
std::shared_ptr<node_manager> kernel_module_impl::get_node_manager(std::shared_ptr<static_entity> pStaticEntity) const {
    if (pStaticEntity == nullptr)
    {
        return nullptr;
    }

    auto pCStaticEntity = std::dynamic_pointer_cast<static_entity_impl>(pStaticEntity);
    if (pCStaticEntity == nullptr)
    {
        return nullptr;
    }

    return pCStaticEntity->get_node_manager();
}

std::shared_ptr<node_manager> kernel_module_impl::get_node_manager(std::shared_ptr<entity> pEntity) const {
    if (pEntity == nullptr)
    {
        return nullptr;
    }

    auto pCEnity = std::dynamic_pointer_cast<entity_impl>(pEntity);
    if (pCEnity == nullptr)
    {
        return nullptr;
    }

    return pCEnity->get_node_manager();
}

std::shared_ptr<node_manager> kernel_module_impl::get_node_manager(row* pRow) const {
    if (pRow == nullptr)
    {
        return nullptr;
    }

    auto pCRow = dynamic_cast<row_impl*>(pRow);
    if (pCRow == nullptr)
    {
        return nullptr;
    }

    return pCRow->get_node_manager();
}

std::shared_ptr<table_manager> kernel_module_impl::get_table_manager(std::shared_ptr<entity> pEntity) const {
    if (pEntity == nullptr)
    {
        return nullptr;
    }

    auto pCEnity = std::dynamic_pointer_cast<entity_impl>(pEntity);
    if (pCEnity == nullptr)
    {
        return nullptr;
    }

    return pCEnity->get_table_manager();
}

std::shared_ptr<container_manager> kernel_module_impl::get_container_manager(std::shared_ptr<entity> pEntity) const {
    if (pEntity == nullptr)
    {
        return nullptr;
    }

    auto pCEnity = std::dynamic_pointer_cast<entity_impl>(pEntity);
    if (pCEnity == nullptr)
    {
        return nullptr;
    }

    return pCEnity->get_container_manager();
}

std::shared_ptr<event_manager> kernel_module_impl::get_event_manager(std::shared_ptr<entity> pEntity) const {
    if (pEntity == nullptr)
    {
        return nullptr;
    }

    auto pCEnity = std::dynamic_pointer_cast<entity_impl>(pEntity);
    if (pCEnity == nullptr)
    {
        return nullptr;
    }

    return pCEnity->get_event_manager();
}

} // namespace ark
