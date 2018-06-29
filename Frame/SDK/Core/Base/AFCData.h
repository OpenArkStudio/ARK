/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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

#include "SDK/Core/Base/AFCoreDef.hpp"
#include "SDK/Core/Base/AFIData.h"
#include "SDK/Core/Base/AFPlatform.hpp"
#include "SDK/Core/Base/AFMisc.hpp"
#include "SDK/Core/Base/AFMemAlloc.hpp"

class AFDataAlloc
{
public:
    AFDataAlloc() = default;
    ~AFDataAlloc() = default;

    void* Alloc(size_t size)
    {
        void* ptr = ARK_ALLOC(size);
        memset(ptr, 0, size);
        return ptr;
    }

    void Free(void* ptr, size_t size)
    {
        ARK_DEALLOC(ptr);
    }

    void Swap(AFDataAlloc& src)
    {
        //do nothing
    }
};

template<size_t BUFFER_SIZE, typename ALLOC = AFDataAlloc>
class AFBaseData : public AFIData
{
public:
    using self_t = AFBaseData<BUFFER_SIZE, ALLOC>;

    AFBaseData()
    {
        mnType = DT_UNKNOWN;
        mn64Value = 0;
    }

    virtual ~AFBaseData()
    {
        Release();
    }

    AFBaseData(const self_t& src)
    {
        this->mnType = src.mnType;

        switch (this->mnType)
        {
        case DT_BOOLEAN:
            mbValue = src.mbValue;
            break;

        case DT_INT:
            mnValue = src.mnValue;
            break;

        case DT_INT64:
            mn64Value = src.mn64Value;
            break;

        case DT_FLOAT:
            mfValue = src.mfValue;
            break;

        case DT_DOUBLE:
            mdValue = src.mdValue;
            break;

        case DT_STRING:
            InnerSetString(src.GetString());
            break;

        case DT_OBJECT:
            mxGUID = src.mxGUID;
            break;

        case DT_POINTER:
            mpVaule = src.mpVaule;
            break;

        case DT_USERDATA:
            SetUserData(src);
            break;

        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }
    }

    explicit AFBaseData(const AFIData& src)
    {
        this->mnType = src.GetType();

        switch (this->mnType)
        {
        case DT_BOOLEAN:
            mbValue = src.GetBool();
            break;

        case DT_INT:
            mnValue = src.GetInt();
            break;

        case DT_INT64:
            mn64Value = src.GetInt64();
            break;

        case DT_FLOAT:
            mfValue = src.GetFloat();
            break;

        case DT_DOUBLE:
            mdValue = src.GetDouble();
            break;

        case DT_STRING:
            InnerSetString(src.GetString());
            break;

        case DT_OBJECT:
            mxGUID = src.GetObject();
            break;

        case DT_POINTER:
            mpVaule = src.GetPointer();
            break;

        case DT_USERDATA:
            SetUserData(src);
            break;

        default:
            break;
        }
    }

    AFBaseData(int type, bool value)
    {
        assert(type == DT_BOOLEAN);

        mnType = DT_BOOLEAN;
        mbValue = value;
    }

    AFBaseData(int type, int value)
    {
        assert(type == DT_INT);

        mnType = DT_INT;
        mnValue = value;
    }

    AFBaseData(int type, int64_t value)
    {
        assert(type == DT_INT64);

        mnType = DT_INT64;
        mn64Value = value;
    }

    AFBaseData(int type, float value)
    {
        assert(type == DT_FLOAT);

        mnType = DT_FLOAT;
        mfValue = value;
    }

    AFBaseData(int type, double value)
    {
        assert(type == DT_DOUBLE);

        mnType = DT_DOUBLE;
        mdValue = value;
    }

    AFBaseData(int type, const char* value)
    {
        assert(type == DT_STRING);

        mnType = DT_STRING;
        InnerSetString(value);
    }

    AFBaseData(int type, const AFGUID& value)
    {
        assert(type == DT_OBJECT);

        mnType = DT_OBJECT;
        mxGUID = value;
    }

