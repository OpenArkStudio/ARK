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

#include "kernel/interface/AFITableInner.hpp"
#include "kernel/interface/AFIData.hpp"
#include "base/AFMacros.hpp"
#include "base/AFMap.hpp"

namespace ark {

class AFCRow final : public AFIRow
{
public:
    AFCRow() = delete;

    // constructor
    explicit AFCRow(AFITableInner* pTableInner, uint32_t row);

    // get row
    uint32_t GetRow() const override;

    // get row data
    bool GetBool(const uint32_t index) override;
    int32_t GetInt32(const uint32_t index) override;
    int64_t GetInt64(const uint32_t index) override;
    uint32_t GetUInt32(const uint32_t index) override;
    uint64_t GetUInt64(const uint32_t index) override;
    float GetFloat(const uint32_t index) override;
    double GetDouble(const uint32_t index) override;
    const std::string& GetString(const uint32_t index) override;
    const std::wstring& GetWString(const uint32_t index) override;
    const AFGUID& GetGUID(const uint32_t index) override;

    bool GetBool(const std::string& name) override;
    int32_t GetInt32(const std::string& name) override;
    int64_t GetInt64(const std::string& name) override;
    uint32_t GetUInt32(const std::string& name) override;
    uint64_t GetUInt64(const std::string& name) override;
    float GetFloat(const std::string& name) override;
    double GetDouble(const std::string& name) override;
    const std::string& GetString(const std::string& name) override;
    const std::wstring& GetWString(const std::string& name) override;
    const AFGUID& GetGUID(const std::string& name) override;

    // set row data
    bool SetBool(const uint32_t index, bool value) override;
    bool SetInt32(const uint32_t index, int32_t value) override;
    bool SetInt64(const uint32_t index, int64_t value) override;
    bool SetUInt32(const uint32_t index, uint32_t value) override;
    bool SetUInt64(const uint32_t index, uint64_t value) override;
    bool SetFloat(const uint32_t index, float value) override;
    bool SetDouble(const uint32_t index, double value) override;
    bool SetString(const uint32_t index, const std::string& value) override;
    bool SetWString(const uint32_t index, const std::wstring& value) override;
    bool SetGUID(const uint32_t index, const AFGUID& value) override;

    bool SetBool(const std::string& name, bool value) override;
    bool SetInt32(const std::string& name, int32_t value) override;
    bool SetInt64(const std::string& name, int64_t value) override;
    bool SetUInt32(const std::string& name, uint32_t value) override;
    bool SetUInt64(const std::string& name, uint64_t value) override;
    bool SetFloat(const std::string& name, float value) override;
    bool SetDouble(const std::string& name, double value) override;
    bool SetString(const std::string& name, const std::string& value) override;
    bool SetWString(const std::string& name, const std::wstring& value) override;
    bool SetGUID(const std::string& name, const AFGUID& value) override;

    AFINode* First() override;
    AFINode* Next() override;

private:
    // find data
    AFINode* FindData(const uint32_t index, bool bCreate = false);
    bool OnTableCallBack(const uint32_t index, const AFIData& old_data, const AFIData& new_data);

private:
    // row
    uint32_t row_{0u};

    // table ex
    AFITableInner* table_inner_{nullptr};

    // data
    using DataList = AFNewMap<uint32_t, AFINode>;
    DataList data_;
    DataList::iterator iter_;
};

} // namespace ark
