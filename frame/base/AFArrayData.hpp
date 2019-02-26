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

namespace ark
{

    class AFArrayData : public AFIDataEx
    {
    public:
        AFArrayData() = default;

        void Init(const AFMetaClassSetting* meta_class, const AFDataSetting* data)
        {
            AFIDataEx::Init(meta_class, data);
            uint32_t init_size = 0;
            AFMisc::FromString(data->init_value_, init_size);
            Resize(init_size);
        }

        void Clear() override
        {
            for (auto iter : datas_)
            {
                iter->Clear();
            }
        }

        bool IsValid() override
        {
            return !datas_.empty();
        }

        uint32_t Size() override
        {
            return datas_.size();
        }

        void Resize(uint32_t size) override
        {
            datas_.resize(size);
        }

        AFIDataEx* FirstData() override
        {
            if (datas_.empty())
            {
                return nullptr;
            }

            return datas_[0];
        }

        AFIDataEx* NextData() override
        {
            //TODO:
            return nullptr;
        }

        void CopyFrom(AFIDataEx* rhs) override
        {
            rhs->SaveTo(this);
        }

        void SaveTo(AFIDataEx* rhs) override
        {
            auto key = NULL_INT64;
            for (auto iter : datas_)
            {
                auto result = GetData(key++);
                if (result != nullptr)
                {
                    iter->SaveTo(result);
                }
            }
        }

        AFIDataEx* GetData(uint64_t key) override
        {
            if (key >= Size())
            {
                return nullptr;
            }

            return datas_[key];
        }

        AFIDataEx* GetData(uint64_t key, const std::string& data_name) override
        {
            auto result = GetData(key);
            if (result == nullptr)
            {
                return nullptr;
            }

            return result->FindData(data_name);
        }

        bool AddData(uint64_t key, AFIDataEx* data) override
        {
            return datas_.insert(key, data);
        }

        bool AddData(uint64_t key, const std::string& data_name, AFIDataEx* data) override
        {
            auto result = GetData(key);
            if (result == nullptr)
            {
                return false;
            }

            return result->AddData(data_name, data);
        }

        bool RemoveData(uint64_t key) override
        {
            auto result = GetData(key);
            if (result == nullptr)
            {
                return false;
            }

            result->Clear();
            return true;
        }

        bool RemoveData(uint64_t key, const std::string& data_name) override
        {
            auto result = GetData(key);
            if (result == nullptr)
            {
                return false;
            }

            return RemoveData(data_name);
        }

        std::string ToString() override
        {
            //[(1)(2)(3)]
            std::string result = "[";

            for (int i = 0; i < datas_.size(); ++i)
            {
                result += "(";
                result += datas_[i]->ToString();
                result += ")";
            }

            result += "]";
            return result;
        }

        void FromString(const std::string& result)
        {
            //TODO
        }

    private:
        AFArrayPod<AFIDataEx*, 1, AFArrayPodAlloc> datas_;
    };

}