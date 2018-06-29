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

#include "SDK/Core/Base/AFPlatform.hpp"
#include "SDK/Core/Base/AFGUID.h"
#include "SDK/Core/Base/AFString.hpp"
#include "SDK/Core/Base/AFIData.h"

class AFIDataList
{
public:
    virtual ~AFIDataList() = default;

    virtual bool Concat(const AFIDataList& src) = 0;

    virtual bool Append(const AFIData& data) = 0;
    virtual bool Append(const AFIDataList& src, size_t start, size_t count) = 0;

    virtual void Clear() = 0;

    virtual bool Empty() const = 0;

    virtual size_t GetCount() const = 0;

    virtual int GetType(size_t index) const = 0;

    //add data
    virtual bool AddBool(bool value) = 0;
    virtual bool AddInt(int value) = 0;
    virtual bool AddInt64(int64_t value) = 0;
    virtual bool AddFloat(float value) = 0;
    virtual bool AddDouble(double value) = 0;
    virtual bool AddString(const char* value) = 0;
    virtual bool AddObject(const AFGUID& value) = 0;
    virtual bool AddPointer(void* value) = 0;
    virtual bool AddUserData(const void* pData, size_t size) = 0;
    virtual bool AddRawUserData(void* value) = 0;

    //get data
    virtual bool Bool(size_t index) const = 0;
    virtual int Int(size_t index) const = 0;
    virtual int64_t Int64(size_t index) const = 0;
    virtual float Float(size_t index) const = 0;
    virtual double Double(size_t index) const = 0;
    virtual const char* String(size_t index) const = 0;
    virtual AFGUID Object(size_t index) const = 0;
    virtual void* Pointer(size_t index) const = 0;
    virtual const void* UserData(size_t index, size_t& size) const = 0;
    virtual void* RawUserData(size_t index) const = 0;
    virtual const std::string ToString(size_t index) = 0;

    //get memory usage
    virtual size_t GetMemUsage() const = 0;

    template<typename... Args>
    bool TypeEx(const int arg1, const Args& ... args) const
    {
        bool bRet = true;
        int values[] = { arg1, args... };

        int index = 0;

        for (auto v : values)
        {
            if (v == DT_UNKNOWN)
            {
                bRet = false;
                return bRet;
            }

            AF_DATA_TYPE varType = (AF_DATA_TYPE)GetType(index);

            if (varType != v)
            {
                bRet = false;
                break;
            }

            ++index;
        }

        return bRet;
    }

    bool ToAFIData(size_t index, AFIData& xData) const
    {
        if (GetType(index) != xData.GetType())
        {
            return false;
        }

        switch (xData.GetType())
        {
        case DT_BOOLEAN:
            xData.SetBool(Bool(index));
            break;

        case DT_INT:
            xData.SetInt(Int(index));
            break;

        case DT_INT64:
            xData.SetInt64(Int64(index));
            break;

        case DT_FLOAT:
            xData.SetFloat(Float(index));
            break;

        case DT_DOUBLE:
            xData.SetDouble(Double(index));
            break;

        case DT_STRING:
            xData.SetString(String(index));
            break;

        case DT_OBJECT:
            xData.SetObject(Object(index));
            break;

        default:
            break;
        }

        return true;
    }

    bool Equal(size_t index, const AFIData& xData) const
    {
        if (GetType(index) != xData.GetType())
        {
            return false;
        }

        switch (xData.GetType())
        {
        case DT_BOOLEAN:
            return xData.GetBool() == Bool(index);
            break;

        case DT_INT:
            return xData.GetInt() == Int(index);
            break;

        case DT_INT64:
            return xData.GetInt64() == Int64(index);
            break;

        case DT_FLOAT:
            return AFMisc::IsZeroFloat(xData.GetFloat() - Float(index));
            break;

        case DT_DOUBLE:
            return AFMisc::IsZeroDouble(xData.GetDouble() - Double(index));
            break;

        case DT_STRING:
            return std::string(xData.GetString()) == std::string(String(index));
            break;

        case DT_OBJECT:
            return xData.GetObject() == Object(index);
            break;

        default:
            break;
        }

        return true;
    }

    //operator <<
    inline AFIDataList& operator<<(bool value)
    {
        bool bRet = AddBool(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(int8_t value)
    {
        bool bRet = AddInt(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(uint8_t value)
    {
        bool bRet = AddInt(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(int16_t value)
    {
        bool bRet = AddInt(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(uint16_t value)
    {
        bool  bRet = AddInt(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(int32_t value)
    {
        bool bRet = AddInt(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(uint32_t value)
    {
        bool bRet = AddInt(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(int64_t value)
    {
        bool bRet = AddInt64(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(uint64_t value)
    {
        bool bRet = AddInt64(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(float value)
    {
        bool bRet = AddFloat(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(double value)
    {
        bool bRet = AddDouble(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }
    inline AFIDataList& operator<<(const char* value)
    {
        bool bRet = AddString(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }
    inline AFIDataList& operator<<(const std::string& value)
    {
        bool bRet = AddString(value.c_str());
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(const AFString<char, 128>& value)
    {
        bool bRet = AddString(value.c_str());
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(const AFGUID& value)
    {
        bool bRet = AddObject(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(const AFIDataList& value)
    {
        bool bRet = Concat(value);
        ARK_ASSERT_NO_EFFECT(bRet);
        return *this;
    }
};