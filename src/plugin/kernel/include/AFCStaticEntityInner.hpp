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

#include "kernel/interface/AFIStaticEntityInner.hpp"
#include "base/AFMap.hpp"

namespace ark {

class AFCStaticEntityInner final : public AFIStaticEntityInner
{
public:
    using TableList = AFNewMap<uint32_t, AFITable>;

    AFCStaticEntityInner() = delete;

    explicit AFCStaticEntityInner(ARK_SHARE_PTR<AFClassMeta> pClassMeta, const ID_TYPE_ARG config_id);

    ~AFCStaticEntityInner() override;

    uint32_t GetIndex(const std::string& name) override;

    // query data
    const std::string& GetClassName() const override;
    const ID_TYPE_ARG GetConfigID() const override;
    bool GetBool(const std::string& name) override;
    int32_t GetInt32(const std::string& name) override;
    uint32_t GetUInt32(const std::string& name) override;
    int64_t GetInt64(const std::string& name) override;
    float GetFloat(const std::string& name) override;
    double GetDouble(const std::string& name) override;
    const std::string& GetString(const std::string& name) override;
    const std::wstring& GetWString(const std::string& name) override;

    bool GetBool(const uint32_t index) override;
    int32_t GetInt32(const uint32_t index) override;
    uint32_t GetUInt32(const uint32_t index) override;
    int64_t GetInt64(const uint32_t index) override;
    float GetFloat(const uint32_t index) override;
    double GetDouble(const uint32_t index) override;
    const std::string& GetString(const uint32_t index) override;
    const std::wstring& GetWString(const uint32_t index) override;

    // data operation
    AFINode* CreateData(ARK_SHARE_PTR<AFNodeMeta> pDataMeta) override;
    AFINode* FindData(const uint32_t index, bool bCreate = false) override;

    // table meta operation
    AFITable* AddTable(const AFGUID& guid, const uint32_t index) override;

    AFITable* FindTable(const uint32_t index) override;

    void Reset() override;
    bool IsEmpty() const override;
    const DataList& GetDataList() override;
    ARK_SHARE_PTR<AFClassMeta> GetClassMeta() override;

private:
    // class meta
    ARK_SHARE_PTR<AFClassMeta> class_meta_{nullptr};

    // config id
    ID_TYPE config_id_{0};

    // data list
    DataList data_list_;

    // table list
    TableList table_list_;
};

} // namespace ark
