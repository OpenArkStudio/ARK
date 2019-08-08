/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "base/AFPlatform.hpp"
#include "spdlog/fmt/fmt.h"

// Input param type
#ifndef IN
#define IN
#endif

// Output param type
#ifndef OUT
#define OUT
#endif

// Input and output param type
#ifndef INOUT
#define INOUT
#endif

#define ARRAY_CLEAR(v) memset((v), 0x0, sizeof((v)))
#define MEMORY_CLEAR(v) memset(&(v), 0x0, sizeof((v)))
#define MEMORY_CLEAR_POINTER(v) memset((v), 0xx, sizeof(*(v)))
#define ARRAY_LENTGH(v) std::extent<decltype(v)>::value

#define MAX_NAME 256
#define MAX_BUF 256

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

//////////////////////////////////////////////////////////////////////////
/* isaacs - added strlcpy implementation for systems that lack it. */
#ifndef strlcpy
/*  $NetBSD: strlcpy.c,v 1.1 2010/12/05 03:15:43 christos Exp $ */
/*  from OpenBSD: strlcpy.c,v 1.4 1999/05/01 18:56:41 millert Exp   */

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
static size_t strlcpy(char* dst, const char* src, size_t siz)
{
    char* d = dst;
    const char* s = src;
    size_t n = siz;

    /* Copy as many bytes as will fit */
    if ((n != 0) && ((--n) != 0))
    {
        do
        {
            if ((*d++ = *s++) == 0)
                break;
        } while (--n != 0);
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0)
    {
        if (siz != 0)
            *d = '\0'; /* NUL-terminate dst */

        while (*s++)
            ;
    }

    return (s - src - 1); /* count does not include NUL */
}
#endif
//////////////////////////////////////////////////////////////////////////

#ifdef ARK_PLATFORM_WIN

// Windows
#define ARK_SPRINTF sprintf_s
#define ARK_STRICMP _stricmp
#define ARK_SLEEP(s) Sleep(s)
#define ARK_STRNCPY strlcpy
#define ARK_ASSERT(exp_, msg_, file_, func_)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(exp_))                                                                                                   \
        {                                                                                                              \
            std::string strInfo("Message:");                                                                           \
            strInfo += msg_ + std::string(" don't exist or some warning") + std::string("\n\nFile:") +                 \
                       std::string(file_) + std::string("\n Function:") + func_;                                       \
            MessageBox(0, TEXT(strInfo.c_str()), TEXT("Error_" #exp_), MB_RETRYCANCEL | MB_ICONERROR);                 \
        }                                                                                                              \
        assert(exp_);                                                                                                  \
    } while (false);

#define ARK_EXPORT_FUNC extern "C" __declspec(dllexport)
#define ARK_EXPORT __declspec(dllexport)
#define ARK_IMPORT __declspec(dllimport)
#define ARK_UNUSED

#define DYNLIB_HANDLE hInstance
#define DYNLIB_LOAD(a) LoadLibraryEx(a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
#define DYNLIB_GETSYM(a, b) GetProcAddress(a, b)
#define DYNLIB_UNLOAD(a) FreeLibrary(a)

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#define ARK_FOLDER_SEP '\\'

#elif defined(ARK_PLATFORM_LINUX)

// Linux
#define ARK_SPRINTF snprintf
#define ARK_STRICMP strcasecmp
#define ARK_SLEEP(s) usleep(s * 1000)
#define ARK_STRNCPY strlcpy
#define ARK_ASSERT(exp_, msg_, file_, func_)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((exp_))                                                                                                    \
            break;                                                                                                     \
        assert(exp_);                                                                                                  \
    } while (0);

#define ARK_EXPORT_FUNC extern "C" __attribute((visibility("default")))
#define ARK_EXPORT __attribute((visibility("default")))
#define ARK_IMPORT __attribute((visibility("default")))
#define ARK_UNUSED __attribute__((unused))

#define DYNLIB_HANDLE void*
#define DYNLIB_LOAD(a) dlopen(a, RTLD_LAZY | RTLD_GLOBAL)
#define DYNLIB_GETSYM(a, b) dlsym(a, b)
#define DYNLIB_UNLOAD(a) dlclose(a)

