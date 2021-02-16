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

#include "kernel/interface/AFIData.hpp"
#include "base/AFMacros.hpp"
#include "base/container/AFMap.hpp"
#include "kernel/interface/AFITable.hpp"
#include "AFTableMeta.hpp"

namespace ark {

class AFCTable final : public AFITable
{
public:
    using TableData = AFMap<uint32_t, AFIRow>;
    using TABLE_CALLBACK_FUNCTOR =
        std::function<int(const ArkMaskType, AFINode*, const TABLE_EVENT_DATA&, const AFIData&, const AFIData&)>;

private:
    friend class AFCKernelModule;

    // table meta
    std::shared_ptr<AFTableMeta> table_meta_{nullptr};

    // current row
    uint32_t current_row_{0};

    // table data
    TableData data_;

    // table iterator
    TableData::const_iterator iter_;

    // call back
    TABLE_CALLBACK_FUNCTOR func_;

public:
    AFCTable() = delete;

    // constructor
    explicit AFCTable(std::shared_ptr<AFTableMeta> pTableMeta, TABLE_CALLBACK_FUNCTOR&& func);

    const std::string& GetName() const override;

    uint32_t GetColCount() const override;

    ArkDataType GetColType(const uint32_t index) const override;

    const ArkMaskType GetMask() const override;
    bool HaveMask(const ArkDataMask mask) const override;
    bool IsPublic() const override;
    bool IsPrivate() const override;
    bool IsRealTime() const override;
    bool IsSave() const override;

    // table get
    uint32_t GetRowCount() const override;

    // table set
    AFIRow* AddRow(uint32_t row = 0u) override;
    AFIRow* AddRow(uint32_t row, const AFIDataList& args) override;
    AFIRow* FindRow(uint32_t row) const override;

    bool RemoveRow(uint32_t row) override;

    void Clear() override;

    // find
    uint32_t FindInt32(const uint32_t index, const int32_t value) const override;
    uint32_t FindInt64(const uint32_t index, const int64_t value) const override;
    uint32_t FindBool(const uint32_t index, bool value) const override;
    uint32_t FindFloat(const uint32_t index, float value) const override;
    uint32_t FindDouble(const uint32_t index, double value) const override;
    uint32_t FindString(const uint32_t index, const std::string& value) const override;
    uint32_t FindWString(const uint32_t index, const std::wstring& value) const override;
    uint32_t FindGUID(const uint32_t index, const guid_t& value) const override;

    AFIRow* First() override;
    AFIRow* Next() override;

    uint32_t GetIndex() const override;
    uint32_t GetIndex(const std::string& name) const override;

private:
    uint32_t SelectRow() const;

    void ReleaseRow(AFIRow* row_data);

    void OnTableChanged(uint32_t row, ArkTableOpType op_type);

    int OnRowChanged(uint32_t row, AFINode* pNode, const AFIData& old_data, const AFIData& new_data);

    AFIRow* CreateRow(uint32_t row, const AFIDataList& args);
};

} // namespace ark
