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

namespace ark {

template<typename TYPE>
class char_traits
{
};

template<>
class char_traits<char>
{
public:
    static size_t length(const char* value) {
        return strlen(value);
    }

    static int compare(const char* lvalue, const char* rvalue) {
        return strcmp(lvalue, rvalue);
    }

    static void copy(char* dst, const char* src, size_t len) {
        memcpy(dst, src, len);
    }

    static void put(char* dst, char value) {
        *dst = value;
    }

    static const char* empty_value() {
        return "";
    }

    static size_t find(const char* dst, const char* find, size_t begin = 0) {
        const char* pos = strstr(&dst[begin], find);

        if (pos == nullptr)
        {
            return -1;
        }

        return size_t(pos - dst);
    }

    static size_t n_pos() {
        return -1;
    }
};

//////////////////////////////////////////////////////////////////////////

class string_allocator
{
public:
    string_allocator() {}

    ~string_allocator() {}

    void* alloc(size_t size) {
        return new char[size];
    }

    void free(void* ptr, size_t size) {
        delete[] (char*)ptr;
    }

    void swap(string_allocator& src) {
        // Do nothing
    }
};

template<typename TYPE, size_t SIZE, typename TRAITS = char_traits<TYPE>, typename ALLOC = string_allocator>
class small_string
{
    using self_t = small_string<TYPE, SIZE, TRAITS, ALLOC>;

public:
    small_string() {
        mxStack[0] = 0;
        mpData = mxStack;
        mnCapacity = SIZE;
        mnSize = 0;
    }

    ~small_string() {
        if (mnCapacity > SIZE)
        {
            mxAlloc.free(mpData, mnCapacity * sizeof(TYPE));
        }
    }

    small_string(const self_t& src) {
        init(src.c_str(), src.length());
    }

    explicit small_string(const TYPE* src) {
        init(src, TRAITS::length(src));
    }

    small_string(const TYPE* src, size_t len) {
        init(src, len);
    }

    small_string(const TYPE* value1, const TYPE* value2) {
        init_with_two(value1, TRAITS::length(value1), value2, TRAITS::length(value2));
    }

    small_string(const TYPE* value1, size_t len1, const TYPE* value2, size_t len2) {
        init_with_two(value1, len1, value2, len2);
    }

    template<size_t NEW_SIZE>
    explicit small_string(const small_string<TYPE, NEW_SIZE, TRAITS, ALLOC>& src) {
        init(src.c_str(), src.length());
    }

    void swap(self_t& src) noexcept {
        size_t tmp_size = src.mnSize;
        size_t tmp_capacity = src.mnCapacity;
        TYPE* tmp_data = src.mpData;
        TYPE tmp_stack[SIZE];

        if (tmp_capacity <= SIZE)
        {
            TRAITS::copy(tmp_stack, src.mxStack, tmp_size + 1);
        }

        src.mnSize = this->mnSize;
        src.mnCapacity = this->mnCapacity;

        if (this->mnCapacity <= SIZE)
        {
            TRAITS::copy(src.mxStack, this->mxStack, this->mnSize + 1);
            src.mpData = src.mxStack;
        }
        else
        {
            src.mpData = this->mpData;
        }

        this->mnSize = tmp_size;
        this->mnCapacity = tmp_capacity;

        if (tmp_capacity <= SIZE)
        {
            TRAITS::copy(this->mxStack, tmp_stack, tmp_size + 1);
            this->mpData = this->mxStack;
        }
        else
        {
            mpData = tmp_data;
        }

        mxAlloc.swap(src.mxAlloc);
    }

    self_t& operator=(const self_t& src) {
        if (this != &src)
        {
            inner_assign(src.c_str(), src.length());
        }

        return *this;
    }

    template<size_t NEW_SIZE>
    self_t& operator=(const small_string<TYPE, NEW_SIZE, TRAITS, ALLOC>& src) {
        return inner_assign(src.c_str(), src.length());
    }

    self_t& operator=(const TYPE* src) {
        return inner_assign(src, TRAITS::length(src));
    }

    self_t& operator=(TYPE value) {
        return inner_assign(&value, 1);
    }

    self_t& operator+=(const self_t& src) {
        return inner_append(src.c_str(), src.length());
    }

    self_t& operator+=(const TYPE* src) {
        return inner_append(src, TRAITS::length(src));
    }

    self_t& operator+=(TYPE value) {
        return inner_append(&value, 1);
    }

    size_t length() const {
        return mnSize;
    }

    size_t size() const {
        return mnSize;
    }

    bool empty() const {
        return (0 == mnSize);
    }

    size_t capacity() const {
        return mnCapacity;
    }

    const TYPE* c_str() const {
        return mpData;
    }

    void clear() {
        if (mpData != mxStack)
        {
            mxAlloc.free(mpData, mnCapacity * sizeof(TYPE));
        }

        mxStack[0] = 0;
        mpData = mxStack;
        mnCapacity = SIZE;
        mnSize = 0;
    }

    bool compare(const self_t& src) const {
        return (TRAITS::compare(this->c_str(), src.c_str()) == 0);
    }

    bool compare(const TYPE* src) const {
        return (TRAITS::compare(this->c_str(), src) == 0);
    }

    self_t& append(const self_t& src) {
        return inner_append(src.c_str(), src.length());
    }

    self_t& append(const TYPE* src, const size_t len) {
        return inner_append(src, len);
    }

    self_t& append(const TYPE* src) {
        return inner_append(src, TRAITS::length(src));
    }

