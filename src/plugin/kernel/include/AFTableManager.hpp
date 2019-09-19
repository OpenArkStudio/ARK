/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "kernel/include/AFCTable.hpp"
#include "kernel/include/AFCData.hpp"
#include "kernel/include/AFCRow.hpp"

namespace ark {

class AFTableManager final
{
public:
    using TableList = AFNewMap<uint32_t, AFITable>;

private:
    // class meta
    ARK_SHARE_PTR<AFClassMeta> class_meta_{nullptr};

    // table list
    TableList table_list_;
    TableList::const_iterator iter_table_;

public:
    AFTableManager() = delete;

    explicit AFTableManager(ARK_SHARE_PTR<AFClassMeta> pClassMeta)
    {
        class_meta_ = pClassMeta;
    }

    // table operation
    AFITable* AddTable(const AFGUID& guid, const uint32_t index)
    {
        ARK_ASSERT_RET_VAL(class_meta_ != nullptr, nullptr);

        AFITable* pTable = table_list_.find_value(index);
        ARK_ASSERT_RET_VAL(pTable == nullptr, nullptr);

        auto pTableMeta = class_meta_->FindTableMeta(index);
        ARK_ASSERT_RET_VAL(pTableMeta != nullptr, nullptr);

        pTable = ARK_NEW AFCTable(pTableMeta, class_meta_->GetClassCallBackManager(), guid);
        ARK_ASSERT_RET_VAL(pTable != nullptr, nullptr);

        if (!table_list_.insert(index, pTable).second)
        {
            ARK_DELETE(pTable);
            return nullptr;
        }

        return pTable;
    }

    AFITable* FindTable(const uint32_t index)
    {
        return table_list_.find_value(index);
    }

    AFITable* First()
    {
        iter_table_ = table_list_.begin();
        ARK_ASSERT_RET_VAL(iter_table_ != table_list_.end(), nullptr);

        return iter_table_->second;
    }

    AFITable* Next()
    {
        ARK_ASSERT_RET_VAL(iter_table_ != table_list_.end(), nullptr);

        iter_table_++;
        ARK_ASSERT_RET_VAL(iter_table_ != table_list_.end(), nullptr);

        return iter_table_->second;
    }
};

} // namespace ark
