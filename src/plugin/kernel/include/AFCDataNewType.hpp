/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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
#include "base/AFCoreDef.hpp"
#include "base/AFMisc.hpp"
#include "kernel/interface/AFIDataNewType.hpp"

namespace ark {

class AFDataAllocNewType
{
public:
    void* Alloc(size_t size)
    {
        ARK_NEW_ARRAY_RET(char, size);
    }

    void Free(void* ptr, size_t size)
    {
        ARK_DELETE_ARRAY(char, ptr);
    }

    void Swap(AFDataAllocNewType& src)
    {
        // do nothing
    }
};

template<size_t BUFFER_SIZE, typename ALLOC = AFDataAllocNewType>
class AFDataBaseNewType : public AFIDataNewType
{
public:
    using self_t = AFDataBaseNewType<BUFFER_SIZE, ALLOC>;

    AFDataBaseNewType()
    {
        mnType = ArkDataType::DT_EMPTY;
        mn64Value = 0;
    }

    ~AFDataBaseNewType() override
    {
        Release();
    }

    AFDataBaseNewType(const self_t& src)
    {
        this->mnType = src.mnType;

        switch (this->mnType)
        {
        case ArkDataType::DT_BOOLEAN:
            mbValue = src.mbValue;
            break;
        case ArkDataType::DT_INT32:
            mnValue = src.mnValue;
            break;
        case ArkDataType::DT_INT64:
            mn64Value = src.mn64Value;
            break;
        case ArkDataType::DT_FLOAT:
            mfValue = src.mfValue;
            break;
        case ArkDataType::DT_DOUBLE:
            mdValue = src.mdValue;
            break;
        case ArkDataType::DT_STRING:
            InnerSetString(src.GetString());
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }
    }

    explicit AFDataBaseNewType(const AFIDataNewType& src)
    {
        this->mnType = src.GetType();

        switch (this->mnType)
        {
        case ArkDataType::DT_BOOLEAN:
            mbValue = src.GetBool();
            break;
        case ArkDataType::DT_INT32:
            mnValue = src.GetInt();
            break;
        case ArkDataType::DT_INT64:
            mn64Value = src.GetInt64();
            break;
        case ArkDataType::DT_FLOAT:
            mfValue = src.GetFloat();
            break;
        case ArkDataType::DT_DOUBLE:
            mdValue = src.GetDouble();
            break;
        case ArkDataType::DT_STRING:
            InnerSetString(src.GetString());
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }
    }

    AFDataBaseNewType(ArkDataType type, bool value)
    {
        assert(type == ArkDataType::DT_BOOLEAN);

        mnType = ArkDataType::DT_BOOLEAN;
        mbValue = value;
    }

    AFDataBaseNewType(ArkDataType type, int value)
    {
        assert(type == ArkDataType::DT_INT32);

        mnType = ArkDataType::DT_INT32;
        mnValue = value;
    }

    AFDataBaseNewType(ArkDataType type, uint32_t value)
    {
        assert(type == ArkDataType::DT_UINT32);

        mnType = ArkDataType::DT_UINT32;
        mnValue = value;
    }

    AFDataBaseNewType(ArkDataType type, int64_t value)
    {
        assert(type == ArkDataType::DT_INT64);

        mnType = ArkDataType::DT_INT64;
        mn64Value = value;
    }

    AFDataBaseNewType(ArkDataType type, float value)
    {
        assert(type == ArkDataType::DT_FLOAT);

        mnType = ArkDataType::DT_FLOAT;
        mfValue = value;
    }

    AFDataBaseNewType(ArkDataType type, double value)
    {
        assert(type == ArkDataType::DT_DOUBLE);

        mnType = ArkDataType::DT_DOUBLE;
        mdValue = value;
    }

    AFDataBaseNewType(ArkDataType type, const char* value)
    {
        assert(type == ArkDataType::DT_STRING);

        mnType = ArkDataType::DT_STRING;
        InnerSetString(value);
    }

