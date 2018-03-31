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

#include "AFPlatform.hpp"
#include "AFMacros.hpp"
#include "AFMemPool.hpp"
#include "AFMisc.hpp"

#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG

#define ARK_ALLOC(bytes)                AFMemAlloc::AllocDebug(bytes, __FILE__, __LINE__)
#define ARK_REALLOC(addr, bytes)        AFMemAlloc::ReallocDebug(addr, bytes, __FILE__, __LINE__)
#define ARK_CALLOC(count, bytes)        AFMemAlloc::CallocDebug(count, bytes, __FILE__, __LINE__)
#define ARK_FREE(p)                     AFMemAlloc::FreeDebug(p)


#else

#define ARK_ALLOC(bytes)                AFMemAlloc::Alloc(bytes)
#define ARK_REALLOC(addr, bytes)        AFMemAlloc::Realloc(addr, bytes)
#define ARK_CALLOC(count, bytes)        AFMemAlloc::Calloc(count, bytes)
#define ARK_FREE(p)                     AFMemAlloc::Free(p)

#endif

#define POOL_SIZE 8

struct AllocItem
{
    int32_t length = 0;
    std::string filename = "";
    int32_t line = 0;
};

class AFMemAlloc
{
public:
    AFMemAlloc();
    ~AFMemAlloc();

    static void Start(bool need_record = true);
    static void CheckLeak();

    //need call first
    static void InitPool();
    static void ClearPool();

    //Debug
    //Allocate some memory
    static void* AllocDebug(size_t bytes, const char* file, int line);
    //Reallocate some memory
    static void* ReallocDebug(void* addr, size_t bytes, const char* file, int line);
    //void* calloc(size_t numElements, size_t sizeOfElement);
    //output is already as zero(with init)
    static void* CallocDebug(size_t count, size_t bytes, const char* file, int line);
    //Free
    static void FreeDebug(void* p);

    //Release
    static void* Alloc(size_t bytes);
    static void* Realloc(void* addr, size_t bytes);
    static void* Calloc(size_t count, size_t bytes);
    static void Free(void* p);

    //Utils
    static inline int32_t SizeToPoolIndex(size_t size);
    static void Dump();

private:
    //internal interface
    static void* AllocInternal(size_t bytes);
    static void* ReallocInternal(void* addr, size_t bytes);
    static void* CallocInternal(size_t count, size_t bytes);
    static void FreeInternal(void* p);
    //Pool interface
    static void* AllocFromPool(uint32_t bytes);
    static void* ReallocFromPool(void* addr, size_t bytes);
    static void* CallocFromPool(size_t count, size_t bytes);
    static size_t FreeFromPool(void* p);
};