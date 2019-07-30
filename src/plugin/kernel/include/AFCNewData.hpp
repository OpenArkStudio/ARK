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

#include "interface/AFINewData.h"
#include "AFVector.hpp"
#include "AFMap.hpp"

namespace ark {
// bool
class AFBoolData : public AFINewData
{
    int32_t data_{NULL_INT};

public:
    void Reset() override
    {
        data_ = NULL_BOOLEAN;
    }

    bool Valid() override
    {
        return data_ != NULL_BOOLEAN;
    }

    void CopyFrom(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        SetValue(other->GetValue<bool>());
    }

    void SaveTo(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        other->SetValue(GetValue<bool>())
    }

    std::string ToString() override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString(value);
    }

protected:
    bool GetBool() override
    {
        return data_;
    }

    void SetBool(bool value) override
    {
        data_ = value;
    }

private:
    bool data_{NULL_BOOLEAN};
};

// int32_t
class AFIntData : public AFINewData
{
public:
    void Reset() override
    {
        data_ = NULL_INT;
    }

    bool Valid() override
    {
        return data_ != NULL_INT;
    }

    void CopyFrom(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        SetValue(other->GetValue<int32_t>());
    }

    void SaveTo(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        other->SetValue(GetValue<int32_t>())
    }

    std::string ToString() override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString(value);
    }

protected:
    int32_t GetInt() override
    {
        return data_;
    }

    void SetInt(int32_t value) override
    {
        data_ = value;
    }
};

// uint32_t
class AFUIntData : public AFINewData
{
    uint32_t data_{NULL_INT};

public:
    void Reset() override
    {
        data_ = NULL_INT;
    }

    bool Valid() override
    {
        return data_ != NULL_INT;
    }

    void CopyFrom(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        SetValue(other->GetValue<uint32_t>());
    }

    void SaveTo(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        other->SetValue(GetValue<uint32_t>())
    }

    std::string ToString() override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString(value);
    }

protected:
    uint32_t GetUInt() override
    {
        return data_;
    }

    void SetUInt(uint32_t value) override
    {
        data_ = value;
    }
};

// int64_t
class AFInt64Data : public AFINewData
{
    int64_t data_{NULL_INT64};

public:
    void Reset() override
    {
        data_ = NULL_INT64;
    }

    bool Valid() override
    {
        return data_ != NULL_INT64;
    }

    void CopyFrom(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        SetValue(other->GetValue<int64_t>());
    }

    void SaveTo(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        other->SetValue(GetValue<int64_t>())
    }

    std::string ToString() override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString(value);
    }

protected:
    int64_t GetInt64() override
    {
        return data_;
    }

    void SetInt64(int64_t value) override
    {
        data_ = value;
    }
};

// uint64_t
class AFUInt64Data : public AFINewData
{
    uint64_t data_{NULL_INT64};

public:
    void Reset() override
    {
        data_ = NULL_INT64;
    }

    bool Valid() override
    {
        return data_ != NULL_INT64;
    }

    void CopyFrom(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        SetValue(other->GetValue<uint64_t>());
    }

    void SaveTo(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        other->SetValue(GetValue<uint64_t>())
    }

    std::string ToString() override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString(value);
    }

protected:
    uint64_t GetUInt64() override
    {
        return data_;
    }

    void SetUInt(uint64_t value) override
    {
        data_ = value;
    }
};

// float
class AFFloatData : public AFINewData
{
    float data_{NULL_FLOAT};

public:
    void Reset() override
    {
        data_ = NULL_FLOAT;
    }

    bool Valid() override
    {
        return data_ != NULL_FLOAT;
    }

    void CopyFrom(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        SetValue(other->GetValue<float>());
    }

    void SaveTo(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        other->SetValue(GetValue<float>())
    }

    std::string ToString() override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString(value);
    }

protected:
    float GetFloat() override
    {
        return data_;
    }

    void SetFloat(float value) override
    {
        data_ = value;
    }
};

// double
class AFDoubleData : public AFINewData
{
    double data_{NULL_DOUBLE};

public:
    void Reset() override
    {
        data_ = NULL_DOUBLE;
    }

