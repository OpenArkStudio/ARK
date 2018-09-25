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

#include "AFCoreDef.hpp"
#include "AFMacros.hpp"

template<typename TYPE>
class StringTraits
{

};

template<>
class StringTraits<char>
{
public:
    static size_t Hash(const char* value)
    {
        return GetHashValue(value);
    }

    static size_t Length(const char* value)
    {
        return strlen(value);
    }

    static bool Equal(const char* lvalue, const char* rvalue)
    {
        return (0 == strcmp(lvalue, rvalue));
    }

    static void Copy(char* dst, const char* src, size_t len)
    {
        memcpy(dst, src, len);
    }
};

template<typename TYPE>
class StringTraitsNoCase : public StringTraits<TYPE>
{

};

template<>
class StringTraitsNoCase<char> : public StringTraits<char>
{
public:
    static size_t hash(const char* value)
    {
        return GetHashValueNoCase(value);
    }

    static bool equal(const char* lvalue, const char* rvalue)
    {
        return (0 == ARK_STRICMP(lvalue, rvalue));
    }
};
//////////////////////////////////////////////////////////////////////////
class StringPodAlloc
{
public:
    StringPodAlloc() = default;
    ~StringPodAlloc() = default;

    void* Alloc(size_t size)
    {
        ARK_NEW_ARRAY_RET(char, size);
    }

    void Free(void* ptr, size_t size)
    {
        ARK_DELETE_ARRAY(char, ptr);
    }

    void Swap(StringPodAlloc& src)
    {
        //Do nothing
    }
};

template<typename TYPE, typename DATA>
class StringPodNode
{
public:
    StringPodNode* next;
    size_t hash;
    DATA data;
    TYPE name[1]; //like TYPE* name /  char* name
};

//////////////////////////////////////////////////////////////////////////

//predeclared
template<typename TYPE, typename DATA, typename TRAITS = StringTraits<TYPE>, typename ALLOC = StringPodAlloc>
class StringPod;

template<typename TYPE, typename DATA, typename TRAITS = StringTraits<TYPE>, typename ALLOC = StringPodAlloc>
class StringPodIter
{
private:
    using hash_t = StringPod<TYPE, DATA, TRAITS, ALLOC>;
    using node_t = StringPodNode<TYPE, DATA>;

public:
    StringPodIter() = delete;

    StringPodIter(const hash_t* self, node_t* node)
    {
        mpSelf = self;
        mpNode = node;
    }

    StringPodIter& operator++()
    {
        node_t* next = mpNode->next;

        if (next != nullptr)
        {
            mpNode = next;
        }
        else
        {
            mpNode = mpSelf->ToNext(mpNode);
        }

        return *this;
    }

    StringPodIter& operator++(int)
    {
        StringPodIter tmp(*this);
        ++(*this);
        return tmp;
    }

    bool operator==(const StringPodIter& other) const
    {
        return (mpNode == other.mpNode);
    }

    bool operator!=(const StringPodIter& other) const
    {
        return (mpNode != other.mpNode);
    }

    const TYPE* GetKey() const
    {
        return mpNode->name;
    }

    DATA* GetData() const
    {
        return mpNode->data;
    }

    node_t* GetNode() const
    {
        return mpNode;
    }
private:
    const hash_t* mpSelf;
    node_t* mpNode;
};

template<typename TYPE, typename DATA, typename TRAITS, typename ALLOC>
class StringPod
{
private:
    using hash_t = StringPod<TYPE, DATA, TRAITS, ALLOC>;
    using node_t = StringPodNode<TYPE, DATA>;

    using iterator = StringPodIter<TYPE, DATA, TRAITS, ALLOC>;
    using const_iterator = StringPodIter<TYPE, DATA, TRAITS, ALLOC>;

public:
    explicit StringPod(size_t size = 0)
    {
        mnSize = size;
        mnCount = 0;

        if (size > 0)
        {
            mpBuckets = (node_t**)mxAlloc.Alloc(sizeof(node_t*) * size);
            memset(mpBuckets, 0, sizeof(node_t*) * size);
        }
        else
        {
            mpBuckets = NULL;
        }
    }

