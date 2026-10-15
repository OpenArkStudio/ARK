/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "ark/kernel/container_impl.hpp"
#include "ark/kernel/container_manager_impl.hpp"
#include "ark/kernel/data_impl.hpp"
#include "ark/kernel/entity_impl.hpp"
#include "ark/kernel/event_manager_impl.hpp"
#include "ark/kernel/static_entity_impl.hpp"

namespace ark {

entity_impl::entity_impl(std::shared_ptr<class_meta> pClassMeta, const guid_t& guid, const id_type config_id,
    const int32_t map_id, const int32_t map_entity_id, const data_list& data_list)
    : guid_(guid)
    , config_id_(config_id) {
    class_meta_ = pClassMeta;

    // todo : create by class name
    opt_charactor_ = std::make_shared<entity_opt_charactor>(map_id, map_entity_id);

    // data node
    auto func = std::bind(
        &entity_impl::on_node_call_back, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_pNodeManager = std::make_shared<node_manager>(pClassMeta, data_list, std::move(func));

    // data table
    m_pTableManager = std::make_shared<table_manager>(pClassMeta);

    m_pEventManager = std::make_shared<event_manager_impl>(guid_);
    m_pCallBackManager = pClassMeta->get_class_call_back_manager();
}

void entity_impl::update() {
    //
}

// get unique id
const guid_t& entity_impl::get_id() const {
    return guid_;
}

std::shared_ptr<container> entity_impl::get_parent_container() const {
    return parent_container_;
}

bool entity_impl::set_parent_container(std::shared_ptr<container> pContainer) {
    parent_container_ = pContainer;
    return true;
}

bool entity_impl::is_public(const std::string& name) const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, false);

    auto index = class_meta_->get_index(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return is_public(index);
}

bool entity_impl::is_public(const uint32_t index) const {
    return have_mask(index, data_mask::PF_SYNC_VIEW);
}

bool entity_impl::is_private(const std::string& name) const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, false);

    auto index = class_meta_->get_index(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return is_private(index);
}

bool entity_impl::is_private(const uint32_t index) const {
    return have_mask(index, data_mask::PF_SYNC_SELF);
}

bool entity_impl::is_save(const std::string& name) const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, false);

    auto index = class_meta_->get_index(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return is_save(index);
}

bool entity_impl::is_save(const uint32_t index) const {
    return have_mask(index, data_mask::PF_SAVE);
}

bool entity_impl::is_real_time(const std::string& name) const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, false);

    auto index = class_meta_->get_index(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return is_real_time(index);
}

bool entity_impl::is_real_time(const uint32_t index) const {
    return have_mask(index, data_mask::PF_REAL_TIME);
}

bool entity_impl::have_mask(const std::string& name, data_mask feature) const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, false);

    auto index = class_meta_->get_index(name);
    ARK_ASSERT_RET_VAL(index > 0, false);

    return have_mask(index, feature);
}

bool entity_impl::have_mask(const uint32_t index, data_mask feature) const {
    node* p = m_pNodeManager->get_node(index);

    ARK_ASSERT_RET_VAL(p != nullptr, false);

    return p->have_mask(feature);
}

mask_type entity_impl::get_mask(const uint32_t index) const {
    node* p = m_pNodeManager->get_node(index);

    ARK_ASSERT_RET_VAL(p != nullptr, 0);

    return p->get_mask();
}

const std::string& entity_impl::get_class_name() const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, NULL_STR);

    return class_meta_->get_name();
}

data_type entity_impl::get_node_type(const std::string& name) const {
    auto node = m_pNodeManager->get_node(name);
    ARK_ASSERT_RET_VAL(node != nullptr, data_type::DT_EMPTY);

    return node->get_type();
}

data_type entity_impl::get_node_type(const uint32_t value) const {
    auto node = m_pNodeManager->get_node(value);
    ARK_ASSERT_RET_VAL(node != nullptr, data_type::DT_EMPTY);

    return node->get_type();
}