    AFBaseData(int type, const void* value, size_t size)
    {
        assert(type == DT_USERDATA);

        mnType = DT_USERDATA;
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
        int tmp_type = this->mnType;
        int64_t tmp_value = this->mn64Value;
        AFGUID tmp_guid = this->mxGUID;
        uint32_t tmp_alloc_len = this->mnAllocLen;
        char tmp_buffer[BUFFER_SIZE] = { 0 };
        bool tmp_use_buffer = (tmp_type == DT_STRING) && (this->mstrValue == this->mBuffer);

        if ((src.mnType == DT_STRING) && (src.mstrValue == src.mBuffer))
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
            if (src.mnType == DT_OBJECT)
            {
                this->mxGUID = src.mxGUID;
                mnAllocLen = src.mnAllocLen;
            }
            else
            {
                this->mn64Value = src.mn64Value;
                mnAllocLen = src.mnAllocLen;
            }
        }

        this->mnType = src.mnType;

        if (tmp_use_buffer)
        {
            memcpy(src.mBuffer, tmp_buffer, BUFFER_SIZE);
            src.mstrValue = src.mBuffer;
        }
        else
        {
            if (src.mnType == DT_OBJECT)
            {
                src.mxGUID = tmp_guid;
            }
            else
            {
                src.mn64Value = tmp_value;
            }

            src.mnAllocLen = tmp_alloc_len;
        }

