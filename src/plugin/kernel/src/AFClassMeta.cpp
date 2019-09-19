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

#include "kernel/include/AFClassMeta.hpp"

namespace ark {

AFClassMeta::AFClassMeta(const std::string& name)
    : name_(name)
{
    class_meta_call_back_ = std::make_shared<AFClassCallBackManager>();
}

AFClassMeta::~AFClassMeta()
{
    container_meta_list_.clear();
    data_meta_list_.clear();
    table_meta_list_.clear();
    name_index_list_.clear();
}

size_t AFClassMeta::GetNodeCount() const
{
    return data_meta_list_.size();
}

const std::string& AFClassMeta::GetName() const
{
    return name_;
}

void AFClassMeta::SetResPath(const std::string& value)
{
    res_path_ = value;
}

const std::string& AFClassMeta::GetResPath() const
{
    return res_path_;
}

// create data meta
ARK_SHARE_PTR<AFNodeMeta> AFClassMeta::CreateDataMeta(const std::string& name, const uint32_t index)
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

ARK_SHARE_PTR<AFNodeMeta> AFClassMeta::FindDataMeta(const uint32_t index)
{
    return data_meta_list_.find_value(index);
}

// create table meta
ARK_SHARE_PTR<AFTableMeta> AFClassMeta::CreateTableMeta(const std::string& name, const uint32_t index)
{
    // check arg
    ARK_ASSERT_RET_VAL(!name.empty() && index > 0, nullptr);

    ARK_ASSERT_RET_VAL(table_meta_list_.find(index) == table_meta_list_.end(), nullptr);

    ARK_ASSERT_RET_VAL(NameIndexInsert(name, index), nullptr);

    ARK_SHARE_PTR<AFTableMeta> pMeta = std::make_shared<AFTableMeta>(name, index);
    table_meta_list_.insert(index, pMeta);

    return pMeta;
}

ARK_SHARE_PTR<AFTableMeta> AFClassMeta::FindTableMeta(const uint32_t index)
{
    return table_meta_list_.find_value(index);
}

// create container meta
ARK_SHARE_PTR<AFContainerMeta> AFClassMeta::CreateContainerMeta(
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

ARK_SHARE_PTR<AFContainerMeta> AFClassMeta::FindContainerMeta(const uint32_t index)
{
    return container_meta_list_.find_value(index);
}

const AFClassMeta::DataMetaList& AFClassMeta::GetDataMetaList() const
{
    return data_meta_list_;
}

const AFClassMeta::TableMetaList& AFClassMeta::GetTableMetaList() const
{
    return table_meta_list_;
}

const AFClassMeta::ContainerMetaList& AFClassMeta::GetContainerMetaList() const
{
    return container_meta_list_;
}

ARK_SHARE_PTR<AFClassCallBackManager> AFClassMeta::GetClassCallBackManager() const
{
    return class_meta_call_back_;
}

uint32_t AFClassMeta::GetIndex(const std::string& name)
{
    auto iter = name_index_list_.find(name);
    if (iter == name_index_list_.end())
    {
        return NULL_INT;
    }

    return iter->second;
}

bool AFClassMeta::IsEntityMeta() const
{
    return res_path_.empty();
}

bool AFClassMeta::NameIndexInsert(const std::string& name, const uint32_t index)
{
    return name_index_list_.insert(std::make_pair(name, index)).second;
}

} // namespace ark