    bool Valid() override
    {
        return data_ != NULL_DOUBLE;
    }

    void CopyFrom(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        SetValue(other->GetValue<double>());
    }

    void SaveTo(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        other->SetValue(GetValue<double>())
    }

    std::string ToString() override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString(value);
    }

protected:
    double GetDouble() override
    {
        return data_;
    }

    void SetDouble(double value) override
    {
        data_ = value;
    }
};

// std::string
class AFStringData : public AFINewData
{
    std::string data_{NULL_STR};

public:
    void Reset() override
    {
        data_ = NULL_STR;
    }

    bool Valid() override
    {
        return data_ != NULL_STR;
    }

    void CopyFrom(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        SetValue(other->GetValue<std::string>());
    }

    void SaveTo(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        other->SetValue(GetValue<std::string>())
    }

    std::string ToString() override
    {
        return data_;
    }

    void FromString(const std::string& value) override
    {
        data_ = value;
    }

protected:
    const std::string& GetString() override
    {
        return data_;
    }

    void SetString(const std::string& value) override
    {
        data_ = value;
    }
};

// AFVector3D
class AFVec3DData : public AFINewData
{
    AFVector3D data_{NULL_VECTOR3D};

public:
    void Reset() override
    {
        data_ = NULL_VECTOR3D;
    }

    bool Valid() override
    {
        return data_ != NULL_VECTOR3D;
    }

    void CopyFrom(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        SetValue(other->GetValue<AFVector3D>());
    }

    void SaveTo(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        other->SetValue(GetValue<AFVector3D>())
    }

    std::string ToString() override
    {
        return data_.ToString();
    }

    void FromString(const std::string& value) override
    {
        data_.FromString(value)
    }

protected:
    const AFVector3D& GetVector3D() override
    {
        return data_;
    }

    void SetString(const AFVector3D& value) override
    {
        data_ = value;
    }
};

// array of AFINewData
class AFArrayData : public AFINewData
{
    AFPtrVector<AFINewData> vec_data_;

public:
    void Init(const AFIClassMeta* class_meta, const AFIDataMeta* data_meta) override
    {
        AFINewData::Init(class_meta, data_meta);
        auto size = AFMisc::FromString<uint32_t>(data_meta->GetInitValue());
        vec_data_.resize(size);

        for (auto i = 0; i < size; ++i)
        {
            AFINewData* data = AFDataFactory::Create(data_meta->GetSubMetaClass());
            data->SetParent(this);
            data->Init(class_meta, data_meta);

            data->SetValue(0u);
            vec_data_.insert(i, data);
        }
    }

    uint32_t Size() override
    {
        return static_cast<uint32_t>(vec_data_.size());
    }

    void Reset() override
    {
        for (auto& sub : vec_data_)
        {
            sub->Reset();
        }
    }

    bool Valid() override
    {
        return true;
    }

    void CopyFrom(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        other->SaveTo(this);
    }

    void SaveTo(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        std::size_t key = 0;
        for (auto sub : datas_)
        {
            auto data = other->GetData(key++);
            if (data != nullptr)
            {
                sub->SaveTo(data);
            }
        }
    }

    AFINewData* GetData(uint64_t key) override
    {
        return vec_data_[static_cast<std::size_t>(key)];
    }

    AFINewData* GetData(uint64_t key, const std::string& data_name) override
    {
        auto data = GetData(key);
        if (data == nullptr)
        {
            return nullptr;
        }

        return data->GetData(data_name);
    }

    bool AddData(uint64_t key, AFINewData* data) override
    {
        vec_data_.push_back(key, data);
        return true;
    }

    bool AddData(uint64_t key, const std::string& data_name, AFINewData* data) override
    {
        auto find_data = GetData(key);
        if (find_data == nullptr)
        {
            return false;
        }

        return find_data->AddData(data_name, data);
    }

    bool RemoveData(uint64_t key) override
    {
        auto data = GetData(key);
        if (data == nullptr)
        {
            return false;
        }

        data->Reset();
        return true;
    }

