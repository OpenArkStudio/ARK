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
#include "base/AFVector3D.hpp"
#include "base/AFDataSetting.hpp"
#include "base/AFMetaClassSetting.hpp"
#include "base/AFDataSetting.hpp"

namespace ark
{

    class AFIDataEx
    {
    public:
        virtual ~AFIDataEx() = default;

        virtual bool Init(const AFMetaClassSetting* class_setting, const AFEntityMetaData* data_setting)
        {
            ARK_ASSERT_RET_VAL(class_setting != nullptr && data_setting != nullptr, false);

            class_setting_ = class_setting;
            data_setting_ = data_setting;

            FromString(data_setting->init_value_);

            return false;
        }

        virtual void Clear() = 0;
        virtual bool IsValid() = 0;
        virtual uint32_t Size()
        {
            return 1;
        }

        virtual void Resize(uint32_t size) {}
        virtual const std::string& GetName() const
        {
            return data_setting_->name_;
        }

        virtual ArkDataType GetType() const
        {
            return data_setting_->type_;
        }

        virtual uint64_t GetKey() const
        {
            return NULL_INT64;
        }

        virtual void SetKey(uint64_t id) {}

        const AFMetaClassSetting* GetClassSetting()
        {
            return class_setting_;
        }

        const AFEntityMetaData* GetDataSetting()
        {
            return data_setting_;
        }

        bool HaveMask(ArkDataMaskType mask)
        {
            return data_setting_->HaveMask(mask);
        }

        virtual void SetParent(AFIDataEx* data)
        {
            parent_ = data;
        }

        virtual AFIDataEx* GetParent()
        {
            return parent_;
        }

        virtual AFIDataEx* FirstData()
        {
            return this;
        }

        virtual AFIDataEx* NextData()
        {
            return nullptr;
        }

        virtual void CopyFrom(AFIDataEx* rhs) = 0;
        virtual void SaveTo(AFIDataEx* rhs) = 0;

        virtual void FromString(const std::string& value) = 0;
        virtual std::string ToString() = 0;

        virtual void ToMap(std::map<std::string, std::string>& value) {}
        virtual void FromMap(const std::map<std::string, std::string>& value) {}

        //////////////////////////////////////////////////////////////////////////
        //For build-in type
        template <typename T>
        T NullValue()
        {
            return static_cast<T>(NULL_INT);
        }

        //////////////////////////////////////////////////////////////////////////
        //Get functions
        template <typename T = int32_t>
        T GetValue()
        {
            switch (data_setting_->type_)
            {
            case ArkDataType::DT_BOOLEAN:
                return static_cast<T>(GetBool());
                break;
            case ArkDataType::DT_INT32:
                return static_cast<T>(GetInt32());
                break;
            case ArkDataType::DT_UINT32:
                return static_cast<T>(GetUInt32());
                break;
            case ArkDataType::DT_INT64:
                return static_cast<T>(GetInt64());
                break;
            case ArkDataType::DT_UINT64:
                return static_cast<T>(GetUInt64());
                break;
            case ArkDataType::DT_FLOAT:
                return static_cast<T>(GetFloat());
                break;
            case ArkDataType::DT_DOUBLE:
                return static_cast<T>(GetDouble());
                break;
            default:
                ARK_ASSERT_NO_EFFECT(0);
                break;
            }

            return Invalid<T>();
        }

        template <typename T = int32_t>
        T GetValue(const std::string& data_name)
        {
            auto data = FindData(data_name);
            if (data == nullptr)
            {
                return Invalid<T>();
            }

            return data->GetValue<T>();
        }

        template <typename T = int32_t>
        T GetValue(const std::string& data_name, const std::string& child_name)
        {
            auto data = FindData(data_name, child_name);
            if (data == nullptr)
            {
                return Invalid<T>();
            }

            return data->GetValue<T>();
        }

        template <typename T = int32_t>
        T GetValue(uint64_t key, const std::string& data_name)
        {
            auto data = GetData(key, data_name);
            if (data == nullptr)
            {
                return Invalid<T>();
            }

            return data->GetValue<T>();
        }

