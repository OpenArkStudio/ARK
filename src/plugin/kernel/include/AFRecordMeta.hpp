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
#include "base/AFArrayPod.hpp"
#include "base/AFMap.hpp"
#include "AFDataNewMeta.hpp"

namespace ark {

class AFRecordMeta final
{
public:
    using ColMetaList = AFNewSmartPtrHashmap<std::string, AFDataNewMeta>;

    AFRecordMeta() = delete;

    explicit AFRecordMeta(const std::string& name)
        : name_(name)
    {
        // todo
    }

    virtual ~AFRecordMeta()
    {
        col_meta_list_.clear();
    }

    const std::string& GetName() const
    {
        return name_;
    }

    size_t GetColCount() const
    {
        return col_meta_list_.size();
    }

    bool AddColMeta(const std::string& name, ARK_SHARE_PTR<AFDataNewMeta> pMeta)
    {
        ARK_ASSERT_RET_VAL(pMeta != nullptr, false);

        return col_meta_list_.insert(name, pMeta).second;
    }

    ARK_SHARE_PTR<AFDataNewMeta> FindMeta(const std::string& name)
    {
        return col_meta_list_.find_value(name);
    }

    const ArkDataType GetColType(const std::string& name)
    {
        auto pMeta = col_meta_list_.find_value(name);
        ARK_ASSERT_RET_VAL(pMeta != nullptr, ArkDataType::DT_EMPTY);

        return pMeta->GetType();
    }

    const AFFeatureType GetFeatureType() const
    {
        return feature_;
    }

    void SetFeatureType(const AFFeatureType& feature)
    {
        feature_ = feature;
    }

    bool IsPublic() const
    {
        return feature_.test(ArkTableNodeFeature::PF_PUBLIC);
    }

    bool IsPrivate() const
    {
        return feature_.test(ArkTableNodeFeature::PF_PRIVATE);
    }

    bool IsRealTime() const
    {
        return feature_.test(ArkTableNodeFeature::PF_REAL_TIME);
    }

    bool IsSave() const
    {
        return feature_.test(ArkTableNodeFeature::PF_SAVE);
    }

    void SetTypeName(const std::string& value)
    {
        type_name_ = value;
    }

    const std::string& GetTypeName() const
    {
        return type_name_;
    }

private:
    // record name
    std::string name_{NULL_STR};

    // data type name
    std::string type_name_{NULL_STR};

    // record col type
    ColMetaList col_meta_list_;

    // feature type
    AFFeatureType feature_;
};

} // namespace ark
