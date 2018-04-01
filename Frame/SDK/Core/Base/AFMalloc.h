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
#include "AFMisc.hpp"

//object pool
#define ARK_CREATE_OBJECT(name) AFMalloc::Instance()->Create<name>(ARK_FUNCTION_LINE);
#define ARK_CREATE_BATCH_OBJECT(name, batch) AFMalloc::Instance()->Create<name, batch>(ARK_FUNCTION_LINE);
#define ARK_CREATE_OBJECT_WITH_ARG_1(name, batch, param) AFMalloc::Instance()->Create<name, batch >(param, ARK_FUNCTION_LINE);
#define ARK_CREATE_OBJECT_WITH_ARG_2(name, batch, param1, param2) AFMalloc::Instance()->Create<name, batch>(param1, param2, ARK_FUNCTION_LINE);
#define ARK_DELETE_OBJECT(name, object) AFMalloc::Instance()->Delete<name>(object, ARK_FUNCTION_LINE);

//memory pool
#define ARK_MALLOC(name, size) AFMalloc::Instance()->Malloc<name>(size, 30, ARK_FUNCTION_LINE);
#define ARK_FREE(name, memory, size) AFMalloc::Instance()->Free<name>(memory, size, ARK_FUNCTION_LINE);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LogData
{
public:
    LogData() {}

    void AddData(uint64_t size, uint64_t totalsize)
    {
        ++mnCount;
        mnUseSize += size;
        mnTotalSize = totalsize;
    }

    void DecData(uint64_t size)
    {
        mnCount -= std::min(mnCount, (uint32_t)1);
        mnUseSize -= std::min(mnUseSize, size);
    }

public:
    uint32_t mnCount = 0;
    uint64_t mnUseSize = 0;
    uint64_t mnTotalSize = 0;
};

class LogBlock
{
public:
    void AddBlock(const std::string& type, uint64_t size, uint64_t total_size)
    {
        LogData* pData = FindData(type);
        pData->AddData(size, total_size);
    }

    void DecBlock(const std::string& type, uint64_t size)
    {
        LogData* pData = FindData(type);
        pData->DecData(size);
    }

protected:
    LogData* FindData(const std::string& type)
    {
        auto iter = mxLogDatas.find(type);
        if(iter == mxLogDatas.end())
        {
            auto logdata = new LogData();
            iter = mxLogDatas.insert(std::make_pair(type, logdata)).first;
        }

        return iter->second;
    }

public:
    std::map<std::string, LogData*> mxLogDatas;
};

class AFMemoryLog
{
public:
    AFMemoryLog()
    {
        mbOpen = true;
    }

    ~AFMemoryLog()
    {
        for(auto iter : mxSTLogBlock.mxLogDatas)
        {
            delete iter.second;
        }

        for(auto iter : mxMTLogBlock.mxLogDatas)
        {
            delete iter.second;
        }

        mxSTLogBlock.mxLogDatas.clear();
        mxMTLogBlock.mxLogDatas.clear();;
    }

    void SetOpen(bool open)
    {
        mbOpen = open;
    }

    bool IsOpen() const
    {
        return mbOpen;
    }

    LogBlock GetMTMemory()
    {
        return mxMTLogBlock;
    }
    void AddMTMemory(const std::string& type, uint64_t size, uint64_t total_size)
    {
        if(!mbOpen)
        {
            return;
        }

        mxMTLogBlock.AddBlock(type, size, total_size);
    }

    void DecMTMemory(const std::string& type, uint64_t size)
    {
        if(!mbOpen)
        {
            return;
        }

        mxMTLogBlock.DecBlock(type, size);
    }
    //////////////////////////////////////////////////////////////////////////
    LogBlock& GetSTMemory()
    {
        return mxSTLogBlock;
    }

    void AddSTMemory(const std::string& type, uint64_t size, uint64_t total_size)
    {
        if(!mbOpen)
        {
            return;
        }

        mxSTLogBlock.AddBlock(type, size, total_size);
    }