#define ARK_FOLDER_SEP '/'

#elif defined(ARK_PLATFORM_DARWIN)

#define DYNLIB_HANDLE void*
#define DYNLIB_LOAD(a) mac_loadDylib(a)
#define DYNLIB_GETSYM(a, b) dlsym(a, b)
#define DYNLIB_UNLOAD(a) dlclose(a)

#endif

#define ARK_ASSERT_RET_VAL(exp_, val)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((exp_))                                                                                                    \
            break;                                                                                                     \
        assert(exp_);                                                                                                  \
        return val;                                                                                                    \
    } while (false);

#define ARK_ASSERT_RET_VAL_NO_EFFECT(exp_, val)                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((exp_))                                                                                                    \
            break;                                                                                                     \
        return val;                                                                                                    \
    } while (false);

#define ARK_ASSERT_BREAK(exp_)                                                                                         \
    if (!(exp_))                                                                                                       \
    {                                                                                                                  \
        assert(exp_);                                                                                                  \
        break;                                                                                                         \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
    }

#define ARK_ASSERT_CONTINUE(exp_)                                                                                      \
    if (!(exp_))                                                                                                       \
    {                                                                                                                  \
        assert(exp_);                                                                                                  \
        continue;                                                                                                      \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
    }

#define ARK_ASSERT_RET_NONE(exp_)                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((exp_))                                                                                                    \
            break;                                                                                                     \
        assert(exp_);                                                                                                  \
        return;                                                                                                        \
    } while (false);

#define ARK_ASSERT_NO_EFFECT(exp_)                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        if (exp_)                                                                                                      \
            break;                                                                                                     \
        assert(exp_);                                                                                                  \
    } while (false)

#if defined(USE_BOOST)
#include <boost/lexical_cast.hpp>
#define ARK_LEXICAL_CAST boost::lexical_cast
template<typename TD>
using ARK_SHARE_PTR = boost::shared_ptr<TD>;
#else
#include "common/lexical_cast.hpp"
#define ARK_LEXICAL_CAST lexical_cast
template<typename TD>
using ARK_SHARE_PTR = std::shared_ptr<TD>;
#endif

// cpp version
#if (__cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800))
#define ARK_HAVE_LANG_CXX11 1
#endif

#if (__cplusplus >= 201402L || (defined(_MSC_VER) && _MSC_VER >= 1900))
#define ARK_HAVE_LANG_CXX14 1
#endif

#if (__cplusplus >= 201703L || (defined(_MSC_VER) && _MSC_VER >= 1910))
#define ARK_HAVE_LANG_CXX17 1
#endif

#if defined(ARK_HAVE_LANG_CXX11) || defined(ARK_HAVE_LANG_CXX14) || !defined(ARK_HAVE_LANG_CXX17)
#define ARK_NOEXCEPT noexcept
#define ARK_CONSTEXPR constexpr
#define ARK_FINAL final
#else
#define ARK_NOEXCEPT throw()
#define ARK_CONSTEXPR
#define ARK_FINAL
#endif

#ifndef ARK_FUNCTION_LINE
#define ARK_FUNCTION_LINE __FUNCTION__, __LINE__
#endif

#ifndef ARK_FILE_FUNCTION_LINE
#define ARK_FILE_FUNCTION_LINE __FILE__, __FUNCTION__, __LINE__
#endif

#ifndef ARK_NEW
#define ARK_NEW new // new (nothrow) when new failed, return NULL
#endif

#ifndef ARK_NEW_ARRAY
#define ARK_NEW_ARRAY(T, size) new T[size];
#endif

#ifndef ARK_NEW_ARRAY_RET
#define ARK_NEW_ARRAY_RET(T, size)                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        return new (nothrow) T[size];                                                                                  \
    } while (false);
#endif //! ARK_NEW_ARRAY_RET

#ifndef ARK_DELETE
#define ARK_DELETE(p)                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        if (p != nullptr)                                                                                              \
        {                                                                                                              \
            delete p;                                                                                                  \
            p = nullptr;                                                                                               \
        }                                                                                                              \
    } while (false);
