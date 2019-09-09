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

#include "kernel/include/AFCTableInner.hpp"
#include "kernel/include/AFCData.hpp"
#include "kernel/include/AFCRow.hpp"

namespace ark {

// constructor
AFCTableInner::AFCTableInner(
    ARK_SHARE_PTR<AFTableMeta> pTableMeta, ARK_SHARE_PTR<AFClassCallBackManager> pCallBackManager, const AFGUID& guid)
    : guid_(guid)
{
    table_meta_ = pTableMeta;
    m_pCallBackManager = pCallBackManager;
}

AFCTableInner::~AFCTableInner()
{
    Clear();
}

const std::string& AFCTableInner::GetName() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, NULL_STR);

    return table_meta_->GetName();
}

uint32_t AFCTableInner::GetColCount() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, (uint32_t)NULL_INT);

    return table_meta_->GetColCount();
}

ArkDataType AFCTableInner::GetColType(const uint32_t index) const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, ArkDataType::DT_EMPTY);

    return table_meta_->GetColType(index);
}

const AFFeatureType AFCTableInner::GetFeature() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, NULL_INT);

    return table_meta_->GetFeature();
}

bool AFCTableInner::HaveFeature(const ArkTableNodeFeature feature) const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, false);

    return table_meta_->HaveFeature(feature);
}

bool AFCTableInner::IsPublic() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, false);

    return table_meta_->IsPublic();
}

bool AFCTableInner::IsPrivate() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, false);

    return table_meta_->IsPrivate();
}

bool AFCTableInner::IsRealTime() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, false);

    return table_meta_->IsRealTime();
}

bool AFCTableInner::IsSave() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, false);

    return table_meta_->IsSave();
}

// table get
uint32_t AFCTableInner::GetRowCount() const
{
    return data_.size();
}

// table set
AFIRow* AFCTableInner::AddRow(uint32_t row /* = 0u*/)
{
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr && table_meta_ != nullptr, nullptr);

    AFIRow* pRowData = nullptr;
    if (row == 0)
    {
        row = ++current_row;
        pRowData = ARK_NEW AFCRow(this, row);
    }
    else
    {
        pRowData = data_.find_value(row);
        ARK_ASSERT_RET_VAL(pRowData == nullptr, nullptr); // return if found

        if (current_row < row)
        {
            current_row = row;
        }

        pRowData = ARK_NEW AFCRow(this, row);
    }

    ARK_ASSERT_RET_VAL(pRowData != nullptr, nullptr);

    data_.insert(pRowData->GetRow(), pRowData);

    // call back
    OnRowDataChanged(row, ArkTableOpType::TABLE_ADD);

    return pRowData;
}

AFIRow* AFCTableInner::FindRow(uint32_t row)
{
    ARK_ASSERT_RET_VAL(row != 0, nullptr);

    return data_.find_value(row);
}

bool AFCTableInner::RemoveRow(uint32_t row)
{
    ARK_ASSERT_RET_VAL(data_.find_value(row) != nullptr, false);

    // call back
    OnRowDataChanged(row, ArkTableOpType::TABLE_DELETE);

    data_.remove(row);

    return true;
}

void AFCTableInner::Clear()
{
    for (auto iter : data_)
    {
        OnRowDataChanged(iter.first, ArkTableOpType::TABLE_DELETE);
    }

    data_.removeall();
}

// ex interface
bool AFCTableInner::OnRowDataChanged(
    uint32_t row, const uint32_t index, const AFIData& old_data, const AFIData& new_data) const
{
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr || table_meta_ != nullptr, false);

    TABLE_EVENT_DATA xData;
    xData.table_name_ = table_meta_->GetName();
    xData.table_index_ = table_meta_->GetIndex();
    xData.data_index_ = index;
    xData.row_ = row;
    xData.op_type_ = (size_t)ArkTableOpType::TABLE_UPDATE;

    m_pCallBackManager->OnTableCallBack(guid_, xData, old_data, new_data);

    return true;
}

ARK_SHARE_PTR<AFTableMeta> AFCTableInner::GetMeta() const
{
    return table_meta_;
}

// find
uint32_t AFCTableInner::FindInt32(const uint32_t index, const int32_t value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetInt32(index) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

uint32_t AFCTableInner::FindInt64(const uint32_t index, const int64_t value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetInt64(index) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

uint32_t AFCTableInner::FindBool(const uint32_t index, bool value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetBool(index) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

uint32_t AFCTableInner::FindFloat(const uint32_t index, float value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetFloat(index) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

uint32_t AFCTableInner::FindDouble(const uint32_t index, double value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetDouble(index) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

uint32_t AFCTableInner::FindString(const uint32_t index, const std::string& value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetString(index) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

uint32_t AFCTableInner::FindWString(const uint32_t index, const std::wstring& value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetWString(index) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

uint32_t AFCTableInner::FindGUID(const uint32_t index, const AFGUID& value)
{
    for (auto iter : data_)
    {
        auto pRowData = iter.second;
        ARK_ASSERT_RET_VAL(pRowData != nullptr, 0u);

        if (pRowData->GetGUID(index) == value)
        {
            return iter.first;
        }
    }

    return 0u;
}

AFIRow* AFCTableInner::First()
{
    iter_ = data_.begin();
    ARK_ASSERT_RET_VAL(iter_ != data_.end(), nullptr);

    return iter_->second;
}

AFIRow* AFCTableInner::Next()
{
    ARK_ASSERT_RET_VAL(iter_ != data_.end(), nullptr);

    iter_++;
    ARK_ASSERT_RET_VAL(iter_ != data_.end(), nullptr);

    return iter_->second;
}

uint32_t AFCTableInner::GetIndex()
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, NULL_INT);

    return table_meta_->GetIndex();
}

uint32_t AFCTableInner::GetIndex(const std::string& name)
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, NULL_INT);

    return table_meta_->GetIndex(name);
}

void AFCTableInner::ReleaseRow(AFIRow* row_data)
{
    ARK_DELETE(row_data);
}

void AFCTableInner::OnRowDataChanged(uint32_t row, ArkTableOpType op_type)
{
    TABLE_EVENT_DATA xData;
    xData.table_name_ = table_meta_->GetName();
    xData.table_index_ = table_meta_->GetIndex();
    xData.row_ = row;
    xData.op_type_ = (size_t)op_type;

    m_pCallBackManager->OnTableCallBack(
        guid_, xData, AFCData(ArkDataType::DT_BOOLEAN, false), AFCData(ArkDataType::DT_BOOLEAN, false));
}

} // namespace ark
