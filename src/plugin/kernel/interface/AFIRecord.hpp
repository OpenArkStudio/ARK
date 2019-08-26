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

#include "AFIDataList.hpp"
#include "AFIDataNew.hpp"

namespace ark {

class AFIRowData
{
public:
    virtual ~AFIRowData() = default;

    // query row
    virtual size_t GetRow() const = 0;

    // get row data
    virtual bool GetBool(const std::string& name) = 0;
    virtual int32_t GetInt32(const std::string& name) = 0;
    virtual int64_t GetInt64(const std::string& name) = 0;
    virtual float GetFloat(const std::string& name) = 0;
    virtual double GetDouble(const std::string& name) = 0;
    virtual const std::string& GetString(const std::string& name) = 0;
    virtual const std::wstring& GetWString(const std::string& name) = 0;
    virtual const AFGUID& GetObject(const std::string& name) = 0;

    // set row data
    virtual bool SetBool(const std::string& name, bool value) = 0;
    virtual bool SetInt32(const std::string& name, int32_t value) = 0;
    virtual bool SetInt64(const std::string& name, int64_t value) = 0;
    virtual bool SetFloat(const std::string& name, float value) = 0;
    virtual bool SetDouble(const std::string& name, double value) = 0;
    virtual bool SetString(const std::string& name, const std::string& value) = 0;
    virtual bool SetWString(const std::string& name, const std::wstring& value) = 0;
    virtual bool SetObject(const std::string& name, const AFGUID& value) = 0;
};

class AFIRecord
{
public:
    virtual ~AFIRecord() = default;

    // record meta interface
    virtual const std::string& GetName() const = 0;
    virtual const size_t GetColCount() const = 0;
    virtual const ArkDataType GetColType(const std::string& name) const = 0;
    virtual const AFFeatureType GetFeatureType() const = 0;
    virtual bool IsPublic() const = 0;
    virtual bool IsPrivate() const = 0;
    virtual bool IsRealTime() const = 0;
    virtual bool IsSave() const = 0;

    virtual size_t FindInt32(const std::string& name, const int32_t value) = 0;
    virtual size_t FindInt64(const std::string& name, const int64_t value) = 0;
    virtual size_t FindBool(const std::string& name, bool value) = 0;
    virtual size_t FindFloat(const std::string& name, float value) = 0;
    virtual size_t FindDouble(const std::string& name, double value) = 0;
    virtual size_t FindString(const std::string& name, const std::string& value) = 0;
    virtual size_t FindWString(const std::string& name, const std::wstring& value) = 0;
    virtual size_t FindObject(const std::string& name, const AFGUID& value) = 0;

    // record get
    virtual size_t GetRowCount() const = 0;

    // record set
    virtual AFIRowData* AddRow(size_t row) = 0;
    virtual AFIRowData* AddRow(size_t row, const AFIDataList& data) = 0;
    virtual AFIRowData* FindRow(size_t row) = 0;
    virtual bool RemoveRow(size_t row) = 0;
    virtual void Clear() = 0;
};

} // namespace ark
