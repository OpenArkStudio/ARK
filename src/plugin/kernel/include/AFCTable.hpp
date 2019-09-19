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

#include "kernel/interface/AFIData.hpp"
#include "base/AFMacros.hpp"
#include "base/AFMap.hpp"
#include "AFClassCallBackManager.hpp"
#include "kernel/interface/AFITable.hpp"
#include "AFTableMeta.hpp"

namespace ark {

class AFCTable final : public AFITable
{
public:
    using TableData = AFMap<uint32_t, AFIRow>;

private:
    // object unique guid
    AFGUID guid_{NULL_GUID};

    // table meta
    ARK_SHARE_PTR<AFTableMeta> table_meta_{nullptr};

    // current row
    uint32_t current_row{0u};

    // table data
    TableData data_;

    // table iterator
    TableData::const_iterator iter_;

    // call back
    ARK_SHARE_PTR<AFClassCallBackManager> m_pCallBackManager{nullptr};

public:
    AFCTable() = delete;

    // constructor
    explicit AFCTable(ARK_SHARE_PTR<AFTableMeta> pTableMeta, ARK_SHARE_PTR<AFClassCallBackManager> pCallBackManager,
        const AFGUID& guid);

    ~AFCTable() override;

    const std::string& GetName() const override;

    uint32_t GetColCount() const override;

    ArkDataType GetColType(const uint32_t index) const override;

    const ArkMaskType GetMask() const override;
    bool HaveMask(const ArkTableNodeMask mask) const override;
    bool IsPublic() const override;
    bool IsPrivate() const override;
    bool IsRealTime() const override;
    bool IsSave() const override;

    // table get
    uint32_t GetRowCount() const override;

    // table set
    AFIRow* AddRow(uint32_t row = 0u) override;
    AFIRow* AddRow(uint32_t row, const AFIDataList& args) override;
    AFIRow* FindRow(uint32_t row) override;

    bool RemoveRow(uint32_t row) override;

    void Clear() override;

    // find
    uint32_t FindInt32(const uint32_t index, const int32_t value) override;
    uint32_t FindInt64(const uint32_t index, const int64_t value) override;
    uint32_t FindBool(const uint32_t index, bool value) override;
    uint32_t FindFloat(const uint32_t index, float value) override;
    uint32_t FindDouble(const uint32_t index, double value) override;
    uint32_t FindString(const uint32_t index, const std::string& value) override;
    uint32_t FindWString(const uint32_t index, const std::wstring& value) override;
    uint32_t FindGUID(const uint32_t index, const AFGUID& value) override;

    AFIRow* First() override;
    AFIRow* Next() override;

    uint32_t GetIndex() override;
    uint32_t GetIndex(const std::string& name) override;

private:
    void ReleaseRow(AFIRow* row_data);

    void OnTableChanged(uint32_t row, ArkTableOpType op_type);

    // ex interface
    int OnRowDataChanged(uint32_t row, const uint32_t index, const AFIData& old_data, const AFIData& new_data);

    AFIRow* CreateRow(uint32_t row, const AFIDataList& args);
};

} // namespace ark
