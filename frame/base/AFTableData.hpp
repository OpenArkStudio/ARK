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

    class AFTableData : public AFIDataEx
    {
    public:
        AFTableData() = default;

        void Clear() override
        {
            datas_.clear();
        }

        bool IsValid() override
        {
            return datas_.empty();
        }

        uint32_t Size() override
        {
            return datas_.size();
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            //TODO:
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            rhs->CopyFrom(this);
        }

        AFIDataEx* FirstData() override
        {
            return datas_.begin()->second;
        }

        AFIDataEx* NextData() override
        {
            //TODO
            return nullptr;
        }

        AFIDataEx* GetData(uint64_t key) override
        {
            return datas_.find_value(key);
        }

        AFIDataEx* GetData(uint64_t key, const std::string& data_name) override
        {
            auto data = GetData(key);
            ARK_ASSERT_RET_VAL(data != nullptr, nullptr);

            return data->GetData(data_name);
        }

        AFIDataEx* GetData(uint64_t key, uint64_t child_key) override
        {
            auto data = GetData(key);
            ARK_ASSERT_RET_VAL(data != nullptr, nullptr);

            return data->GetData(child_key);
        }

        AFIDataEx* GetData(uint64_t key, uint64_t child_key, const std::string& data_name) override
        {
            auto data = GetData(key);
            ARK_ASSERT_RET_VAL(data != nullptr, nullptr);

            return data->GetData(child_key, data_name);
        }

        AFIDataEx* GetData(uint64_t key, const std::string& data_name, uint64_t child_key) override
        {
            auto data = GetData(key);
            ARK_ASSERT_RET_VAL(data != nullptr, nullptr);

            return data->GetData(data_name, child_key);
        }

        bool AddData(uint64_t key, AFIDataEx* data) override
        {
            data->SetKey(key);
            data->SetParent(this);
            return datas_.insert(key, data).second;
        }

        bool AddData(uint64_t key, uint64_t child_key, AFIDataEx* data) override
        {
            auto data = GetData(key);
            ARK_ASSERT_RET_VAL(data != nullptr, false);

            return AddData(child_key, data);
        }

        bool AddData(uint64_t key, const std::string& data_name, AFIDataEx* data) override
        {
            auto data = GetData(key);
            ARK_ASSERT_RET_VAL(data != nullptr, false);

            return AddData(data_name, data);
        }

        bool RemoveData(uint64_t key) override
        {
            return datas_.erase(key);
        }

        bool RemoveData(uint64_t key, const std::string& data_name) override
        {
            auto data = GetData(key);
            ARK_ASSERT_RET_VAL(data != nullptr, false);

            return data->RemoveData(data_name);
        }

        void FindData(const std::string& data_name, uint64_t value, std::list<AFIDataEx*>& result_list) override
        {
            //TODO
        }

        bool CheckData(const std::string& data_name, uint64_t value, const std::string& check_name, uint64_t check_value) override
        {
            //TODO
        }

    private:
        AFMap<uint64_t, AFIDataEx> datas_;
    };

}