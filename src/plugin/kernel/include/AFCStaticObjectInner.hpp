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

#include "kernel/interface/AFIStaticObjectInner.hpp"
#include "base/AFMap.hpp"

namespace ark {

class AFCStaticObjectInner final : public AFIStaticObjectInner
{
public:
    using RecordList = AFNewHashmap<std::string, AFIRecord>;

    AFCStaticObjectInner() = delete;

    explicit AFCStaticObjectInner(ARK_SHARE_PTR<AFClassMeta> pClassMeta, uint64_t config_id);

    ~AFCStaticObjectInner() override;

    // query data
    const std::string& GetClassName() const override;
    uint64_t GetConfigID() const override;
    bool GetBool(const std::string& name) override;
    int32_t GetInt32(const std::string& name) override;
    uint32_t GetUInt32(const std::string& name) override;
    int64_t GetInt64(const std::string& name) override;
    float GetFloat(const std::string& name) override;
    double GetDouble(const std::string& name) override;
    const std::string& GetString(const std::string& name) override;
    const std::wstring& GetWString(const std::string& name) override;

    // data operation
    AFIDataNew* CreateData(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta) override;
    AFIDataNew* FindData(const std::string& name, bool bCreate = false) override;

    // record meta operation
    AFIRecord* AddRecord(const AFGUID& guid, const std::string& name) override;
    AFIRecord* FindRecord(const std::string& name) override;
    void Reset() override;
    bool IsEmpty() const override;
    const DataList& GetDataList() override;
    ARK_SHARE_PTR<AFClassMeta> GetClassMeta() override;

private:
    // class meta
    ARK_SHARE_PTR<AFClassMeta> class_meta_{nullptr};

    // config id
    uint64_t config_id_{NULL_INT64};

    // data list
    DataList data_list_;

    // record list
    RecordList record_list_;
};

} // namespace ark
