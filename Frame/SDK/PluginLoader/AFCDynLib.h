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

#include "SDK/Core/Base/AFPlatform.hpp"
#include "SDK/Core/Base/AFMacros.hpp"

#if ARK_PLATFORM == PLATFORM_WIN

#define DYNLIB_HANDLE hInstance
#define DYNLIB_LOAD(a) LoadLibraryEx(a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
#define DYNLIB_GETSYM(a, b) GetProcAddress(a, b)
#define DYNLIB_UNLOAD(a) FreeLibrary(a)

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#elif ARK_PLATFORM == PLATFORM_UNIX

#include <dlfcn.h>
#define DYNLIB_HANDLE void*
#define DYNLIB_LOAD(a) dlopen(a, RTLD_LAZY | RTLD_GLOBAL)
#define DYNLIB_GETSYM(a, b) dlsym(a, b)
#define DYNLIB_UNLOAD(a) dlclose(a)

#elif ARK_PLATFORM == PLATFORM_APPLE

#define DYNLIB_HANDLE void*
#define DYNLIB_LOAD(a) mac_loadDylib(a)
#define DYNLIB_GETSYM(a, b) dlsym(a, b)
#define DYNLIB_UNLOAD(a) dlclose(a)

#endif

class AFCDynLib
{
public:
    explicit AFCDynLib(const std::string& strName) :
        mstrName(strName),
        mbMain(false)
    {
#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
        mstrName.append("_d");
#endif

#if ARK_PLATFORM == PLATFORM_WIN
        mstrName.append(".dll");
#elif ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == AF_PLATFORM_ANDROID
        mstrName.append(".so");
#elif ARK_PLATFORM == PLATFORM_APPLE || ARK_PLATFORM == AF_PLATFORM_APPLE_IOS
        mstrName.append(".so");
#endif

        CONSOLE_LOG << "LoadPlugin: " << mstrName << std::endl;
    }

    ~AFCDynLib()
    {

    }

    bool Load();

    bool UnLoad();

    //Get the name of the library
    const std::string& GetName(void) const
    {
        return mstrName;
    }

    const bool GetMain(void) const
    {
        return mbMain;
    }

    void* GetSymbol(const char* szProcName);

private:
    std::string mstrName;
    bool mbMain;

    DYNLIB_HANDLE mInst;
};