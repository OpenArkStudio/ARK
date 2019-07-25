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

#include "interface/AFIDataMeta.h"

namespace ark {

class AFCEntityDataMeta : public AFIDataMeta
{
public:
    using AFDataMask = std::bitset<16>;

    bool HaveMask(const ArkDataMaskType type) const override
    {
        return mask_.test(type);
    }

    void SetInitValue(const std::string& value) override
    {
        init_value_ = value;
    }

    const std::string& GetInitValue() const override
    {
        return init_value_;
    }

    void SetSubMetaClass(const std::string& value) override
    {
        sub_meta_class_ = value;
    }

    const std::string& GetSubMetaClass() const override
    {
        return sub_meta_class_;
    }

private:
    std::string init_value_{};
    std::string sub_meta_class_{};
    AFDataMask mask_{0};
    // TODO:other
};

class AFCConfigDataMeta : public AFIDataMeta
{
public:
    void SetReferClassMeta(const std::string& value) override
    {
        refer_class_meta_ = value;
    }

    const std::string& GetReferClassMeta() const override
    {
        return refer_class_meta_;
    }

private:
    std::string refer_class_meta_{};
};

} // namespace ark