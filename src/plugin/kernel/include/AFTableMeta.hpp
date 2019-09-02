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
#include "AFNodeMeta.hpp"

namespace ark {

class AFTableMeta final
{
public:
    using ColMetaList = AFNewSmartPtrMap<uint32_t, AFNodeMeta>;

    AFTableMeta() = delete;

    explicit AFTableMeta(const std::string& name, const uint32_t index)
        : name_(name)
        , index_(index)
    {
        // todo
    }

    virtual ~AFTableMeta()
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

    bool AddColMeta(const uint32_t index, ARK_SHARE_PTR<AFNodeMeta> pMeta)
    {
        ARK_ASSERT_RET_VAL(index > 0 && pMeta != nullptr, false);

        ARK_ASSERT_RET_VAL(col_meta_list_.find_value(index) == nullptr, false);

        ARK_ASSERT_RET_VAL(name_index_list_.find(pMeta->GetName()) == name_index_list_.end(), false);

        name_index_list_.insert(std::make_pair(pMeta->GetName(), index));
        return col_meta_list_.insert(index, pMeta).second;
    }

    ARK_SHARE_PTR<AFNodeMeta> FindMeta(const uint32_t index)
    {
        return col_meta_list_.find_value(index);
    }

    ArkDataType GetColType(const uint32_t index)
    {
        auto pMeta = col_meta_list_.find_value(index);
        ARK_ASSERT_RET_VAL(pMeta != nullptr, ArkDataType::DT_EMPTY);

        return pMeta->GetType();
    }

    const AFFeatureType GetFeature() const
    {
        return feature_;
    }

    bool HaveFeature(const ArkTableNodeFeature feature) const
    {
        return feature_.test((size_t)feature);
    }

    void SetFeature(const AFFeatureType& feature)
    {
        feature_ = feature;
    }

    bool IsPublic() const
    {
        return feature_.test((size_t)ArkTableNodeFeature::PF_PUBLIC);
    }

    bool IsPrivate() const
    {
        return feature_.test((size_t)ArkTableNodeFeature::PF_PRIVATE);
    }

    bool IsRealTime() const
    {
        return feature_.test((size_t)ArkTableNodeFeature::PF_REAL_TIME);
    }

    bool IsSave() const
    {
        return feature_.test((size_t)ArkTableNodeFeature::PF_SAVE);
    }

    void SetTypeName(const std::string& value)
    {
        type_name_ = value;
    }

    const std::string& GetTypeName() const
    {
        return type_name_;
    }

    uint32_t GetIndex() const
    {
        return index_;
    }

    uint32_t GetIndex(const std::string& name)
    {
        auto iter = name_index_list_.find(name);
        if (iter == name_index_list_.end())
        {
            return NULL_INT;
        }

        return iter->second;
    }

private:
    // table name
    std::string name_{NULL_STR};

    // index
    uint32_t index_{0u};

    // data type name
    std::string type_name_{NULL_STR};

    // name index list
    using NameIndexList = std::unordered_map<std::string, uint32_t>;
    NameIndexList name_index_list_;

    // table col type
    ColMetaList col_meta_list_;

    // feature type
    AFFeatureType feature_;
};

} // namespace ark
