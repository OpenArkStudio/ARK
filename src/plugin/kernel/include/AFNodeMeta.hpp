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

namespace ark {

class AFNodeMeta final
{
public:
    AFNodeMeta() = delete;

    explicit AFNodeMeta(const std::string& name, const uint32_t index)
        : name_(name)
        , index_(index)
    {
        //
    }

    virtual ~AFNodeMeta() = default;

    const std::string& GetName()
    {
        return name_;
    }

    void SetType(ArkDataType value)
    {
        type_ = value;
    }

    ArkDataType GetType() const
    {
        return type_;
    }

    void SetFeature(AFFeatureType feature)
    {
        feature_ = feature;
    }

    bool HaveFeature(AFNodeFeature feature) const
    {
        return feature_.test((size_t)feature);
    }

    uint32_t GetIndex() const
    {
        return index_;
    }

    AFFeatureType GetFeature() const
    {
        return feature_;
    }

private:
    // data name
    std::string name_{NULL_STR};

    // data type
    ArkDataType type_{ArkDataType::DT_EMPTY};

    //node index
    uint32_t index_{0u};

    // data mask
    AFFeatureType feature_{0};
};

} // namespace ark
