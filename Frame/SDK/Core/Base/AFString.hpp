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

#include <string>
#include "SDK/Core/Base/AFCoreDef.hpp"

template<typename TYPE>
class CharTraits
{

};

template<>
class CharTraits<char>
{
public:
    static size_t Length(const char* value)
    {
        return strlen(value);
    }

    static int Compare(const char* lvalue, const char* rvalue)
    {
        return strcmp(lvalue, rvalue);
    }

    static void Copy(char* dst, const char* src, size_t len)
    {
        memcpy(dst, src, len);
    }

    static void Put(char* dst, char value)
    {
        *dst = value;
    }

    static const char* EmptyValue()
    {
        return "";
    }

    static size_t Find(const char* dst, const char* find, size_t begin = 0)
    {
        const char* pos = strstr(&dst[begin], find);

        if (pos == nullptr)
        {
            return -1;
        }

        return size_t(pos - dst);
    }

    static size_t NPos()
    {
        return -1;
    }
};

//////////////////////////////////////////////////////////////////////////

class AFStringAlloc
{
public:
    AFStringAlloc()
    {

    }

    ~AFStringAlloc()
    {

    }

    void* Alloc(size_t size)
    {
        return new char[size];
    }

    void Freee(void* ptr, size_t size)
    {
        delete[](char*)ptr;
    }

    void Swap(AFStringAlloc& src)
    {
        //Do nothing
    }
};

template<typename TYPE, size_t SIZE, typename TRAITS = CharTraits<TYPE>, typename ALLOC = AFStringAlloc>
class AFString
{
    using self_t = AFString<TYPE, SIZE, TRAITS, ALLOC>;

public:
    AFString()
    {
        mxStack[0] = 0;
        mpData = mxStack;
        mnCapacity = SIZE;
        mnSize = 0;
    }

    ~AFString()
    {
        if (mnCapacity > SIZE)
        {
            mxAlloc.Free(mpData, mnCapacity * sizeof(TYPE));
        }
    }

    AFString(const self_t& src)
    {
        Init(src.c_str(), src.length());
    }

    AFString(const TYPE* src)
    {
        Init(src, TRAITS::Length(src));
    }

    AFString(const TYPE* src, size_t len)
    {
        Init(src, len);
    }

    AFString(const TYPE* value1, const TYPE* value2)
    {
        InitWithTwo(value1, TRAITS::Length(value1), value2, TRAITS::Length(value2));
    }

    AFString(const TYPE* value1, size_t len1, const TYPE* value2, size_t len2)
    {
        InitWithTwo(value1, len1, value2, len2);
    }

    template<size_t NEW_SIZE>
    AFString(const AFString<TYPE, NEW_SIZE, TRAITS, ALLOC>& src)
    {
        Init(src.c_str(), src.length());
    }

    void swap(self_t& src)
    {
        size_t tmp_size = src.mnSize;
        size_t tmp_capacity = src.mnCapacity;
        TYPE* tmp_data = src.mpData;
        TYPE tmp_stack[SIZE];

        if (tmp_capacity <= SIZE)
        {
            TRAITS::Copy(tmp_stack, src.mxStack, tmp_size + 1);
        }

        src.mnSize = this->mnSize;
        src.mnCapacity = this->mnCapacity;

        if (this->mnCapacity <= SIZE)
        {
            TRAITS::Copy(src.mxStack, this->mxStack, this->mnSize + 1);
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
            TRAITS::Copy(this->mxStack, tmp_stack, tmp_size + 1);
            this->mpData = this->mxStack;
        }
        else
        {
            mpData = tmp_data;
        }

        mxAlloc.Swap(src.mxAlloc);
    }

    self_t& operator=(const self_t& src)
    {
        return InnerAssign(src.c_str(), src.length());
    }

    template<size_t NEW_SIZE>
    self_t& operator=(const AFString<TYPE, NEW_SIZE, TRAITS, ALLOC>& src)
    {
        return InnerAssign(src.c_str(), src.length());
    }

    self_t& operator=(const TYPE* src)
    {
        return InnerAssign(src, TRAITS::Length(src));
    }

    self_t& operator=(TYPE value)
    {
        return InnerAssign(&value, 1);
    }

    self_t& operator+=(const self_t& src)
    {
        return InnerAppend(src.c_str(), src.length());
    }

    self_t& operator+=(const TYPE* src)
    {
        return InnerAppend(src, TRAITS::Length(src));
    }

    self_t& operator+=(TYPE value)
    {
        return InnerAppend(&value, 1);
    }

    size_t length() const
    {
        return mnSize;
    }

    size_t size() const
    {
        return mnSize;
    }

    bool empty() const
    {
        return (0 == mnSize);
    }

    size_t capacity() const
    {
        return mnCapacity;
    }

    const TYPE* c_str() const
    {
        return mpData;
    }

    void clear()
    {
        if (mpData != mxStack)
        {
            mxAlloc.Free(mpData, mnCapacity * sizeof(TYPE));
        }

        mxStack[0] = 0;
        mpData = mxStack;
        mnCapacity = SIZE;
        mnSize = 0;
    }

    bool compare(const self_t& src) const
    {
        return (TRAITS::Compare(this->c_str(), src.c_str()) == 0);
    }

    bool compare(const TYPE* src) const
    {
        return (TRAITS::Compare(this->c_str(), src) == 0);
    }