#endif

#ifndef ARK_DELETE_ARRAY
#define ARK_DELETE_ARRAY(T, p)                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        if (p != nullptr)                                                                                              \
        {                                                                                                              \
            T* t_ptr = static_cast<T*>(p);                                                                             \
            delete[] t_ptr;                                                                                            \
            t_ptr = nullptr;                                                                                           \
        }                                                                                                              \
    } while (false);
#endif

#define ARK_TO_STRING(value) std::to_string(value)

#define CONSOLE_LOG std::cout << __FUNCTION__ << ":" << __LINE__ << " "
#define CONSOLE_INFO_LOG std::cout << ""
#define CONSOLE_ERROR_LOG std::cerr << ""

#define ARK_FORMAT(my_fmt, ...) fmt::format(my_fmt, ##__VA_ARGS__);
#define ARK_FORMAT_FUNCTION(my_fmt, ...) fmt::format(std::string("[{}:{}]") + my_fmt, ARK_FUNCTION_LINE, ##__VA_ARGS__);

#define ARK_SRATIC_ASSERT static_assert
#define GET_CLASS_NAME(class_name) (typeid(class_name).name())
//////////////////////////////////////////////////////////////////////////
// Plugin macros
#define ARK_DECLARE_PLUGIN(PLUGIN_CLASS)                                                                               \
    class PLUGIN_CLASS : public AFIPlugin                                                                              \
    {                                                                                                                  \
    public:                                                                                                            \
        int GetPluginVersion()                                                                                         \
        {                                                                                                              \
            return 0;                                                                                                  \
        }                                                                                                              \
        const std::string GetPluginName()                                                                              \
        {                                                                                                              \
            return GET_CLASS_NAME(PLUGIN_CLASS);                                                                       \
        }                                                                                                              \
        void Install() override;                                                                                       \
        void Uninstall() override;                                                                                     \
        AFPluginManager* GetPluginManager() const override                                                             \
        {                                                                                                              \
            return plugin_manager_;                                                                                    \
        }                                                                                                              \
        void SetPluginManager(AFPluginManager* p)                                                                      \
        {                                                                                                              \
            ARK_ASSERT_RET_NONE(p != nullptr);                                                                         \
            plugin_manager_ = p;                                                                                       \
        }                                                                                                              \
                                                                                                                       \
    private:                                                                                                           \
        AFMap<std::string, AFIModule> modules_;                                                                        \
        AFPluginManager* plugin_manager_{nullptr};                                                                     \
    };

#define ARK_DECLARE_PLUGIN_DLL_FUNCTION(PLUGIN_CLASS)                                                                  \
    ARK_EXPORT_FUNC void DllEntryPlugin(AFPluginManager* pPluginManager)                                               \
    {                                                                                                                  \
        pPluginManager->Register<PLUGIN_CLASS>();                                                                      \
    }                                                                                                                  \
    ARK_EXPORT_FUNC void DllExitPlugin(AFPluginManager* pPluginManager)                                                \
    {                                                                                                                  \
        pPluginManager->Deregister<PLUGIN_CLASS>();                                                                    \
    }

#ifdef ARK_PLATFORM_WIN
#define ARK_REGISTER_MODULE(MODULE, DERIVED_MODULE)                                                                    \
    {                                                                                                                  \
        ARK_ASSERT_RET_NONE((std::is_base_of<AFIModule, MODULE>::value));                                              \
        ARK_ASSERT_RET_NONE((std::is_base_of<MODULE, DERIVED_MODULE>::value));                                         \
        AFIModule* pRegModule = ARK_NEW DERIVED_MODULE();                                                              \
        pRegModule->SetPluginManager(GetPluginManager());                                                              \
        pRegModule->SetName(GET_CLASS_NAME(MODULE));                                                                   \
        GetPluginManager()->AddModule(pRegModule->GetName(), pRegModule);                                              \
        modules_.insert(pRegModule->GetName(), pRegModule);                                                            \
        std::string base_name = GET_CLASS_NAME(&AFIModule::Update);                                                    \
        std::string child_name = GET_CLASS_NAME(&DERIVED_MODULE::Update);                                              \
        if (base_name != child_name)                                                                                   \
        {                                                                                                              \
            GetPluginManager()->AddUpdateModule(pRegModule);                                                           \
        }                                                                                                              \
    }
