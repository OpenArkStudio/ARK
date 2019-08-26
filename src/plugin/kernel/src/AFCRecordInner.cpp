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

#include "kernel/include/AFCRecordInner.hpp"
#include "kernel/include/AFCDataNewType.hpp"
#include "kernel/include/AFCRowData.hpp"

namespace ark {

// constructor
AFCRecordInner::AFCRecordInner(
    ARK_SHARE_PTR<AFRecordMeta> pRecordMeta, ARK_SHARE_PTR<AFClassCallBackManager> pCallBackManager, const AFGUID& guid)
    : guid_(guid_)
{
    record_meta_ = pRecordMeta;
    m_pCallBackManager = pCallBackManager;
}

AFCRecordInner::~AFCRecordInner()
{
    Clear();
}

const std::string& AFCRecordInner::GetName() const
{
    ARK_ASSERT_RET_VAL(record_meta_ != nullptr, NULL_STR);

    return record_meta_->GetName();
}

const size_t AFCRecordInner::GetColCount() const
{
    ARK_ASSERT_RET_VAL(record_meta_ != nullptr, (size_t)NULL_INT);

    return record_meta_->GetColCount();
}

const ArkDataType AFCRecordInner::GetColType(const std::string& name) const
{
    ARK_ASSERT_RET_VAL(record_meta_ != nullptr, ArkDataType::DT_EMPTY);

    return record_meta_->GetColType(name);
}

const AFFeatureType AFCRecordInner::GetFeatureType() const
{
    ARK_ASSERT_RET_VAL(record_meta_ != nullptr, NULL_INT);

    return record_meta_->GetFeatureType();
}

bool AFCRecordInner::IsPublic() const
{
    ARK_ASSERT_RET_VAL(record_meta_ != nullptr, false);

    return record_meta_->IsPublic();
}

bool AFCRecordInner::IsPrivate() const
{
    ARK_ASSERT_RET_VAL(record_meta_ != nullptr, false);

    return record_meta_->IsPrivate();
}

bool AFCRecordInner::IsRealTime() const
{
    ARK_ASSERT_RET_VAL(record_meta_ != nullptr, false);

    return record_meta_->IsRealTime();
}

bool AFCRecordInner::IsSave() const
{
    ARK_ASSERT_RET_VAL(record_meta_ != nullptr, false);

    return record_meta_->IsSave();
}

// record get
size_t AFCRecordInner::GetRowCount() const
{
    return data_.size();
}

// record set
AFIRowData* AFCRecordInner::AddRow(size_t row)
{
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr && record_meta_ != nullptr, nullptr);

    AFIRowData* pRowData = nullptr;
    if (row == 0)
    {
        row = ++current_row;
        pRowData = ARK_NEW AFCRowData(this, row);
    }
    else
    {
        pRowData = data_.find_value(row);
        ARK_ASSERT_RET_VAL(pRowData == nullptr, pRowData); // return if found

        if (current_row < row)
        {
            current_row = row;
        }

        pRowData = ARK_NEW AFCRowData(this, row);
    }

    ARK_ASSERT_RET_VAL(pRowData != nullptr, nullptr);

    data_.insert(pRowData->GetRow(), pRowData);

    // call back
    OnRowDataChanged(row, ArkTableOpType::TABLE_ADD);

    return pRowData;
}

AFIRowData* AFCRecordInner::AddRow(size_t row, const AFIDataList& data)
{
    // todo
    // todo: add event handler

    return nullptr;
}

AFIRowData* AFCRecordInner::FindRow(size_t row)
{
    ARK_ASSERT_RET_VAL(row != 0, nullptr);

    return data_.find_value(row);
}

bool AFCRecordInner::RemoveRow(size_t row)
{
    ARK_ASSERT_RET_VAL(data_.find_value(row) != nullptr, false);

    // call back
    OnRowDataChanged(row, ArkTableOpType::TABLE_DELETE);

    data_.remove(row);

    return true;
}

void AFCRecordInner::Clear()
{
    for (auto iter : data_)
    {
        OnRowDataChanged(iter.first, ArkTableOpType::TABLE_DELETE);
    }

    data_.removeall();
}

// ex interface
bool AFCRecordInner::OnRowDataChanged(
    size_t row, const std::string& name, const AFIDataNewType& old_data, const AFIDataNewType& new_data) const
{
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr || record_meta_ != nullptr, false);

    DATA_RECORD_EVENT_DATA xData;
    xData.record_name_ = record_meta_->GetName();
    xData.data_name_ = name;
    xData.row_ = row;
    xData.op_type_ = ArkTableOpType::TABLE_UPDATE;

    m_pCallBackManager->OnRecordCallBack(guid_, xData, old_data, new_data);

    return true;
}

ARK_SHARE_PTR<AFRecordMeta> AFCRecordInner::GetMeta() const
{
    return record_meta_;
}

// find
size_t AFCRecordInner::FindInt32(const std::string& name, const int32_t value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetInt32(name) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

size_t AFCRecordInner::FindInt64(const std::string& name, const int64_t value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetInt64(name) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

size_t AFCRecordInner::FindBool(const std::string& name, bool value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetBool(name) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

size_t AFCRecordInner::FindFloat(const std::string& name, float value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetFloat(name) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

size_t AFCRecordInner::FindDouble(const std::string& name, double value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetDouble(name) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

size_t AFCRecordInner::FindString(const std::string& name, const std::string& value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetString(name) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

size_t AFCRecordInner::FindWString(const std::string& name, const std::wstring& value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetWString(name) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

size_t AFCRecordInner::FindObject(const std::string& name, const AFGUID& value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetObject(name) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

void AFCRecordInner::ReleaseRow(AFIRowData* row_data)
{
    ARK_DELETE(row_data);
}

void AFCRecordInner::OnRowDataChanged(size_t row, ArkTableOpType op_type)
{
    DATA_RECORD_EVENT_DATA xData;
    xData.record_name_ = record_meta_->GetName();
    xData.data_name_ = NULL_STR;
    xData.row_ = row;
    xData.op_type_ = op_type;

    m_pCallBackManager->OnRecordCallBack(
        guid_, xData, AFCDataNewType(ArkDataType::DT_BOOLEAN, false), AFCDataNewType(ArkDataType::DT_BOOLEAN, false));
}

} // namespace ark