uint32_t entity_impl::get_index(const std::string& name) const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, NULL_INT);

    return class_meta_->get_index(name);
}

id_type entity_impl::get_config_id() const {
    return config_id_;
}

int32_t entity_impl::get_map_id() const {
    ARK_ASSERT_RET_VAL(opt_charactor_ != nullptr, NULL_INT);

    return opt_charactor_->map_id_;
}

int32_t entity_impl::get_map_entity_id() const {
    ARK_ASSERT_RET_VAL(opt_charactor_ != nullptr, NULL_INT);

    return opt_charactor_->map_entity_id_;
}

bool entity_impl::switch_scene(const int32_t map_id, const int32_t map_inst_id, const vector3d& pos) {
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr && opt_charactor_ != nullptr, false);

    if (opt_charactor_->map_id_ == map_id && opt_charactor_->map_entity_id_ == map_inst_id)
    {
        return false;
    }

    m_pCallBackManager->on_leave_scene_event(guid_, opt_charactor_->map_id_, opt_charactor_->map_entity_id_);

    opt_charactor_->map_id_ = map_id;
    opt_charactor_->map_entity_id_ = map_inst_id;
    opt_charactor_->pos_ = pos;

    m_pCallBackManager->on_enter_scene_event(guid_, opt_charactor_->map_id_, opt_charactor_->map_entity_id_);

    return true;
}

const vector3d& entity_impl::get_position() const {
    ARK_ASSERT_RET_VAL(opt_charactor_ != nullptr, NULL_VECTOR3D);

    return opt_charactor_->pos_;
}

float entity_impl::get_position_x() const {
    ARK_ASSERT_RET_VAL(opt_charactor_ != nullptr, NULL_FLOAT);

    return opt_charactor_->pos_.x;
}

float entity_impl::get_position_y() const {
    ARK_ASSERT_RET_VAL(opt_charactor_ != nullptr, NULL_FLOAT);

    return opt_charactor_->pos_.y;
}

float entity_impl::get_position_z() const {
    ARK_ASSERT_RET_VAL(opt_charactor_ != nullptr, NULL_FLOAT);

    return opt_charactor_->pos_.z;
}

float entity_impl::get_orient() const {
    ARK_ASSERT_RET_VAL(opt_charactor_ != nullptr, NULL_FLOAT);

    return opt_charactor_->orient_;
}

bool entity_impl::set_position(const vector3d& position, const float orient) {
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr && opt_charactor_ != nullptr, false);

    if (opt_charactor_->pos_ == position && opt_charactor_->orient_ == orient)
    {
        return false;
    }

    vector3d old_pos = opt_charactor_->pos_;

    opt_charactor_->pos_ = position;
    opt_charactor_->orient_ = orient;

    m_pCallBackManager->on_move_event(guid_, old_pos, opt_charactor_->pos_);

    return true;
}

bool entity_impl::set_position(const float x, const float y, const float z, const float orient) {
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr && opt_charactor_ != nullptr, false);

    if (opt_charactor_->pos_.x == x && opt_charactor_->pos_.y == y && opt_charactor_->pos_.z == z &&
        opt_charactor_->orient_ == orient)
    {
        return false;
    }

    vector3d old_pos = opt_charactor_->pos_;

    opt_charactor_->pos_.x = x;
    opt_charactor_->pos_.y = y;
    opt_charactor_->pos_.z = z;
    opt_charactor_->orient_ = orient;

    m_pCallBackManager->on_move_event(guid_, old_pos, opt_charactor_->pos_);

    return true;
}

// set data
bool entity_impl::set_bool(const std::string& name, bool value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_bool(name, value);
}

bool entity_impl::set_int32(const std::string& name, const int32_t value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_int32(name, value);
}

bool entity_impl::set_u_int32(const std::string& name, const uint32_t value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_u_int32(name, value);
}