    void DecSTMemory(const std::string& type, uint64_t size)
    {
        if(!mbOpen)
        {
            return;
        }

        mxSTLogBlock.DecBlock(type, size);
    }

private:
    //Log switch
    bool mbOpen;

    //multi-thread
    LogBlock mxMTLogBlock;

    //Single-thread
    LogBlock mxSTLogBlock;
};
//////////////////////////////////////////////////////////////////////////
class BlockMemory
{
public:
    BlockMemory(const char* type, uint32_t block_size, uint32_t block_batch, uint32_t block_alignment)
    {
        mstrMemType = type;
        m_pNextBlock = nullptr;
        mnTotalBlockSize = 0;
        mnBlockBatch = block_batch;
        mnBlockSize = (block_size + block_alignment - 1) & (~(block_alignment - 1));
    }

    ~BlockMemory()
    {

    }

    //分配内存
    void* Malloc()
    {
        //如果空余内存没有则新开辟一块内存
        if(m_pNextBlock == nullptr)
        {
            // 确定T内存大小,再根据内存对齐得出需要申请的单个T内存块大小
            // 新开一批内存(申请的内存比原有内存多15字节,用于下面的字节对齐)

            uint32_t newlength = mnBlockBatch * mnBlockSize + 15;
            uint8_t* pnewbatch = new uint8_t[newlength];

            mnTotalBlockSize += newlength;
            mxBlockBatches.push_back(pnewbatch);

            //16字节对齐
            uint8_t* paligned = reinterpret_cast<uint8_t*>(reinterpret_cast<uint64_t>((pnewbatch + 15)) & (~15));

            //将各个元素可以放置的地址确定
            m_pNextBlock = paligned;

            for(uint32_t i = 0; i < mnBlockBatch - 1; ++i)
            {
                uint8_t* lastblock = paligned + i * mnBlockSize;
                uint8_t* nextblock = paligned + (i + 1) * mnBlockSize;
                *(reinterpret_cast<uint64_t*>(lastblock)) = reinterpret_cast<uint64_t>(nextblock);
            }

            uint8_t* nextblock = paligned + (mnBlockBatch - 1) * mnBlockSize;
            *(reinterpret_cast<uint64_t*>(nextblock)) = static_cast<uint64_t>(0);
        }

        uint8_t* pblock = m_pNextBlock;
        m_pNextBlock = reinterpret_cast<uint8_t*>(*reinterpret_cast<uint64_t*>(m_pNextBlock));
        return reinterpret_cast<void*>(pblock);
    }

    //释放内存
    void Free(void* pblock)
    {
        if(pblock == nullptr || m_pNextBlock == nullptr)
        {
            return;
        }

        *(reinterpret_cast<uint64_t*>(pblock)) = reinterpret_cast<uint64_t>(m_pNextBlock);
        m_pNextBlock = reinterpret_cast<uint8_t*>(pblock);
    }

public:
    //类型
    std::string mstrMemType;

    //指向内存块元素下一个位置的指针
    uint8_t* m_pNextBlock;

    // 管理零散的内存块指针
    std::vector<uint8_t*> mxBlockBatches;

    // 每个单体结构的内存块字节数
    uint32_t mnBlockSize;

    // 每批申请的内存块数量
    uint32_t mnBlockBatch;

    // 公共分配的字节数
    uint64_t mnTotalBlockSize;
};
//////////////////////////////////////////////////////////////////////////
class BlockStore
{
public:
    BlockStore(const char* type, uint32_t block_size, uint32_t block_batch, uint32_t block_alignment)
    {
        m_pMutex = ARK_NEW std::mutex();
        m_pBlockMemory = ARK_NEW BlockMemory(type, block_size, block_batch, block_alignment);
    }

