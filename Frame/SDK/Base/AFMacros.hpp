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

#pragma once

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

#define ARK_GUID_POWER 100000
#define ARK_EPOCH 1288834974657L

#if ARK_PLATFORM == PLATFORM_WIN
//windows

inline uint32_t GetSystemTime()
{
    return ::GetTickCount();
}

#define ARK_SPRINTF sprintf_s
#define ARK_STRICMP _stricmp
#define ARK_SLEEP(s) Sleep(s)
#define ARK_STRNCPY strncpy_s
#define ARK_ASSERT(exp_, msg_, file_, func_)        \
    do                                              \
    {                                               \
        if (!(exp_))                                \
        {                                           \
            std::string strInfo("Message:");        \
            strInfo += msg_ + std::string(" don't exist or some warning") + std::string("\n\nFile:") + std::string(file_) + std::string("\n Function:") + func_;\
            MessageBox(0, TEXT(strInfo.c_str()), TEXT("Error_"#exp_), MB_RETRYCANCEL | MB_ICONERROR); \
        }                                           \
        assert(exp_);                               \
    } while (0);

#define ARK_EXPORT extern "C"  __declspec(dllexport)

#else
//linux

inline uint32_t GetSystemTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
};

#define ARK_SPRINTF snprintf
#define ARK_STRICMP strcasecmp
#define ARK_SLEEP(s) usleep(s)
#define ARK_STRNCPY strncpy
#define ARK_ASSERT(exp_, msg_, file_, func_)        \
    do                                              \
    {                                               \
        if ((exp_)) break;                          \
        assert(exp_);                               \
    } while (0);

#define ARK_EXPORT extern "C" __attribute ((visibility("default")))

#endif

#define ARK_ASSERT_RET_VAL(exp_, val)   \
    do                                  \
    {                                   \
        if ((exp_)) break;              \
        assert(exp_);                   \
        return val;                     \
    } while (0);

#define ARK_ASSERT_BREAK(exp_)          \
    if (!(exp_))                        \
    {                                   \
        assert(exp_);                   \
        break;                          \
    }                                   \
    else {}

#define ARK_ASSERT_CONTINUE(exp_)       \
    if (!(exp_))                        \
    {                                   \
        assert(exp_);                   \
        continue;                       \
    }                                   \
    else {}

#define ARK_ASSERT_RET_NONE(exp_)       \
    do                                  \
    {                                   \
        if ((exp_)) break;              \
        assert(exp_);                   \
        return;                         \
    } while (0);

#define ARK_ASSERT_NO_EFFECT(exp_)      \
    do                                  \
    {                                   \
        if (exp_) break;                \
        assert(exp_);                   \
    } while(0)

#if defined(USE_BOOST)
#  include <boost/lexical_cast.hpp>
#  define ARK_LEXICAL_CAST boost::lexical_cast
#  define ARK_SHARE_PTR boost::shared_ptr
#else
#  include "common/lexical_cast.hpp"
#  define ARK_LEXICAL_CAST lexical_cast
#  define ARK_SHARE_PTR std::shared_ptr
#endif

inline bool IsZeroFloat(const float value)
{
    return std::abs(value) < std::numeric_limits<float>::epsilon();
}

inline bool IsZeroDouble(const double value)
{
    return std::abs(value) < std::numeric_limits<double>::epsilon();
}

inline bool IsFloatEqual(const float lhs, const float rhs)
{
    return std::abs(lhs - rhs) < std::numeric_limits<float>::epsilon();
}

inline bool IsDoubleEqual(const double lhs, const double rhs)
{
    return std::abs(lhs - rhs) < std::numeric_limits<double>::epsilon();
}

#if ARK_PLATFORM == PLATFORM_WIN
#define PROTOBUF_USE_DLLS
#endif

#define ARK_SINGLETON_INIT(TYPE) template<> TYPE* Singleton<TYPE>::instance_ = 0;
/*
#if ARK_PLATFORM == PLATFORM_WIN
#  if defined(ARK_USE_TCMALLOC)
#  undef ARK_USE_TCMALLOC
#  endif
#else
#define ARK_USE_TCMALLOC
#endif
*/
template<typename T>
bool ARK_FROM_STR(const std::string& strValue, T& nValue)
{
    try
    {
        nValue = ARK_LEXICAL_CAST<T>(strValue);
        return true;
    }
    catch(...)
    {
        return false;
    }

    return false;
}

template<typename T>
bool ARK_TO_STR(std::string& strValue, const T& nValue)
{
    try
    {
        strValue = ARK_LEXICAL_CAST<std::string>(nValue);
        return true;
    }
    catch(...)
    {
        return false;
    }

    return false;
}

// clear player data time
#define CLEAR_HOUR 5
