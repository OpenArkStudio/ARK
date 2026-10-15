/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "ark/core_def.hpp"
#include "ark/macros.hpp"

namespace ark {

template<typename TYPE>
class string_traits
{
};

template<>
class string_traits<char>
{
public:
    static size_t hash(const char* value) {
        return get_hash_value(value);
    }

    static size_t length(const char* value) {
        return strlen(value);
    }

    static bool equal(const char* lvalue, const char* rvalue) {
        return (0 == strcmp(lvalue, rvalue));
    }

    static void copy(char* dst, const char* src, size_t len) {
        memcpy(dst, src, len);
    }
};

template<typename TYPE>
class string_traits_no_case : public string_traits<TYPE>
{
};

template<>
class string_traits_no_case<char> : public string_traits<char>
{
public:
    static size_t hash(const char* value) {
        return get_hash_value_no_case(value);
    }

    static bool equal(const char* lvalue, const char* rvalue) {
        return (0 == ARK_STRICMP(lvalue, rvalue));
    }
};

//////////////////////////////////////////////////////////////////////////
class pod_string_allocator
{
public:
    pod_string_allocator() = default;
    ~pod_string_allocator() = default;

    void* alloc(size_t size) {
        ARK_NEW_ARRAY_RET(char, size);
    }

    void free(void* ptr, size_t size) {
        ARK_DELETE_ARRAY(char, ptr);
    }

    void swap(pod_string_allocator& src) {
        // Do nothing
    }
};

template<typename TYPE, typename DATA>
class pod_string_node
{
public:
    pod_string_node* next;
    size_t hash;
    DATA data;
    TYPE name[1]; // like TYPE* name /  char* name
};

//////////////////////////////////////////////////////////////////////////

// predeclared
template<typename TYPE, typename DATA, typename TRAITS = string_traits<TYPE>, typename ALLOC = pod_string_allocator>
class pod_string;

template<typename TYPE, typename DATA, typename TRAITS = string_traits<TYPE>, typename ALLOC = pod_string_allocator>
class pod_string_iterator
{
private:
    using hash_t = pod_string<TYPE, DATA, TRAITS, ALLOC>;
    using node_t = pod_string_node<TYPE, DATA>;

public:
    pod_string_iterator() = delete;

    pod_string_iterator(const hash_t* self, node_t* node)
        : mpSelf(self)
        , mpNode(node) {}

    pod_string_iterator& operator++() {
        node_t* next = mpNode->next;

        if (next != nullptr)
        {
            mpNode = next;
        }
        else
        {
            mpNode = mpSelf->to_next(mpNode);
        }

        return *this;
    }

    pod_string_iterator& operator++(int) {
        pod_string_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    bool operator==(const pod_string_iterator& other) const {
        return (mpNode == other.mpNode);
    }

    bool operator!=(const pod_string_iterator& other) const {
        return (mpNode != other.mpNode);
    }

    const TYPE* get_key() const {
        return mpNode->name;
    }

    DATA* get_data() const {
        return mpNode->data;
    }

    node_t* get_node() const {
        return mpNode;
    }

private:
    const hash_t* mpSelf;
    node_t* mpNode;
};

template<typename TYPE, typename DATA, typename TRAITS, typename ALLOC>
class pod_string
{
private:
    using hash_t = pod_string<TYPE, DATA, TRAITS, ALLOC>;
    using node_t = pod_string_node<TYPE, DATA>;

    using iterator = pod_string_iterator<TYPE, DATA, TRAITS, ALLOC>;
    using const_iterator = pod_string_iterator<TYPE, DATA, TRAITS, ALLOC>;

public:
    explicit pod_string(size_t size = 0)
        : mnSize(size) {
        mnCount = 0;

        if (size > 0)
        {
            mpBuckets = (node_t**)mxAlloc.alloc(sizeof(node_t*) * size);
            memset(mpBuckets, 0, sizeof(node_t*) * size);
        }
        else
        {
            mpBuckets = NULL;
        }
    }

