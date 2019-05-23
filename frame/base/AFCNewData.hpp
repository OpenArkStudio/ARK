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

namespace ark
{

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
}