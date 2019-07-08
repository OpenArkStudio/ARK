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
#include "base/AFDefine.hpp"
#include "AFIClassMeta.h"
#include "AFIDataMeta.h"

namespace ark {

class AFINewData
{
    using StringMap = std::map<std::string, std::string>;

    const AFIClassMeta *class_meta_{nullptr};
    const AFIDataMeta *data_meta_{nullptr};
    AFINewData *parent_{nullptr};

public:
    virtual bool Init(const AFIClassMeta *class_meta, const AFIDataMeta *data_meta)
    {
        ARK_ASSERT_RET_VAL(class_meta != nullptr && data_meta != nullptr, false);
        class_meta_ = class_meta;
        data_meta_ = data_meta;
        return true;
    }

    const AFIClassMeta *GetClassMeta() const
    {
        return class_meta_;
    }

    const AFIDataMeta *GetDataMeta() const
    {
        return data_meta_;
    }

    virtual void Reset() = 0;
    virtual bool Valid() = 0;
    virtual uint32_t Size()
    {
        return 1;
    }

    virtual const std::string &GetName() const
    {
        return data_meta_->GetName();
    }

    virtual ArkDataType GetType() const
    {
        return data_meta_->GetType();
    }

    // Get/Set key id(in table, key means GUID, in Object, key means objectID)
    virtual uint64_t GetKeyID()
    {
        return 0;
    }

    virtual void SetKeyID(uint64_t id)
    {
        // Do nothing in base class
    }

    // Check the data has some mask
    bool HaveMask(ArkDataMaskType mask)
    {
        return data_meta_->HaveMask(mask);
    }

    AFINewData *GetParent()
    {
        return parent_;
    }

    void SetParent(AFINewData *data)
    {
        ARK_ASSERT_RET_NONE(data != nullptr);
        parent_ = data;
    }

    // Copy and save
    virtual void CopyFrom(AFINewData *other) = 0;
    virtual void SaveTo(AFINewData *other) = 0;

    // Serialize to string
    virtual std::string ToString() = 0;
    virtual void FromString(const std::string &value) = 0;

    // String map
    virtual void ToMap(StringMap &value)
    {
        // Do nothing in base class
    }

    virtual void FromMap(const StringMap &value)
    {
        // Do nothing in base class
    }

    //////////////////////////////////////////////////////////////////////////
    // build-in type
    template<typename T>
    const T &DefaultValue()
    {
        return static_cast<T>(NULL_INT);
    }

    template<typename T = uint64_t>
    const T &GetValue()
    {
        switch (GetType())
        {
        case ArkDataType::DT_BOOLEAN:
            return static_cast<T>(GetBool());
            break;
        case ArkDataType::DT_INT32:
            return static_cast<T>(GetInt());
            break;
        case ArkDataType::DT_UINT32:
            return static_cast<T>(GetUInt());
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
        // DT_STRING/DT_VECTOR3D template specialization
        default:
            break;
        }

        return DefaultValue<T>();
    }

    template<typename T = uint64_t>
    T GetValue(const std::string data_name)
    {
        auto data = GetData(data_name);
        ARK_ASSERT_RET_VAL(data != nullptr, DefaultValue<T>());

        return data->GetValue<T>();
    }

    template<typename T = uint64_t>
    T GetValue(const std::string &parent_data_name, const std::string &data_name)
    {
        auto data = GetData(parent_data_name, data_name);
        ARK_ASSERT_RET_VAL(data != nullptr, DefaultValue<T>());

        return data->GetValue<T>();
    }

    template<typename T = uint64_t>
    T GetValue(uint64_t key, const std::string &data_name)
    {
        auto data = GetData(key, data_name);
        ARK_ASSERT_RET_VAL(data != nullptr, DefaultValue<T>());

        return data->GetValue<T>();
    }

    template<typename T = uint64_t>
    T GetValue(const std::string &parent_data_name, uint64_t key, const std::string &data_name)
    {
        auto data = GetData(parent_data_name, key);
        ARK_ASSERT_RET_VAL(data != nullptr, DefaultValue<T>());

        return data->GetValue<T>(data_name);
    }

    template<typename T = uint64_t>
    void SetValue(const T &value)
    {
        switch (type_)
        {
        case ArkDataType::DT_BOOLEAN:
            SetBool(static_cast<bool>(value));
            break;
        case ArkDataType::DT_INT32:
            SetInt(static_cast<int32_t>(value));
            break;
        case ArkDataType::DT_UINT32:
            SetUInt(static_cast<uint32_t>(value));
            break;
        case ArkDataType::DT_INT64:
            SetInt64(static_cast<int64_t>(value));
            break;
        case ArkDataType::DT_UINT64:
            SetUInt64(static_cast<uint64_t>(value));
            break;
        case ArkDataType::DT_FLOAT:
            SetFloat(static_cast<float>(value));
            break;
        case ArkDataType::DT_DOUBLE:
            SetDouble(static_cast<double>(value));
            break;
        // DT_STRING/DT_VECTOR3D is template specialization
        default:
            break;
        }
    }