    self_t& append(TYPE value) {
        return inner_append(&value, 1);
    }

    size_t find(const TYPE* src, const size_t begin = 0) {
        return TRAITS::find(mpData, src, begin);
    }

    size_t find_last_of(TYPE value) {
        for (int i = int(mnSize - 1); i >= 0; --i)
        {
            if (mpData[i] == value)
            {
                return i;
            }
        }

        return npos();
    }

    size_t npos() {
        return TRAITS::n_pos();
    }

    self_t substr(size_t begin, size_t len) {
        return self_t(&mpData[begin], len);
    }

    size_t get_mem_usage() const {
        size_t size = sizeof(self_t);

        if (mnCapacity > SIZE)
        {
            size += mnCapacity * sizeof(TYPE);
        }

        return size;
    }

private:
    void init(const TYPE* src, size_t len) {
        mnSize = len;

        if (mnSize < SIZE)
        {
            mnCapacity = SIZE;
            mpData = mxStack;
        }
        else
        {
            mnCapacity = (mnSize + 1) * 2;
            mpData = (TYPE*)mxAlloc.alloc(mnCapacity * sizeof(TYPE));
        }

        TRAITS::copy(mpData, src, mnSize);
        TRAITS::put(mpData + mnSize, 0);
    }

    void init_with_two(const TYPE* value1, size_t size1, const TYPE* value2, size_t size2) {
        mnSize = size1 + size2;

        if (mnSize < SIZE)
        {
            mnCapacity = SIZE;
            mpData = mxStack;
        }
        else
        {
            mnCapacity = (mnSize + 1) * 2;
            mpData = (TYPE*)mxAlloc.alloc(mnCapacity * sizeof(TYPE));
        }

        TRAITS::copy(mpData, value1, size1);
        TRAITS::copy(mpData + size1, value2, size2);
        TRAITS::put(mpData + size1 + size2, 0);
    }

    self_t& inner_assign(const TYPE* src, size_t len) {
        if (len < mnCapacity)
        {
            TRAITS::copy(mpData, src, len);
            TRAITS::put(mpData + len, 0);
            mnSize = len;
        }
        else
        {
            self_t tmp(src, len);
            this->swap(tmp);
        }

        return (*this);
    }

    self_t& inner_append(const TYPE* src, size_t len) {
        const size_t NEW_SIZE = this->mnSize + len;

        if (NEW_SIZE < mnCapacity)
        {
            TRAITS::copy(mpData + mnSize, src, len);
            TRAITS::put(mpData + mnSize + len, 0);
            mnSize = NEW_SIZE;
        }
        else
        {
            self_t tmp(this->c_str(), this->length(), src, len);
            this->swap(tmp);
        }

        return (*this);
    }

    ALLOC mxAlloc;
    TYPE mxStack[SIZE];
    TYPE* mpData;
    size_t mnCapacity;
    size_t mnSize;
};

typedef small_string<char, 16, char_traits<char>, core_allocator> data_node_name;
typedef small_string<char, 16, char_traits<char>, core_allocator> data_table_name;

template<typename T>
class hash
{
};

template<>
class hash<data_node_name>
{
public:
    size_t operator()(data_node_name const& pro) const {
        const char* value = pro.c_str();
        return get_hash_value(value);
    }
};

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(
    const small_string<TYPE, SIZE, TRAITS, ALLOC>& s1, const small_string<TYPE, SIZE, TRAITS, ALLOC>& s2) {
    return (s1.compare(s2) == 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(const small_string<TYPE, SIZE, TRAITS, ALLOC>& s1, const TYPE* s2) {
    return (s1.compare(s2) == 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(const TYPE* s1, const small_string<TYPE, SIZE, TRAITS, ALLOC>& s2) {
    return (s2.compare(s1) == 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(
    const small_string<TYPE, SIZE, TRAITS, ALLOC>& s1, const small_string<TYPE, SIZE, TRAITS, ALLOC>& s2) {
    return (s1.compare(s2) != 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(const small_string<TYPE, SIZE, TRAITS, ALLOC>& s1, const TYPE* s2) {
    return (s1.compare(s2) != 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(const TYPE* s1, const small_string<TYPE, SIZE, TRAITS, ALLOC>& s2) {
    return (s2.compare(s1) != 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline small_string<TYPE, SIZE, TRAITS, ALLOC> operator+(
    const small_string<TYPE, SIZE, TRAITS, ALLOC>& s1, const small_string<TYPE, SIZE, TRAITS, ALLOC>& s2) {
    return small_string<TYPE, SIZE, TRAITS, ALLOC>(s1).append(s2);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline small_string<TYPE, SIZE, TRAITS, ALLOC> operator+(
    const small_string<TYPE, SIZE, TRAITS, ALLOC>& s1, const TYPE* s2) {
    return small_string<TYPE, SIZE, TRAITS, ALLOC>(s1).append(s2);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline small_string<TYPE, SIZE, TRAITS, ALLOC> operator+(const small_string<TYPE, SIZE, TRAITS, ALLOC>& s1, TYPE ch) {
    return small_string<TYPE, SIZE, TRAITS, ALLOC>(s1).append(ch);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline small_string<TYPE, SIZE, TRAITS, ALLOC> operator+(
    const TYPE* s1, const small_string<TYPE, SIZE, TRAITS, ALLOC>& s2) {
    return small_string<TYPE, SIZE, TRAITS, ALLOC>(s1).append(s2);
}

} // namespace ark