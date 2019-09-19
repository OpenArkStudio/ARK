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

#include "kernel/include/AFCTable.hpp"
#include "kernel/include/AFCData.hpp"
#include "kernel/include/AFCRow.hpp"
#include "kernel/include/AFCDataList.hpp"

namespace ark {

// constructor
AFCTable::AFCTable(
    ARK_SHARE_PTR<AFTableMeta> pTableMeta, ARK_SHARE_PTR<AFClassCallBackManager> pCallBackManager, const AFGUID& guid)
    : guid_(guid)
{
    table_meta_ = pTableMeta;
    m_pCallBackManager = pCallBackManager;
}

AFCTable::~AFCTable()
{
    Clear();
}

const std::string& AFCTable::GetName() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, NULL_STR);

    return table_meta_->GetName();
}

uint32_t AFCTable::GetColCount() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, (uint32_t)NULL_INT);

    return table_meta_->GetColCount();
}

ArkDataType AFCTable::GetColType(const uint32_t index) const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, ArkDataType::DT_EMPTY);

    return table_meta_->GetColType(index);
}

const AFFeatureType AFCTable::GetFeature() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, NULL_INT);

    return table_meta_->GetFeature();
}

bool AFCTable::HaveFeature(const ArkTableNodeFeature feature) const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, false);

    return table_meta_->HaveFeature(feature);
}

bool AFCTable::IsPublic() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, false);

    return table_meta_->IsPublic();
}

bool AFCTable::IsPrivate() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, false);

    return table_meta_->IsPrivate();
}

bool AFCTable::IsRealTime() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, false);

    return table_meta_->IsRealTime();
}

bool AFCTable::IsSave() const
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, false);

    return table_meta_->IsSave();
}

// table get
uint32_t AFCTable::GetRowCount() const
{
    return data_.size();
}

// table set
AFIRow* AFCTable::AddRow(uint32_t row /* = 0u*/)
{
    return AddRow(row, AFCDataList());
}

AFIRow* AFCTable::AddRow(uint32_t row, const AFIDataList& args)
{
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr, nullptr);

    ARK_ASSERT_RET_VAL(args.GetCount() % 2 == 0, nullptr);

    AFIRow* pRowData = nullptr;
    if (row == 0)
    {
        row = ++current_row;
        pRowData = CreateRow(row, args);
    }
    else
    {
        pRowData = data_.find_value(row);
        ARK_ASSERT_RET_VAL(pRowData == nullptr, nullptr); // return if found

        if (current_row < row)
        {
            current_row = row;
        }

        pRowData = CreateRow(row, args);
    }

    // call back
    OnTableChanged(row, ArkTableOpType::TABLE_ADD);

    return pRowData;
}

AFIRow* AFCTable::FindRow(uint32_t row)
{
    ARK_ASSERT_RET_VAL(row != 0, nullptr);

    return data_.find_value(row);
}

bool AFCTable::RemoveRow(uint32_t row)
{
    ARK_ASSERT_RET_VAL(data_.find_value(row) != nullptr, false);

    // call back
    OnTableChanged(row, ArkTableOpType::TABLE_DELETE);

    data_.erase(row);

    return true;
}

void AFCTable::Clear()
{
    OnTableChanged(0u, ArkTableOpType::TABLE_CLEAR);
}

// find
uint32_t AFCTable::FindInt32(const uint32_t index, const int32_t value)
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

uint32_t AFCTable::FindInt64(const uint32_t index, const int64_t value)
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

uint32_t AFCTable::FindBool(const uint32_t index, bool value)
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

uint32_t AFCTable::FindFloat(const uint32_t index, float value)
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

uint32_t AFCTable::FindDouble(const uint32_t index, double value)
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

uint32_t AFCTable::FindString(const uint32_t index, const std::string& value)
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

uint32_t AFCTable::FindWString(const uint32_t index, const std::wstring& value)
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

uint32_t AFCTable::FindGUID(const uint32_t index, const AFGUID& value)
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

AFIRow* AFCTable::First()
{
    iter_ = data_.begin();
    ARK_ASSERT_RET_VAL(iter_ != data_.end(), nullptr);

    return iter_->second;
}

AFIRow* AFCTable::Next()
{
    ARK_ASSERT_RET_VAL(iter_ != data_.end(), nullptr);

    iter_++;
    ARK_ASSERT_RET_VAL(iter_ != data_.end(), nullptr);

    return iter_->second;
}

uint32_t AFCTable::GetIndex()
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, NULL_INT);

    return table_meta_->GetIndex();
}

uint32_t AFCTable::GetIndex(const std::string& name)
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, NULL_INT);

    return table_meta_->GetIndex(name);
}

void AFCTable::ReleaseRow(AFIRow* row_data)
{
    ARK_DELETE(row_data);
}

void AFCTable::OnTableChanged(uint32_t row, ArkTableOpType op_type)
{
    TABLE_EVENT_DATA xData;
    xData.table_name_ = table_meta_->GetName();
    xData.table_index_ = table_meta_->GetIndex();
    xData.row_ = row;
    xData.op_type_ = (size_t)op_type;

    m_pCallBackManager->OnTableCallBack(
        guid_, xData, AFCData(ArkDataType::DT_BOOLEAN, false), AFCData(ArkDataType::DT_BOOLEAN, false));
}

int AFCTable::OnRowDataChanged(uint32_t row, const uint32_t index, const AFIData& old_data, const AFIData& new_data)
{
    ARK_ASSERT_RET_VAL(m_pCallBackManager != nullptr || table_meta_ != nullptr, false);

    TABLE_EVENT_DATA xData;
    xData.table_name_ = table_meta_->GetName();
    xData.table_index_ = table_meta_->GetIndex();
    xData.data_index_ = index;
    xData.row_ = row;
    xData.op_type_ = (size_t)ArkTableOpType::TABLE_UPDATE;

    m_pCallBackManager->OnTableCallBack(guid_, xData, old_data, new_data);

    return 0;
}

AFIRow* AFCTable::CreateRow(uint32_t row, const AFIDataList& args)
{
    ARK_ASSERT_RET_VAL(table_meta_ != nullptr, nullptr);

    auto pClassMeta = table_meta_->GetClassMeta();
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, nullptr);

    auto func = std::bind(&AFCTable::OnRowDataChanged, this, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3, std::placeholders::_4);

    return ARK_NEW AFCRow(pClassMeta, row, args, std::move(func));
}

} // namespace ark