    StringPod(const hash_t& src)
    {
        const size_t size = src.mnSize;
        mnCount = 0;
        mnSize = size;

        if (size > 0)
        {
            mpBuckets = (node_t**)mxAlloc.Alloc(sizeof(node_t*) * size);
            memset(mpBuckets, 0, sizeof(node_t*) * size);

            for (size_t i = 0; i < size; ++i)
            {
                node_t* p = src.mpBuckets[i];

                while (p)
                {
                    if (!Add(p->name, p->data))
                    {
                        ARK_ASSERT(0, "Add fail ", __FILE__, __FUNCTION__);
                    }

                    p = p->next;
                }
            }
        }
        else
        {
            mpBuckets = NULL;
        }
    }

    ~StringPod()
    {
        Clear();

        if (mpBuckets)
        {
            mxAlloc.Free(mpBuckets, sizeof(node_t*) * mnSize);
        }
    }

    void Swap(hash_t& src)
    {
        node_t** tmp_buckets = src.mpBuckets;
        size_t tmp_size = src.mnSize;
        size_t tmp_count = src.mnCount;

        src.mpBuckets = this->mpBuckets;
        src.mnSize = this->mnSize;
        src.mnCount = this->mnCount;
        this->mpBuckets = tmp_buckets;
        this->mnSize = tmp_size;
        this->mnCount = tmp_count;
        mxAlloc.Swap(src.mxAlloc);
    }

    void Clear()
    {
        for (size_t i = 0; i < mnSize; ++i)
        {
            node_t* p = mpBuckets[i];

            while (p)
            {
                node_t* next = p->next;
                DeleteNode(p);
                p = next;
            }

            mpBuckets[i] = NULL;
        }
    }

    size_t GetCount() const
    {
        return mnCount;
    }

    bool Set(const TYPE* name, const DATA& data)
    {
        node_t* node = FindNode(name);

        if (node == nullptr)
        {
            return Add(name, data);
        }

        node->data = data;
        return true;
    }

    bool Add(const TYPE* name, const DATA& data)
    {
        assert(name != nullptr);

        if (mnCount == mnSize)
        {
            Expand();
        }

        size_t hash = TRAITS::Hash(name);
        size_t bucket = GetBucket(hash);
        node_t* p = NewNode(name);

        p->next = mpBuckets[bucket];
        p->hash = hash;
        p->data = data;

        mpBuckets[bucket] = p;
        ++mnCount;
        return true;
    }

    bool Add(const TYPE* name, const DATA& data, TYPE*& pName)
    {
        assert(name != nullptr);

        if (mnCount == mnSize)
        {
            Expand();
        }

        size_t hash = TRAITS::Hash(name);
        size_t bucket = GetBucket(hash);
        node_t* p = NewNode(name);

        p->next = mpBuckets[bucket];
        p->hash = hash;
        p->data = data;

        mpBuckets[bucket] = p;
        ++mnCount;

        pName = p->name;
        return true;
    }

    bool Remove(const TYPE* name)
    {
        assert(name != nullptr);

        if (mnSize == 0)
        {
            return false;
        }

        size_t hash = TRAITS::Hash(name);
        size_t bucket = GetBucket(hash);
        node_t* p = mpBuckets[bucket];

        while (p)
        {
            if ((p->hash == hash) && TRAITS::Equal(p->name, name))
            {
                EraseNode(bucket, p);
                DeleteNode(p);
                --mnCount;
                return true;
            }

            p = p->next;
        }

        return false;
    }

    bool RemoveData(const TYPE* name, const DATA& data)
    {
        assert(name != nullptr);

        if (mnSize == 0)
        {
            return false;
        }

        size_t hash = TRAITS::Hash(name);
        size_t bucket = GetBucket(hash);
        node_t* p = mpBuckets[bucket];

        while (p)
        {
            if ((p->hash == hash) && TRAITS::Equal(p->name, name) && (p->data == data))
            {
                EraseNode(bucket, p);
                DeleteNode(p);
                --mnCount;
                return true;
            }

            p = p->next;
        }

        return false;
    }

    bool exists(const TYPE* name) const
    {
        return (FindNode(name) != nullptr);
    }

    iterator find(const TYPE* name)
    {
        return iterator(this, FindNode(name));
    }

    const_iterator find(const TYPE* name) const
    {
        return const_iterator(this, FindNode(name));
    }