    virtual ~BlockStore()
    {
        ARK_DELETE(m_pMutex);
        ARK_DELETE(m_pBlockMemory);
    }

    void* MallocBlock()
    {
        std::lock_guard<std::mutex> locker(*m_pMutex);

        m_pMemoryLog->AddMTMemory(m_pBlockMemory->mstrMemType, m_pBlockMemory->mnBlockSize, m_pBlockMemory->mnTotalBlockSize);
        return m_pBlockMemory->Malloc();
    }

    void FreeBlock(void* pObject)
    {
        if(pObject == nullptr)
        {
            return;
        }

        std::lock_guard<std::mutex> locker(*m_pMutex);

        m_pBlockMemory->Free(pObject);
        m_pMemoryLog->DecMTMemory(m_pBlockMemory->mstrMemType, m_pBlockMemory->mnBlockSize);
    }

    void SetLogMemory(AFMemoryLog* logmemory)
    {
        m_pMemoryLog = logmemory;
    }

private:
    explicit BlockStore();
    explicit BlockStore(const BlockStore&);
    BlockStore& operator = (const BlockStore&);

protected:
    std::mutex* m_pMutex;
    BlockMemory* m_pBlockMemory;
    AFMemoryLog* m_pMemoryLog;
};

class STBlockStore : public BlockStore
{
public:
    STBlockStore(const char* type, uint32_t block_size, uint32_t block_batch, uint32_t block_alignment);
};

class MTBlockStore : public BlockStore
{
public:
    MTBlockStore(const char* type, uint32_t block_size, uint32_t block_batch, uint32_t block_alignment);
};
//////////////////////////////////////////////////////////////////////////
#define Memory_Alignment 4
#define __USE_MEMORY_POOL__
#define MILLION_SIZE (1024.0f * 1024.0f)

class AFMemory
{
public:
    AFMemory()
    {
        mxObjectMutex = ARK_NEW std::mutex();
        mxMemoryMutex = ARK_NEW std::mutex();
        mxMemoryLog = ARK_NEW AFMemoryLog();
    }

    ~AFMemory()
    {
        {
            std::lock_guard<std::mutex> locker(*mxObjectMutex);
            for(auto iter : mxMTObjects)
            {
                ARK_DELETE(iter.second);
            }
            mxMTObjects.clear();
            mxMTObjectBlocks.clear();
            ARK_DELETE(mxObjectMutex);
        }

        {
            std::lock_guard<std::mutex> locker(*mxMemoryMutex);
            for(auto iter : mxMTMemories)
            {
                ARK_DELETE(iter.second);
            }
            mxMTMemories.clear();
            mxMTMemoryBlocks.clear();

            ARK_DELETE(mxMemoryMutex);
        }

        ARK_DELETE(mxMemoryLog);
    }

    void* Create(const char* name, uint32_t size, uint32_t batch, const char* function, uint32_t line)
    {
        auto blockstore = CreateObjectBlock(name, size, batch);
        auto object = blockstore->MallocBlock();
        if(object != nullptr)
        {
            std::lock_guard<std::mutex> locker(*mxObjectMutex);
            mxMTObjectBlocks.insert(std::make_pair(object, blockstore));
        }
        else
        {
            printf("[%s:%u] [%s] Object Malloc Failed!", function, line, name);
        }

        return object;
    }

    void Delete(void* object, const char* function, uint32_t line)
    {
        if(object == nullptr)
        {
            return;
        }

        auto blockstore = FindObjectBlock(object);
        if(blockstore == nullptr)
        {
            printf("[%s:%u] Object Free Failed!", function, line);
            return;
        }

        blockstore->FreeBlock(object);
    }

