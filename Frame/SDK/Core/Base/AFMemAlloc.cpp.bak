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

#include "AFMemAlloc.h"

bool g_needRecord = false;
std::atomic_int g_totalAlloc(0);
std::map<void*, AllocItem> g_memMap;
std::mutex g_memMapMutex;

bool g_usePool = false;
AFMemPool* g_pool[POOL_SIZE];
uint32_t g_poolTotal[POOL_SIZE] = { 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024 };
uint32_t g_poolSize[POOL_SIZE] = { 16, 32, 64, 128, 256, 512, 1024, 2048 };
uint32_t g_poolUsed[POOL_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0 };



AFMemAlloc::AFMemAlloc()
{

}

AFMemAlloc::~AFMemAlloc()
{

}

void AFMemAlloc::Start(bool need_record/* = true*/)
{
    g_totalAlloc = 0;
    g_memMap.clear();
    g_needRecord = need_record;
}

void AFMemAlloc::CheckLeak()
{
    printf("Memory check leak start-----------------------\n");

    if (g_totalAlloc == 0)
    {
        printf("No memory leak!\n");
    }
    else
    {
        printf("Find leak, %d bytes!\n", g_totalAlloc.load());
        for (auto& iter : g_memMap)
        {
            AllocItem& record = iter.second;
            printf("%s(%d) %d bytes\n", record.filename.c_str(), record.line, record.length);
        }
    }

    printf("Memory check leak end-------------------------\n");
}

void AFMemAlloc::InitPool()
{
    for (int i = 0; i < POOL_SIZE; ++i)
    {
        g_pool[i] = ARK_NEW AFMemPool(g_poolTotal[i], g_poolSize[i]);
    }

    g_usePool = true;
}

void AFMemAlloc::ClearPool()
{
    for (int i = 0; i < POOL_SIZE; ++i)
    {
        delete g_pool[i];
    }

    g_usePool = false;
}

void* AFMemAlloc::AllocDebug(size_t bytes, const char* file, int line)
{
    ARK_ASSERT_NO_EFFECT(bytes > 0);

    void* ptr = AllocInternal(bytes);
    if (ptr == nullptr)
    {
        return nullptr;
    }

    if (g_needRecord)
    {
        AllocItem record;
        record.length = bytes;
        record.filename = file;
        record.line = line;
        g_memMapMutex.lock();
        g_memMap[ptr] = record;
        g_memMapMutex.unlock();
        g_totalAlloc += bytes;
    }

    return ptr;
}

void* AFMemAlloc::ReallocDebug(void* addr, size_t bytes, const char* file, int line)
{
    if (g_needRecord)
    {
        if (addr != nullptr)
        {
            AllocItem record = g_memMap[addr];
            g_totalAlloc -= record.length;
            g_memMapMutex.lock();
            g_memMap.erase(addr);
            g_memMapMutex.unlock();
        }
    }

    ARK_ASSERT_NO_EFFECT(bytes > 0);

    void* ptr = ReallocInternal(addr, bytes);
    if (ptr == nullptr)
    {
        return nullptr;
    }

    if (g_needRecord)
    {
        AllocItem record;
        record.length = bytes;
        record.filename = file;
        record.line = line;
        g_memMapMutex.lock();
        g_memMap[ptr] = record;
        g_memMapMutex.unlock();
        g_totalAlloc += bytes;
    }

    return ptr;
}

void* AFMemAlloc::CallocDebug(size_t count, size_t bytes, const char* file, int line)
{
    ARK_ASSERT_NO_EFFECT(bytes > 0);

    void* ptr = CallocInternal(count, bytes);
    if (ptr == nullptr)
    {
        return nullptr;
    }

    if (g_needRecord)
    {
        AllocItem record;
        record.length = bytes;
        record.filename = file;
        record.line = line;
        g_memMapMutex.lock();
        g_memMap[ptr] = record;
        g_memMapMutex.unlock();
        g_totalAlloc += bytes;
    }

    return ptr;
}

void AFMemAlloc::FreeDebug(void* p)
{
    if (g_needRecord)
    {
        if (p != nullptr)
        {
            AllocItem record = g_memMap[p];
            g_totalAlloc -= record.length;
            g_memMapMutex.lock();
            g_memMap.erase(p);
            g_memMapMutex.unlock();
        }
    }

    FreeInternal(p);
}

