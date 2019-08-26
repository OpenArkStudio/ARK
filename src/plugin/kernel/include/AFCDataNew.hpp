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

#include "plugin/kernel/interface/AFIDataNew.hpp"

namespace ark {

#define DATA_NEW_META_OP_DECLARE                                                                                       \
    virtual void SetMeta(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)                                                       \
    {                                                                                                                  \
        data_meta_ = pDataMeta;                                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    virtual ARK_SHARE_PTR<AFDataNewMeta> GetMeta()                                                                     \
    {                                                                                                                  \
        return data_meta_;                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    virtual const std::string& GetName()                                                                               \
    {                                                                                                                  \
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, NULL_STR);                                                           \
                                                                                                                       \
        return data_meta_->GetName();                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
private:                                                                                                               \
    ARK_SHARE_PTR<AFDataNewMeta> data_meta_{nullptr};

// data int32_t
class AFDataInt32 final : public AFIDataNew
{
public:
    explicit AFDataInt32(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFDataInt32() override = default;

    ArkDataType GetType() const override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, ArkDataType::DT_EMPTY);
        return data_meta_->GetType();
    }

    void Reset() override
    {
        data_ = NULL_INT;
    }

    bool IsNull() const override
    {
        return NULL_INT == data_;
    }

    void CopyFrom(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetInt32(other->GetInt32());
    }

    void SaveTo(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        other->SetInt32(GetInt32());
    }

    int32_t GetInt32() const override
    {
        return data_;
    }

    void SetInt32(int32_t value) override
    {
        data_ = value;
    }

    std::string ToString() const override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString<int32_t>(value);
    }

    bool HaveMask(ArkDataMaskType mask) override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, false);

        return data_meta_->HaveMask(mask);
    }

    DATA_NEW_META_OP_DECLARE

    // data value
    int32_t data_{NULL_INT};
};

// data uint32_t
class AFDataUInt32 final : public AFIDataNew
{
public:
    explicit AFDataUInt32(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFDataUInt32() override = default;

    ArkDataType GetType() const override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, ArkDataType::DT_EMPTY);
        return data_meta_->GetType();
    }

    void Reset() override
    {
        data_ = NULL_INT;
    }

    bool IsNull() const override
    {
        return NULL_INT == data_;
    }

    void CopyFrom(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetInt32(other->GetUInt32());
    }

    void SaveTo(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        other->SetUInt32(GetUInt32());
    }

    uint32_t GetUInt32() const override
    {
        return data_;
    }

    void SetUInt32(uint32_t value) override
    {
        data_ = value;
    }

    std::string ToString() const override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString<uint32_t>(value);
    }

    bool HaveMask(ArkDataMaskType mask) override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, false);

        return data_meta_->HaveMask(mask);
    }

    DATA_NEW_META_OP_DECLARE

    // data value
    uint32_t data_{NULL_INT};
};

// data string
class AFDataString final : public AFIDataNew
{
public:
    explicit AFDataString(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFDataString() override = default;

    ArkDataType GetType() const override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, ArkDataType::DT_EMPTY);
        return data_meta_->GetType();
    }

    void Reset() override
    {
        data_ = NULL_STR;
    }

    bool IsNull() const override
    {
        return data_.empty();
    }

    void CopyFrom(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetString(other->GetString());
    }

    void SaveTo(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        other->SetString(GetString());
    }

    const std::string& GetString() const override
    {
        return data_;
    }

    void SetString(const std::string& value) override
    {
        data_ = value;
    }

    std::string ToString() const override
    {
        return data_;
    }

    void FromString(const std::string& value) override
    {
        data_ = value;
    }

    bool HaveMask(ArkDataMaskType mask) override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, false);

        return data_meta_->HaveMask(mask);
    }

    DATA_NEW_META_OP_DECLARE

    // data value
    std::string data_{NULL_STR};
};

