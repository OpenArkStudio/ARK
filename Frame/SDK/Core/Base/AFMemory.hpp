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
    //閾捐〃鍓嶅悗鎸囬拡
    MemoryBlock*prev;
    MemoryBlock* next;
    //鍒嗛厤MemoryBlock鎬诲ぇ灏忋€?
    int mSize;
    //鏈垎閰嶅潡涓暟
    int free;
    //棣栦釜鏈垎閰嶅潡绱㈠紩
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
    //鍒嗛厤涓嶅悓鍐呭瓨鍧楁椂鍏跺搴旂殑鏄犲皠琛?
    std::map<int, int>poolMap;
    //鍐呭瓨姹犲榻愬ぇ灏忋€?
    const int POOLALIGNMENT = 8;
    //鍒濆鍖栧唴瀛樺潡
    int initBlockCount;
    //鍐呭瓨鍧椾笉瓒冲闀跨殑鍧楁暟銆?
    int growBlockcount;
    //棣栧瓙鍐呭瓨姹犱笅鏍?
    unsigned firstIndex;
    //鏈唴瀛樻睜涓嬫爣銆?
    unsigned lastIndex;
    //鏈€澶?6绉嶄笉鍚屽唴瀛樺潡澶у皬锛屼篃灏辨槸璇村瓙鍐呭瓨姹犳渶澶氭湁16涓€?
    MemoryBlock* memoryHashMap[16];
    MemoryBlock** mpPtr;
    //璁＄畻涓嶅悓鍐呭瓨鍧楀搴旂殑hashCode
    int Hash(int);
    //瀵归綈瀛楄妭
    int AlignBytes(int);
    //杩斿洖鍒嗛厤鍧楀ぇ灏忋€?
    int GetUnitSize(int);
protected:
    static MemoryPool* memoryPoolInstance;
    MemoryPool(int initBlockSize = 1024, int growBlockSize = 256);
public:
    //鍒嗛厤鍐呭瓨
    void* Alloc(int);
    //閲婃斁鍐呭瓨銆?
    void FreeAlloc(void*);
    static void Initialize(MemoryPool* p);
    //杩斿洖鍏ㄥ眬鍐呭瓨姹犲疄渚?
    static MemoryPool* GetInstance();
    ~MemoryPool();
};