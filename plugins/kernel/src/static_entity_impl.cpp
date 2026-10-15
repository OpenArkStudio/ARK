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

#include "ark/kernel/class_meta.hpp"
#include "ark/kernel/node_impl.hpp"
#include "ark/kernel/static_entity_impl.hpp"
#include "ark/kernel/table_impl.hpp"

namespace ark {

static_entity_impl::static_entity_impl(
    std::shared_ptr<class_meta> pClassMeta, std::shared_ptr<node_manager> pNodeManager, const id_type config_id)
    : config_id_(config_id) {
    class_meta_ = pClassMeta;
    m_pNodeManager = pNodeManager;
}

// query data
const std::string& static_entity_impl::get_class_name() const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, NULL_STR);

    return class_meta_->get_name();
}

id_type static_entity_impl::get_config_id() const {
    return config_id_;
}

bool static_entity_impl::get_bool(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_BOOLEAN);

    return m_pNodeManager->get_bool(name);
}

int32_t static_entity_impl::get_int32(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->get_int32(name);
}

uint32_t static_entity_impl::get_u_int32(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->get_u_int32(name);
}

int64_t static_entity_impl::get_int64(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->get_int64(name);
}

int64_t static_entity_impl::get_u_int64(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->get_u_int64(name);
}

float static_entity_impl::get_float(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_FLOAT);

    return m_pNodeManager->get_float(name);
}

double static_entity_impl::get_double(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_DOUBLE);

    return m_pNodeManager->get_double(name);
}

const std::string& static_entity_impl::get_string(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_STR);

    return m_pNodeManager->get_string(name);
}

const std::wstring& static_entity_impl::get_w_string(const std::string& name) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_WIDESTR);

    return m_pNodeManager->get_w_string(name);
}

bool static_entity_impl::get_bool(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_BOOLEAN);

    return m_pNodeManager->get_bool(index);
}

int32_t static_entity_impl::get_int32(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->get_int32(index);
}

uint32_t static_entity_impl::get_u_int32(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->get_u_int32(index);
}

int64_t static_entity_impl::get_int64(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->get_int64(index);
}

int64_t static_entity_impl::get_u_int64(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->get_u_int64(index);
}

float static_entity_impl::get_float(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_FLOAT);

    return m_pNodeManager->get_float(index);
}

double static_entity_impl::get_double(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_DOUBLE);

    return m_pNodeManager->get_double(index);
}

const std::string& static_entity_impl::get_string(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_STR);

    return m_pNodeManager->get_string(index);
}

const std::wstring& static_entity_impl::get_w_string(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_WIDESTR);

    return m_pNodeManager->get_w_string(index);
}

std::shared_ptr<node_manager> static_entity_impl::get_node_manager() const {
    return m_pNodeManager;
}

} // namespace ark
