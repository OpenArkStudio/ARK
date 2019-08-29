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

#include "AFClassMeta.hpp"
#include "base/AFMap.hpp"
#include "base/AFList.hpp"

namespace ark {

class AFClassMetaManager final
{
public:
    using ClassMetaList = AFNewSmartPtrHashmap<std::string, AFClassMeta>;

    AFClassMetaManager() = default;
    virtual ~AFClassMetaManager()
    {
        type_class_meta_list_.clear();
        class_meta_list_.clear();
    }

    // create meta
    ARK_SHARE_PTR<AFClassMeta> CreateMeta(const std::string& name)
    {
        // return if found
        auto iter = class_meta_list_.find(name);
        if (iter != class_meta_list_.end())
        {
            return iter->second;
        }

        // create new class meta
        ARK_SHARE_PTR<AFClassMeta> pMeta = std::make_shared<AFClassMeta>(name);
        class_meta_list_.insert(name, pMeta);

        return pMeta;
    }

    // find meta do not create
    ARK_SHARE_PTR<AFClassMeta> FindMeta(const std::string& name)
    {
        return class_meta_list_.find_value(name);
    }

    const ClassMetaList& GetMetaList() const
    {
        return class_meta_list_;
    }

    // add type class meta
    void AddTypeClassMeta(const std::string& name)
    {
        ARK_ASSERT_RET_NONE(!type_class_meta_list_.exist(name));

        type_class_meta_list_.push_back(name);
    }

    // exact object meta and table meta of class meta
    bool AfterLoaded()
    {
        for (auto iter : class_meta_list_)
        {
            auto& name = iter.first;
            auto meta = iter.second;
            ARK_ASSERT_RET_VAL(meta != nullptr, false);

            // continue if meta included by others
            if (type_class_meta_list_.exist(name))
            {
                continue;
            }

            if (!AfterLoaded(meta))
            {
                return false;
            }
        }

        return true;
    }

private:
    bool AfterLoaded(ARK_SHARE_PTR<AFClassMeta> pMeta)
    {
        ARK_ASSERT_RET_VAL(pMeta != nullptr, false);

        //auto& class_list = pMeta->GetClassList();
        //for (auto& iter : class_list)
        //{
        //    const std::string& type_name = iter;
        //    auto pClassMeta = FindMeta(type_name);
        //    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

        //    ExactToMeta(pMeta, pClassMeta);
        //}

        //// init table col meta
        //auto& table_meta_list = pMeta->GetTableMetaList();
        //for (auto iter : table_meta_list)
        //{
        //    auto pTableMeta = iter.second;
        //    ARK_ASSERT_RET_VAL(pTableMeta != nullptr, false);

        //    auto& type_name = pTableMeta->GetTypeName();
        //    auto pClassMeta = FindMeta(type_name);
        //    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

        //    auto data_meta_list = pClassMeta->GetDataMetaList();
        //    for (auto iter_data : data_meta_list)
        //    {
        //        auto& name = iter_data.first;
        //        auto pDataMeta = iter_data.second;
        //        pTableMeta->AddColMeta(name, pDataMeta);
        //    }
        //}

        //// init container meta
        //auto& container_list = pMeta->GetContainerList();
        //for (auto iter : container_list)
        //{
        //    const std::string& name = iter.first;
        //    const std::string& type_name = iter.second;

        //    auto pClassMeta = FindMeta(type_name);
        //    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

        //    pMeta->AddContainer(name, pClassMeta);
        //}

        return true;
    }

    bool ExactToMeta(ARK_SHARE_PTR<AFClassMeta> pMeta, ARK_SHARE_PTR<AFClassMeta> pClassMeta)
    {
        ARK_ASSERT_RET_VAL(pMeta != nullptr && pClassMeta != nullptr, false);

        //// exact data meta
        //auto& meta_list = pClassMeta->GetDataMetaList();
        //for (auto iter : meta_list)
        //{
        //    auto& name = iter.first;
        //    auto pDataMeta = iter.second;
        //    ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

        //    ARK_ASSERT_RET_VAL(!pMeta->AddDataMeta(pDataMeta), false);
        //}

        //// exact table class
        //auto& table_meta_list = pClassMeta->GetTableMetaList();
        //for (auto iter_table : table_meta_list)
        //{
        //    auto pTableMeta = iter_table.second;
        //    ARK_ASSERT_RET_VAL(pTableMeta != nullptr, false);

        //    ARK_ASSERT_RET_VAL(!pMeta->AddTableMeta(pTableMeta), false);
        //}

        //// exact container class
        //auto& container_list = pClassMeta->GetContainerList();
        //for (auto iter : container_list)
        //{
        //    ARK_ASSERT_RET_VAL(!pMeta->AddContainer(iter.first, iter.second), false);
        //}

        //// exact class meta
        //auto& class_list = pClassMeta->GetClassList();
        //for (auto iter : class_list)
        //{
        //    const std::string& type_name = iter;
        //    auto pOtherClassMeta = FindMeta(type_name);
        //    ARK_ASSERT_RET_VAL(pOtherClassMeta != nullptr, false);

        //    ExactToMeta(pMeta, pOtherClassMeta);
        //}

        return true;
    }

private:
    // class meta list
    ClassMetaList class_meta_list_;

    // type class meta
    AFList<std::string> type_class_meta_list_;
};

} // namespace ark
