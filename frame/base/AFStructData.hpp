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
#include "AFMap.hpp"

namespace ark
{

    class AFStructData : public AFIDataEx
    {
    public:
        AFStructData() = default;

        ArkDataType GetType const
        {
            return DT_STRUCT;
        }

        void Clear() override
        {
            key_ = NULL_INT64;
            for (auto iter : datas_)
            {
                iter.second->Clear();
            }
        }

        bool IsValid() override
        {
            return key_ != NULL_INT64;
        }

        uint32_t Size() override
        {
            return datas_.size();
        }

        uint64_t GetKey() override
        {
            return key_;
        }

        void SetKey(uint64_t key) override
        {
            key_ = key;
        }

        AFIDataEx* FirstData() override
        {
            return datas_.begin().second;
        }

        AFIDataEx* NextData() override
        {
            //TODO:
            return nullptr;
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);

            SetKey(rhs->GetKey());
            for (auto iter : datas_)
            {
                auto& key = iter.first;
                auto data = iter.second;

                auto result = rhs->GetData(key);
                if (result != nullptr)
                {
                    data->CopyFrom(result);
                }
            }
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            ARK_ASSERT_RET_NONE(rhs != nullptr);

            rhs->CopyFrom(this);
        }

        std::string ToString() override
        {
            //TODO:trans to json format
        }

        void FromString(const std::string& value)
        {
            //TODO: from json string
        }

        void ToMap(std::map<std::string, std::string>& result) override
        {
            //TODO
        }

        void FromMap(const std::map<std::string, std::string>& value)
        {
            //TODO
        }

        AFIDataEx* GetData(const std::string& data_name) override
        {
            return datas_.find_value(data_name);
        }

        AFIDataEx* GetData(const std::string& data_name, uint64_t key) override
        {
            auto data = GetData(data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, nullptr);

            return data->GetData(key);
        }

        AFIDataEx* GetData(const std::string& data_name, const std::string& child_name) override
        {
            if (data_name.empty())
            {
                return GetData(child_name);
            }

            auto data = GetData(data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, nullptr);

            return data->GetData(child_name);
        }

        AFIDataEx* GetData(const std::string& data_name, uint64_t key, const std::string& child_name) override
        {
            auto data = GetData(data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, nullptr);

            return data->GetData(key, child_name);
        }

        AFIDataEx* GetData(const std::string& data_name, const std::string& child_name, uint64_t key) override
        {
            auto data = GetData(data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, nullptr);

            return data->GetData(child_name, key);
        }

        AFIDataEx* GetData(const std::string& data_name, const std::string& child_name, const std::string& sub_name) override
        {
            auto data = GetData(data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, nullptr);

            return data->GetData(child_name, sub_name);
        }

        //////////////////////////////////////////////////////////////////////////

        bool AddData(const std::string& data_name, AFIDataEx* data) override
        {
            data->SetParent(this);
            return datas_.insert(data_name, data).second;
        }

        bool AddData(const std::string& data_name, uint64_t key, AFIDataEx* data) override
        {
            auto data = GetData(data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, false);

            return data->AddData(key, data);
        }

        bool AddData(const std::string& data_name, const std::string& child_name, AFIDataEx* data) override
        {
            auto data = GetData(data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, false);

            return data->AddData(child_name, data);
        }

        //////////////////////////////////////////////////////////////////////////

        bool RemoveData(const std::string& data_name) override
        {
            return datas_.erase(data_name);
        }

        bool RemoveData(const std::string& data_name, uint64_t key) override
        {
            auto data = GetData(data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, false);

            return data->RemoveData(key);
        }

        bool RemoveData(const std::string& data_name, const std::string& child_name) override
        {
            auto data = GetData(data_name);
            ARK_ASSERT_RET_VAL(data != nullptr, false);

            return data->RemoveData(child_name);
        }

    private:
        uint64_t key_{ 0 };
        AFMapEx<std::string, AFIDataEx> datas_;
    };

}