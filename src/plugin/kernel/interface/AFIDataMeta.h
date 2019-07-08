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

#include "base/AFPlatform.hpp"
#include "base/AFEnum.hpp"

namespace ark {

class AFIDataMeta
{
public:
    void SetName(const std::string &value)
    {
        name_ = value;
    }

    const std::string &GetName() const
    {
        return name_;
    }

    void SetType(const ArkDataType value)
    {
        type_ = value;
    }

    const ArkDataType GetType() const
    {
        return type_;
    }
    //////////////////////////////////////////////////////////////////////////
    virtual bool HaveMask(const ArkDataMaskType type) const
    {
        return false;
    }

    virtual void SetInitValue(const std::string &value)
    {
        // Do nothing in base class
    }

    virtual const std::string &GetInitValue() const
    {
        return NULL_STR;
    }

    virtual void SetSubMetaClass(const std::string &value)
    {
        // Do nothing in base class
    }

    virtual const std::string &GetSubMetaClass() const
    {
        return NULL_STR;
    }
    //////////////////////////////////////////////////////////////////////////
    virtual void SetReferClassMeta(const std::string &value)
    {
        // Do nothing in base class
    }

    virtual const std::string &GetReferClassMeta() const
    {
        return NULL_STR;
    }

private:
    std::string name_{};
    ArkDataType type_{DT_EMPTY};
};

} // namespace ark