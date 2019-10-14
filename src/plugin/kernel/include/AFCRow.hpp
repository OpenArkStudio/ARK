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

#include "kernel/interface/AFIData.hpp"
#include "base/AFMacros.hpp"
#include "base/AFMap.hpp"
#include "AFNodeManager.hpp"
#include "kernel/interface/AFITable.hpp"
#include "kernel/interface/AFIDataList.hpp"

namespace ark {

class AFCRow final : public AFIRow
{
private:
    friend class AFCKernelModule;

    // row
    uint32_t row_{0u};

    // table call back
    using TABLE_COMPONENT_FUNCTOR = std::function<int(const uint32_t, const uint32_t, const AFIData&, const AFIData&)>;
    TABLE_COMPONENT_FUNCTOR func_;

    // data
    std::shared_ptr<AFNodeManager> m_pNodeManager{nullptr};

public:
    AFCRow() = delete;

    // constructor
    explicit AFCRow(
        std::shared_ptr<AFClassMeta> pClassMeta, uint32_t row, const AFIDataList& args, TABLE_COMPONENT_FUNCTOR&& func);

    // get row
    uint32_t GetRow() const override;

    // get row data
    bool GetBool(const uint32_t index) const override;
    int32_t GetInt32(const uint32_t index) const override;
    int64_t GetInt64(const uint32_t index) const override;
    uint32_t GetUInt32(const uint32_t index) const override;
    uint64_t GetUInt64(const uint32_t index) const override;
    float GetFloat(const uint32_t index) const override;
    double GetDouble(const uint32_t index) const override;
    const std::string& GetString(const uint32_t index) const override;
    const std::wstring& GetWString(const uint32_t index) const override;
    const AFGUID& GetGUID(const uint32_t index) const override;

    bool GetBool(const std::string& name) const override;
    int32_t GetInt32(const std::string& name) const override;
    uint32_t GetUInt32(const std::string& name) const override;
    int64_t GetInt64(const std::string& name) const override;
    uint64_t GetUInt64(const std::string& name) const override;
    float GetFloat(const std::string& name) const override;
    double GetDouble(const std::string& name) const override;
    const std::string& GetString(const std::string& name) const override;
    const std::wstring& GetWString(const std::string& name) const override;
    const AFGUID& GetGUID(const std::string& name) const override;

    // set row data
    bool SetBool(const uint32_t index, bool value) override;
    bool SetInt32(const uint32_t index, int32_t value) override;
    bool SetUInt32(const uint32_t index, uint32_t value) override;
    bool SetInt64(const uint32_t index, int64_t value) override;
    bool SetUInt64(const uint32_t index, uint64_t value) override;
    bool SetFloat(const uint32_t index, float value) override;
    bool SetDouble(const uint32_t index, double value) override;
    bool SetString(const uint32_t index, const std::string& value) override;
    bool SetWString(const uint32_t index, const std::wstring& value) override;
    bool SetGUID(const uint32_t index, const AFGUID& value) override;

    bool SetBool(const std::string& name, bool value) override;
    bool SetInt32(const std::string& name, int32_t value) override;
    bool SetUInt32(const std::string& name, uint32_t value) override;
    bool SetInt64(const std::string& name, int64_t value) override;
    bool SetUInt64(const std::string& name, uint64_t value) override;
    bool SetFloat(const std::string& name, float value) override;
    bool SetDouble(const std::string& name, double value) override;
    bool SetString(const std::string& name, const std::string& value) override;
    bool SetWString(const std::string& name, const std::wstring& value) override;
    bool SetGUID(const std::string& name, const AFGUID& value) override;

private:
    void InitData(const AFIDataList& args);

    std::shared_ptr<AFNodeManager> GetNodeManager() const;

    int OnDataCallBack(const std::string& name, const uint32_t index, const AFIData& old_data, const AFIData& new_data);
};

} // namespace ark
