/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

namespace ark {

class AFContainerMeta final
{
private:
    // container name
    std::string name_{NULL_STR};

    // container index
    uint32_t index_{0u};

    // container's class name
    std::string class_name_{NULL_STR};

    // mask
    ArkMaskType mask_{0};

public:
    AFContainerMeta() = delete;

    explicit AFContainerMeta(
        const std::string& name, const uint32_t index, const std::string& class_name, const ArkMaskType mask)
        : name_(name)
        , index_(index)
        , class_name_(class_name)
        , mask_(mask)
    {
    }

    virtual ~AFContainerMeta() = default;

    const std::string& GetName() const
    {
        return name_;
    }

    const std::string& GetClassName() const
    {
        return class_name_;
    }

    uint32_t GetIndex() const
    {
        return index_;
    }

    const ArkMaskType GetMask() const
    {
        return mask_;
    }
};

} // namespace ark
