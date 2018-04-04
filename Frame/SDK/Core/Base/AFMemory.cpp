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

#include "AFMemory.hpp"

MemoryBlock::MemoryBlock(uint16_t initblock, uint16_t unitSize)
    :prev(nullptr),
    next(nullptr),
    mSize(initblock* (unitSize + 2)),
    free(initblock - 1),
    first(1)
{
    memset(mPad, 0, mSize - 20);
    char* padData = mPad;
    for (int i = 1; i < initblock; i++)
    {
        *reinterpret_cast<uint16_t*>(padData) = i;
        padData += (unitSize + 2);
    }
}

void* MemoryBlock::operator new(size_t, uint16_t initBlock, uint16_t unitSize)
{
    return ::operator new(sizeof(MemoryBlock) + initBlock * (unitSize + 2));
}

void MemoryBlock::operator delete(void* p)
{
    ::operator delete(p);
}

//////////////////////////////////////////////////////////////////////////

MemoryPool* MemoryPool::memoryPoolInstance = nullptr;
uint16_t MemoryPool::poolMapIndex = 0;

MemoryPool::MemoryPool(int initBlockSize /* = 1024 */, int growBlockSize /* = 256 */)
    :initBlockCount(initBlockSize),
    growBlockcount(growBlockSize),
    firstIndex(0),
    lastIndex(0),
    mpPtr(nullptr) {
    memset(memoryHashMap, 0, sizeof(memoryHashMap));
}

int MemoryPool::Hash(int value)
{
    assert(value > 0);
    int hashCode = poolMapIndex;
    if (hashCode > 0 && (value == poolMap[hashCode - 1]))
    {
        return hashCode - 1;
    }
    poolMap[poolMapIndex++] = value;
    return hashCode;
}

int MemoryPool::AlignBytes(int by)
{
    assert(by > 0);
    if (by <= 4)
    {
        return 4;
    }
    int alignedBytes = (by + POOLALIGNMENT - 1)&(~(POOLALIGNMENT - 1));
    return alignedBytes;
}

int MemoryPool::GetUnitSize(int index)
{
    int unitSize = poolMap[index];
    assert(unitSize != 0 && "unitSize can not be zero");
    return unitSize;
}

void MemoryPool::Initialize(MemoryPool* p)
{
    if (p == nullptr)
    {
        p = new MemoryPool();
    }

    memoryPoolInstance = p;
}

MemoryPool* MemoryPool::GetInstance()
{
    return memoryPoolInstance;
}

void* MemoryPool::Alloc(int bytes)
{
    int alignedByte = AlignBytes(bytes);
    MemoryBlock* block = nullptr;
    MemoryBlock* blockHead = nullptr;
    int hashCode = Hash(alignedByte);

    block = memoryHashMap[hashCode];
    if (block == nullptr)
    {
        assert(memoryHashMap[hashCode] == nullptr);
        memoryHashMap[hashCode] = new (initBlockCount, alignedByte)MemoryBlock(initBlockCount, alignedByte);
        block = memoryHashMap[hashCode];
        firstIndex = hashCode;
        if (hashCode >= firstIndex)
        {
            lastIndex = hashCode;
        }
        return block->mPad + 2;
    }
    blockHead = block;
    while (block && !block->free)
    {
        /*if (block->next == nullptr)
        {
        blockHead = block;
        }*/
        block = block->next;
    }

    if (block)
    {
        char* ptr = block->mPad + (block->first*(alignedByte + 2));
        block->first = *((uint16_t*)ptr);
        --block->free;
        return (ptr);
    }
    else
    {
        if (growBlockcount == 0)
        {
            return nullptr;
        }
        block = new(growBlockcount, alignedByte)MemoryBlock(growBlockcount, alignedByte);
        if (block == nullptr)
        {
            return nullptr;
        }
        block->next = blockHead;
        blockHead->prev = block;
        memoryHashMap[hashCode] = block;

        return block->mPad + 2;
    }
}
void MemoryPool::FreeAlloc(void* p)
{
    unsigned long* pBlock = (unsigned long*)p;
    int unitSize = 0;
    int indexer = 0;
    MemoryBlock* tmp = nullptr;
    for (int i = 0; i < 16; i++)
    {
        tmp = memoryHashMap[i];
        if (memoryHashMap[i] != nullptr)
        {
            while (tmp != nullptr)
            {
                if ((unsigned long*)tmp < (unsigned long*)pBlock &&
                    (unsigned long*)pBlock < (unsigned long*)((char*)tmp + tmp->mSize))
                {
                    unitSize = GetUnitSize(i);
                    indexer = i;
                    break;
                }
                tmp = tmp->next;
            }

            if (indexer != 0)
            {
                break;
            }
        }
    }
    if (tmp == nullptr)
    {
        return;
    }

    *(uint16_t*)p = tmp->first;
    tmp->first = ((char*)pBlock - (char*)tmp->mPad) / (unitSize + 2);
    ++tmp->free;
    if ((unitSize + 2)* tmp->free == tmp->mSize)
    {
        //delete
        if (tmp->prev == nullptr && tmp->next != nullptr)
        {
            memoryHashMap[indexer] = tmp->next;
            tmp->next = nullptr;
            tmp->prev = nullptr;
        }
        else if (tmp->prev != nullptr && tmp->next != nullptr)
        {
            tmp->prev->next = tmp->next;
            tmp->next->prev = tmp->prev;
            tmp->next = nullptr;
            tmp->prev = nullptr;
        }
        else if (tmp->next == nullptr && tmp->prev != nullptr)
        {
            tmp->prev->next = tmp->next;
            tmp->next = nullptr;
            tmp->prev = nullptr;
        }
        delete tmp;
        memoryHashMap[indexer] = nullptr;
    }
    else
    {
        //do nothing
    }
}

MemoryPool::~MemoryPool()
{
    for (auto it : memoryHashMap)
    {
        if (it != nullptr)
        {
            delete it;
        }
    }
}