    bool GetData(const TYPE* name, DATA& data) const
    {
        node_t* p = FindNode(name);

        if (p == nullptr)
        {
            return false;
        }

        data = p->data;
        return true;
    }

    iterator Begin()
    {
        for (size_t i = 0; i < mnSize; ++i)
        {
            if (mpBuckets[i])
            {
                return iterator(this, mpBuckets[i]);
            }
        }

        return End();
    }

    iterator End()
    {
        return iterator(this, NULL);
    }

    const_iterator Begin() const
    {
        for (size_t i = 0; i < mnSize; ++i)
        {
            if (mpBuckets[i])
            {
                return const_iterator(this, mpBuckets[i]);
            }
        }

        return End();
    }

    const_iterator End() const
    {
        return const_iterator(this, NULL);
    }

    iterator Erase(iterator iter)
    {
        iterator tmp(iter);
        ++tmp;
        node_t* p = iter.GetNode();
        EraseNode(GetBucket(p->hash), p);
        DeleteNode(p);
        --mnCount;

        return tmp;
    }

    size_t get_mem_usage() const
    {
        size_t size = sizeof(hash_t);

        for (size_t i = 0; i < mnSize; ++i)
        {
            node_t* p = mpBuckets[i];

            while (p)
            {
                size += sizeof(node_t) + TRAITS::length(p->name) * sizeof(TYPE);
                p = p->next;
            }
        }

        size += sizeof(node_t) * mnSize;
        return size;
    }

protected:
    size_t GetBucket(size_t hash) const
    {
        return (hash % mnSize);
    }

    node_t* NewNode(const TYPE* name)
    {
        const size_t length = TRAITS::Length(name);
        const size_t size = sizeof(node_t) + length * sizeof(TYPE);
        node_t* p = (node_t*)mxAlloc.Alloc(size);
        TRAITS::Copy(p->name, name, length + 1);
        return p;
    }

    void DeleteNode(node_t* p)
    {
        mxAlloc.Free(p, sizeof(node_t) + TRAITS::Length(p->name) * sizeof(TYPE));
    }

    void EraseNode(size_t bucket, node_t* p)
    {
        assert(bucket < mnSize);
        assert(p != nullptr);

        node_t* node = mpBuckets[bucket];

        if (node == p)
        {
            mpBuckets[bucket] = p->next;
            return;
        }

        while (node)
        {
            if (node->next == p)
            {
                node->next = p->next;
                return;
            }

            node = node->next;
        }
    }

    node_t* FindNode(const TYPE* name) const
    {
        assert(name != nullptr);

        if (0 == mnSize)
        {
            return nullptr;
        }

        size_t hash = TRAITS::Hash(name);
        size_t bucket = GetBucket(hash);
        node_t* node = mpBuckets[bucket];

        while (node)
        {
            if ((node->hash == hash) && TRAITS::Equal(node->name, name))
            {
                return node;
            }

            node = node->next;
        }

        return NULL;
    }

    void Expand()
    {
        size_t new_size = mnSize * 2 + 1; //hash bucket
        node_t** new_buckets = (node_t**)mxAlloc.Alloc(sizeof(node_t*) * new_size);
        memset(new_buckets, 0, sizeof(node_t*) * new_size);

        for (size_t i = 0; i < mnSize; ++i)
        {
            if (nullptr != mpBuckets)
            {
                node_t* p = mpBuckets[i];

                while (p)
                {
                    node_t* next = p->next;
                    size_t bucket = size_t(p->hash) % new_size;

                    p->next = new_buckets[bucket];
                    new_buckets[bucket] = p;
                    p = next;
                }
            }
        }

        if (mpBuckets)
        {
            mxAlloc.Free(mpBuckets, sizeof(node_t*) * mnSize);
        }

        mpBuckets = new_buckets;
        mnSize = new_size;
    }

    node_t* ToNext(node_t* node) const
    {
        assert(node != nullptr);

        for (size_t i = GetBucket(node->hash) + 1; i < mnSize; ++i)
        {
            if (mpBuckets[i])
            {
                return mpBuckets[i];
            }
        }

        return nullptr;
    }
private:
    friend class StringPodIter<TYPE, DATA, TRAITS, ALLOC>;
    ALLOC mxAlloc;
    node_t** mpBuckets;
    size_t mnSize;
    size_t mnCount;
};

