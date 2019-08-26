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

#include "kernel/include/AFCRowData.hpp"
#include "kernel/include/AFCDataNewType.hpp"
#include "kernel/include/AFCDataNew.hpp"

namespace ark {

// constructor
AFCRowData::AFCRowData(AFIRecordInner* pRecordEx, size_t row)
    : row_(row)
{
    record_ex_ = pRecordEx;
}

AFCRowData::~AFCRowData()
{
    data_.removeall();
}

// get row
size_t AFCRowData::GetRow() const
{
    return row_;
}

// get row data
bool AFCRowData::GetBool(const std::string& name)
{
    AFIDataNew* pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    return pData->GetBool();
}

int32_t AFCRowData::GetInt32(const std::string& name)
{
    AFIDataNew* pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT);

    return pData->GetInt32();
}

int64_t AFCRowData::GetInt64(const std::string& name)
{
    AFIDataNew* pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_INT64);

    return pData->GetInt64();
}

float AFCRowData::GetFloat(const std::string& name)
{
    AFIDataNew* pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_FLOAT);

    return pData->GetFloat();
}

double AFCRowData::GetDouble(const std::string& name)
{
    AFIDataNew* pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_DOUBLE);

    return pData->GetDouble();
}

const std::string& AFCRowData::GetString(const std::string& name)
{
    AFIDataNew* pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_STR);

    return pData->GetString();
}

const std::wstring& AFCRowData::GetWString(const std::string& name)
{
    AFIDataNew* pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_WIDESTR);

    return pData->GetWString();
}

const AFGUID& AFCRowData::GetObject(const std::string& name)
{
    AFIDataNew* pData = FindData(name);
    ARK_ASSERT_RET_VAL(pData != nullptr, NULL_GUID);

    return pData->GetObject();
}

// set row data
bool AFCRowData::SetBool(const std::string& name, bool value)
{
    AFIDataNew* pData = FindData(name, true);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_BOOLEAN, false);

    auto old_value = pData->GetBool();
    pData->SetBool(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        OnRecordCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

bool AFCRowData::SetInt32(const std::string& name, int32_t value)
{
    AFIDataNew* pData = FindData(name, true);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_INT32, false);

    auto old_value = pData->GetInt32();
    pData->SetInt32(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        OnRecordCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

bool AFCRowData::SetInt64(const std::string& name, int64_t value)
{
    AFIDataNew* pData = FindData(name, true);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_INT64, false);

    auto old_value = pData->GetInt64();
    pData->SetInt64(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        OnRecordCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

bool AFCRowData::SetFloat(const std::string& name, float value)
{
    AFIDataNew* pData = FindData(name, true);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_FLOAT, false);

    auto old_value = pData->GetFloat();
    pData->SetFloat(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        OnRecordCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

bool AFCRowData::SetDouble(const std::string& name, double value)
{
    AFIDataNew* pData = FindData(name, true);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_DOUBLE, false);

    auto old_value = pData->GetDouble();
    pData->SetDouble(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        OnRecordCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

bool AFCRowData::SetString(const std::string& name, const std::string& value)
{
    AFIDataNew* pData = FindData(name, true);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_STRING, false);

    auto old_value = pData->GetString();
    pData->SetString(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        OnRecordCallBack(name, AFCDataNewType(data_type, old_value.c_str()), AFCDataNewType(data_type, value.c_str()));
    }

    return true;
}

bool AFCRowData::SetWString(const std::string& name, const std::wstring& value)
{
    AFIDataNew* pData = FindData(name, true);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_WSTRING, false);

    auto old_value = pData->GetWString();
    pData->SetWString(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        OnRecordCallBack(name, AFCDataNewType(data_type, old_value.c_str()), AFCDataNewType(data_type, value.c_str()));
    }

    return true;
}

bool AFCRowData::SetObject(const std::string& name, const AFGUID& value)
{
    AFIDataNew* pData = FindData(name, true);
    ARK_ASSERT_RET_VAL(pData != nullptr, false);

    ARK_ASSERT_RET_VAL(pData->GetType() == ArkDataType::DT_OBJECT, false);

    auto old_value = pData->GetObject();
    pData->SetObject(value);

    if (old_value != value)
    {
        // data callbacks
        ArkDataType data_type = pData->GetType();
        OnRecordCallBack(name, AFCDataNewType(data_type, old_value), AFCDataNewType(data_type, value));
    }

    return true;
}

// find data
AFIDataNew* AFCRowData::FindData(const std::string& name, bool bCreate /* = false*/)
{
    auto pData = data_.find_value(name);

    if (nullptr == pData && bCreate)
    {
        ARK_ASSERT_RET_VAL(record_ex_ != nullptr, nullptr);

        auto pRecordMeta = record_ex_->GetMeta();
        ARK_ASSERT_RET_VAL(pRecordMeta != nullptr, nullptr);

        auto pDataMeta = pRecordMeta->FindMeta(name);
        ARK_ASSERT_RET_VAL(pDataMeta != nullptr, nullptr);

        pData = CreateDataByMeta(pDataMeta);
        if (!data_.insert(name, pData).second)
        {
            ARK_DELETE(pData);
            return nullptr;
        }
    }

    return pData;
}

bool AFCRowData::OnRecordCallBack(
    const std::string& name, const AFIDataNewType& old_data, const AFIDataNewType& new_data)
{
    ARK_ASSERT_RET_VAL(record_ex_ != nullptr, false);

    record_ex_->OnRowDataChanged(row_, name, old_data, new_data);

    return true;
}

} // namespace ark