    template<typename T = uint64_t>
    void SetValue(const std::string &data_name, const T &value)
    {
        auto data = GetData(data_name);
        ARK_ASSERT_RET_NONE(data != nullptr);

        data->SetValue<T>(value);
    }

    template<typename T = uint64_t>
    void SetValue(const std::string &parent_data_name, const std::string &data_name, const T &value)
    {
        auto data = GetData(parent_data_name, data_name);
        ARK_ASSERT_RET_NONE(data != nullptr);

        data->SetValue<T>(value);
    }

    template<typename T = uint64_t>
    void SetValue(uint64_t key, const std::string &data_name, const T &value)
    {
        auto data = GetData(key, data_name);
        ARK_ASSERT_RET_NONE(data != nullptr);

        data->SetValue<T>(value);
    }

    template<typename T = uint64_t>
    void SetValue(const std::string &parent_data_name, uint64_t key, const std::string &data_name, const T &value)
    {
        auto data = GetData(parent_data_name, key);
        ARK_ASSERT_RET_NONE(data != nullptr);

        data->SetValue<T>(data_name, value);
    }

    template<typename T = uint64_t>
    T OpValue(ArkDataOpType op, const T &value)
    {
        auto cur_value = GetValue<T>();
        auto new_value = AFMisc::Operate<T>(op, base, value);
        SetValue<T>(new_value);
        return new_value;
    }

    template<typename T = uint64_t>
    T OpValue(const std::string &data_name, ArkDataOpType op, const T &value)
    {
        auto data = GetData(data_name);
        ARK_ASSERT_RET_VAL(data != nullptr, DefaultValue<T>());

        return data->OpValue<T>(op, value);
    }

    template<typename T = uint64_t>
    T OpValue(const std::string &parent_data_name, const std::string &data_name, ArkDataOpType op, const T &value)
    {
        auto data = GetData(parent_data_name, data_name);
        ARK_ASSERT_RET_VAL(data != nullptr, DefaultValue<T>());

        return data->OpValue(op, value);
    }

    template<typename T = uint64_t>
    void OpValue(uint64_t key, const std::string &data_name, ArkDataOpType op, const T &value)
    {
        auto data = GetData(key, data_name);
        ARK_ASSERT_RET_VAL(data != nullptr, DefaultValue<T>());

        return data->OpValue(op, value);
    }

    template<typename T = uint64_t>
    void OpValue(const std::string &data_name, uint64_t key, ArkDataOpType op, const T &value)
    {
        auto data = GetData(data_name, key);
        ARK_ASSERT_RET_VAL(data != nullptr, DefaultValue<T>());

        return data->OpValue(op, value);
    }

    template<typename T = uint64_t>
    void OpValue(const std::string &parent_data_name, uint64_t key, const std::string &data_name, ArkDataOpType op, const T &value)
    {
        auto data = GetData(parent_data_name, key, data_name);
        ARK_ASSERT_RET_VAL(data != nullptr, DefaultValue<T>());

        return data->OpValue(op, value);
    }

    //////////////////////////////////////////////////////////////////////////
    // Object
    virtual AFINewData *GetData(const std::string &data_name)
    {
        // Implement in the derived class.
        return nullptr;
    }

    virtual AFINewData *GetData(const std::string &data_name, uint64_t key)
    {
        // Implement in the derived class.
        return nullptr;
    }

    virtual AFINewData *GetData(const std::string &parent_data_name, const std::string &child_data_name)
    {
        // Implement in the derived class.
        return nullptr;
    }

    virtual AFINewData *GetData(const std::string &parent_data_name, uint64_t key, const std::string &child_data_name)
    {
        // Implement in the derived class.
        return nullptr;
    }

    virtual AFINewData *GetData(const std::string &parent_data_name, const std::string &child_data_name, uint64_t key)
    {
        // Implement in the derived class.
        return nullptr;
    }

    virtual AFINewData *GetData(const std::string &parent_data_name, const std::string &child_data_name, const std::string &data_name)
    {
        // Implement in the derived class.
        return nullptr;
    }

    virtual bool AddData(const std::string &data_name, AFINewData *data)
    {
        // Implement in the derived class.
        return false;
    }

    virtual bool AddData(const std::string &data_name, uint64_t key, AFINewData *data)
    {
        // Implement in the derived class.
        return false;
    }

