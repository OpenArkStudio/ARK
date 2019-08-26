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
#include "AFClassCallBackManager.hpp"

namespace ark {

class AFCRecordInner final : public AFIRecordInner
{
public:
    using RecordData = AFNewMap<size_t, AFIRowData>;

    AFCRecordInner() = delete;

    // constructor
    explicit AFCRecordInner(ARK_SHARE_PTR<AFRecordMeta> pRecordMeta,
        ARK_SHARE_PTR<AFClassCallBackManager> pCallBackManager, const AFGUID& guid);

    ~AFCRecordInner() override;

    const std::string& GetName() const override;

    const size_t GetColCount() const override;

    const ArkDataType GetColType(const std::string& name) const override;

    const AFFeatureType GetFeatureType() const override;
    bool IsPublic() const override;
    bool IsPrivate() const override;
    bool IsRealTime() const override;
    bool IsSave() const override;

    // record get
    size_t GetRowCount() const override;

    // record set
    AFIRowData* AddRow(size_t row) override;
    AFIRowData* AddRow(size_t row, const AFIDataList& data) override;
    AFIRowData* FindRow(size_t row) override;

    bool RemoveRow(size_t row) override;

    void Clear() override;

    // ex interface
    bool OnRowDataChanged(size_t row, const std::string& name, const AFIDataNewType& old_data,
        const AFIDataNewType& new_data) const override;

    ARK_SHARE_PTR<AFRecordMeta> GetMeta() const override;

    // find
    size_t FindInt32(const std::string& name, const int32_t value) override;
    size_t FindInt64(const std::string& name, const int64_t value) override;
    size_t FindBool(const std::string& name, bool value) override;
    size_t FindFloat(const std::string& name, float value) override;
    size_t FindDouble(const std::string& name, double value) override;
    size_t FindString(const std::string& name, const std::string& value) override;
    size_t FindWString(const std::string& name, const std::wstring& value) override;
    size_t FindObject(const std::string& name, const AFGUID& value) override;

private:
    void ReleaseRow(AFIRowData* row_data);

    void OnRowDataChanged(size_t row, ArkTableOpType op_type);

private:
    // object unique guid
    AFGUID guid_{NULL_GUID};

    // record meta
    ARK_SHARE_PTR<AFRecordMeta> record_meta_{nullptr};

    // current row
    size_t current_row{0u};

    // record data
    RecordData data_;

    // call back
    ARK_SHARE_PTR<AFClassCallBackManager> m_pCallBackManager{nullptr};
};

} // namespace ark