void* AFMemAlloc::Alloc(size_t bytes)
{
    ARK_ASSERT_NO_EFFECT(bytes > 0);

    void* ptr = AllocInternal(bytes);
    if (ptr == nullptr)
    {
        return nullptr;
    }

    return ptr;
}

void* AFMemAlloc::Realloc(void* addr, size_t bytes)
{
    ARK_ASSERT_NO_EFFECT(bytes > 0);

    void* ptr = ReallocInternal(addr, bytes);
    if (ptr == nullptr)
    {
        return nullptr;
    }

    return ptr;
}

void* AFMemAlloc::Calloc(size_t count, size_t bytes)
{
    ARK_ASSERT_NO_EFFECT(bytes > 0);

    void* ptr = CallocInternal(count, bytes);
    if (ptr == nullptr)
    {
        return nullptr;
    }

    return ptr;
}

void AFMemAlloc::Free(void* p)
{
    FreeInternal(p);
}

int32_t AFMemAlloc::SizeToPoolIndex(size_t size)
{
    uint32_t allocSize = AFMisc::GetNearest2N(size);
    uint32_t nIndexT = allocSize >> 4;
    int32_t cIndex = -1;
    while (nIndexT)
    {
        nIndexT = nIndexT >> 1;
        ++cIndex;
    }

    return cIndex;
}

void AFMemAlloc::Dump()
{
    printf("Memory dump start---------------------\n");
    for (int i = 0; i < POOL_SIZE; ++i)
    {
        printf("[Size:%d]:Used:%d    /   Total:%d    (%d bytes) \r\n", g_poolSize[i], g_poolUsed[i], g_poolTotal[i], g_poolSize[i] * g_poolUsed[i]);
    }

    printf("Total: %d bytes used\n", g_totalAlloc.load());
    printf("Memory dump end-----------------------\n");
}

void* AFMemAlloc::AllocInternal(size_t bytes)
{
    if (g_usePool)
    {
        return AllocFromPool(bytes);
    }
    else
    {
        return ::malloc(bytes);
    }
}

void* AFMemAlloc::ReallocInternal(void* addr, size_t bytes)
{
    if (g_usePool)
    {
        return ReallocFromPool(addr, bytes);
    }
    else
    {
        return ::realloc(addr, bytes);
    }
}

void* AFMemAlloc::CallocInternal(size_t count, size_t bytes)
{
    if (g_usePool)
    {
        return CallocFromPool(count, bytes);
    }
    else
    {
        return ::calloc(count, bytes);
    }
}

void AFMemAlloc::FreeInternal(void* p)
{
    if (g_usePool)
    {
        FreeFromPool(p);
    }
    else
    {
        ::free(p);
    }
}

void* AFMemAlloc::AllocFromPool(uint32_t bytes)
{
    if (bytes < 16)
    {
        bytes = 16;
    }
    else if (bytes > 2048)
    {
        return ::malloc(bytes);
    }

    int32_t index = SizeToPoolIndex(bytes);
    ARK_ASSERT_NO_EFFECT(index != -1);

    g_poolUsed[index]++;
    return g_pool[index]->Alloc(bytes);
}

void* AFMemAlloc::ReallocFromPool(void* addr, size_t bytes)
{
    if (addr == nullptr)
    {
        return AllocFromPool(bytes);
    }

    uint32_t poolIndex = FreeFromPool(addr);
    if (poolIndex != -1)
    {
        void* ptr = AllocFromPool(bytes);
        memcpy(ptr, addr, g_pool[poolIndex]->mnUnitSize);
        return ptr;
    }
    else
    {
        return ::realloc(addr, bytes);
    }
}

void* AFMemAlloc::CallocFromPool(size_t count, size_t bytes)
{
    return AllocFromPool(count * bytes);
}

size_t AFMemAlloc::FreeFromPool(void* p)
{
    uint32_t poolIndex = -1;
    for (int i = 0; i < POOL_SIZE; ++i)
    {
        if (g_pool[i]->InPool(p))
        {
            poolIndex = i;
            g_pool[poolIndex]->Free(p);
            g_poolUsed[poolIndex]--;
            break;
        }
    }

    if (poolIndex < 0)
    {
        ::free(p);
    }

    return poolIndex;
}