    self_t& operator=(const self_t& src)
    {
        self_t tmp(src);
        Swap(tmp);
        return *this;
    }

    self_t& operator=(const AFIDataNewType& src)
    {
        self_t tmp(src);
        Swap(tmp);
        return *this;
    }

    void Swap(self_t& src)
    {
        ArkDataType tmp_type = this->mnType;
        int64_t tmp_value = this->mn64Value;
        // AFGUID tmp_guid = this->mxGUID;
        uint32_t tmp_alloc_len = this->mnAllocLen;
        char tmp_buffer[BUFFER_SIZE] = {0};
        bool tmp_use_buffer = (tmp_type == ArkDataType::DT_STRING) && (this->mstrValue == this->mBuffer);

        if ((src.mnType == ArkDataType::DT_STRING) && (src.mstrValue == src.mBuffer))
        {
            if (tmp_use_buffer)
            {
                memcpy(tmp_buffer, this->mBuffer, BUFFER_SIZE);
            }

            memcpy(this->mBuffer, src.mBuffer, BUFFER_SIZE);
            mstrValue = mBuffer;
        }
        else
        {
            this->mn64Value = src.mn64Value;
            mnAllocLen = src.mnAllocLen;
        }

        this->mnType = src.mnType;

        if (tmp_use_buffer)
        {
            memcpy(src.mBuffer, tmp_buffer, BUFFER_SIZE);
            src.mstrValue = src.mBuffer;
        }
        else
        {
            src.mn64Value = tmp_value;
            src.mnAllocLen = tmp_alloc_len;
        }

        src.mnType = tmp_type;
        mxAlloc.Swap(src.mxAlloc);
    }
    //////////////////////////////////////////////////////////////////////////

    ArkDataType GetType() const override
    {
        return mnType;
    }

    void SetDefaultValue(ArkDataType type) override
    {
        switch (type)
        {
        case ArkDataType::DT_BOOLEAN:
            SetBool(NULL_BOOLEAN);
            break;
        case ArkDataType::DT_INT32:
            SetInt(NULL_INT);
            break;
        case ArkDataType::DT_INT64:
            SetInt64(NULL_INT64);
            break;
        case ArkDataType::DT_FLOAT:
            SetFloat(NULL_FLOAT);
            break;
        case ArkDataType::DT_DOUBLE:
            SetDouble(NULL_DOUBLE);
            break;
        case ArkDataType::DT_STRING:
            SetString(NULL_STR.c_str());
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }
    }

    bool IsNullValue() const override
    {
        switch (GetType())
        {
        case ArkDataType::DT_BOOLEAN:
            return mbValue == NULL_BOOLEAN;
            break;
        case ArkDataType::DT_INT32:
            return mnValue == NULL_INT;
            break;
        case ArkDataType::DT_INT64:
            return mn64Value == NULL_INT64;
            break;
        case ArkDataType::DT_FLOAT:
            return AFMisc::IsZeroFloat(mfValue);
            break;
        case ArkDataType::DT_DOUBLE:
            return AFMisc::IsZeroDouble(mdValue);
            break;
        case ArkDataType::DT_STRING:
            return mstrValue == NULL_STR.c_str();
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }

        return false;
    }

    // Get data
    bool GetBool() const override
    {
        ARK_ASSERT_RET_VAL(mnType == ArkDataType::DT_BOOLEAN, NULL_BOOLEAN);
        return mbValue;
    }

    int GetInt() const override
    {
        ARK_ASSERT_RET_VAL(mnType == ArkDataType::DT_INT32, NULL_INT);
        return mnValue;
    }

    int64_t GetInt64() const override
    {
        ARK_ASSERT_RET_VAL(mnType == ArkDataType::DT_INT64, NULL_INT64);
        return mn64Value;
    }

    float GetFloat() const override
    {
        ARK_ASSERT_RET_VAL(mnType == ArkDataType::DT_FLOAT, NULL_FLOAT);
        return mfValue;
    }

