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
#include "AFMemory.hpp"

//Input param type
#ifndef IN
#define IN
#endif

//Output param type
#ifndef OUT
#define OUT
#endif

//Input and output param type
#ifndef INOUT
#define INOUT
#endif

#define ARRAY_CLEAR(v)              memset((v), 0x0, sizeof((v)))
#define MEMORY_CLEAR(v)             memset(&(v), 0x0, sizeof((v)))
#define MEMORY_CLEAR_POINTER(v)     memset((v), 0xx, sizeof(*(v)))
#define ARRAY_LENTGH(v)               (sizeof(v) / sizeof(v[0]))

#define MAX_NAME    256
#define MAX_BUF     256

#ifndef MAX_PATH
#define MAX_PATH    256
#endif

#define ARK_GUID_POWER 100000
#define ARK_EPOCH 1288834974657L

#if ARK_PLATFORM == PLATFORM_WIN

//windows
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
#define ARK_UNUSED

#else

//linux
#define ARK_SPRINTF snprintf
#define ARK_STRICMP strcasecmp
#define ARK_SLEEP(s) usleep(s * 1000)
extern "C" size_t strlcpy(char *dst, const char *src, size_t siz);
#define ARK_STRNCPY strlcpy
#define ARK_ASSERT(exp_, msg_, file_, func_)        \
    do                                              \
    {                                               \
        if ((exp_)) break;                          \
        assert(exp_);                               \
    } while (0);

#define ARK_EXPORT extern "C" __attribute ((visibility("default")))
#define ARK_UNUSED __attribute__((unused))

#endif

template<bool> struct ARK_STATIC_ASSERTION_FAILURE;
template<> struct ARK_STATIC_ASSERTION_FAILURE<true>
{
    enum
    {
        value = 1
    };
};

template<int x> struct ark_static_assert_test {};

#define ARK_STATIC_ASSERT(x) \
    typedef ark_static_assert_test<sizeof(ARK_STATIC_ASSERTION_FAILURE<(bool)(x)>)> \
        __FUNCTION__##__LINE__ ARK_UNUSED

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

//Google Protobuffer use dll
#ifndef PROTOBUF_USE_DLLS
#define PROTOBUF_USE_DLLS
#endif

//Singleton
#define ARK_SINGLETON_INIT(TYPE) template<> TYPE* Singleton<TYPE>::instance_ = 0;

//cpp version
#if (__cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800))
#define HAVE_LANG_CXX11 1
#endif

#if (__cplusplus >= 201402L || (defined(_MSC_VER) && _MSC_VER >= 1900))
#define HAVE_LANG_CXX14 1
#endif

#if (__cplusplus >= 201703L || (defined(_MSC_VER) && _MSC_VER >= 1910))
#define HAVE_LANG_CXX17 1
#endif

/*
#if ARK_PLATFORM == PLATFORM_WIN
#  if defined(ARK_USE_TCMALLOC)
#  undef ARK_USE_TCMALLOC
#  endif
#else
#define ARK_USE_TCMALLOC
#endif
*/

#ifndef ARK_FUNCTION_LINE
#define ARK_FUNCTION_LINE __FUNCTION__, __LINE__
#endif

#ifndef ARK_FILE_FUNCTION_LINE
#define ARK_FILE_FUNCTION_LINE __FILE__, __FUNCTION__, __LINE__
#endif

#ifndef ARK_NEW
#define ARK_NEW new
#endif

#ifndef ARK_DELETE
#define ARK_DELETE(p) if (p!= nullptr) { delete p; p = nullptr; }
#endif

#ifndef ARK_DELETE_ARRAY
#define ARK_DELETE_ARRAY(p) if (p != nullptr) { delete[] p; p = nullptr; }
#endif

#define ARK_TO_STRING(value) std::to_string(value)

#undef max
#undef min

#define ARK_ALLOC(ptr, name, size) ptr = (name*)MemoryPool::GetInstance()->Alloc(size); memset(ptr, 0x0, sizeof(name));
#define ARK_DEALLOC(ptr, name) MemoryPool::GetInstance()->FreeAlloc(ptr);

//clear player data time
#define CLEAR_HOUR 5
