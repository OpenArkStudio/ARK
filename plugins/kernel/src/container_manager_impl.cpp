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

#include "ark/kernel/container_manager_impl.hpp"

namespace ark {

// find container
std::shared_ptr<container> container_manager_impl::find_container(const uint32_t index) const {
    return container_data_.find_value(index);
}

std::shared_ptr<container> container_manager_impl::create_container(
    std::shared_ptr<class_meta> pClassMeta, const uint32_t index, const guid_t& parent_id) {
    // return if found
    auto pContainer = find_container(index);
    if (nullptr != pContainer)
    {
        return pContainer;
    }

    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, nullptr);

    auto pContainerClassMeta = pClassMeta->find_container_meta(index);
    ARK_ASSERT_RET_VAL(pContainerClassMeta != nullptr, nullptr);

    auto pCallBack = pClassMeta->get_class_call_back_manager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, nullptr);

    pContainer = std::make_shared<container_impl>(pContainerClassMeta, parent_id, pCallBack);
    ARK_ASSERT_RET_VAL(pContainer != nullptr, nullptr);

    container_data_.insert(index, pContainer);

    return pContainer;
}

bool container_manager_impl::destroy_container(const uint32_t index) {
    return container_data_.erase(index);
}

const container_manager::container_list& container_manager_impl::get_container_list() const {
    return container_data_;
}

} // namespace ark