        template <typename T = int32_t>
        T GetValue(const std::string& data_name, uint64_t key, const std::string& child_name)
        {
            auto data = GetData(data_name, key);
            if (data == nullptr)
            {
                return Invalid<T>();
            }

            return data->GetValue<T>(child_name);
        }

        //////////////////////////////////////////////////////////////////////////
        //Set functions
        template <typename T = int32_t>
        void SetValue(T value)
        {
            switch (data_setting_->type_)
            {
            case ArkDataType::DT_BOOLEAN:
                return SetBool(static_cast<bool>(value));
                break;
            case ArkDataType::DT_INT32:
                return SetInt32(static_cast<int32_t>(value));
                break;
            case ArkDataType::DT_UINT32:
                return SetUInt32(static_cast<uint32_t>(value));
                break;
            case ArkDataType::DT_INT64:
                return SetInt64(static_cast<int64_t>(value));
                break;
            case ArkDataType::DT_UINT64:
                return SetUInt64(static_cast<uint64_t>(value));
                break;
            case ArkDataType::DT_FLOAT:
                return SetFloat(static_cast<float>(value));
                break;
            case ArkDataType::DT_DOUBLE:
                return SetDouble(static_cast<double>(value));
                break;
            default:
                ARK_ASSERT_NO_EFFECT(0);
                break;
            }
        }

        template <typename T = int32_t>
        void SetValue(const std::string& data_name, T value)
        {
            auto data = GetData(data_name);
            ARK_ASSERT_RET_NONE(data != nullptr);

            data->SetValue<T>(value);
        }

        template <typename T = int32_t>
        void SetValue(const std::string& data_name, const std::string& child_name, T value)
        {
            auto data = GetData(data_name, child_name);
            ARK_ASSERT_RET_NONE(data != nullptr);

            data->SetValue<T>(value);
        }

        template <typename T = int32_t>
        void SetValue(uint64_t key, const std::string& data_name, T value)
        {
            auto data = GetData(key, data_name);
            ARK_ASSERT_RET_NONE(data != nullptr);

            data->SetValue<T>(value);
        }

        template <typename T = int32_t>
        void SetValue(const std::string& data_name, uint64_t key, const std::string& child_name, T value)
        {
            auto data = GetData(data_name, key);
            ARK_ASSERT_RET_NONE(data != nullptr);

            data->SetValue<T>(child_name, value);
        }

        //////////////////////////////////////////////////////////////////////////
        //Operate functions
        template <typename T = int32_t>
        T OpValue(ArkDataOpType op, T value)
        {
            auto base_value = GetValue<T>();
            auto new_value = AFMisc::Operate<T>(op, base_value, value);
            SetValue<T>(new_value);
            return new_value;
        }

        template <typename T = int32_t>
        T OpValue(const std::string& data_name, ArkDataOpType op, T value)
        {
            auto data = GetData(data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, Invalid<T>());

            return data->OpValue<T>(op, value);
        }

        template <typename T = int32_t>
        T OpValue(const std::string& data_name, const std::string& child_name, ArkDataOpType op, T value)
        {
            auto data = GetData(data_name, data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, Invalid<T>());

            return data->OpValue(op, value);
        }

        template <typename T = int32_t>
        T OpValue(uint64_t key, const std::string& data_name, ArkDataOpType op, T value)
        {
            auto data = GetData(key, data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, Invalid<T>());

            return data->OpValue<T>(op, value);
        }

        template <typename T = int32_t>
        T OpValue(const std::string& data_name, uint64_t key, ArkDataOpType op, T value)
        {
            auto data = GetData(data_name, key);
            ARK_ASSERT_RET_VAL(data != nullptr, Invalid<T>());

            return data->OpValue<T>(op, value);
        }

        template <typename T = int32_t>
        T OpValue(const std::string& data_name, uint64_t key, const std::string& child_name, ArkDataOpType op, T value)
        {
            auto data = GetData(data_name, key, child_name);
            ARK_ASSERT_RET_VAL(data != nullptr, Invalid<T>());

            return data->OpValue<T>(op, value);
        }

        //////////////////////////////////////////////////////////////////////////
        //For complex data structure (self defined)
        //sub_struct
        virtual AFIDataEx* GetData(const std::string& data_name)
        {
            return nullptr;
        }