    self_t& append(const self_t& src)
    {
        return InnerAppend(src.c_str(), src.length());
    }

    self_t& append(const TYPE* src, const size_t len)
    {
        return InnerAppend(src, len);
    }

    self_t& append(const TYPE* src)
    {
        return InnerAppend(src, TRAITS::Length(src));
    }

    self_t& append(TYPE value)
    {
        return InnerAppend(&value, 1);
    }

    size_t find(const TYPE* src, const size_t begin = 0)
    {
        return TRAITS::Find(mpData, src, begin);
    }

    size_t find_last_of(TYPE value)
    {
        for (size_t i = mnSize - 1; i >= 0; --i)
        {
            if (mpData[i] == value)
            {
                return i;
            }
        }

        return npos();
    }

    size_t npos()
    {
        return TRAITS::NPos();
    }

    self_t substr(size_t begin, size_t len)
    {
        return self_t(&mpData[begin], len);
    }

    size_t get_mem_usage() const
    {
        size_t size = sizeof(self_t);

        if (mnCapacity > SIZE)
        {
            size += mnCapacity * sizeof(TYPE);
        }

        return size;
    }

private:
    void Init(const TYPE* src, size_t len)
    {
        mnSize = len;

        if (mnSize < SIZE)
        {
            mnCapacity = SIZE;
            mpData = mxStack;
        }
        else
        {
            mnCapacity = (mnSize + 1) * 2;
            mpData = (TYPE*)mxAlloc.Alloc(mnCapacity * sizeof(TYPE));
        }

        TRAITS::Copy(mpData, src, mnSize);
        TRAITS::Put(mpData + mnSize, 0);
    }

    void InitWithTwo(const TYPE* value1, size_t size1, const TYPE* value2, size_t size2)
    {
        mnSize = size1 + size2;

        if (mnSize < SIZE)
        {
            mnCapacity = SIZE;
            mpData = mxStack;
        }
        else
        {
            mnCapacity = (mnSize + 1) * 2;
            mpData = (TYPE*)mxAlloc.Alloc(mnCapacity * sizeof(TYPE));
        }

        TRAITS::Copy(mpData, value1, size1);
        TRAITS::Copy(mpData + size1, value2, size2);
        TRAITS::Put(mpData + size1 + size2, 0);
    }

    self_t& InnerAssign(const TYPE* src, size_t len)
    {
        if (len < mnCapacity)
        {
            TRAITS::Copy(mpData, src, len);
            TRAITS::Put(mpData + len, 0);
            mnSize = len;
        }
        else
        {
            self_t tmp(src, len);
            this->swap(tmp);
        }

        return (*this);
    }

    self_t& InnerAppend(const TYPE* src, size_t len)
    {
        const size_t NEW_SIZE = this->mnSize + len;

        if (NEW_SIZE < mnCapacity)
        {
            TRAITS::Copy(mpData + mnSize, src, len);
            TRAITS::Put(mpData + mnSize + len, 0);
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

typedef AFString<char, 16, CharTraits<char>, CoreAlloc> DataNodeName;
typedef AFString<char, 16, CharTraits<char>, CoreAlloc> DataTableName;

namespace std
{
template<>
class hash<DataNodeName>
{
public:
    size_t operator()(DataNodeName const& pro) const
    {
        const char* value = pro.c_str();
        return GetHashValue(value);
    }
};
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(const AFString<TYPE, SIZE, TRAITS, ALLOC>& s1, const AFString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
    return (s1.compare(s2) == 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(const AFString<TYPE, SIZE, TRAITS, ALLOC>& s1, const TYPE* s2)
{
    return (s1.compare(s2) == 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator==(const TYPE* s1, const AFString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
    return (s2.compare(s1) == 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(const AFString<TYPE, SIZE, TRAITS, ALLOC>& s1, const AFString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
    return (s1.compare(s2) != 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(const AFString<TYPE, SIZE, TRAITS, ALLOC>& s1, const TYPE* s2)
{
    return (s1.compare(s2) != 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline bool operator!=(const TYPE* s1, const AFString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
    return (s2.compare(s1) != 0);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline AFString<TYPE, SIZE, TRAITS, ALLOC> operator+(const AFString<TYPE, SIZE, TRAITS, ALLOC>& s1, const AFString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
    return AFString<TYPE, SIZE, TRAITS, ALLOC>(s1).append(s2);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline AFString<TYPE, SIZE, TRAITS, ALLOC> operator+(const AFString<TYPE, SIZE, TRAITS, ALLOC>& s1, const TYPE* s2)
{
    return AFString<TYPE, SIZE, TRAITS, ALLOC>(s1).append(s2);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline AFString<TYPE, SIZE, TRAITS, ALLOC> operator+(const AFString<TYPE, SIZE, TRAITS, ALLOC>& s1, TYPE ch)
{
    return AFString<TYPE, SIZE, TRAITS, ALLOC>(s1).append(ch);
}

template<typename TYPE, size_t SIZE, typename TRAITS, typename ALLOC>
inline AFString<TYPE, SIZE, TRAITS, ALLOC> operator+(const TYPE* s1, const AFString<TYPE, SIZE, TRAITS, ALLOC>& s2)
{
    return AFString<TYPE, SIZE, TRAITS, ALLOC>(s1).append(s2);
}