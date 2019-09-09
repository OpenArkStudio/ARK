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

#include "base/AFMacros.hpp"
#include "base/AFMap.hpp"
#include "base/AFDefine.hpp"
#include "base/AFEnum.hpp"
#include "AFNodeMeta.hpp"
#include "AFTableMeta.hpp"
#include "AFClassCallBackManager.hpp"
#include "AFContainerMeta.hpp"

namespace ark {

class AFClassMeta final
{
public:
    using ClassList = std::vector<std::string>;
    using ContainerList = std::map<uint32_t, std::string>;
    using NameIndexList = std::unordered_map<std::string, uint32_t>;
    using DataMetaList = AFNewSmartPtrMap<uint32_t, AFNodeMeta>;
    using TableMetaList = AFNewSmartPtrMap<uint32_t, AFTableMeta>;
    using ContainerMetaList = AFNewSmartPtrMap<uint32_t, AFContainerMeta>;

    AFClassMeta() = delete;

    explicit AFClassMeta(const std::string& name)
        : name_(name)
    {
        class_meta_call_back_ = std::make_shared<AFClassCallBackManager>();
    }

    virtual ~AFClassMeta()
    {
        container_meta_list_.clear();
        data_meta_list_.clear();
        table_meta_list_.clear();
        name_index_list_.clear();
    }

    const std::string& GetName() const
    {
        return name_;
    }

    void SetResPath(const std::string& value)
    {
        res_path_ = value;
    }

    const std::string& GetResPath() const
    {
        return res_path_;
    }

    // create data meta
    ARK_SHARE_PTR<AFNodeMeta> CreateDataMeta(const std::string& name, const uint32_t index)
    {
        // check arg
        ARK_ASSERT_RET_VAL(!name.empty() && index > 0, nullptr);

        ARK_ASSERT_RET_VAL(data_meta_list_.find(index) == data_meta_list_.end(), nullptr);

        ARK_ASSERT_RET_VAL(NameIndexInsert(name, index), nullptr);

        // create new data meta
        ARK_SHARE_PTR<AFNodeMeta> pMeta = std::make_shared<AFNodeMeta>(name, index);
        data_meta_list_.insert(index, pMeta);

        return pMeta;
    }

    ARK_SHARE_PTR<AFNodeMeta> FindDataMeta(const uint32_t index)
    {
        return data_meta_list_.find_value(index);
    }

    // create table meta
    ARK_SHARE_PTR<AFTableMeta> CreateTableMeta(const std::string& name, const uint32_t index)
    {
        // check arg
        ARK_ASSERT_RET_VAL(!name.empty() && index > 0, nullptr);

        ARK_ASSERT_RET_VAL(table_meta_list_.find(index) == table_meta_list_.end(), nullptr);

        ARK_ASSERT_RET_VAL(NameIndexInsert(name, index), nullptr);

        ARK_SHARE_PTR<AFTableMeta> pMeta = std::make_shared<AFTableMeta>(name, index);
        table_meta_list_.insert(index, pMeta);

        return pMeta;
    }

    ARK_SHARE_PTR<AFTableMeta> FindTableMeta(const uint32_t index)
    {
        return table_meta_list_.find_value(index);
    }

    // create table meta
    ARK_SHARE_PTR<AFContainerMeta> CreateContainerMeta(
        const std::string& name, const uint32_t index, const std::string& class_name)
    {
        // check arg
        ARK_ASSERT_RET_VAL(!name.empty() && index > 0, nullptr);

        ARK_ASSERT_RET_VAL(container_meta_list_.find(index) == container_meta_list_.end(), nullptr);

        ARK_ASSERT_RET_VAL(NameIndexInsert(name, index), nullptr);

        ARK_SHARE_PTR<AFContainerMeta> pMeta = std::make_shared<AFContainerMeta>(name, index, class_name);
        container_meta_list_.insert(index, pMeta);

        return pMeta;
    }

    ARK_SHARE_PTR<AFContainerMeta> FindContainerMeta(const uint32_t index)
    {
        return container_meta_list_.find_value(index);
    }

    const DataMetaList& GetDataMetaList() const
    {
        return data_meta_list_;
    }

    const TableMetaList& GetTableMetaList() const
    {
        return table_meta_list_;
    }

    ARK_SHARE_PTR<AFClassCallBackManager> GetClassCallBackManager() const
    {
        return class_meta_call_back_;
    }

    uint32_t GetIndex(const std::string& name)
    {
        auto iter = name_index_list_.find(name);
        if (iter == name_index_list_.end())
        {
            return NULL_INT;
        }

        return iter->second;
    }

    bool IsEntityMeta() const
    {
        return res_path_.empty();
    }

private:
    bool NameIndexInsert(const std::string& name, const uint32_t index)
    {
        return name_index_list_.insert(std::make_pair(name, index)).second;
    }

private:
    // class name
    std::string name_{NULL_STR};

    // res path
    std::string res_path_{NULL_STR};

    // name index
    NameIndexList name_index_list_;

    // data meta list
    DataMetaList data_meta_list_;

    // table meta list
    TableMetaList table_meta_list_;

    // container meta list
    ContainerMetaList container_meta_list_;

    // class meta call back
    ARK_SHARE_PTR<AFClassCallBackManager> class_meta_call_back_;
};

} // namespace ark
