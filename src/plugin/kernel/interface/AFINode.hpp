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

#include "base/AFPlatform.hpp"
#include "kernel/include/AFNodeMeta.hpp"

namespace ark {

class AFINode
{
public:
    virtual ~AFINode() = default;

    virtual ArkDataType GetType() const = 0;
    virtual void Reset() = 0;
    virtual bool IsNull() const = 0;
    virtual void CopyFrom(AFINode* other) = 0;
    virtual void SaveTo(AFINode* other) = 0;
    virtual bool HaveMask(ArkDataMask mask) = 0;
    virtual ArkMaskType GetMask() const = 0;
    virtual ID_TYPE GetValue() const = 0;

    // Get data
    virtual bool GetBool() const
    {
        return NULL_BOOLEAN;
    };

    virtual int32_t GetInt32() const
    {
        return NULL_INT;
    }

    virtual uint32_t GetUInt32() const
    {
        return NULL_INT;
    }

    virtual int64_t GetInt64() const
    {
        return NULL_INT64;
    }

    virtual uint64_t GetUInt64() const
    {
        return NULL_INT64;
    }

    virtual float GetFloat() const
    {
        return NULL_FLOAT;
    }

    virtual double GetDouble() const
    {
        return NULL_DOUBLE;
    }

    virtual const std::string& GetString() const
    {
        return NULL_STR;
    }

    virtual const std::wstring& GetWString() const
    {
        return NULL_WIDESTR;
    }

    virtual const guid_t& GetObject() const
    {
        return NULL_GUID;
    }

    virtual void* GetPointer() const
    {
        return nullptr;
    }

    virtual const void* GetUserData(size_t& size) const
    {
        return nullptr;
    }

    virtual void* GetRawUserData() const
    {
        return nullptr;
    }

    // Set data
    virtual void SetUnknown() {}
    virtual void SetBool(bool value) {}
    virtual void SetInt64(int64_t value) {}
    virtual void SetUInt64(uint64_t value) {}
    virtual void SetFloat(float value) {}
    virtual void SetDouble(double value) {}
    virtual void SetString(const std::string& value) {}
    virtual void SetPointer(void* value) {}
    virtual void SetUserData(const void* value, size_t size) {}
    virtual void SetRawUserData(void* value) {}
    virtual void SetInt32(int32_t value) {}
    virtual void SetUInt32(uint32_t value) {}
    virtual void SetWString(const std::wstring& value) {}
    virtual void SetObject(const guid_t& guid) {}

    // other handle
    virtual void Assign(const AFIData& src) {}
    virtual size_t GetMemUsage() const
    {
        return 0;
    }

    virtual std::string ToString() const
    {
        return NULL_STR;
    }

    virtual void FromString(const std::string& value) {}

    // set meta
    virtual void SetMeta(std::shared_ptr<AFNodeMeta> pDataMeta) = 0;
    virtual std::shared_ptr<AFNodeMeta> GetMeta() = 0;
    virtual const std::string& GetName() = 0;
    virtual uint32_t GetIndex() const = 0;
};

} // namespace ark