bool entity_impl::set_int64(const std::string& name, const int64_t value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_int64(name, value);
}

bool entity_impl::set_u_int64(const std::string& name, const uint64_t value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_u_int64(name, value);
}

bool entity_impl::set_float(const std::string& name, const float value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_float(name, value);
}

bool entity_impl::set_double(const std::string& name, const double value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_double(name, value);
}

bool entity_impl::set_string(const std::string& name, const std::string& value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_string(name, value);
}

bool entity_impl::set_w_string(const std::string& name, const std::wstring& value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_w_string(name, value);
}

bool entity_impl::set_guid(const std::string& name, const guid_t& value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_guid(name, value);
}

bool entity_impl::set_bool(const uint32_t index, bool value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_bool(index, value);
}

bool entity_impl::set_int32(const uint32_t index, const int32_t value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_int32(index, value);
}

bool entity_impl::set_u_int32(const uint32_t index, const uint32_t value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_u_int32(index, value);
}

bool entity_impl::set_int64(const uint32_t index, const int64_t value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_int64(index, value);
}

bool entity_impl::set_u_int64(const uint32_t index, const uint64_t value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_u_int64(index, value);
}

bool entity_impl::set_float(const uint32_t index, const float value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_float(index, value);
}

bool entity_impl::set_double(const uint32_t index, const double value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_double(index, value);
}

bool entity_impl::set_string(const uint32_t index, const std::string& value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_string(index, value);
}

bool entity_impl::set_w_string(const uint32_t index, const std::wstring& value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_w_string(index, value);
}

bool entity_impl::set_guid(const uint32_t index, const guid_t& value) {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    return m_pNodeManager->set_guid(index, value);
}

bool entity_impl::get_node(const std::string& name, data& data) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    auto pNode = m_pNodeManager->get_node(name);
    ARK_ASSERT_RET_VAL(pNode != nullptr, false);

    return data.from(pNode);
}

// get data
bool entity_impl::get_bool(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_BOOLEAN);

    return m_pNodeManager->get_bool(name);
}

int32_t entity_impl::get_int32(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->get_int32(name);
}

uint32_t entity_impl::get_u_int32(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->get_u_int32(name);
}

int64_t entity_impl::get_int64(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->get_int64(name);
}

uint64_t entity_impl::get_u_int64(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->get_u_int64(name);
}

float entity_impl::get_float(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_FLOAT);

    return m_pNodeManager->get_float(name);
}

double entity_impl::get_double(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_DOUBLE);

    return m_pNodeManager->get_double(name);
}

const std::string& entity_impl::get_string(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_STR);

    return m_pNodeManager->get_string(name);
}

const std::wstring& entity_impl::get_w_string(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_WIDESTR);

    return m_pNodeManager->get_w_string(name);
}

const guid_t& entity_impl::get_guid(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_GUID);

    return m_pNodeManager->get_guid(name);
}

bool entity_impl::get_node(const uint32_t index, data& data) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, false);

    auto pNode = m_pNodeManager->get_node(index);
    ARK_ASSERT_RET_VAL(pNode != nullptr, false);

    return data.from(pNode);
}

bool entity_impl::get_bool(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_BOOLEAN);

    return m_pNodeManager->get_bool(index);
}

int32_t entity_impl::get_int32(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->get_int32(index);
}

uint32_t entity_impl::get_u_int32(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->get_u_int32(index);
}

int64_t entity_impl::get_int64(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->get_int64(index);
}

uint64_t entity_impl::get_u_int64(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->get_u_int64(index);
}

float entity_impl::get_float(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_FLOAT);

    return m_pNodeManager->get_float(index);
}

double entity_impl::get_double(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_DOUBLE);

    return m_pNodeManager->get_double(index);
}

const std::string& entity_impl::get_string(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_STR);

    return m_pNodeManager->get_string(index);
}

