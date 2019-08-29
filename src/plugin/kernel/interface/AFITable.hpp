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
#include "AFINode.hpp"

namespace ark {

class AFIRow
{
public:
    virtual ~AFIRow() = default;

    // query row
    virtual uint32_t GetRow() const = 0;

    // get row data
    virtual bool GetBool(const uint32_t index) = 0;
    virtual int32_t GetInt32(const uint32_t index) = 0;
    virtual int64_t GetInt64(const uint32_t index) = 0;
    virtual float GetFloat(const uint32_t index) = 0;
    virtual double GetDouble(const uint32_t index) = 0;
    virtual const std::string& GetString(const uint32_t index) = 0;
    virtual const std::wstring& GetWString(const uint32_t index) = 0;
    virtual const AFGUID& GetObject(const uint32_t index) = 0;

    // set row data
    virtual bool SetBool(const uint32_t index, bool value) = 0;
    virtual bool SetInt32(const uint32_t index, int32_t value) = 0;
    virtual bool SetInt64(const uint32_t index, int64_t value) = 0;
    virtual bool SetFloat(const uint32_t index, float value) = 0;
    virtual bool SetDouble(const uint32_t index, double value) = 0;
    virtual bool SetString(const uint32_t index, const std::string& value) = 0;
    virtual bool SetWString(const uint32_t index, const std::wstring& value) = 0;
    virtual bool SetObject(const uint32_t index, const AFGUID& value) = 0;
};

class AFITable
{
public:
    virtual ~AFITable() = default;

    // table meta interface
    virtual const std::string& GetName() const = 0;
    virtual const uint32_t GetColCount() const = 0;
    virtual const ArkDataType GetColType(const uint32_t index) const = 0;
    virtual const AFFeatureType GetFeatureType() const = 0;
    virtual bool IsPublic() const = 0;
    virtual bool IsPrivate() const = 0;
    virtual bool IsRealTime() const = 0;
    virtual bool IsSave() const = 0;

    virtual uint32_t FindInt32(const uint32_t index, const int32_t value) = 0;
    virtual uint32_t FindInt64(const uint32_t index, const int64_t value) = 0;
    virtual uint32_t FindBool(const uint32_t index, bool value) = 0;
    virtual uint32_t FindFloat(const uint32_t index, float value) = 0;
    virtual uint32_t FindDouble(const uint32_t index, double value) = 0;
    virtual uint32_t FindString(const uint32_t index, const std::string& value) = 0;
    virtual uint32_t FindWString(const uint32_t index, const std::wstring& value) = 0;
    virtual uint32_t FindObject(const uint32_t index, const AFGUID& value) = 0;

    // table get
    virtual uint32_t GetRowCount() const = 0;

    // table set
    virtual AFIRow* AddRow(uint32_t row) = 0;
    virtual AFIRow* AddRow(uint32_t row, const AFIDataList& data) = 0;
    virtual AFIRow* FindRow(uint32_t row) = 0;
    virtual bool RemoveRow(uint32_t row) = 0;
    virtual void Clear() = 0;
};

} // namespace ark
