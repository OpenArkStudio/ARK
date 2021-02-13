/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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
#include "kernel/interface/AFIData.hpp"

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
class AFDataBase final : public AFIData
{
public:
    using self_t = AFDataBase<BUFFER_SIZE, ALLOC>;

    AFDataBase()
    {
        mnType = ArkDataType::DT_EMPTY;
        mn64Value = 0;
    }

    ~AFDataBase() override
    {
        Release();
    }

    AFDataBase(const self_t& src)
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
        case ArkDataType::DT_UINT32:
            mnU32Value = src.mnU32Value;
            break;
        case ArkDataType::DT_UINT64:
            mnU64Value = src.mnU64Value;
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
        case ArkDataType::DT_POINTER:
            mpVaule = src.mpVaule;
            break;
        case ArkDataType::DT_USERDATA:
            SetUserData(src);
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }
    }

    explicit AFDataBase(const AFIData& src)
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
        case ArkDataType::DT_UINT32:
            mnU32Value = src.GetUInt();
            break;
        case ArkDataType::DT_UINT64:
            mnU64Value = src.GetUInt64();
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
        case ArkDataType::DT_POINTER:
            mpVaule = src.GetPointer();
            break;
        case ArkDataType::DT_USERDATA:
            SetUserData(src);
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }
    }

    bool From(const AFINode* pData) override
    {
        if (pData == nullptr)
        {
            return false;
        }

        this->mnType = pData->GetType();

        switch (this->mnType)
        {
        case ArkDataType::DT_BOOLEAN:
            mbValue = pData->GetBool();
            break;
        case ArkDataType::DT_INT32:
            mnValue = pData->GetInt32();
            break;
        case ArkDataType::DT_INT64:
            mn64Value = pData->GetInt64();
            break;
        case ArkDataType::DT_UINT32:
            mnU32Value = pData->GetUInt32();
            break;
        case ArkDataType::DT_UINT64:
            mnU64Value = pData->GetUInt64();
            break;
        case ArkDataType::DT_FLOAT:
            mfValue = pData->GetFloat();
            break;
        case ArkDataType::DT_DOUBLE:
            mdValue = pData->GetDouble();
            break;
        case ArkDataType::DT_STRING:
            InnerSetString(pData->GetString().c_str());
            break;
        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }

        return true;
    }

    AFDataBase(ArkDataType type, bool value)
    {
        assert(type == ArkDataType::DT_BOOLEAN);

        mnType = ArkDataType::DT_BOOLEAN;
        mbValue = value;
    }

    AFDataBase(ArkDataType type, int value)
    {
        assert(type == ArkDataType::DT_INT32);

        mnType = ArkDataType::DT_INT32;
        mnValue = value;
    }

    AFDataBase(ArkDataType type, uint32_t value)
    {
        assert(type == ArkDataType::DT_UINT32);

        mnType = ArkDataType::DT_UINT32;
        mnValue = value;
    }

    AFDataBase(ArkDataType type, int64_t value)
    {
        assert(type == ArkDataType::DT_INT64);

        mnType = ArkDataType::DT_INT64;
        mn64Value = value;
    }

    AFDataBase(ArkDataType type, uint64_t value)
    {
        assert(type == ArkDataType::DT_UINT64);

        mnType = ArkDataType::DT_UINT64;
        mn64Value = value;
    }

    AFDataBase(ArkDataType type, float value)
    {
        assert(type == ArkDataType::DT_FLOAT);

        mnType = ArkDataType::DT_FLOAT;
        mfValue = value;
    }

    AFDataBase(ArkDataType type, double value)
    {
        assert(type == ArkDataType::DT_DOUBLE);

        mnType = ArkDataType::DT_DOUBLE;
        mdValue = value;
    }

    AFDataBase(ArkDataType type, const char* value)
    {
        assert(type == ArkDataType::DT_STRING);

        mnType = ArkDataType::DT_STRING;
        InnerSetString(value);
    }

    AFDataBase(ArkDataType type, const void* value, size_t size)
    {
        assert(type == ArkDataType::DT_USERDATA);

        mnType = ArkDataType::DT_USERDATA;
        InnerSetUserData(value, size);
    }

    self_t& operator=(const self_t& src)
    {
        self_t tmp(src);
        Swap(tmp);
        return *this;
    }

    self_t& operator=(const AFIData& src)
    {
        self_t tmp(src);
        Swap(tmp);
        return *this;
    }

    void Swap(self_t& src)
    {
        ArkDataType tmp_type = this->mnType;
        int64_t tmp_value = this->mn64Value;
        // guid_t tmp_guid = this->mxGUID;
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
        case ArkDataType::DT_UINT32:
            SetUInt(NULL_INT);
            break;
        case ArkDataType::DT_UINT64:
            SetUInt64(NULL_INT64);
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
        case ArkDataType::DT_UINT32:
            return mnU32Value == NULL_INT;
            break;
        case ArkDataType::DT_UINT64:
            return mnU64Value == NULL_INT64;
            break;
        case ArkDataType::DT_FLOAT:
            return IS_FLOAT_ZERO(float, mfValue);
            break;
        case ArkDataType::DT_DOUBLE:
            return IS_FLOAT_ZERO(double, mfValue);
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

    uint32_t GetUInt() const override
    {
        ARK_ASSERT_RET_VAL(mnType == ArkDataType::DT_UINT32, NULL_INT);
        return mnU32Value;
    }

    uint64_t GetUInt64() const override
    {
        ARK_ASSERT_RET_VAL(mnType == ArkDataType::DT_UINT64, NULL_INT64);
        return mnU64Value;
    }

    void* GetPointer() const override
    {
        ARK_ASSERT_RET_VAL(mnType == ArkDataType::DT_POINTER, nullptr);
        return mpVaule;
    }

    const void* GetUserData(size_t& size) const override
    {
        assert(mnType == ArkDataType::DT_USERDATA);

        if (mnType != ArkDataType::DT_USERDATA)
        {
            size = 0;
            return nullptr;
        }

        size = AFIData::GetUserDataSize(mpUserData);
        return AFIData::GetUserData(mpUserData);
    }

    void* GetRawUserData() const override
    {
        ARK_ASSERT_RET_VAL(mnType == ArkDataType::DT_USERDATA, nullptr);
        return mpUserData;
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

    void SetUInt(uint32_t value) override
    {
        Release();
        mnType = ArkDataType::DT_UINT32;
        mnU32Value = value;
    }

    void SetUInt64(uint64_t value) override
    {
        Release();
        mnType = ArkDataType::DT_UINT64;
        mnU64Value = value;
    }

    void SetPointer(void* value) override
    {
        Release();
        mnType = ArkDataType::DT_POINTER;
        mpVaule = value;
    }

    void SetUserData(const void* value, size_t size) override
    {
        Release();
        mnType = ArkDataType::DT_USERDATA;
        InnerSetUserData(value, size);
    }

    void SetRawUserData(void* value) override
    {
        SetUserData(AFIData::GetUserData(value), AFIData::GetUserDataSize(value));
    }

    void Assign(const AFIData& src) override
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
        case ArkDataType::DT_USERDATA:
        {
            if (mpUserData != nullptr)
            {
                size += AFIData::GetUserDataSize(mpUserData);
            }
        }
        break;
        default:
            break;
        }

        return size;
    }

    std::string ToString() const override
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
        case ArkDataType::DT_UINT32:
            return ARK_TO_STRING(this->mnU32Value);
            break;
        case ArkDataType::DT_UINT64:
            return ARK_TO_STRING(this->mnU64Value);
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
        case ArkDataType::DT_USERDATA:
        {
            if (mpUserData != nullptr)
            {
                mxAlloc.Free(mpUserData, mnAllocLen);
                mpUserData = nullptr;
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

    void InnerSetUserData(const void* data, size_t size)
    {
        size_t value_size = GetRawUserDataSize(size);
        char* p = (char*)mxAlloc.Alloc(value_size);
        InitRawUserData(p, data, size);
        mpUserData = p;
        mnAllocLen = (uint32_t)value_size;
    }

    void SetUserData(const self_t& src)
    {
        size_t size;
        const void* pData = src.GetUserData(size);
        InnerSetUserData(pData, size);
    }

    void SetUserData(const AFIData& src)
    {
        size_t size;
        const void* pData = src.GetUserData(size);
        InnerSetUserData(pData, size);
    }

private:
    ALLOC mxAlloc;
    ArkDataType mnType;

    // This union size is 8 bytes
    union
    {
        bool mbValue;
        int mnValue;
        uint32_t mnU32Value;
        int64_t mn64Value;
        uint64_t mnU64Value;
        float mfValue;
        double mdValue;
        char* mstrValue;
        void* mpVaule;
        char* mpUserData;
    };

    // buffer
    union
    {
        uint32_t mnAllocLen{0};
        char mBuffer[BUFFER_SIZE];
    };
};

// special
using AFCData = AFDataBase<4, CoreAlloc>;

} // namespace ark