const std::wstring& entity_impl::get_w_string(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_WIDESTR);

    return m_pNodeManager->get_w_string(index);
}

const guid_t& entity_impl::get_guid(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_GUID);

    return m_pNodeManager->get_guid(index);
}

// container operation
std::shared_ptr<container> entity_impl::find_container(const std::string& name) {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, nullptr);

    auto index = class_meta_->get_index(name);
    ARK_ASSERT_RET_VAL(index > 0, nullptr);

    return find_container(index);
}

std::shared_ptr<container> entity_impl::find_container(const uint32_t index) {
    auto pContainerManager = get_container_manager();
    ARK_ASSERT_RET_VAL(pContainerManager != nullptr, nullptr);

    auto pContainer = pContainerManager->find_container(index);
    if (pContainer == nullptr)
    {
        // create a new container
        pContainer = pContainerManager->create_container(class_meta_, index, guid_);
    }

    return pContainer;
}

// table data
table* entity_impl::find_table(const std::string& name) {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, nullptr);

    auto index = class_meta_->get_index(name);
    ARK_ASSERT_RET_VAL(index > 0, nullptr);

    return find_table(index);
}

table* entity_impl::find_table(const uint32_t index) {
    auto pTable = m_pTableManager->find_table(index);
    if (nullptr == pTable)
    {
        auto func = std::bind(&entity_impl::on_table_call_back, this, std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
        pTable = m_pTableManager->create_table(index, std::move(func));
    }

    return pTable;
}

std::shared_ptr<event_manager> entity_impl::get_event_manager() const {
    return m_pEventManager;
}

std::shared_ptr<class_meta> entity_impl::get_class_meta() const {
    return class_meta_;
}

std::shared_ptr<node_manager> entity_impl::get_node_manager() const {
    return m_pNodeManager;
}

std::shared_ptr<table_manager> entity_impl::get_table_manager() const {
    return m_pTableManager;
}

std::shared_ptr<container_manager> entity_impl::get_container_manager() const {
    ARK_ASSERT_RET_VAL(opt_charactor_ != nullptr, nullptr);

    return opt_charactor_->m_pContainerManager;
}

int entity_impl::on_node_call_back(node* pNode, const data& old_data, const data& new_data) {
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr, 0);

    m_pCallBackManager->on_node_call_back(guid_, pNode, old_data, new_data);

    return 0;
}

int entity_impl::on_table_call_back(
    const mask_type mask, node* pNode, const table_event_data& event_data, const data& old_data, const data& new_data) {
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr, 0);

    m_pCallBackManager->on_table_call_back(guid_, mask, pNode, event_data, old_data, new_data);

    return 0;
}

bool entity_impl::add_custom_bool(const std::string& name, bool value) {
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    data* pData = ARK_NEW data_impl;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->set_bool(value);
    if (!custom_data_list_.insert(name, pData).second)
    {
        ARK_DELETE(pData);
        return false;
    }

    return true;
}

bool entity_impl::add_custom_int32(const std::string& name, const int32_t value) {
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    data* pData = ARK_NEW data_impl;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->set_int(value);
    if (!custom_data_list_.insert(name, pData).second)
    {
        ARK_DELETE(pData);
        return false;
    }

    return true;
}

bool entity_impl::add_custom_u_int32(const std::string& name, const uint32_t value) {
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    data* pData = ARK_NEW data_impl;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->set_u_int(value);
    if (!custom_data_list_.insert(name, pData).second)
    {
        ARK_DELETE(pData);
        return false;
    }

    return true;
}

bool entity_impl::add_custom_int64(const std::string& name, const int64_t value) {
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    data* pData = ARK_NEW data_impl;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->set_int64(value);
    if (!custom_data_list_.insert(name, pData).second)
    {
        ARK_DELETE(pData);
        return false;
    }

    return true;
}

