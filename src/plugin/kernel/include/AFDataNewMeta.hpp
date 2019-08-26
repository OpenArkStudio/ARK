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

class AFDataNewMeta final
{
public:
    using AFDataMask = std::bitset<16>;

    AFDataNewMeta() = delete;

    explicit AFDataNewMeta(const std::string& name)
        : name_(name)
    {
        //
    }

    virtual ~AFDataNewMeta() = default;

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

    void SetMask(ArkDataMaskType value)
    {
        mask_.set((size_t)value, true);
    }

    bool HaveMask(ArkDataMaskType mask) const
    {
        return mask_.test((size_t)mask);
    }

private:
    // data name
    std::string name_{NULL_STR};

    // data type
    ArkDataType type_{ArkDataType::DT_EMPTY};

    // data mask
    AFDataMask mask_{0};
};

} // namespace ark
