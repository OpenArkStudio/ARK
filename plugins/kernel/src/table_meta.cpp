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

#include "ark/kernel/node_meta.hpp"
#include "ark/kernel/table_meta.hpp"

namespace ark {

table_meta::table_meta(const std::string& name, const uint32_t index)
    : name_(name)
    , index_(index) {}

const std::string& table_meta::get_name() const {
    return name_;
}

size_t table_meta::get_col_count() const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, NULL_INT);

    return class_meta_->get_node_count();
}

void table_meta::set_class_meta(std::shared_ptr<class_meta> pClassMeta) {
    class_meta_ = pClassMeta;
}

std::shared_ptr<node_meta> table_meta::find_meta(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, nullptr);

    return class_meta_->find_data_meta(index);
}

std::shared_ptr<class_meta> table_meta::get_class_meta() const {
    return class_meta_;
}

data_type table_meta::get_col_type(const uint32_t index) const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, data_type::DT_EMPTY);

    auto pMeta = class_meta_->find_data_meta(index);
    ARK_ASSERT_RET_VAL(pMeta != nullptr, data_type::DT_EMPTY);

    return pMeta->get_type();
}

const mask_type table_meta::get_mask() const {
    return mask_;
}

bool table_meta::have_mask(const data_mask mask) const {
    return mask_.test((size_t)mask);
}

void table_meta::set_mask(const mask_type& feature) {
    mask_ = feature;
}

bool table_meta::is_public() const {
    return mask_.test((size_t)data_mask::PF_SYNC_VIEW);
}

bool table_meta::is_private() const {
    return mask_.test((size_t)data_mask::PF_SYNC_SELF);
}

bool table_meta::is_real_time() const {
    return mask_.test((size_t)data_mask::PF_REAL_TIME);
}

bool table_meta::is_save() const {
    return mask_.test((size_t)data_mask::PF_SAVE);
}

void table_meta::set_type_name(const std::string& value) {
    type_name_ = value;
}

const std::string& table_meta::get_type_name() const {
    return type_name_;
}

uint32_t table_meta::get_index() const {
    return index_;
}

uint32_t table_meta::get_index(const std::string& name) const {
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, NULL_INT);

    return class_meta_->get_index(name);
}

} // namespace ark
