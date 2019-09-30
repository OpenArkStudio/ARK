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

#include "base/AFMap.hpp"
#include "kernel/interface/AFIStaticEntity.hpp"
#include "kernel/interface/AFITable.hpp"
#include "AFCEntity.hpp"

namespace ark {

class AFCStaticEntity final : public AFIStaticEntity
{
public:
    using TableList = AFMap<uint32_t, AFITable>;

private:
    friend class AFCEntity;

    // config id
    ID_TYPE config_id_{0};

    // class meta
    ARK_SHARE_PTR<AFClassMeta> class_meta_{nullptr};

    // data
    ARK_SHARE_PTR<AFNodeManager> m_pNodeManager{nullptr};

public:
    AFCStaticEntity() = delete;

    explicit AFCStaticEntity(
        ARK_SHARE_PTR<AFClassMeta> pClassMeta, ARK_SHARE_PTR<AFNodeManager> pNodeManager, const ID_TYPE config_id);

    // query data
    const std::string& GetClassName() const override;
    ID_TYPE GetConfigID() const override;
    bool GetBool(const std::string& name) const override;
    int32_t GetInt32(const std::string& name) const override;
    uint32_t GetUInt32(const std::string& name) const override;
    int64_t GetInt64(const std::string& name) const override;
    int64_t GetUInt64(const std::string& name) const override;
    float GetFloat(const std::string& name) const override;
    double GetDouble(const std::string& name) const override;
    const std::string& GetString(const std::string& name) const override;
    const std::wstring& GetWString(const std::string& name) const override;

    bool GetBool(const uint32_t index) const override;
    int32_t GetInt32(const uint32_t index) const override;
    uint32_t GetUInt32(const uint32_t index) const override;
    int64_t GetInt64(const uint32_t index) const override;
    int64_t GetUInt64(const uint32_t index) const override;
    float GetFloat(const uint32_t index) const override;
    double GetDouble(const uint32_t index) const override;
    const std::string& GetString(const uint32_t index) const override;
    const std::wstring& GetWString(const uint32_t index) const override;
};

} // namespace ark
