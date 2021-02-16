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
    virtual bool GetBool(const uint32_t index) const = 0;
    virtual int32_t GetInt32(const uint32_t index) const = 0;
    virtual int64_t GetInt64(const uint32_t index) const = 0;
    virtual uint32_t GetUInt32(const uint32_t index) const = 0;
    virtual uint64_t GetUInt64(const uint32_t index) const = 0;
    virtual float GetFloat(const uint32_t index) const = 0;
    virtual double GetDouble(const uint32_t index) const = 0;
    virtual const std::string& GetString(const uint32_t index) const = 0;
    virtual const std::wstring& GetWString(const uint32_t index) const = 0;
    virtual const guid_t& GetGUID(const uint32_t index) const = 0;

    virtual bool GetBool(const std::string& name) const = 0;
    virtual int32_t GetInt32(const std::string& name) const = 0;
    virtual int64_t GetInt64(const std::string& name) const = 0;
    virtual uint32_t GetUInt32(const std::string& name) const = 0;
    virtual uint64_t GetUInt64(const std::string& name) const = 0;
    virtual float GetFloat(const std::string& name) const = 0;
    virtual double GetDouble(const std::string& name) const = 0;
    virtual const std::string& GetString(const std::string& name) const = 0;
    virtual const std::wstring& GetWString(const std::string& name) const = 0;
    virtual const guid_t& GetGUID(const std::string& name) const = 0;

    // set row data
    virtual bool SetBool(const uint32_t index, bool value) = 0;
    virtual bool SetInt32(const uint32_t index, int32_t value) = 0;
    virtual bool SetInt64(const uint32_t index, int64_t value) = 0;
    virtual bool SetUInt32(const uint32_t index, uint32_t value) = 0;
    virtual bool SetUInt64(const uint32_t index, uint64_t value) = 0;
    virtual bool SetFloat(const uint32_t index, float value) = 0;
    virtual bool SetDouble(const uint32_t index, double value) = 0;
    virtual bool SetString(const uint32_t index, const std::string& value) = 0;
    virtual bool SetWString(const uint32_t index, const std::wstring& value) = 0;
    virtual bool SetGUID(const uint32_t index, const guid_t& value) = 0;

    virtual bool SetBool(const std::string& name, bool value) = 0;
    virtual bool SetInt32(const std::string& name, int32_t value) = 0;
    virtual bool SetInt64(const std::string& name, int64_t value) = 0;
    virtual bool SetUInt32(const std::string& name, uint32_t value) = 0;
    virtual bool SetUInt64(const std::string& name, uint64_t value) = 0;
    virtual bool SetFloat(const std::string& name, float value) = 0;
    virtual bool SetDouble(const std::string& name, double value) = 0;
    virtual bool SetString(const std::string& name, const std::string& value) = 0;
    virtual bool SetWString(const std::string& name, const std::wstring& value) = 0;
    virtual bool SetGUID(const std::string& name, const guid_t& value) = 0;
};

class AFITable
{
public:
    virtual ~AFITable() = default;

    // table meta interface
    virtual uint32_t GetIndex(const std::string& name) const = 0;
    virtual const std::string& GetName() const = 0;
    virtual uint32_t GetColCount() const = 0;
    virtual ArkDataType GetColType(const uint32_t index) const = 0;
    virtual const ArkMaskType GetMask() const = 0;
    virtual bool HaveMask(const ArkDataMask feature) const = 0;
    virtual bool IsPublic() const = 0;
    virtual bool IsPrivate() const = 0;
    virtual bool IsRealTime() const = 0;
    virtual bool IsSave() const = 0;

    virtual uint32_t FindInt32(const uint32_t index, const int32_t value) const = 0;
    virtual uint32_t FindInt64(const uint32_t index, const int64_t value) const = 0;
    virtual uint32_t FindBool(const uint32_t index, bool value) const = 0;
    virtual uint32_t FindFloat(const uint32_t index, float value) const = 0;
    virtual uint32_t FindDouble(const uint32_t index, double value) const = 0;
    virtual uint32_t FindString(const uint32_t index, const std::string& value) const = 0;
    virtual uint32_t FindWString(const uint32_t index, const std::wstring& value) const = 0;
    virtual uint32_t FindGUID(const uint32_t index, const guid_t& value) const = 0;

    virtual AFIRow* First() = 0;
    virtual AFIRow* Next() = 0;

    virtual uint32_t GetIndex() const = 0;

    // table get
    virtual uint32_t GetRowCount() const = 0;

    // table set
    virtual AFIRow* AddRow(uint32_t row = 0u) = 0;
    virtual AFIRow* AddRow(uint32_t row, const AFIDataList& args) = 0;
    virtual AFIRow* FindRow(uint32_t row) const = 0;
    virtual bool RemoveRow(uint32_t row) = 0;
    virtual void Clear() = 0;
};

} // namespace ark