        src.mnType = tmp_type;
        mxAlloc.Swap(src.mxAlloc);
    }
    //////////////////////////////////////////////////////////////////////////

    virtual int GetType() const
    {
        return mnType;
    }

    virtual void SetDefaultValue(int type)
    {
        switch (type)
        {
        case DT_BOOLEAN:
            SetBool(NULL_BOOLEAN);
            break;

        case DT_INT:
            SetInt(NULL_INT);
            break;

        case DT_INT64:
            SetInt64(NULL_INT64);
            break;

        case DT_FLOAT:
            SetFloat(NULL_FLOAT);
            break;

        case DT_DOUBLE:
            SetDouble(NULL_DOUBLE);
            break;

        case DT_STRING:
            SetString("");
            break;

        case DT_OBJECT:
            SetObject(AFGUID(0));
            break;

        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }
    }

    virtual bool IsNullValue() const
    {
        switch (GetType())
        {
        case DT_BOOLEAN:
            return mbValue == NULL_BOOLEAN;
            break;

        case DT_INT:
            return mnValue == NULL_INT;
            break;

        case DT_INT64:
            return mn64Value == NULL_INT64;
            break;

        case DT_FLOAT:
            return AFMisc::IsZeroFloat(mfValue);
            break;

        case DT_DOUBLE:
            return AFMisc::IsZeroDouble(mdValue);
            break;

        case DT_STRING:
            return mstrValue == NULL_STR.c_str();
            break;

        case DT_OBJECT:
            return GetObject() == NULL_GUID;
            break;

        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }

        return false;
    }

    //Get data
    virtual bool GetBool() const
    {
        ARK_ASSERT_RET_VAL(mnType == DT_BOOLEAN, NULL_BOOLEAN);
        return mbValue;
    }

    virtual int GetInt() const
    {
        ARK_ASSERT_RET_VAL(mnType == DT_INT, NULL_INT);
        return mnValue;
    }

    virtual int64_t GetInt64() const
    {
        ARK_ASSERT_RET_VAL(mnType == DT_INT64, NULL_INT64);
        return mn64Value;
    }

    virtual float GetFloat() const
    {
        ARK_ASSERT_RET_VAL(mnType == DT_FLOAT, NULL_FLOAT);
        return mfValue;
    }

    virtual double GetDouble() const
    {
        ARK_ASSERT_RET_VAL(mnType == DT_DOUBLE, NULL_DOUBLE);
        return mdValue;
    }

    virtual const char* GetString() const
    {
        ARK_ASSERT_RET_VAL(mnType == DT_STRING, NULL_STR.c_str());
        return mstrValue;
    }

    virtual AFGUID GetObject() const
    {
        ARK_ASSERT_RET_VAL(mnType == DT_OBJECT, NULL_GUID);
        return mxGUID;
    }

    virtual void* GetPointer() const
    {
        ARK_ASSERT_RET_VAL(mnType == DT_POINTER, NULL);
        return mpVaule;
    }

    virtual const void* GetUserData(size_t& size) const
    {
        assert(mnType == DT_USERDATA);

        if (mnType != DT_USERDATA)
        {
            size = 0;
            return NULL;
        }

        size = AFIData::GetUserDataSize(mpUserData);
        return AFIData::GetUserData(mpUserData);
    }

    virtual void* GetRawUserData() const
    {
        ARK_ASSERT_RET_VAL(mnType == DT_USERDATA, NULL);
        return mpUserData;
    }

    //Set data
    virtual void SetUnknown()
    {
        Release();
        mnType = DT_UNKNOWN;
        mn64Value = 0;
    }

    virtual void SetBool(bool value)
    {
        Release();
        mnType = DT_BOOLEAN;
        mbValue = value;
    }

    virtual void SetInt(int value)
    {
        Release();
        mnType = DT_INT;
        mnValue = value;
    }

    virtual void SetInt64(int64_t value)
    {
        Release();
        mnType = DT_INT64;
        mn64Value = value;
    }

    virtual void SetFloat(float value)
    {
        Release();
        mnType = DT_FLOAT;
        mfValue = value;
    }

    virtual void SetDouble(double value)
    {
        Release();
        mnType = DT_DOUBLE;
        mdValue = value;
    }

    virtual void SetString(const char* value)
    {
        Release();
        mnType = DT_STRING;
        InnerSetString(value);
    }

    virtual void SetObject(const AFGUID& value)
    {
        Release();
        mnType = DT_OBJECT;
        mxGUID = value;
    }

    virtual void SetPointer(void* value)
    {
        Release();
        mnType = DT_POINTER;
        mpVaule = value;
    }

    virtual void SetUserData(const void* value, size_t size)
    {
        Release();
        mnType = DT_USERDATA;
        InnerSetUserData(value, size);
    }

    virtual void SetRawUserData(void* value)
    {
        SetUserData(AFIData::GetUserData(value), AFIData::GetUserDataSize(value));
    }

    virtual void Assign(const AFIData& src)
    {
        self_t tmp(src);
        Swap(tmp);
    }

    virtual size_t GetMemUsage() const
    {
        size_t size = sizeof(self_t);

        switch (mnType)
        {
        case DT_STRING:
            {
                if (mstrValue != nullptr)
                {
                    size += strlen(mstrValue) + 1;
                }
            }
            break;

        case DT_USERDATA:
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

    virtual std::string ToString()
    {
        switch (GetType())
        {
        case DT_BOOLEAN:
            return ARK_TO_STRING(this->mbValue);
            break;

        case DT_INT:
            return ARK_TO_STRING(this->mnValue);
            break;

        case DT_INT64:
            return ARK_TO_STRING(this->mn64Value);
            break;

        case DT_FLOAT:
            return ARK_TO_STRING(this->mfValue);
            break;

        case DT_DOUBLE:
            return ARK_TO_STRING(this->mdValue);
            break;

        case DT_STRING:
            return this->mstrValue;
            break;

        case DT_OBJECT:
            return GetObject().ToString();
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
        case DT_STRING:
            {
                if (mstrValue != mBuffer)
                {
                    mxAlloc.Free(mstrValue, mnAllocLen);
                    mstrValue = nullptr;
                }
            }
            break;

        case DT_USERDATA:
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
        char* p = NULL;

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
    int mnType;

    //This union size = 16
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
        AFGUID mxGUID;
    };

    //buffer
    union
    {
        uint32_t mnAllocLen;
        char mBuffer[BUFFER_SIZE];
    };
};

//special
using AFCData = AFBaseData<4, CoreAlloc>;