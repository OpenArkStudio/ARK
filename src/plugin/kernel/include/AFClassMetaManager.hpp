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

#include "base/AFMap.hpp"
#include "base/AFList.hpp"
#include "AFClassMeta.hpp"

namespace ark {

class AFClassMetaManager final
{
public:
    using ClassMetaList = AFSmartPtrHashmap<std::string, AFClassMeta>;

private:
    // class meta list
    ClassMetaList class_meta_list_;

public:
    AFClassMetaManager() = default;
    virtual ~AFClassMetaManager() = default;

    // create meta
    std::shared_ptr<AFClassMeta> CreateMeta(const std::string& name)
    {
        // return if found
        auto iter = class_meta_list_.find(name);
        if (iter != class_meta_list_.end())
        {
            return iter->second;
        }

        // create new class meta
        std::shared_ptr<AFClassMeta> pMeta = std::make_shared<AFClassMeta>(name);
        class_meta_list_.insert(name, pMeta);

        return pMeta;
    }

    // find meta do not create
    std::shared_ptr<AFClassMeta> FindMeta(const std::string& name) const
    {
        return class_meta_list_.find_value(name);
    }

    const ClassMetaList& GetMetaList() const
    {
        return class_meta_list_;
    }

    bool AfterLoaded()
    {
        for (auto iter_class_meta : class_meta_list_)
        {
            auto meta = iter_class_meta.second;
            ARK_ASSERT_RET_VAL(meta != nullptr, false);

            // init table col meta
            auto& table_meta_list = meta->GetTableMetaList();
            for (auto iter : table_meta_list)
            {
                auto pTableMeta = iter.second;
                ARK_ASSERT_RET_VAL(pTableMeta != nullptr, false);

                auto& type_name = pTableMeta->GetTypeName();
                auto pClassMeta = FindMeta(type_name);
                ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

                pTableMeta->SetClassMeta(pClassMeta);
            }
        }

        return true;
    }
};

} // namespace ark
