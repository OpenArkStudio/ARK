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

#include "AFPlatform.hpp"

namespace ark
{
    using AFDataMask = std::bitset<8>;
    class AFDataSetting
    {
    public:
        AFDataSetting() = default;
        bool HaveMask(const uint8_t pos) const
        {
            return mask_.test(pos);
        }

        std::string name_{};
        ArkDataType type_{DT_UNKNOWN};
        std::string init_value_{};
        std::string sub_meta_class_{};
        uint32_t mask_{0};
        std::string key_name_{};
        //To add more
    };
}