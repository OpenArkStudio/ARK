/*
* This source file is part of ArkNX
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

namespace ark
{
	//bool
    class AFBoolData : public AFINewData
    {
    public:
        AFBoolData()
        {
            type_ = ArkDataType::DT_BOOLEAN;
        }

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
        bool data_{ NULL_BOOLEAN };
    };

	//int32_t
	class AFIntData : public AFINewData
	{
	public:
		AFIntData()
		{
			type_ = ArkDataType::DT_INT32;
		}

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

	private:
		int32_t data_{ NULL_INT };
	};

	//uint32_t
	class AFUIntData : public AFINewData
	{
	public:
		AFUIntData()
		{
			type_ = ArkDataType::DT_UINT32;
		}

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

	private:
		uint32_t data_{ NULL_INT };
	};

	//int64_t
	class AFInt64Data : public AFINewData
	{
	public:
		AFInt64Data()
		{
			type_ = ArkDataType::DT_INT64;
		}

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

	private:
		int64_t data_{ NULL_INT64 };
	};

	//uint64_t
	class AFUInt64Data : public AFINewData
	{
	public:
		AFUIntData()
		{
			type_ = ArkDataType::DT_UINT64;
		}

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

	private:
		uint64_t data_{ NULL_INT64 };
	};

	//float
	class AFFloatData : public AFINewData
	{
	public:
		AFFloatData()
		{
			type_ = ArkDataType::DT_FLOAT;
		}

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

	private:
		float data_{ NULL_FLOAT };
	};

	//double
	class AFDoubleData : public AFINewData
	{
	public:
		AFDoubleData()
		{
			type_ = ArkDataType::DT_DOUBLE;
		}

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

	private:
		double data_{ NULL_DOUBLE };
	};

	//std::string
	class AFStringData : public AFINewData
	{
	public:
		AFStringData()
		{
			type_ = ArkDataType::DT_STRING;
		}

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

	private:
		std::string data_{ NULL_STR };
	};

	//AFVector3D
	class AFVec3DData : public AFINewData
	{
	public:
		AFVec3DData()
		{
			type_ = ArkDataType::DT_VECTOR3D;
		}

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

	private:
		AFVector3D data_{ NULL_VECTOR3D };
	};

	//array of AFINewData
	class AFArrayData : public AFINewData
	{
	public:
		AFArrayData()
		{
			type_ = ArkDataType::DT_ARRAY;
		}

		~AFArrayData() override = default;

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
			//TODO: do not need for now
			return NULL_STR;
		}

		void FromString(const std::string& value) override
		{
			//TODO: do not need for now
		}

	private:
		AFPtrVector<AFINewData> vec_data_;
	};

}