// data bool
class AFDataBool final : public AFIDataNew
{
public:
    explicit AFDataBool(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFDataBool() override = default;

    ArkDataType GetType() const override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, ArkDataType::DT_EMPTY);
        return data_meta_->GetType();
    }

    void Reset() override
    {
        data_ = NULL_BOOLEAN;
    }

    bool IsNull() const override
    {
        return data_ == NULL_BOOLEAN;
    }

    void CopyFrom(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetBool(other->GetBool());
    }

    void SaveTo(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        other->SetBool(GetBool());
    }

    bool GetBool() const override
    {
        return data_;
    }

    void SetBool(bool value) override
    {
        data_ = value;
    }

    std::string ToString() const override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString<bool>(value);
    }

    bool HaveMask(ArkDataMaskType mask) override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, false);

        return data_meta_->HaveMask(mask);
    }

    DATA_NEW_META_OP_DECLARE

    // data value
    bool data_{NULL_BOOLEAN};
};

// data int64
class AFDataInt64 final : public AFIDataNew
{
public:
    explicit AFDataInt64(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFDataInt64() override = default;

    ArkDataType GetType() const override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, ArkDataType::DT_EMPTY);
        return data_meta_->GetType();
    }

    void Reset() override
    {
        data_ = NULL_BOOLEAN;
    }

    bool IsNull() const override
    {
        return data_ == NULL_INT;
    }

    void CopyFrom(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetInt64(other->GetInt64());
    }

    void SaveTo(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        other->SetInt64(GetInt64());
    }

    int64_t GetInt64() const override
    {
        return data_;
    }

    void SetInt64(int64_t value) override
    {
        data_ = value;
    }

    std::string ToString() const override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString<int64_t>(value);
    }

    bool HaveMask(ArkDataMaskType mask) override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, false);

        return data_meta_->HaveMask(mask);
    }

    DATA_NEW_META_OP_DECLARE

    // data value
    int64_t data_{NULL_INT64};
};

// data uint64
class AFDataUInt64 final : public AFIDataNew
{
public:
    explicit AFDataUInt64(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFDataUInt64() override = default;

    ArkDataType GetType() const override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, ArkDataType::DT_EMPTY);
        return data_meta_->GetType();
    }

    void Reset() override
    {
        data_ = NULL_BOOLEAN;
    }

    bool IsNull() const override
    {
        return data_ == NULL_INT;
    }

    void CopyFrom(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetUInt64(other->GetUInt64());
    }

    void SaveTo(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        other->SetUInt64(GetUInt64());
    }

    uint64_t GetUInt64() const override
    {
        return data_;
    }

    void SetUInt64(uint64_t value) override
    {
        data_ = value;
    }

    std::string ToString() const override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString<uint64_t>(value);
    }

    bool HaveMask(ArkDataMaskType mask) override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, false);

        return data_meta_->HaveMask(mask);
    }

    DATA_NEW_META_OP_DECLARE

    // data value
    uint64_t data_{NULL_INT64};
};

// data float
class AFDataFloat final : public AFIDataNew
{
public:
    explicit AFDataFloat(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFDataFloat() override = default;

    ArkDataType GetType() const override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, ArkDataType::DT_EMPTY);
        return data_meta_->GetType();
    }

    void Reset() override
    {
        data_ = NULL_BOOLEAN;
    }

    bool IsNull() const override
    {
        return data_ == NULL_INT;
    }

    void CopyFrom(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetFloat(other->GetFloat());
    }

    void SaveTo(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        other->SetFloat(GetFloat());
    }

    float GetFloat() const override
    {
        return data_;
    }

    void SetFloat(float value) override
    {
        data_ = value;
    }

    std::string ToString() const override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString<float>(value);
    }

    bool HaveMask(ArkDataMaskType mask) override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, false);

        return data_meta_->HaveMask(mask);
    }

    DATA_NEW_META_OP_DECLARE

    // data value
    float data_{NULL_FLOAT};
};