bool entity_impl::add_custom_float(const std::string& name, const float value) {
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    data* pData = ARK_NEW data_impl;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->set_float(value);
    if (!custom_data_list_.insert(name, pData).second)
    {
        ARK_DELETE(pData);
        return false;
    }

    return true;
}

bool entity_impl::add_custom_double(const std::string& name, const double value) {
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    data* pData = ARK_NEW data_impl;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->set_double(value);
    if (!custom_data_list_.insert(name, pData).second)
    {
        ARK_DELETE(pData);
        return false;
    }

    return true;
}

bool entity_impl::add_custom_string(const std::string& name, const std::string& value) {
    ARK_ASSERT_RET_VAL(custom_data_list_.find(name) == custom_data_list_.end(), false);

    data* pData = ARK_NEW data_impl;
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    pData->set_string(value.c_str());
    if (!custom_data_list_.insert(name, pData).second)
    {
        ARK_DELETE(pData);
        return false;
    }

    return true;
}

bool entity_impl::add_custom_w_string(const std::string& name, const std::wstring& value) {
    return false;
}

bool entity_impl::add_custom_guid(const std::string& name, const guid_t& value) {
    return false;
}

bool entity_impl::set_custom_bool(const std::string& name, bool value) {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->get_type() == data_type::DT_BOOLEAN, false);

    pData->set_bool(value);

    return true;
}

bool entity_impl::set_custom_int32(const std::string& name, const int32_t value) {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->get_type() == data_type::DT_INT32, false);

    pData->set_int(value);

    return true;
}

bool entity_impl::set_custom_u_int32(const std::string& name, const uint32_t value) {
    return false;
}

bool entity_impl::set_custom_int64(const std::string& name, const int64_t value) {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->get_type() == data_type::DT_INT64, false);

    pData->set_int64(value);

    return true;
}

bool entity_impl::set_custom_float(const std::string& name, const float value) {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->get_type() == data_type::DT_FLOAT, false);

    pData->set_float(value);

    return true;
}

bool entity_impl::set_custom_double(const std::string& name, const double value) {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->get_type() == data_type::DT_DOUBLE, false);

    pData->set_double(value);

    return true;
}

bool entity_impl::set_custom_string(const std::string& name, const std::string& value) {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->get_type() == data_type::DT_STRING, false);

    pData->set_string(value.c_str());

    return true;
}

bool entity_impl::set_custom_w_string(const std::string& name, const std::wstring& value) {
    return false;
}

bool entity_impl::set_custom_guid(const std::string& name, const guid_t& value) {
    return false;
}

bool entity_impl::get_custom_bool(const std::string& name) const {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    return pData->get_bool();
}

int32_t entity_impl::get_custom_int32(const std::string& name) const {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->get_int();
}

uint32_t entity_impl::get_custom_u_int32(const std::string& name) const {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->get_u_int();
}

int64_t entity_impl::get_custom_int64(const std::string& name) const {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->get_int64();
}

float entity_impl::get_custom_float(const std::string& name) const {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_FLOAT);

    return pData->get_float();
}

double entity_impl::get_custom_double(const std::string& name) const {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_DOUBLE);

    return pData->get_double();
}

const char* entity_impl::get_custom_string(const std::string& name) const {
    auto pData = custom_data_list_.find_value(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_STR.c_str());

    return pData->get_string();
}

const std::wstring& entity_impl::get_custom_w_string(const std::string& name) const {
    return NULL_WIDESTR;
}

const guid_t& entity_impl::get_custom_guid(const std::string& name) const {
    return NULL_GUID;
}

bool entity_impl::find_custom_data(const std::string& name) const {
    return custom_data_list_.find(name) != custom_data_list_.end();
}

bool entity_impl::remove_custom_data(const std::string& name) {
    return custom_data_list_.erase(name);
}

bool entity_impl::is_sent() const {
    return sent_;
}

void entity_impl::update_sent() {
    sent_ = true;
}

} // namespace ark
