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

class AFTableMeta;

class AFClassMeta final
{
public:
    using ClassList = std::vector<std::string>;
    using ContainerList = std::map<uint32_t, std::string>;
    using NameIndexList = std::unordered_map<std::string, uint32_t>;
    using DataMetaList = AFNewSmartPtrMap<uint32_t, AFNodeMeta>;
    using TableMetaList = AFNewSmartPtrMap<uint32_t, AFTableMeta>;
    using ContainerMetaList = AFNewSmartPtrMap<uint32_t, AFContainerMeta>;

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

public:
    AFClassMeta() = delete;

    explicit AFClassMeta(const std::string& name);
    virtual ~AFClassMeta();

    size_t GetNodeCount() const;
    const std::string& GetName() const;

    void SetResPath(const std::string& value);
    const std::string& GetResPath() const;

    // create data meta
    ARK_SHARE_PTR<AFNodeMeta> CreateDataMeta(const std::string& name, const uint32_t index);
    ARK_SHARE_PTR<AFNodeMeta> FindDataMeta(const uint32_t index);

    // create table meta
    ARK_SHARE_PTR<AFTableMeta> CreateTableMeta(const std::string& name, const uint32_t index);
    ARK_SHARE_PTR<AFTableMeta> FindTableMeta(const uint32_t index);

    // create table meta
    ARK_SHARE_PTR<AFContainerMeta> CreateContainerMeta(
        const std::string& name, const uint32_t index, const std::string& class_name);
    ARK_SHARE_PTR<AFContainerMeta> FindContainerMeta(const uint32_t index);

    const DataMetaList& GetDataMetaList() const;
    const TableMetaList& GetTableMetaList() const;
    const ContainerMetaList& GetContainerMetaList() const;

    ARK_SHARE_PTR<AFClassCallBackManager> GetClassCallBackManager() const;

    uint32_t GetIndex(const std::string& name);

    bool IsEntityMeta() const;

private:
    bool NameIndexInsert(const std::string& name, const uint32_t index);
};

} // namespace ark
