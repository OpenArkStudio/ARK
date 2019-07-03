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

#include "interface/AFIDataEx.hpp"

namespace ark
{
    //bool
    class AFBoolData : public AFIDataEx
    {
    public:
        AFBoolData() = default;

        void Clear() override
        {
            data_ = NULL_BOOLEAN;
        }

        bool IsValid() override
        {
            return data_ != NULL_BOOLEAN;
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            SetValue(rhs->GetValue<bool>());
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            rhs->SetValue(GetValue<bool>());
        }

        std::string ToString() override
        {
            return ARK_TO_STRING(data_);
        }

        void FromString(const std::string& value)
        {
            AFMisc::FromString(value, data_);
        }

    protected:
        bool GetBool() override
        {
            return data_;
        }

        void SetBool(const bool& value) override
        {
            data_ = value;
        }

    private:
        bool data_{ NULL_BOOLEAN };
    };

    //int32_t
    class AFInt32Data : public AFIDataEx
    {
    public:
        AFInt32Data() = default;

        void Clear() override
        {
            data_ = NULL_INT;
        }

        bool IsValid() override
        {
            return data_ != NULL_INT;
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            SetValue(rhs->GetValue<int32_t>());
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            rhs->SetValue(GetValue<int32_t>());
        }

        std::string ToString() override
        {
            return ARK_TO_STRING(data_);
        }

        void FromString(const std::string& value)
        {
            AFMisc::FromString(value, data_);
        }

    protected:
        int32_t GetInt32() override
        {
            return data_;
        }

        void SetInt32(const int32_t& value) override
        {
            data_ = value;
        }

    private:
        int32_t data_{ NULL_INT };
    };

    //uint32_t
    class AFUInt32Data : public AFIDataEx
    {
    public:
        AFUInt32Data() = default;

        void Clear() override
        {
            data_ = NULL_INT;
        }

        bool IsValid() override
        {
            return data_ != NULL_INT;
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            SetValue(rhs->GetValue<uint32_t>());
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            rhs->SetValue(GetValue<uint32_t>());
        }

        std::string ToString() override
        {
            return ARK_TO_STRING(data_);
        }

        void FromString(const std::string& value)
        {
            AFMisc::FromString(value, data_);
        }

    protected:
        uint32_t GetUInt32() override
        {
            return data_;
        }

        void SetUInt32(const uint32_t& value) override
        {
            data_ = value;
        }

    private:
        uint32_t data_{ NULL_INT };
    };

    //int64_t
    class AFInt64Data : public AFIDataEx
    {
    public:
        AFInt64Data() = default;

        void Clear() override
        {
            data_ = NULL_INT64;
        }

        bool IsValid() override
        {
            return data_ != NULL_INT64;
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            SetValue(rhs->GetValue<int64_t>());
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            rhs->SetValue(GetValue<int64_t>());
        }

        std::string ToString() override
        {
            return ARK_TO_STRING(data_);
        }

        void FromString(const std::string& value)
        {
            AFMisc::FromString(value, data_);
        }

    protected:
        int64_t GetInt64() override
        {
            return data_;
        }

        void SetInt64(const int64_t& value) override
        {
            data_ = value;
        }

    private:
        int64_t data_{ NULL_INT64 };
    };

    //uint64_t
    class AFUInt64Data : public AFIDataEx
    {
    public:
        AFUInt64Data() = default;

        void Clear() override
        {
            data_ = NULL_INT64;
        }

        bool IsValid() override
        {
            return data_ != NULL_INT64;
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            SetValue(rhs->GetValue<uint64_t>());
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            rhs->SetValue(GetValue<uint64_t>());
        }

        std::string ToString() override
        {
            return ARK_TO_STRING(data_);
        }

        void FromString(const std::string& value)
        {
            AFMisc::FromString(value, data_);
        }

    protected:
        uint64_t GetUInt64() override
        {
            return data_;
        }

        void SetUInt32(const uint64_t& value) override
        {
            data_ = value;
        }

    private:
        uint64_t data_{ NULL_INT64 };
    };

    //float
    class AFFloatData : public AFIDataEx
    {
    public:
        AFFloatData() = default;

        void Clear() override
        {
            data_ = NULL_FLOAT;
        }

        bool IsValid() override
        {
            return data_ != NULL_FLOAT;
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            SetValue(rhs->GetValue<float>());
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            rhs->SetValue(GetValue<float>());
        }

        std::string ToString() override
        {
            return ARK_TO_STRING(data_);
        }

        void FromString(const std::string& value)
        {
            AFMisc::FromString(value, data_);
        }

    protected:
        float GetFloat() override
        {
            return data_;
        }

        void SetFloat(const float& value) override
        {
            data_ = value;
        }

    private:
        float data_{ NULL_FLOAT };
    };

    //double
    class AFDoubleData : public AFIDataEx
    {
    public:
        AFDoubleData() = default;

        void Clear() override
        {
            data_ = NULL_DOUBLE;
        }

        bool IsValid() override
        {
            return data_ != NULL_DOUBLE;
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            SetValue(rhs->GetValue<double>());
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            rhs->SetValue(GetValue<double>());
        }

        std::string ToString() override
        {
            return ARK_TO_STRING(data_);
        }

        void FromString(const std::string& value)
        {
            AFMisc::FromString(value, data_);
        }

    protected:
        double GetDouble() override
        {
            return data_;
        }

        void SetFloat(const double& value) override
        {
            data_ = value;
        }

    private:
        double data_{ NULL_DOUBLE };
    };

    //self-defined vector3D
    class AFVector3Data : public AFIDataEx
    {
    public:
        AFVector3Data() = default;

        void Clear() override
        {
            data_ = NULL_VECTOR3D;
        }

        bool IsValid() override
        {
            return data_ != NULL_VECTOR3D;
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            SetValue(rhs->GetValue<AFVector3D>());
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            rhs->SetValue(GetValue<AFVector3D>());
        }

        std::string ToString() override
        {
            return AFVector3D::ToString(data_);
        }

        void FromString(const std::string& value)
        {
            AFVector3D::FromString(value, data_);
        }

    protected:
        const AFVector3D& GetVector3D() override
        {
            return data_;
        }

        void SetVector3D(const AFVector3D& value) override
        {
            data_ = value;
        }

    private:
        AFVector3D data_{ NULL_VECTOR3D };
    };

    //string
    class AFStringData : public AFIDataEx
    {
    public:
        AFStringData() = default;
        void Clear() override
        {
            data_ = NULL_STR;
        }

        bool IsValid() override
        {
            return data_ != NULL_STR;
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            SetValue(rhs->GetValue<std::string>());
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);
            rhs->SetValue(GetValue<std::string>());
        }

        std::string ToString() override
        {
            return data_;
        }

        void FromString(const std::string& value)
        {
            data_ = value;
        }

    protected:
        const std::string& GetString() override
        {
            return data_;
        }

        void SetString(const std::string& value)
        {
            data_ = value;
        }

    private:
        std::string data_{};
    };

}