    bool RemoveData(uint64_t key, const std::string& data_name) override
    {
        auto data = GetData(key);
        if (data == nullptr)
        {
            return false;
        }

        return data->RemoveData(data_name);
    }

    std::string ToString() override
    {
        // TODO: do not need for now
        return NULL_STR;
    }

    void FromString(const std::string& value) override
    {
        // TODO: do not need for now
    }
};

// Table of AFINewData
class AFTableData final : public AFINewData
{
    AFNewHashmap<uint64_t, AFINewData> map_datas_;

public:
    void Reset() override
    {
        map_datas_.clear();
    }

    bool Valid() override
    {
        return map_datas_.size() > 0;
    }

    uint32_t Size() override
    {
        return map_datas_.size();
    }

    void CopyFrom(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        other->SaveTo(this);
    }

    void SaveTo(AFINewData* other) override
    {
        ARK_ASSERT_RET_NONE(other != nullptr);
        // SAVE this TO other
        for (auto data : map_datas_)
        {
            auto new_data = AFDataFactory::CreateData(data->GetDataMeta());
            if (new_data != nullptr)
            {
                data->SaveTo(new_data);
                AddData(new_data->GetKeyID(), new_data);
            }
        }
    }

    AFINewData* GetData(uint64_t key) override
    {
        return map_datas_.find(key);
    }

    AFINewData* GetData(uint64_t key, const std::string& data_name) override
    {
        auto data = GetData(key);
        if (data == nullptr)
        {
            return nullptr;
        }

        return data->GetData(data_name);
    }

    AFINewData* GetData(uint64_t parent_key, uint64_t child_key) override
    {
        auto data = GetData(parent_key);
        if (data == nullptr)
        {
            return nullptr;
        }

        return data->GetData(child_key);
    }

    AFINewData* GetData(uint64_t parent_key, uint64_t child_key, const std::string& data_name) override
    {
        auto data = GetData(parent_key);
        if (data == nullptr)
        {
            return nullptr;
        }

        return data->GetData(child_key, data_name);
    }

    AFINewData* GetData(uint64_t parent_key, const std::string& data_name, uint64_t child_key) override
    {
        auto data = GetData(parent_key);
        if (data == nullptr)
        {
            return nullptr;
        }

        return data->GetData(data_name, child_key);
    }

    bool AddData(uint64_t key, AFINewData* data) override
    {
        data->SetKeyID(key);
        data->SetParent(this);
        return map_datas_.insert(key, data).second;
    }

    bool AddData(uint64_t parent_key, uint64_t child_key, AFINewData* data) override
    {
        auto find_data = GetData(parent_key);
        if (find_data == nullptr)
        {
            return false;
        }

        return find_data->AddData(child_key, data);
    }

    bool AddData(uint64_t key, const std::string& data_name, AFINewData* data) override
    {
        auto find_data = GetData(key);
        if (find_data == nullptr)
        {
            return false;
        }

        return find_data->AddData(data_name, data);
    }

    bool RemoveData(uint64_t key) override
    {
        return map_datas_.erase(key);
    }

    bool RemoveData(uint64_t key, const std::string& data_name) override
    {
        auto data = GetData(key);
        if (data == nullptr)
        {
            return false;
        }

        return data->RemoveData(data_name);
    }

    void FindData(const std::string& data_name, uint64_t value, std::list<AFINewData*>& find_list) override
    {
        for (auto data : map_datas_)
        {
            auto data_value = data.second->GetValue(data_name);
            if (data_value == value)
            {
                find_list.emplace_back(data);
            }
        }
    }

    bool CheckData(
        const std::string& data_name, uint64_t value, const std::string& check_name, uint64_t chenck_value) override
    {
        // TODO: do not need for now.
    }

    std::string ToString() override
    {
        // TODO: do not need for now
        return NULL_STR;
    }

    void FromString(const std::string& value) override
    {
        // TODO: do not need for now
    }
};