    double GetDouble() const override
    {
        ARK_ASSERT_RET_VAL(mnType == ArkDataType::DT_DOUBLE, NULL_DOUBLE);
        return mdValue;
    }

    const char* GetString() const override
    {
        ARK_ASSERT_RET_VAL(mnType == ArkDataType::DT_STRING, NULL_STR.c_str());
        return mstrValue;
    }

    // Set data
    void SetUnknown() override
    {
        Release();
        mnType = ArkDataType::DT_EMPTY;
        mn64Value = 0;
    }

    void SetBool(bool value) override
    {
        Release();
        mnType = ArkDataType::DT_BOOLEAN;
        mbValue = value;
    }

    void SetInt(int value) override
    {
        Release();
        mnType = ArkDataType::DT_INT32;
        mnValue = value;
    }

    void SetInt64(int64_t value) override
    {
        Release();
        mnType = ArkDataType::DT_INT64;
        mn64Value = value;
    }

    void SetFloat(float value) override
    {
        Release();
        mnType = ArkDataType::DT_FLOAT;
        mfValue = value;
    }

    void SetDouble(double value) override
    {
        Release();
        mnType = ArkDataType::DT_DOUBLE;
        mdValue = value;
    }

    void SetString(const char* value) override
    {
        Release();
        mnType = ArkDataType::DT_STRING;
        InnerSetString(value);
    }

    void Assign(const AFIDataNewType& src) override
    {
        self_t tmp(src);
        Swap(tmp);
    }

    size_t GetMemUsage() const override
    {
        size_t size = sizeof(self_t);

        switch (mnType)
        {
        case ArkDataType::DT_STRING:
        {
            if (mstrValue != nullptr)
            {
                size += strlen(mstrValue) + 1;
            }
        }
        break;
        default:
            break;
        }

        return size;
    }

    std::string ToString() override
    {
        switch (GetType())
        {
        case ArkDataType::DT_BOOLEAN:
            return ARK_TO_STRING(this->mbValue);
            break;
        case ArkDataType::DT_INT32:
            return ARK_TO_STRING(this->mnValue);
            break;
        case ArkDataType::DT_INT64:
            return ARK_TO_STRING(this->mn64Value);
            break;
        case ArkDataType::DT_FLOAT:
            return ARK_TO_STRING(this->mfValue);
            break;
        case ArkDataType::DT_DOUBLE:
            return ARK_TO_STRING(this->mdValue);
            break;
        case ArkDataType::DT_STRING:
            return this->mstrValue;
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }

        return NULL_STR;
    }

    void Release()
    {
        switch (mnType)
        {
        case ArkDataType::DT_STRING:
        {
            if (mstrValue != mBuffer)
            {
                mxAlloc.Free(mstrValue, mnAllocLen);
                mstrValue = nullptr;
            }
        }
        break;
        default:
            break;
        }
    }

protected:
    void InnerSetString(const char* value)
    {
        const size_t value_size = strlen(value) + 1;
        char* p = nullptr;

        if (value_size > BUFFER_SIZE)
        {
            p = (char*)mxAlloc.Alloc(value_size);
            mnAllocLen = (uint32_t)value_size;
        }
        else
        {
            p = mBuffer;
        }

        memcpy(p, value, value_size);
        mstrValue = p;
    }

private:
    ALLOC mxAlloc;
    ArkDataType mnType;

    // This union size is 8 bytes
    union
    {
        bool mbValue;
        int mnValue;
        int64_t mn64Value;
        float mfValue;
        double mdValue;
        char* mstrValue;
        void* mpVaule;
        char* mpUserData;
    };

    // buffer
    union
    {
        uint32_t mnAllocLen;
        char mBuffer[BUFFER_SIZE];
    };
};

// special
using AFCDataNewType = AFDataBaseNewType<4, CoreAlloc>;

} // namespace ark