    pod_string(const hash_t& src) {
        const size_t size = src.mnSize;
        mnCount = 0;
        mnSize = size;

        if (size > 0)
        {
            mpBuckets = (node_t**)mxAlloc.alloc(sizeof(node_t*) * size);
            memset(mpBuckets, 0, sizeof(node_t*) * size);

            for (size_t i = 0; i < size; ++i)
            {
                node_t* p = src.mpBuckets[i];

                while (p)
                {
                    if (!add(p->name, p->data))
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

    ~pod_string() {
        clear();

        if (mpBuckets)
        {
            mxAlloc.free(mpBuckets, sizeof(node_t*) * mnSize);
        }
    }

    void swap(hash_t& src) {
        node_t** tmp_buckets = src.mpBuckets;
        size_t tmp_size = src.mnSize;
        size_t tmp_count = src.mnCount;

        src.mpBuckets = this->mpBuckets;
        src.mnSize = this->mnSize;
        src.mnCount = this->mnCount;
        this->mpBuckets = tmp_buckets;
        this->mnSize = tmp_size;
        this->mnCount = tmp_count;
        mxAlloc.swap(src.mxAlloc);
    }

    void clear() {
        for (size_t i = 0; i < mnSize; ++i)
        {
            node_t* p = mpBuckets[i];

            while (p)
            {
                node_t* next = p->next;
                delete_node(p);
                p = next;
            }

            mpBuckets[i] = NULL;
        }
    }

    size_t get_count() const {
        return mnCount;
    }

    bool set(const TYPE* name, const DATA& data) {
        node_t* node = find_node(name);

        if (node == nullptr)
        {
            return add(name, data);
        }

        node->data = data;
        return true;
    }

    bool add(const TYPE* name, const DATA& data) {
        assert(name != nullptr);

        if (mnCount == mnSize)
        {
            expand();
        }

        size_t hash = TRAITS::hash(name);
        size_t bucket = get_bucket(hash);
        node_t* p = new_node(name);

        p->next = mpBuckets[bucket];
        p->hash = hash;
        p->data = data;

        mpBuckets[bucket] = p;
        ++mnCount;
        return true;
    }

    bool add(const TYPE* name, const DATA& data, TYPE*& pName) {
        assert(name != nullptr);

        if (mnCount == mnSize)
        {
            expand();
        }

        size_t hash = TRAITS::hash(name);
        size_t bucket = get_bucket(hash);
        node_t* p = new_node(name);

        p->next = mpBuckets[bucket];
        p->hash = hash;
        p->data = data;

        mpBuckets[bucket] = p;
        ++mnCount;

        pName = p->name;
        return true;
    }

    bool remove(const TYPE* name) {
        assert(name != nullptr);

        if (mnSize == 0)
        {
            return false;
        }

        size_t hash = TRAITS::hash(name);
        size_t bucket = get_bucket(hash);
        node_t* p = mpBuckets[bucket];

        while (p)
        {
            if ((p->hash == hash) && TRAITS::equal(p->name, name))
            {
                erase_node(bucket, p);
                delete_node(p);
                --mnCount;
                return true;
            }

            p = p->next;
        }

        return false;
    }

    bool remove_data(const TYPE* name, const DATA& data) {
        assert(name != nullptr);

        if (mnSize == 0)
        {
            return false;
        }

        size_t hash = TRAITS::hash(name);
        size_t bucket = get_bucket(hash);
        node_t* p = mpBuckets[bucket];

        while (p)
        {
            if ((p->hash == hash) && TRAITS::equal(p->name, name) && (p->data == data))
            {
                erase_node(bucket, p);
                delete_node(p);
                --mnCount;
                return true;
            }

            p = p->next;
        }

        return false;
    }

    bool exists(const TYPE* name) const {
        return (find_node(name) != nullptr);
    }

    iterator find(const TYPE* name) {
        return iterator(this, find_node(name));
    }

    const_iterator find(const TYPE* name) const {
        return const_iterator(this, find_node(name));
    }

    bool get_data(const TYPE* name, DATA& data) const {
        node_t* p = find_node(name);

        if (p == nullptr)
        {
            return false;
        }

        data = p->data;
        return true;
    }

    iterator begin() {
        for (size_t i = 0; i < mnSize; ++i)
        {
            if (mpBuckets[i])
            {
                return iterator(this, mpBuckets[i]);
            }
        }

        return end();
    }

    iterator end() {
        return iterator(this, NULL);
    }

    const_iterator begin() const {
        for (size_t i = 0; i < mnSize; ++i)
        {
            if (mpBuckets[i])
            {
                return const_iterator(this, mpBuckets[i]);
            }
        }

        return end();
    }

    const_iterator end() const {
        return const_iterator(this, NULL);
    }

    iterator erase(iterator iter) {
        iterator tmp(iter);
        ++tmp;
        node_t* p = iter.get_node();
        erase_node(get_bucket(p->hash), p);
        delete_node(p);
        --mnCount;

        return tmp;
    }

    size_t get_mem_usage() const {
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
    size_t get_bucket(size_t hash) const {
        return (hash % mnSize);
    }

    node_t* new_node(const TYPE* name) {
        const size_t length = TRAITS::length(name);
        const size_t size = sizeof(node_t) + length * sizeof(TYPE);
        node_t* p = (node_t*)mxAlloc.alloc(size);
        TRAITS::copy(p->name, name, length + 1);
        return p;
    }

    void delete_node(node_t* p) {
        mxAlloc.free(p, sizeof(node_t) + TRAITS::length(p->name) * sizeof(TYPE));
    }

    void erase_node(size_t bucket, node_t* p) {
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

    node_t* find_node(const TYPE* name) const {
        assert(name != nullptr);

        if (0 == mnSize)
        {
            return nullptr;
        }

        size_t hash = TRAITS::hash(name);
        size_t bucket = get_bucket(hash);
        node_t* node = mpBuckets[bucket];

        while (node)
        {
            if ((node->hash == hash) && TRAITS::equal(node->name, name))
            {
                return node;
            }

            node = node->next;
        }

        return nullptr;
    }

    void expand() {
        size_t new_size = mnSize * 2 + 1; // hash bucket
        node_t** new_buckets = (node_t**)mxAlloc.alloc(sizeof(node_t*) * new_size);
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
            mxAlloc.free(mpBuckets, sizeof(node_t*) * mnSize);
        }

        mpBuckets = new_buckets;
        mnSize = new_size;
    }

    node_t* to_next(node_t* node) const {
        assert(node != nullptr);

        for (size_t i = get_bucket(node->hash) + 1; i < mnSize; ++i)
        {
            if (mpBuckets[i])
            {
                return mpBuckets[i];
            }
        }

        return nullptr;
    }

private:
    friend class pod_string_iterator<TYPE, DATA, TRAITS, ALLOC>;
    ALLOC mxAlloc;
    node_t** mpBuckets;
    size_t mnSize;
    size_t mnCount;
};

} // namespace ark