        virtual AFIDataEx* GetData(const std::string& data_name, uint64_t key)
        {
            return nullptr;
        }

        virtual AFIDataEx* GetData(const std::string& data_name, const std::string& child_name)
        {
            return nullptr;
        }

        virtual AFIDataEx* GetData(const std::string& data_name, uint64_t key, const std::string& child_name)
        {
            return nullptr;
        }

        virtual AFIDataEx* GetData(const std::string& data_name, const std::string& child_name, uint64_t key)
        {
            return nullptr;
        }

        virtual AFIDataEx* GetData(const std::string& data_name, const std::string& child_name, const std::string& sub_name)
        {
            return nullptr;
        }

        virtual bool AddData(const std::string& data_name, AFIDataEx* data)
        {
            return false;
        }

        virtual bool AddData(const std::string& data_name, uint64_t key, AFIDataEx* data)
        {
            return false;
        }

        virtual bool AddData(const std::string& data_name, const std::string& child_name, AFIDataEx* data)
        {
            return false;
        }

        virtual bool RemoveData(const std::string& data_name)
        {
            return false;
        }

        virtual bool RemoveData(const std::string& data_name, uint64_t key)
        {
            return false;
        }

        virtual bool RemoveData(const std::string& data_name, const std::string& child_name)
        {
            return false;
        }

        //DataTable
        virtual AFIDataEx* GetData(uint64_t key)
        {
            return nullptr;
        }

        virtual AFIDataEx* GetData(uint64_t key, const std::string& data_name)
        {
            return nullptr;
        }

        virtual AFIDataEx* GetData(uint64_t key, uint64_t child_key)
        {
            return nullptr;
        }

        virtual AFIDataEx* GetData(uint64_t key, uint64_t child_key, const std::string* data_name)
        {
            return nullptr;
        }

        virtual AFIDataEx* GetData(uint64_t key, const std::string& data_name, uint64_t child_key)
        {
            return nullptr;
        }

        virtual bool AddData(uint64_t key, AFIDataEx* data)
        {
            return false;
        }

        virtual bool AddData(uint64_t key, uint64_t child_key, AFIDataEx* data)
        {
            return false;
        }

        virtual bool AddData(uint64_t key, const std::string& data_name, AFIDataEx* data)
        {
            return false;
        }

        virtual bool RemoveData(uint64_t key)
        {
            return false;
        }

        virtual bool RemoveData(uint64_t key, const std::string& data_name)
        {
            return false;
        }

        virtual void FindData(const std::string& data_name, uint64_t value, std::list<AFIDataEx*>& result_list) {}

        virtual bool CheckData(const std::string& data_name, uint64_t value, const std::string& check_name, uint64_t check_value)
        {
            return false;
        }

    protected:
        virtual bool GetBool() const
        {
            return NULL_BOOLEAN;
        }

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

        virtual const AFVector3D& GetVector3D() const
        {
            return NULL_VECTOR3D;
        }

        virtual void SetBool(const bool&) const {}
        virtual void SetInt32(const int32_t&) const {}
        virtual void SetUInt32(const uint32_t&) const {}
        virtual void SetInt64(const int64_t&) const {}
        virtual void SetUInt64(const uint64_t&) const {}
        virtual void SetFloat(const float&) const {}
        virtual void SetDouble(const double&) const {}
        virtual void SetString(const std::string&) const {}
        virtual void SetVector3D(const AFVector3D&) const {}

    public:
        const AFEntityMetaData* data_setting_{ nullptr };
        const AFMetaClassSetting* class_setting_{ nullptr };

    private:
        AFIDataEx* parent_;
    };

    template <>
    inline std::string AFIDataEx::Invalid()
    {
        return NULL_STR;
    }

    template <>
    inline std::string AFIDataEx::GetValue()
    {
        return GetString();
    }

    template <>
    inline void AFIDataEx::SetString(const std::string& value)
    {
        FromString(value);
    }

    template <>
    inline AFVector3D AFIDataEx::GetValue()
    {
        return GetVector3D();
    }

    template <>
    inline void AFIDataEx::SetValue(const AFVector3D& value)
    {
        SetVector3D(value);
    }
}