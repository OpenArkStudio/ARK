/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
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

#include "base/AFMacros.hpp"
#include "base/AFEnum.hpp"
#include "base/AFDefine.hpp"

namespace ark
{

    class AFIData
    {
    public:
        AFIData& operator=(const AFIData& rhs) = delete;

        inline static void* GetUserData(void* value)
        {
            return (char*)value + sizeof(size_t);
        }

        inline static size_t GetUserDataSize(void* value)
        {
            return *((size_t*)value);
        }

        inline static size_t GetRawUserDataSize(size_t size)
        {
            return sizeof(size_t) + size;
        }

        inline static void InitRawUserData(void* p, const void* pData, size_t size)
        {
            *((size_t*)p) = size;
            memcpy((char*)p + sizeof(size_t), pData, size);
        }

        bool equal(const AFIData& src)
        {
            if (this->GetType() != src.GetType())
            {
                return false;
            }

            bool bRet = false;
            switch (this->GetType())
            {
            case DT_BOOLEAN:
                bRet = (GetBool() == src.GetBool());
                break;
            case DT_INT:
                bRet = (GetInt() == src.GetInt());
                break;
            case DT_INT64:
                bRet = (GetInt64() == src.GetInt64());
                break;
            case DT_FLOAT:
                bRet = (GetFloat() == src.GetFloat());
                break;
            case DT_DOUBLE:
                bRet = (GetDouble() == src.GetDouble());
                break;
            case DT_STRING:
                bRet = (GetString() == src.GetString());
                break;
            case DT_POINTER:
                bRet = (GetPointer() == src.GetPointer());
                break;
            case DT_USERDATA:
                break;
            default:
                ARK_ASSERT_NO_EFFECT(false);
                break;
            }

            return bRet;
        }

        bool equal(const bool value)
        {
            if (this->GetType() != DT_BOOLEAN)
            {
                return false;
            }

            return GetBool() == value;
        }

        bool equal(const int value)
        {
            if (this->GetType() != DT_INT)
            {
                return false;
            }

            return GetInt() == value;
        }

        bool equal(const  int64_t value)
        {
            if (this->GetType() != DT_INT64)
            {
                return false;
            }

            return GetInt64() == value;
        }

        bool equal(const float value)
        {
            if (this->GetType() != DT_FLOAT)
            {
                return false;
            }

            return GetFloat() == value;
        }

        bool equal(const double  value)
        {
            if (this->GetType() != DT_DOUBLE)
            {
                return false;
            }

            return GetDouble() == value;
        }

        bool equal(const char*  value)
        {
            if (this->GetType() != DT_STRING)
            {
                return false;
            }

            return GetString() == value;
        }

        bool equal(const void* value)
        {
            if (this->GetType() != DT_POINTER)
            {
                return false;
            }

            return GetPointer() == value;
        }

        virtual ~AFIData() = default;

        virtual int GetType() const = 0;

        virtual void SetDefaultValue(int type) = 0;
        virtual bool IsNullValue() const = 0;

        //Get data
        virtual bool GetBool() const = 0;
        virtual int GetInt() const = 0;
        virtual int64_t GetInt64() const = 0;
        virtual float GetFloat() const = 0;
        virtual double GetDouble() const = 0;
        virtual const char* GetString() const = 0;
        virtual void* GetPointer() const = 0;
        virtual const void* GetUserData(size_t& size) const = 0;
        virtual void* GetRawUserData() const = 0;

        //Set data
        virtual void SetUnknown() = 0;
        virtual void SetBool(bool value) = 0;
        virtual void SetInt(int value) = 0;
        virtual void SetInt64(int64_t value) = 0;
        virtual void SetFloat(float value) = 0;
        virtual void SetDouble(double value) = 0;
        virtual void SetString(const char* value) = 0;
        virtual void SetPointer(void* value) = 0;
        virtual void SetUserData(const void* value, size_t size) = 0;
        virtual void SetRawUserData(void* value) = 0;

        virtual void Assign(const AFIData& src) = 0;
        virtual size_t GetMemUsage() const = 0;
        virtual std::string ToString() = 0;
    };

}