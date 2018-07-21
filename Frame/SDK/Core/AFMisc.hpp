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

#pragma once

#include "AFMacros.hpp"

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
    static bool ARK_FROM_STR(const std::string& strValue, T& nValue)
    {
        try
        {
            nValue = ARK_LEXICAL_CAST<T>(strValue);
            return true;
        }
        catch (std::system_error& ex)
        {
            std::cout << "ARK_FROM_STR failed, code = " << ex.code().message() << " msg = " << ex.what() << std::endl;
            ARK_ASSERT_NO_EFFECT(0);
            return false;
        }

        return false;
    }

    template<typename T>
    static bool ARK_TO_STR(std::string& strValue, const T& nValue)
    {
        try
        {
            strValue = ARK_LEXICAL_CAST<std::string>(nValue);
            return true;
        }
        catch (std::system_error& ex)
        {
            std::cout << "ARK_TO_STR failed, code = " << ex.code().message() << " msg = " << ex.what() << std::endl;
            ARK_ASSERT_NO_EFFECT(0);
            return false;
        }

        return false;
    }

    static int64_t GetSystemTime()
    {
#if ARK_PLATFORM == PLATFORM_WIN
        return ::GetTickCount64();
#else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
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
};