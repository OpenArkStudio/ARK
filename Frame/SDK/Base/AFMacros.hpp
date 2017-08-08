/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#ifndef AF_MACROS_H
#define AF_MACROS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "AFPlatform.hpp"

#define ARRAY_CLEAR(v)              memset((v), 0x0, sizeof((v)))
#define MEMORY_CLEAR(v)             memset(&(v), 0x0, sizeof((v)))
#define MEMORY_CLEAR_POINTER(v)     memset((v), 0xx, sizeof(*(v)))
#define ARRAY_SIZE(v)               (sizeof(v) / sizeof(v[0]))

#define MAX_NAME    256
#define MAX_BUF     256

#ifndef MAX_PATH
#  define MAX_PATH    256
#endif

#define ARK_NEW new

#if ARK_PLATFORM == PLATFORM_WIN

inline uint32_t GetSystemTime()
{
    return ::GetTickCount();
}

#define ARK_SPRINTF sprintf_s
#define ARK_STRICMP _stricmp
#define ARK_SLEEP(s) Sleep(s)

#else

inline uint32_t GetSystemTime()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
};

#define ARK_SPRINTF snprintf
#define ARK_STRICMP strcasecmp
#define ARK_SLEEP(s) usleep(s)

#endif

#if defined(USE_BOOST)
#  include <boost/lexical_cast.hpp>
#  define ARK_LEXICAL_CAST boost::lexical_cast
#  define ARK_SHARE_PTR boost::shared_ptr
#else
#  include "common/lexical_cast.hpp"
#  define ARK_LEXICAL_CAST lexical_cast
#  define ARK_SHARE_PTR std::shared_ptr
#endif

inline bool zero_float(const float fValue)
{
    return std::abs(fValue) < std::numeric_limits<float>::epsilon();
}

inline bool zero_double(const double dValue)
{
    return std::abs(dValue) < std::numeric_limits<double>::epsilon();
}

inline bool float_equal(const float lhs, const float rhs)
{
    return std::abs(lhs - rhs) < std::numeric_limits<float>::epsilon();
}

inline bool double_equal(const double lhs, const double rhs)
{
    return std::abs(lhs - rhs) < std::numeric_limits<double>::epsilon();
}

#if ARK_PLATFORM == PLATFORM_WIN
#define PROTOBUF_USE_DLLS
#endif

#define ARK_SINGLETON_INIT(TYPE) template<> TYPE* Singleton<TYPE>::instance_ = 0;

#endif // AF_MACROS_H