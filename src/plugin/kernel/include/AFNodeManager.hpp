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
#include "kernel/interface/AFINode.hpp"

namespace ark {

class AFNodeManager final
{
public:
    using DataList = AFNewMap<uint32_t, AFINode>;
    using NODE_COMPONENT_FUNCTOR =
        std::function<int(const std::string&, const uint32_t, const AFIData&, const AFIData&)>;

private:
    // class meta
    ARK_SHARE_PTR<AFClassMeta> class_meta_{nullptr};

    // call back
    NODE_COMPONENT_FUNCTOR func_;

    // data list
    DataList data_list_;
    DataList::const_iterator iter_data_;

public:
    AFNodeManager() = delete;

    explicit AFNodeManager(ARK_SHARE_PTR<AFClassMeta> pClassMeta);
    explicit AFNodeManager(ARK_SHARE_PTR<AFClassMeta> pClassMeta, NODE_COMPONENT_FUNCTOR&& func);

    uint32_t GetIndex(const std::string& name);
    const std::string& GetClassName() const;
    ARK_SHARE_PTR<AFClassMeta> GetClassMeta() const;
    bool IsEmpty() const;

    // data operation
    AFINode* CreateData(ARK_SHARE_PTR<AFNodeMeta> pDataMeta);

    // get node
    AFINode* GetNode(const std::string& name);
    AFINode* GetNode(const uint32_t index);

    // query data
    bool GetBool(const std::string& name);
    int32_t GetInt32(const std::string& name);
    uint32_t GetUInt32(const std::string& name);
    int64_t GetInt64(const std::string& name);
    uint64_t GetUInt64(const std::string& name);
    float GetFloat(const std::string& name);
    double GetDouble(const std::string& name);
    const std::string& GetString(const std::string& name);
    const std::wstring& GetWString(const std::string& name);
    const AFGUID& GetGUID(const std::string& name);

    bool GetBool(const uint32_t index);
    int32_t GetInt32(const uint32_t index);
    uint32_t GetUInt32(const uint32_t index);
    int64_t GetInt64(const uint32_t index);
    uint64_t GetUInt64(const uint32_t index);
    float GetFloat(const uint32_t index);
    double GetDouble(const uint32_t index);
    const std::string& GetString(const uint32_t index);
    const std::wstring& GetWString(const uint32_t index);
    const AFGUID& GetGUID(const uint32_t index);

    // set data
    bool SetBool(const std::string& name, bool value);
    bool SetInt32(const std::string& name, const int32_t value);
    bool SetUInt32(const std::string& name, const uint32_t value);
    bool SetInt64(const std::string& name, const int64_t value);
    bool SetUInt64(const std::string& name, const uint64_t value);
    bool SetFloat(const std::string& name, const float value);
    bool SetDouble(const std::string& name, const double value);
    bool SetString(const std::string& name, const std::string& value);
    bool SetWString(const std::string& name, const std::wstring& value);
    bool SetGUID(const std::string& name, const AFGUID& value);

    bool SetBool(const uint32_t index, bool value);
    bool SetInt32(const uint32_t index, const int32_t value);
    bool SetUInt32(const uint32_t index, const uint32_t value);
    bool SetInt64(const uint32_t index, const int64_t value);
    bool SetUInt64(const uint32_t index, const uint64_t value);
    bool SetFloat(const uint32_t index, const float value);
    bool SetDouble(const uint32_t index, const double value);
    bool SetString(const uint32_t index, const std::string& value);
    bool SetWString(const uint32_t index, const std::wstring& value);
    bool SetGUID(const uint32_t index, const AFGUID& value);

    // other query
    const DataList& GetDataList();

    AFINode* First();
    AFINode* Next();

private:
    AFINode* FindData(const uint32_t index, bool bCreate = false);
};

} // namespace ark
