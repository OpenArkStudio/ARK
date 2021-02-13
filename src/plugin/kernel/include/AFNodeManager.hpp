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

#include "kernel/interface/AFINode.hpp"
#include "AFClassMeta.hpp"
#include "kernel/interface/AFIDataList.hpp"

namespace ark {

class AFNodeManager final
{
public:
    using DataList = AFMap<uint32_t, AFINode>;
    using NODE_MANAGER_FUNCTOR = std::function<int(AFINode*, const AFIData&, const AFIData&)>;

private:
    // class meta
    std::shared_ptr<AFClassMeta> class_meta_{nullptr};

    // call back
    NODE_MANAGER_FUNCTOR func_;

    // data list
    DataList data_list_;

public:
    AFNodeManager() = delete;

    explicit AFNodeManager(std::shared_ptr<AFClassMeta> pClassMeta);
    explicit AFNodeManager(
        std::shared_ptr<AFClassMeta> pClassMeta, const AFIDataList& data_list, NODE_MANAGER_FUNCTOR&& func);

    void InitData(const AFIDataList& args);

    bool IsEmpty() const;

    // data operation
    AFINode* CreateData(std::shared_ptr<AFNodeMeta> pDataMeta);
    bool CreateData(AFINode* pData);
    AFINode* CreateData(const std::string& name);

    // get node
    AFINode* GetNode(const std::string& name) const;
    AFINode* GetNode(const uint32_t index) const;

    // query data
    bool GetBool(const std::string& name) const;
    int32_t GetInt32(const std::string& name) const;
    uint32_t GetUInt32(const std::string& name) const;
    int64_t GetInt64(const std::string& name) const;
    uint64_t GetUInt64(const std::string& name) const;
    float GetFloat(const std::string& name) const;
    double GetDouble(const std::string& name) const;
    const std::string& GetString(const std::string& name) const;
    const std::wstring& GetWString(const std::string& name) const;
    const guid_t& GetGUID(const std::string& name) const;

    bool GetBool(const uint32_t index) const;
    int32_t GetInt32(const uint32_t index) const;
    uint32_t GetUInt32(const uint32_t index) const;
    int64_t GetInt64(const uint32_t index) const;
    uint64_t GetUInt64(const uint32_t index) const;
    float GetFloat(const uint32_t index) const;
    double GetDouble(const uint32_t index) const;
    const std::string& GetString(const uint32_t index) const;
    const std::wstring& GetWString(const uint32_t index) const;
    const guid_t& GetGUID(const uint32_t index) const;

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
    bool SetGUID(const std::string& name, const guid_t& value);

    bool SetBool(const uint32_t index, bool value);
    bool SetInt32(const uint32_t index, const int32_t value);
    bool SetUInt32(const uint32_t index, const uint32_t value);
    bool SetInt64(const uint32_t index, const int64_t value);
    bool SetUInt64(const uint32_t index, const uint64_t value);
    bool SetFloat(const uint32_t index, const float value);
    bool SetDouble(const uint32_t index, const double value);
    bool SetString(const uint32_t index, const std::string& value);
    bool SetWString(const uint32_t index, const std::wstring& value);
    bool SetGUID(const uint32_t index, const guid_t& value);

    // other query
    const DataList& GetDataList() const;

private:
    uint32_t GetIndex(const std::string& name) const;

    AFINode* FindData(const uint32_t index);
    AFINode* FindData(const uint32_t index) const;
};

} // namespace ark