    void* Malloc(uint32_t size, uint32_t batch, const char* function, uint32_t line)
    {
        auto blockstore = CreateMemoryBlock("char", size, batch);
        auto memory = blockstore->MallocBlock();
        if(memory != nullptr)
        {
            std::lock_guard<std::mutex> locker(*mxMemoryMutex);
            mxMTMemoryBlocks.insert(std::make_pair(memory, blockstore));
        }
        else
        {
            printf("[%s:%u] [char] Memory Malloc Failed!", function, line);
        }

        return memory;
    }

    void Free(void* memory, const char* function, uint32_t line)
    {
        if(memory == nullptr)
        {
            return;
        }

        auto blockstore = FindMemoryBlock(memory);
        if(blockstore == nullptr)
        {
            printf("[%s:%u] Memory Free Failed!", function, line);
            return;
        }

        blockstore->FreeBlock(memory);
    }

    //设置日志开关
    void SetLogOpen(bool open)
    {
        mxMemoryLog->SetOpen(open);
    }

    //打印内存信息
    void PrintLogMemory()
    {
        if(!mxMemoryLog->IsOpen())
        {
            return;
        }

        std::cout << "***********************************Print Memory Start****************************************************\n" << std::endl;

        uint64_t total_use_size = 0;
        uint64_t total_malloc_size = 0;

        LogBlock mtlogblock = mxMemoryLog->GetMTMemory();
        for(auto iter = mtlogblock.mxLogDatas.begin(); iter != mtlogblock.mxLogDatas.end(); ++iter)
        {
            auto logdata = iter->second;

            total_use_size += logdata->mnUseSize;
            total_malloc_size += logdata->mnTotalSize;
            PrintLogMemory(iter->first.c_str(), logdata->mnCount, logdata->mnUseSize, logdata->mnTotalSize);
        }

        double total_muse_size = static_cast<double>(total_use_size / MILLION_SIZE);
        double total_mmalloc_size = static_cast<double>(total_malloc_size / MILLION_SIZE);

        std::cout << "******************Print Memory End, Memory[" << total_use_size << "][" << total_muse_size << "]M, Alloc[" << total_mmalloc_size << "]M*************************" << std::endl;
    }

protected:
    //查找内存分配器
    BlockStore* FindObjectBlock(void* object)
    {
        std::lock_guard<std::mutex> locker(*mxObjectMutex);
        auto iter = mxMTObjectBlocks.find(object);
        if(iter == mxMTObjectBlocks.end())
        {
            return nullptr;
        }

        return iter->second;
    }

    BlockStore* CreateObjectBlock(const char* name, uint32_t size, uint32_t batch)
    {
        std::lock_guard<std::mutex> locker(*mxObjectMutex);
        auto iter = mxMTObjects.find(name);
        if(iter == mxMTObjects.end())
        {
            auto blockstore = new BlockStore(name, size, batch, Memory_Alignment);
            blockstore->SetLogMemory(mxMemoryLog);
            iter = mxMTObjects.insert(std::make_pair(name, blockstore)).first;
        }

        return iter->second;
    }

    BlockStore* FindMemoryBlock(void* memory)
    {
        std::lock_guard<std::mutex> locker(*mxMemoryMutex);

        auto iter = mxMTMemoryBlocks.find(memory);
        if(iter == mxMTMemoryBlocks.end())
        {
            return nullptr;
        }

        return iter->second;
    }

    BlockStore* CreateMemoryBlock(const char* name, uint32_t size, uint32_t batch)
    {
        std::lock_guard<std::mutex> locker(*mxMemoryMutex);

        auto iter = mxMTMemories.find(size);
        if(iter == mxMTMemories.end())
        {
            auto blockstore = new BlockStore(name, size, batch, Memory_Alignment);
            blockstore->SetLogMemory(mxMemoryLog);
            iter = mxMTMemories.insert(std::make_pair(size, blockstore)).first;
        }

        return iter->second;
    }

