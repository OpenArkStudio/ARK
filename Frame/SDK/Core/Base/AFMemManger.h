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

#include "SDK/Core/Base/AFPlatform.hpp"
#include "AFSingleton.hpp"

#ifdef ARK_USE_TCMALLOC

#if ARK_PLATFORM == PLATFORM_WIN

#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
#pragma comment(lib, "libtcmalloc_minimal-debug.lib")
#else
#pragma comment(lib, "libtcmalloc_minimal.lib")
#endif

#pragma comment(linker, "/include:__tcmalloc")
#endif // ARK_RUN_MODE_DEBUG

#endif // ARK_USE_TCMALLOC

class AFMemManger: public AFSingleton<AFMemManger>
{
public:
    AFMemManger() {}
    ~AFMemManger() {}

    virtual void FreeMem()
    {
#ifdef ARK_USE_TCMALLOC
        // 回收内存
        MallocExtension::instance()->ReleaseFreeMemory();
#endif //ARK_USE_TCMALLOC
    }
};