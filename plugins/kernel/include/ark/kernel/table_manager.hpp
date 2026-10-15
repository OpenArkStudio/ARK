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

#pragma once

#include "ark/kernel/data_impl.hpp"
#include "ark/kernel/row.hpp"
#include "ark/kernel/table_impl.hpp"

namespace ark {

class table_manager final
{
public:
    using table_list = object_map<uint32_t, table>;

private:
    // class meta
    std::shared_ptr<class_meta> class_meta_{nullptr};

    // table list
    table_list table_list_;

public:
    table_manager() = delete;

    explicit table_manager(std::shared_ptr<class_meta> pClassMeta) {
        class_meta_ = pClassMeta;
    }

    // table operation
    table* create_table(const uint32_t index, table_impl::table_callback_functor&& func) {
        ARK_ASSERT_RET_VAL(class_meta_ != nullptr, nullptr);

        table* pTable = table_list_.find_value(index);
        ARK_ASSERT_RET_VAL(pTable == nullptr, nullptr);

        auto pTableMeta = class_meta_->find_table_meta(index);
        ARK_ASSERT_RET_VAL(pTableMeta != nullptr, nullptr);

        pTable = ARK_NEW table_impl(pTableMeta, std::move(func));
        ARK_ASSERT_RET_VAL(pTable != nullptr, nullptr);

        if (!table_list_.insert(index, pTable).second)
        {
            ARK_DELETE(pTable);
            return nullptr;
        }

        return pTable;
    }

    table* find_table(const uint32_t index) {
        return table_list_.find_value(index);
    }

    const table_list& get_table_list() const {
        return table_list_;
    }
};

} // namespace ark
