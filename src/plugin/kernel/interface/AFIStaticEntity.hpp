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
#include "base/AFDefine.hpp"
#include "kernel/include/AFNodeManager.hpp"

namespace ark {

class AFIStaticEntity
{
public:
    virtual ~AFIStaticEntity() = default;

    // query data
    virtual const std::string& GetClassName() const = 0;
    virtual ID_TYPE GetConfigID() const = 0;
    virtual bool GetBool(const std::string& name) const = 0;
    virtual int32_t GetInt32(const std::string& name) const = 0;
    virtual uint32_t GetUInt32(const std::string& name) const = 0;
    virtual int64_t GetInt64(const std::string& name) const = 0;
    virtual int64_t GetUInt64(const std::string& name) const = 0;
    virtual float GetFloat(const std::string& name) const = 0;
    virtual double GetDouble(const std::string& name) const = 0;
    virtual const std::string& GetString(const std::string& name) const = 0;
    virtual const std::wstring& GetWString(const std::string& name) const = 0;

    virtual bool GetBool(const uint32_t index) const = 0;
    virtual int32_t GetInt32(const uint32_t index) const = 0;
    virtual uint32_t GetUInt32(const uint32_t index) const = 0;
    virtual int64_t GetInt64(const uint32_t index) const = 0;
    virtual int64_t GetUInt64(const uint32_t index) const = 0;
    virtual float GetFloat(const uint32_t index) const = 0;
    virtual double GetDouble(const uint32_t index) const = 0;
    virtual const std::string& GetString(const uint32_t index) const = 0;
    virtual const std::wstring& GetWString(const uint32_t index) const = 0;
};

} // namespace ark