#else
#define ARK_REGISTER_MODULE(MODULE, DERIVED_MODULE)                                                                    \
    {                                                                                                                  \
        ARK_ASSERT_RET_NONE((std::is_base_of<AFIModule, MODULE>::value));                                              \
        ARK_ASSERT_RET_NONE((std::is_base_of<MODULE, DERIVED_MODULE>::value));                                         \
        AFIModule* pRegModule = ARK_NEW DERIVED_MODULE();                                                              \
        pRegModule->SetPluginManager(GetPluginManager());                                                              \
        pRegModule->SetName(GET_CLASS_NAME(MODULE));                                                                   \
        GetPluginManager()->AddModule(pRegModule->GetName(), pRegModule);                                              \
        modules_.insert(pRegModule->GetName(), pRegModule);                                                            \
        AFIModule base;                                                                                                \
        bool (AFIModule::*mfp)() = &AFIModule::Update;                                                                 \
        bool (DERIVED_MODULE::*child_mfp)() = &DERIVED_MODULE::Update;                                                 \
        void* base_update_mfp = (void*)(base.*mfp);                                                                    \
        void* derived_update_mfp = (void*)(static_cast<DERIVED_MODULE*>(pRegModule)->*child_mfp);                      \
        if (base_update_mfp == derived_update_mfp)                                                                     \
        {                                                                                                              \
            GetPluginManager()->AddUpdateModule(pRegModule);                                                           \
        }                                                                                                              \
    }
#endif

#define ARK_DEREGISTER_MODULE(MODULE, DERIVED_MODULE)                                                                  \
    {                                                                                                                  \
        ARK_ASSERT_RET_NONE((std::is_base_of<AFIModule, MODULE>::value));                                              \
        ARK_ASSERT_RET_NONE((std::is_base_of<MODULE, DERIVED_MODULE>::value));                                         \
        AFIModule* pDeregModule = dynamic_cast<AFIModule*>(GetPluginManager()->template FindModule<MODULE>());         \
        ARK_ASSERT_RET_NONE(pDeregModule != nullptr);                                                                  \
        GetPluginManager()->RemoveModule(pDeregModule->GetName());                                                     \
        GetPluginManager()->RemoveUpdateModule(pDeregModule->GetName());                                               \
        modules_.erase(pDeregModule->GetName());                                                                       \
        ARK_DELETE(pDeregModule);                                                                                      \
    }

//////////////////////////////////////////////////////////////////////////
// Module macros
#define ARK_DECLARE_MODULE_FUNCTIONS                                                                                   \
public:                                                                                                                \
    AFPluginManager* GetPluginManager() const override                                                                 \
    {                                                                                                                  \
        return plugin_manager_;                                                                                        \
    }                                                                                                                  \
    void SetPluginManager(AFPluginManager* p) override                                                                 \
    {                                                                                                                  \
        ARK_ASSERT_RET_NONE(p != nullptr);                                                                             \
        plugin_manager_ = p;                                                                                           \
    }                                                                                                                  \
    const std::string& GetName() const override                                                                        \
    {                                                                                                                  \
        return name_;                                                                                                  \
    }                                                                                                                  \
    void SetName(const std::string& value) override                                                                    \
    {                                                                                                                  \
        name_ = value;                                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
protected:                                                                                                             \
    template<typename MODULE>                                                                                          \
    MODULE* FindModule()                                                                                               \
    {                                                                                                                  \
        return GetPluginManager()->template FindModule<MODULE>();                                                      \
    }                                                                                                                  \
                                                                                                                       \
private:                                                                                                               \
    AFPluginManager* plugin_manager_{nullptr};                                                                         \
    std::string name_{};
//////////////////////////////////////////////////////////////////////////

// clear player data time
#define CLEAR_HOUR 5
