/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2019 ArkNX authors.
*
* Licensed under the Apache License, Version 2.0 (the "License").
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

#include "AFMacros.hpp"
#include "AFEnum.hpp"
#include "AFBaseStruct.hpp"

namespace ark
{

    class AFMisc
    {
    public:
        static uint32_t GetNearest2N(uint32_t size)
        {
            if (size == 0)
            {
                return 0;
            }

            if (size <= 8)
            {
                return 8;
            }

            if (size > (1 << 16))
            {
                return 0;
            }

            if ((size & (size - 1)) == 0)
            {
                return size;
            }

            int count = 0;

            while (size)
            {
                size = size >> 1;
                ++count;
            }

            return 1 << count;
        }

        static bool IsZeroFloat(const float value)
        {
            return std::abs(value) < std::numeric_limits<float>::epsilon();
        }

        static bool IsZeroDouble(const double value)
        {
            return std::abs(value) < std::numeric_limits<double>::epsilon();
        }

        static bool IsFloatEqual(const float lhs, const float rhs)
        {
            return std::abs(lhs - rhs) < std::numeric_limits<float>::epsilon();
        }

        static bool IsDoubleEqual(const double lhs, const double rhs)
        {
            return std::abs(lhs - rhs) < std::numeric_limits<double>::epsilon();
        }

        template<typename T>
        static std::string ToString(T& value)
        {
            return ARK_LEXICAL_CAST<std::string>(value);
        }

        template<typename T>
        static bool FromString(const std::string& str_value, T& result)
        {
            try
            {
                result = ARK_LEXICAL_CAST<T>(str_value);
                return true;
            }
            catch (std::system_error& ex)
            {
                CONSOLE_ERROR_LOG << "FromString failed, code = " << ex.code().message() << " msg = " << ex.what() << std::endl;
                ARK_ASSERT_NO_EFFECT(0);
                return false;
            }

            return false;
        }

        template<typename T>
        static T FromString(const std::string& str_value)
        {
            try
            {
                T result = ARK_LEXICAL_CAST<T>(str_value);
                return result;
            }
            catch (std::system_error& ex)
            {
                CONSOLE_ERROR_LOG << "FromString failed, code = " << ex.code().message() << " msg = " << ex.what() << std::endl;
                ARK_ASSERT_NO_EFFECT(0);
                return T();
            }
        }

        static std::string ToLower(std::string s)
        {
            std::transform(s.begin(), s.end(), s.begin(),
                           [](unsigned char c)
            {
                return std::tolower(c);
            });

            return s;
        }
        static std::string ToUpper(std::string s)
        {
            std::transform(s.begin(), s.end(), s.begin(),
                           [](unsigned char c)
            {
                return std::toupper(c);
            });

            return s;
        }

        static bool IsDigit(const std::string& str)
        {
            if (str.empty())
            {
                return false;
            }

            string::const_iterator start = str.begin();
            if (*start == '-')
                ++start;

            return (std::all_of(start, str.end(), [](unsigned char c)
            {
                return isdigit(c);
            }));
        }

        static std::string Bus2Str(int bus_id)
        {
            AFBusAddr bus_addr(bus_id);
            return bus_addr.ToString();
        }

        static ArkDataType CovertDataType(const std::string& type_name)
        {
            static char const* data_type[] =
            {
                "bool",
                "int",
                "uint",
                "int64",
                "uint64",
                "float",
                "double",
                "string",
                "vector3d",
                "struct",
                "table",
                "array",
                //if there's more type, please add type name in here
            };

            static int n = ARRAY_LENTGH(data_type);
            for (int i = 0; i < n; ++i)
            {
                if (type_name == data_type[i])
                {
                    return ArkDataType(i);
                }
            }

            return ArkDataType::DT_EMPTY;
        }

        template<typename T>
        static T Operate(ArkDataOpType op, const T& base, const T& delta)
        {
            switch (op)
            {
            case ArkDataOpType::DOT_INC:
                base += delta;
                break;
            case ArkDataOpType::DOT_DEC:
                base -= std::min(base, delta);
                break;
            case ArkDataOpType::DOT_MUL:
                base *= delta;
                break;
            case ArkDataOpType::DOT_DIV:
                ARK_ASSERT_RET_VAL(delta != 0, base);
                base /= delta;
                break;
            case ArkDataOpType::DOT_SET:
                base = delta;
                break;
            case ArkDataOpType::DOT_AND:
                base &= delta;
                break;
            case ArkDataOpType::DOT_OR:
                base |= delta;
                break;
            default:
                ARK_ASSERT_NO_EFFECT(0);
                break;
            }

            return base;
        }

    };

}