// Object
class AFObjectData : public AFINewData
{
    uint64_t key_;
    AFNewHashmap<std::string, AFINewData> map_datas_;

public:
    void Reset() override
    {
        key_ = NULL_INT64;
        for (auto data : map_datas_)
        {
            data.second->Reset();
        }
    }

    bool Valid() override
    {
        return true;
    }

    uint32_t Size() override
    {
        return map_datas_.size();
    }

    uint64_t GetKeyID() override
    {
        return key_;
    }

    uint64_t SetKeyID(uint64_t id) override
    {
        key_ = id;
        SetValue<uint64_t>(data_meta_->GetKey(), id);
    }

    AFINewData* GetData(const std::string& data_name) override
    {
        return map_datas_.find_value(data_name);
    }

    AFINewData* GetData(const std::string& data_name, uint64_t key) override
    {
        auto data = GetData(data_name);
        if (data == nullptr)
        {
            return nullptr;
        }

        return data->GetData(key);
    }

    AFINewData* GetData(const std::string& parent_data_name, const std::string& child_data_name) override
    {
        if (parent_data_name.empty())
        {
            return GetData(child_data_name);
        }

        auto data = GetData(parent_data_name);
        if (data == nullptr)
        {
            return nullptr;
        }

        return data->GetData(child_data_name);
    }

    AFINewData* GetData(const std::string& parent_data_name, uint64_t key, const std::string& child_data_name) override
    {
        auto data = GetData(parent_data_name);
        if (data == nullptr)
        {
            return nullptr;
        }

        return data->GetData(key, child_data_name);
    }

    AFINewData* GetData(const std::string& parent_data_name, const std::string& child_data_name, uint64_t key) override
    {
        auto data = GetData(parent_data_name);
        if (data == nullptr)
        {
            return nullptr;
        }

        return data->GetData(child_data_name, key);
    }

    AFINewData* GetData(
        const std::string& parent_data_name, const std::string& child_data_name, const std::string& data_name) override
    {
        auto data = GetData(parent_data_name);
        if (data == nullptr)
        {
            return nullptr;
        }

        return data->GetData(child_data_name, data_name);
    }

    bool AddData(const std::string& data_name, AFINewData* data) override
    {
        data->SetParent(this);
        return map_datas_.insert(data_name, data).second;
    }

    bool AddData(const std::string& data_name, uint64_t key, AFINewData* data) override
    {
        auto data = GetData(data_name);
        if (data == nullptr)
        {
            return false;
        }

        return AddData(key, data);
    }

    bool AddData(const std::string& parent_data_name, const std::string& child_data_name, AFINewData* data) override
    {
        auto data = GetData(data_name);
        if (data == nullptr)
        {
            return false;
        }

        return AddData(child_data_name, data);
    }

    bool RemoveData(const std::stirng& data_name) override
    {
        return map_datas_.erase(data_name);
    }

    bool RemoveData(const std::stirng& data_name, uint64_t key) override
    {
        auto data = GetData(data_name);
        if (data == nullptr)
        {
            return false;
        }

        return data->RemoveData(key);
    }

    bool RemoveData(const std::stirng& data_name, const std::string& child_data_name) override
    {
        auto data = GetData(data_name);
        if (data == nullptr)
        {
            return false;
        }

        return data->RemoveData(child_data_name);
    }

    void CopyFrom(AFINewData* other) override
    {
        SetKeyID(other->GetKeyID());

        for (auto iter : map_datas_)
        {
            auto& key = iter.first;
            auto data = iter.second;
            auto find_data = other->GetData(key);
            if (find_data != nullptr)
            {
                data->CopyFrom(find_data);
            }
        }
    }

    void SaveTo(AFINewData* other)
    {
        other->CopyFrom(this);
    }

    std::string ToString() override
    {
        // Unused right now
        return NULL_STR;
    }

    void FromString(const std::string& value) override
    {
        // Unused right now
    }

    void ToMap(StringMap& value) override
    {
        // Unused right now
    }

    void FromMap(const StringMap& value) override
    {
        // Unused right now
    }
};

} // namespace ark