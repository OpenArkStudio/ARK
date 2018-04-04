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

//From:
//https://github.com/DavidLiRemini/MemoryPool

template <typename T>
struct ListNode
{
    ListNode<T>* prev;
    ListNode<T>* next;
    T data;
    ListNode() : prev(nullptr), next(nullptr) {}
};

template <typename T>
class ListPool
{
protected:
    typedef ListNode<T> ListNode;
public:
    typedef ListNode* LinkType;
protected:
    LinkType node;
private:
    LinkType Create_Node()
    {
        LinkType tmp = new ListNode();
        return tmp;
    }
public:
    ListPool()
    {
        Empty_initialize();
    }
    void Empty_initialize()
    {
        node = Create_Node();
        node->prev = node;
        node->next = node;
    }
    void Remove(ListNode& link)
    {
        assert(link.prev != nullptr);
        assert(link.next != nullptr);
        link.prev->next = link.next;
        link.next->prev = link.prev;

        link.next = nullptr;
        link.prev = nullptr;
    }
    void AddNode(const T& x)
    {
        LinkType tmp = Create_Node();
        tmp->data = x;
        tmp->next = node;
        tmp->prev = node->prev;
        node->prev->next = tmp;
        node->prev = tmp;
    }

    uint32_t ListSize() const
    {
        uint32_t t = 0;
        LinkType ptr = node->next;
        while (ptr != node)
        {
            ptr = ptr->next;
            ++t;
        }
        return t;
    }
};

struct MemoryBlock
{
    //链表前后指针
    MemoryBlock*prev;
    MemoryBlock* next;
    //分配MemoryBlock总大小。
    int mSize;
    //未分配块个数
    int free;
    //首个未分配块索引
    int first;
    //Padding
    char mPad[2];
    MemoryBlock(uint16_t initblock = 1, uint16_t unitSize = 0);
    static void* operator new(size_t, uint16_t, uint16_t);
    static void operator delete(void*);
};

class MemoryPool
{
private:
    static uint16_t poolMapIndex;
    //分配不同内存块时其对应的映射表
    std::map<int, int>poolMap;
    //内存池对齐大小。
    const int POOLALIGNMENT = 8;
    //初始化内存块
    int initBlockCount;
    //内存块不足增长的块数。
    int growBlockcount;
    //首子内存池下标
    unsigned firstIndex;
    //末内存池下标。
    unsigned lastIndex;
    //最多16种不同内存块大小，也就是说子内存池最多有16个。
    MemoryBlock* memoryHashMap[16];
    MemoryBlock** mpPtr;
    //计算不同内存块对应的hashCode
    int Hash(int);
    //对齐字节
    int AlignBytes(int);
    //返回分配块大小。
    int GetUnitSize(int);
protected:
    static MemoryPool* memoryPoolInstance;
    MemoryPool(int initBlockSize = 1024, int growBlockSize = 256);
public:
    //分配内存
    void* Alloc(int);
    //释放内存。
    void FreeAlloc(void*);
    static void Initialize(MemoryPool* p);
    //返回全局内存池实例
    static MemoryPool* GetInstance();
    ~MemoryPool();
};