    //打印内存信息
    void PrintLogMemory(const char* type, uint32_t count, uint64_t use_size, uint64_t total_size)
    {
        auto strUseSize = ARK_TO_STRING(use_size);
        double useSizeMB = static_cast<double>(use_size / MILLION_SIZE);
        double totalSizeMB = static_cast<double>(total_size / MILLION_SIZE);

        printf("Count[%u] Memory[%s][%0.2fM] Alloc[%0.2fM] [%s]", count, strUseSize.c_str(), useSizeMB, totalSizeMB, type);
    }

private:
    //多线程的对象分配器
    std::mutex* mxObjectMutex;
    std::map<std::string, BlockStore*> mxMTObjects;
    std::map<void*, BlockStore*> mxMTObjectBlocks;

    //多线程的内存分配器
    std::mutex* mxMemoryMutex;
    std::map<uint32_t, BlockStore*> mxMTMemories;
    std::map<void*, BlockStore*> mxMTMemoryBlocks;

    AFMemoryLog* mxMemoryLog;
};

//////////////////////////////////////////////////////////////////////////
class AFMalloc
{
public:
    ~AFMalloc()
    {
        ARK_DELETE(m_pMemory);
    }

    //init
    static void Initialize(AFMalloc* pMalloc);

    //real memory pointer
    static AFMalloc* Instance();

    //open log
    void SetLogMemoryOpen(bool open)
    {
        m_pMemory->SetLogOpen(open);
    }

    template<class T, uint32_t batch = 30>
    T * Create(const char* function, uint32_t line)
    {
        auto memory = m_pMemory->Create(typeid(T).name(), sizeof(T), batch, function, line);
        return new(memory) T();
    }

    template<class T, uint32_t batch = 30>
    T * Create(uint32_t param, const char* function, uint32_t line)
    {
        auto memory = m_pMemory->Create(typeid(T).name(), sizeof(T), batch, function, line);
        return new(memory) T(param);
    }

    template<class T, uint32_t batch = 30>
    T * Create(uint32_t param1, uint32_t param2, const char* function, uint32_t line)
    {
        auto memory = m_pMemory->Create(typeid(T).name(), sizeof(T), batch, function, line);
        return new(memory) T(param1, param2);
    }

    template<class T>
    void Delete(T* object, const char* function, uint32_t line)
    {
        if(object == nullptr)
        {
            return;
        }
        object->~T();
        m_pMemory->Delete(object, function, line);
    }

    template<class T>
    T* Malloc(uint32_t size, uint32_t batch, const char* function, uint32_t line)
    {
        return reinterpret_cast<T*>(MallocMemory(size, batch, function, line));
    }

    template< class T >
    void Free(void* memory, uint32_t size, const char* function, uint32_t line)
    {
        FreeMemory(memory, size, function, line);
    }

    //print memory log
    void PrintLogMemory()
    {
        m_pMemory->PrintLogMemory();
    }

protected:
    AFMalloc()
    {
        m_pMemory = ARK_NEW AFMemory();
    }
    //real alloc
    static AFMalloc* m_pMalloc;

    //allocation
    void* MallocMemory(uint32_t size, uint32_t batch, const char* function, uint32_t line)
    {
        if(size == 0)
        {
            return nullptr;
        }

        int new_size = AFMisc::GetNearest2N(size);
        if(new_size != 0)
        {
            return m_pMemory->Malloc(new_size, batch, function, line);
        }
        else
        {
            printf("[%s:%u] Memory Too Large!", function, line);
            return ARK_NEW char[size];
        }
    }

    //free
    void FreeMemory(void* memory, uint32_t size, const char* function, uint32_t line)
    {
        if(memory == nullptr || size == 0)
        {
            return;
        }

        int new_size = AFMisc::GetNearest2N(size);
        if(new_size != 0)
        {
            m_pMemory->Free(memory, function, line);
            return;
        }
        else
        {
            char* p = reinterpret_cast<char*>(memory);
            ARK_DELETE_ARRAY(p);
        }
    }

private:
    // 内存分配器
    AFMemory* m_pMemory;
};