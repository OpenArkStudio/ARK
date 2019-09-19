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

#include "plugin/kernel/interface/AFINode.hpp"

namespace ark {

#define DATA_NODE_DECLARE                                                                                              \
    void SetMeta(ARK_SHARE_PTR<AFNodeMeta> pDataMeta) override                                                         \
    {                                                                                                                  \
        data_meta_ = pDataMeta;                                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    ARK_SHARE_PTR<AFNodeMeta> GetMeta() override                                                                       \
    {                                                                                                                  \
        return data_meta_;                                                                                             \
    }                                                                                                                  \
                                                                                                                       \
    const std::string& GetName() override                                                                              \
    {                                                                                                                  \
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, NULL_STR);                                                           \
                                                                                                                       \
        return data_meta_->GetName();                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    uint32_t GetIndex() const override                                                                                 \
    {                                                                                                                  \
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, 0);                                                                  \
                                                                                                                       \
        return data_meta_->GetIndex();                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    ArkMaskType GetMask() const override                                                                               \
    {                                                                                                                  \
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, 0);                                                                  \
                                                                                                                       \
        return data_meta_->GetMask();                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    bool HaveMask(ArkNodeMask mask) override                                                                           \
    {                                                                                                                  \
        ARK_ASSERT_RET_VAL(data_meta_ != nullptr, false);                                                              \
                                                                                                                       \
        return data_meta_->HaveMask(mask);                                                                             \
    }                                                                                                                  \
                                                                                                                       \
private:                                                                                                               \
    ARK_SHARE_PTR<AFNodeMeta> data_meta_{nullptr};                                                                     \
                                                                                                                       \
// data int32_t
class AFNodeInt32 final : public AFINode
{
public:
    explicit AFNodeInt32(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFNodeInt32() override = default;

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

    void CopyFrom(AFINode* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetInt32(other->GetInt32());
    }

    void SaveTo(AFINode* other) override
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

    ID_TYPE GetValue() const override
    {
        return static_cast<ID_TYPE>(data_);
    }

    DATA_NODE_DECLARE

    // data value
    int32_t data_{NULL_INT};
};

// data uint32_t
class AFNodeUInt32 final : public AFINode
{
public:
    explicit AFNodeUInt32(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFNodeUInt32() override = default;

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

    void CopyFrom(AFINode* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetInt32(other->GetUInt32());
    }

    void SaveTo(AFINode* other) override
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

    ID_TYPE GetValue() const override
    {
        return static_cast<ID_TYPE>(data_);
    }

    DATA_NODE_DECLARE

    // data value
    uint32_t data_{NULL_INT};
};

// data string
class AFNodeString final : public AFINode
{
public:
    explicit AFNodeString(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFNodeString() override = default;

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

    void CopyFrom(AFINode* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetString(other->GetString());
    }

    void SaveTo(AFINode* other) override
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

    ID_TYPE GetValue() const override
    {
        return AFMisc::FromString<ID_TYPE>(data_);
    }

    DATA_NODE_DECLARE

    // data value
    std::string data_{NULL_STR};
};

// data bool
class AFNodeBool final : public AFINode
{
public:
    explicit AFNodeBool(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFNodeBool() override = default;

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

    void CopyFrom(AFINode* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetBool(other->GetBool());
    }

    void SaveTo(AFINode* other) override
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

    ID_TYPE GetValue() const override
    {
        return static_cast<ID_TYPE>(data_);
    }

    DATA_NODE_DECLARE

    // data value
    bool data_{NULL_BOOLEAN};
};

// data int64
class AFNodeInt64 final : public AFINode
{
public:
    explicit AFNodeInt64(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFNodeInt64() override = default;

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

    void CopyFrom(AFINode* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetInt64(other->GetInt64());
    }

    void SaveTo(AFINode* other) override
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

    ID_TYPE GetValue() const override
    {
        return static_cast<ID_TYPE>(data_);
    }

    DATA_NODE_DECLARE

    // data value
    int64_t data_{NULL_INT64};
};

// data uint64
class AFNodeUInt64 final : public AFINode
{
public:
    explicit AFNodeUInt64(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFNodeUInt64() override = default;

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

    void CopyFrom(AFINode* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetUInt64(other->GetUInt64());
    }

    void SaveTo(AFINode* other) override
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

    ID_TYPE GetValue() const override
    {
        return static_cast<ID_TYPE>(data_);
    }

    DATA_NODE_DECLARE

    // data value
    uint64_t data_{NULL_INT64};
};

// data float
class AFNodeFloat final : public AFINode
{
public:
    explicit AFNodeFloat(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFNodeFloat() override = default;

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

    void CopyFrom(AFINode* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetFloat(other->GetFloat());
    }

    void SaveTo(AFINode* other) override
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

    ID_TYPE GetValue() const override
    {
        return static_cast<ID_TYPE>(data_);
    }

    DATA_NODE_DECLARE

    // data value
    float data_{NULL_FLOAT};
};

// data double
class AFNodeDouble final : public AFINode
{
public:
    explicit AFNodeDouble(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFNodeDouble() override = default;

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

    void CopyFrom(AFINode* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetDouble(other->GetDouble());
    }

    void SaveTo(AFINode* other) override
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

    ID_TYPE GetValue() const override
    {
        return static_cast<ID_TYPE>(data_);
    }

    DATA_NODE_DECLARE

    // data value
    double data_{NULL_DOUBLE};
};

// data object
class AFNodeGUID final : public AFINode
{
public:
    explicit AFNodeGUID(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
    {
        data_meta_ = pDataMeta;
    }

    ~AFNodeGUID() override = default;

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

    void CopyFrom(AFINode* other) override
    {
        ARK_ASSERT_RET_NONE(data_meta_ != nullptr && other != nullptr);
        ARK_ASSERT_RET_NONE(data_meta_->GetType() == other->GetType());

        SetObject(other->GetObject());
    }

    void SaveTo(AFINode* other) override
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

    ID_TYPE GetValue() const override
    {
        return static_cast<ID_TYPE>(data_);
    }

    DATA_NODE_DECLARE

    // data value
    AFGUID data_{NULL_GUID};
};

// create new data
static AFINode* CreateDataByMeta(ARK_SHARE_PTR<AFNodeMeta> pDataMeta)
{
    ARK_ASSERT_RET_VAL(pDataMeta != nullptr, nullptr);

    AFINode* pData = nullptr;
    switch (pDataMeta->GetType())
    {
        case ark::ArkDataType::DT_BOOLEAN:
            pData = ARK_NEW AFNodeBool(pDataMeta);
            break;
        case ark::ArkDataType::DT_INT32:
            pData = ARK_NEW AFNodeInt32(pDataMeta);
            break;
        case ark::ArkDataType::DT_UINT32:
            pData = ARK_NEW AFNodeUInt32(pDataMeta);
            break;
        case ark::ArkDataType::DT_INT64:
            pData = ARK_NEW AFNodeInt64(pDataMeta);
            break;
        case ark::ArkDataType::DT_UINT64:
            pData = ARK_NEW AFNodeUInt64(pDataMeta);
            break;
        case ark::ArkDataType::DT_FLOAT:
            pData = ARK_NEW AFNodeFloat(pDataMeta);
            break;
        case ark::ArkDataType::DT_DOUBLE:
            pData = ARK_NEW AFNodeDouble(pDataMeta);
            break;
        case ark::ArkDataType::DT_STRING:
            pData = ARK_NEW AFNodeString(pDataMeta);
            break;
        case ark::ArkDataType::DT_VECTOR3D:
            break;
        case ark::ArkDataType::DT_ARRAY:
            break;
        case ark::ArkDataType::DT_TABLE:
            break;
        case ark::ArkDataType::DT_GUID:
            pData = ARK_NEW AFNodeGUID(pDataMeta);
            break;
        default:
            break;
    }

    return pData;
}

} // namespace ark