// data double
class AFDataDouble final : public AFIDataNew
{
public:
    explicit AFDataDouble(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFDataDouble() override = default;

    ArkDataType GetType() const override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, ArkDataType::DT_EMPTY);
        return data_meta_->GetType();
    }

    void Reset() override
    {
        data_ = NULL_BOOLEAN;
    }

    bool IsNull() const override
    {
        return data_ == NULL_INT;
    }

    void CopyFrom(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetDouble(other->GetDouble());
    }

    void SaveTo(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        other->SetDouble(GetDouble());
    }

    double GetDouble() const override
    {
        return data_;
    }

    void SetDouble(double value) override
    {
        data_ = value;
    }

    std::string ToString() const override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString<double>(value);
    }

    bool HaveMask(ArkDataMaskType mask) override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, false);

        return data_meta_->HaveMask(mask);
    }

    DATA_NEW_META_OP_DECLARE

    // data value
    double data_{NULL_DOUBLE};
};

// data object
class AFDataObejct final : public AFIDataNew
{
public:
    explicit AFDataObejct(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFDataObejct() override = default;

    ArkDataType GetType() const override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, ArkDataType::DT_EMPTY);
        return data_meta_->GetType();
    }

    void Reset() override
    {
        data_ = NULL_BOOLEAN;
    }

    bool IsNull() const override
    {
        return data_ == NULL_INT;
    }

    void CopyFrom(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetObject(other->GetObject());
    }

    void SaveTo(AFIDataNew* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        other->SetObject(GetObject());
    }

    const AFGUID& GetObject() const override
    {
        return data_;
    }

    void SetObject(const AFGUID& guid) override
    {
        data_ = guid;
    }

    std::string ToString() const override
    {
        return AFMisc::ToString(data_);
    }

    void FromString(const std::string& value) override
    {
        data_ = AFMisc::FromString<AFGUID>(value);
    }

    bool HaveMask(ArkDataMaskType mask) override
    {
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, false);

        return data_meta_->HaveMask(mask);
    }

    DATA_NEW_META_OP_DECLARE

    // data value
    AFGUID data_{NULL_GUID};
};

// create new data
static AFIDataNew* CreateDataByMeta(ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
{
    ARK_ASSERT_RET_VAL(pDataMeta != nullptr, nullptr);

    AFIDataNew* pData = nullptr;
    switch (pDataMeta->GetType())
    {
    case ark::ArkDataType::DT_BOOLEAN:
        pData = ARK_NEW AFDataBool(pDataMeta);
        break;
    case ark::ArkDataType::DT_INT32:
        pData = ARK_NEW AFDataInt32(pDataMeta);
        break;
    case ark::ArkDataType::DT_UINT32:
        pData = ARK_NEW AFDataUInt32(pDataMeta);
        break;
    case ark::ArkDataType::DT_INT64:
        pData = ARK_NEW AFDataInt64(pDataMeta);
        break;
    case ark::ArkDataType::DT_UINT64:
        pData = ARK_NEW AFDataUInt64(pDataMeta);
        break;
    case ark::ArkDataType::DT_FLOAT:
        pData = ARK_NEW AFDataFloat(pDataMeta);
        break;
    case ark::ArkDataType::DT_DOUBLE:
        pData = ARK_NEW AFDataDouble(pDataMeta);
        break;
    case ark::ArkDataType::DT_STRING:
        pData = ARK_NEW AFDataString(pDataMeta);
        break;
    case ark::ArkDataType::DT_VECTOR3D:
        break;
    case ark::ArkDataType::DT_ARRAY:
        break;
    case ark::ArkDataType::DT_TABLE:
        break;
    case ark::ArkDataType::DT_OBJECT:
        pData = ARK_NEW AFDataObejct(pDataMeta);
        break;
    default:
        break;
    }

    return pData;
}

} // namespace ark
