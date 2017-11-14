/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "SDK/Base/AFPlatform.hpp"
#include "SDK/Base/AFGUID.h"
#include "SDK/Base/AFString.hpp"
#include "SDK/Base/AFIData.h"

class AFIDataList
{
public:
    virtual ~AFIDataList() {}

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

    virtual bool TypeEx(const int nType, ...) const = 0;

    //operator <<
    inline AFIDataList& operator<<(bool value)
    {
        bool bRet = AddBool(value);
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(int8_t value)
    {
        bool bRet = AddInt(value);
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(uint8_t value)
    {
        bool bRet = AddInt(value);
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(int16_t value)
    {
        bool bRet = AddInt(value);
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(uint16_t value)
    {
        bool  bRet = AddInt(value);
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(int32_t value)
    {
        bool bRet = AddInt(value);
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(uint32_t value)
    {
        bool bRet = AddInt(value);
        assert_noeffect(bRet);
        return *this;
    }

//    inline AFIDataList& operator<<(long value)
//    {
//#if ARK_PLATFORM == PLATFORM_WIN || defined(WIN32)
//        bool bRet = AddInt(value);
//#else
//        bool bRet = AddInt64(value);
//#endif
//        assert_noeffect(bRet);
//        return *this;
//    }
//
//    inline AFIDataList& operator<<(unsigned long value)
//    {
//#if ARK_PLATFORM == PLATFORM_WIN || defined(WIN32)
//        bool bRet = AddInt(value);
//#else
//        bool bRet = AddInt64(value);
//#endif
//        assert_noeffect(bRet);
//        return *this;
//    }

    inline AFIDataList& operator<<(int64_t value)
    {
        bool bRet = AddInt64(value);
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(uint64_t value)
    {
        bool bRet = AddInt64(value);
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(float value)
    {
        bool bRet = AddFloat(value);
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(double value)
    {
        bool bRet = AddDouble(value);
        assert_noeffect(bRet);
        return *this;
    }
    inline AFIDataList& operator<<(const char* value)
    {
        bool bRet = AddString(value);
        assert_noeffect(bRet);
        return *this;
    }
    inline AFIDataList& operator<<(const std::string& value)
    {
        bool bRet = AddString(value.c_str());
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(const AFString<char, 128>& value)
    {
        bool bRet = AddString(value.c_str());
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(const AFGUID& value)
    {
        bool bRet = AddObject(value);
        assert_noeffect(bRet);
        return *this;
    }

    inline AFIDataList& operator<<(const AFIDataList& value)
    {
        bool bRet = Concat(value);
        assert_noeffect(bRet);
        return *this;
    }
};