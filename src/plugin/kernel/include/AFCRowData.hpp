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

#include "kernel/interface/AFIRecordInner.hpp"
#include "kernel/interface/AFIDataNewType.hpp"
#include "base/AFMacros.hpp"
#include "base/AFMap.hpp"

namespace ark {

class AFCRowData final : public AFIRowData
{
public:
    AFCRowData() = delete;

    // constructor
    explicit AFCRowData(AFIRecordInner* pRecordEx, size_t row);

    ~AFCRowData() override;

    // get row
    size_t GetRow() const override;

    // get row data
    bool GetBool(const std::string& name) override;
    int32_t GetInt32(const std::string& name) override;
    int64_t GetInt64(const std::string& name) override;
    float GetFloat(const std::string& name) override;
    double GetDouble(const std::string& name) override;
    const std::string& GetString(const std::string& name) override;
    const std::wstring& GetWString(const std::string& name) override;
    const AFGUID& GetObject(const std::string& name) override;

    // set row data
    bool SetBool(const std::string& name, bool value) override;
    bool SetInt32(const std::string& name, int32_t value) override;
    bool SetInt64(const std::string& name, int64_t value) override;
    bool SetFloat(const std::string& name, float value) override;
    bool SetDouble(const std::string& name, double value) override;
    bool SetString(const std::string& name, const std::string& value) override;
    bool SetWString(const std::string& name, const std::wstring& value) override;
    bool SetObject(const std::string& name, const AFGUID& value) override;

private:
    // find data
    AFIDataNew* FindData(const std::string& name, bool bCreate = false);
    bool OnRecordCallBack(const std::string& name, const AFIDataNewType& old_data, const AFIDataNewType& new_data);

private:
    // row
    size_t row_;

    // record ex
    AFIRecordInner* record_ex_{nullptr};

    // data
    AFNewHashmap<std::string, AFIDataNew> data_;
};

} // namespace ark
