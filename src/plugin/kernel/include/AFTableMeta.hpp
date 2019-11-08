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

#include "base/AFMacros.hpp"
#include "base/AFEnum.hpp"
#include "base/AFDefine.hpp"
#include "base/AFMap.hpp"
#include "AFClassMeta.hpp"

namespace ark {

class AFClassMeta;

class AFTableMeta final
{
public:
    using ColMetaList = AFSmartPtrMap<uint32_t, AFNodeMeta>;

private:
    // table name
    std::string name_{NULL_STR};

    // index
    uint32_t index_{0u};

    // data type name
    std::string type_name_{NULL_STR};

    std::shared_ptr<AFClassMeta> class_meta_{nullptr};

    // mask type
    ArkMaskType mask_;

public:
    AFTableMeta() = delete;

    explicit AFTableMeta(const std::string& name, const uint32_t index);

    virtual ~AFTableMeta() = default;

    const std::string& GetName() const;

    size_t GetColCount() const;

    void SetClassMeta(std::shared_ptr<AFClassMeta> pClassMeta);

    std::shared_ptr<AFNodeMeta> FindMeta(const uint32_t index) const;

    std::shared_ptr<AFClassMeta> GetClassMeta() const;

    ArkDataType GetColType(const uint32_t index) const;

    const ArkMaskType GetMask() const;
    bool HaveMask(const ArkDataMask feature) const;
    void SetMask(const ArkMaskType& feature);

    bool IsPublic() const;
    bool IsPrivate() const;
    bool IsRealTime() const;
    bool IsSave() const;

    void SetTypeName(const std::string& value);
    const std::string& GetTypeName() const;

    uint32_t GetIndex() const;
    uint32_t GetIndex(const std::string& name) const;
};

} // namespace ark