    virtual bool AddData(const std::string &parent_data_name, const std::string &child_data_name, AFINewData *data)
    {
        // Implement in the derived class.
        return false;
    }

    virtual bool RemoveData(const std::stirng &data_name)
    {
        // Implement in the derived class.
        return false;
    }

    virtual bool RemoveData(const std::stirng &data_name, uint64_t key)
    {
        // Implement in the derived class.
        return false;
    }

    virtual bool RemoveData(const std::stirng &data_name, const std::string &child_data_name)
    {
        // Implement in the derived class.
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    // Table/Record
    virtual AFINewData *GetData(uint64_t key)
    {
        // Implement in the derived class.
        return nullptr;
    }

    virtual AFINewData *GetData(uint64_t key, const std::string &data_name)
    {
        // Implement in the derived class.
        return nullptr;
    }

    virtual AFINewData *GetData(uint64_t parent_key, uint64_t child_key)
    {
        // Implement in the derived class.
        return nullptr;
    }

    virtual AFINewData *GetData(uint64_t parent_key, uint64_t child_key, const std::string &data_name)
    {
        // Implement in the derived class.
        return nullptr;
    }

    virtual AFINewData *GetData(uint64_t parent_key, const std::string &data_name, uint64_t child_key)
    {
        // Implement in the derived class.
        return nullptr;
    }

    virtual bool AddData(uint64_t key, AFINewData *data)
    {
        // Implement in the derived class.
        return false;
    }

    virtual bool AddData(uint64_t parent_key, uint64_t child_key, AFINewData *data)
    {
        // Implement in the derived class.
        return false;
    }

    virtual bool AddData(uint64_t key, const std::string &data_name, AFINewData *data)
    {
        // Implement in the derived class.
        return false;
    }

    virtual bool RemoveData(uint64_t key)
    {
        // Implement in the derived class.
        return false;
    }

    virtual bool RemoveData(uint64_t key, const std::string &data_name)
    {
        // Implement in the derived class.
        return false;
    }

    virtual void FindData(const std::string &data_name, uint64_t value, std::list<AFINewData *> &find_list)
    {
        // Implement in the derived class.
    }

    virtual bool CheckData(const std::string &data_name, uint64_t value, const std::string &check_name, uint64_t chenck_value)
    {
        // Implement in the derived class.
    }

protected:
    virtual bool GetBool()
    {
        return NULL_BOOLEAN;
    }

    virtual void SetBool(bool value)
    {
        // Do nothing.
    }

    virtual int32_t GetInt()
    {
        return NULL_INT;
    }

    virtual void SetInt(int32_t value)
    {
        // Do nothing.
    }

    virtual uint32_t GetUInt()
    {
        return NULL_INT;
    }

    virtual void SetUInt(uint32_t value)
    {
        // Do nothing.
    }

    virtual int64_t GetInt64()
    {
        return NULL_INT64;
    }

    virtual void SetInt64(int64_t value)
    {
        // Do nothing.
    }

    virtual uint64_t GetUInt64()
    {
        return NULL_INT64;
    }

    virtual void SetUInt64(uint64_t value)
    {
        // Do nothing.
    }

    virtual float GetFloat()
    {
        return NULL_FLOAT;
    }

    virtual void SetFloat(float value)
    {
        // Do nothing.
    }

    virtual double GetDouble()
    {
        return NULL_DOUBLE;
    }

    virtual void SetDouble(double value)
    {
        // Do nothing.
    }

    virtual const std::string &GetString()
    {
        return NULL_STR;
    }

    virtual void SetString(std::string &value)
    {
        // Do nothing.
    }

    virtual const AFVector3D &GetVector3D()
    {
        return NULL_VECTOR3D;
    }

    virtual void SetVector3D(const AFVector3D &value)
    {
        // Do nothing.
    }
};

// template specialization for complex structure
template<>
inline const std::string &AFINewData::DefaultValue()
{
    return NULL_STR;
}

template<>
inline const AFVector3D &AFINewData::DefaultValue()
{
    return NULL_VECTOR3D;
}

template<>
inline const std::string &AFINewData::GetValue()
{
    return GetString();
}

template<>
inline void AFINewData::SetValue(std::string value)
{
    FromString(value);
}

template<>
inline void AFINewData::SetValue(const std::string &value)
{
    FromString(value);
}

template<>
inline const AFVector3D &AFINewData::GetValue()
{
    return GetVector3D();
}

template<>
inline void AFINewData::SetValue(AFVector3D value)
{
    SetVector3D(value);
}

template<>
inline void AFINewData::SetValue(const AFVector3D &value)
{
    SetVector3D(value);
}
} // namespace ark