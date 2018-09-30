/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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

#include "AFError.hpp"

namespace ark
{

    const std::error_category& get_error_category()
    {
        static ark_error_category category;
        return category;
    }

    std::error_code make_error_code(int code)
    {
        return std::error_code(code, get_error_category());
    }

    std::string ark_error_category::message(int err_val) const
    {
        switch (err_val)
        {
        case 1:
            return "test error";
        //TODO:will add message here
        default:
            break;
        }

        return "";
    }

}