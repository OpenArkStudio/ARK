/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include <stdint.h>

class ArrayPodAlloc
{
public:
    ArrayPodAlloc() {}
    ~ArrayPodAlloc() {}

    void* Alloc(size_t size)
    {
        return new char[size];
    }

    void Free(void* ptr, size_t size)
    {
        delete[] (char*)ptr;
    }

    void Swap(ArrayPodAlloc& src)
    {

    }
};

//predeclare
template<typename TYPE, size_t SIZE, typename ALLOC = ArrayPodAlloc>
class ArraryPod;

template<typename TYPE, size_t SIZE, typename ALLOC>
class ArraryPod
{
public:
    using self_t = ArraryPod<TYPE, SIZE, ALLOC>;

public:
    ArraryPod()
    {
        mpData = mxStack;
        mnCapacity = SIZE;
        mnSize = 0;
    }

    ArraryPod(const self_t& src)
    {
        mnSize = src.mnSize;
        if (mnSize <= SIZE)
        {
            mpData = mxStack;
            mnCapacity = SIZE;
        }
        else
        {
            mnCapacity = src.mnCapacity;
            mpData = (TYPE*)mxAlloc.Alloc(mnCapacity * sizeof(TYPE));
        }

        memcpy(mpData, src.mpData, mnSize * sizeof(TYPE));
    }

    ~ArraryPod()
    {
        if (mnCapacity > SIZE)
        {
            mxAlloc.Free(mpData, mnCapacity * sizeof(TYPE));
        }
    }

    self_t& operator=(const self_t& rhs)
    {
        self_t tmp(rhs);
        swap(tmp);
        return *this;
    }

    void swap(self_t& src)
    {
        size_t tmp_size = src.mnSize;
        size_t tmp_capacity = src.mnCapacity;
        TYPE* tmp_data = src.mpData;
        TYPE tmp_stack[SIZE];

        if (tmp_capacity <= SIZE)
        {
            memcpy(tmp_stack, src.mxStack, tmp_size * sizeof(TYPE));
        }

        src.mnSize = this->mnSize;
        src.mnCapacity = this->mnCapacity;

        if (this->mnCapacity <= SIZE)
        {
            memcpy(src.mxStack, this->mxStack, mnSize * sizeof(TYPE));
            src.mpData = src.mxStack;
        }
        else
        {
            src.mpData = this->mpData;
        }
        //////////////////////////////////////////////////////////////////////////
        this->mnSize = tmp_size;
        this->mnCapacity = tmp_capacity;

        if (tmp_capacity <= SIZE)
        {
            memcpy(this->mxStack, tmp_stack, tmp_size * sizeof(TYPE));
            this->mpData = this->mxStack;
        }
        else
        {
            this->mpData = tmp_data;
        }

        this->mxAlloc.Swap(src.mxAlloc);
    }

    bool empty()
    {
        return (mnSize == 0);
    }

    size_t size() const
    {
        return mnSize;
    }

    const TYPE* data() const
    {
        return mpData;
    }

    void push_back(const TYPE& data)
    {
        if (mnSize == mnCapacity)
        {
            size_t new_size = mnSize * 2;
            TYPE* p = (TYPE*)mxAlloc.Alloc(new_size * sizeof(TYPE));
            memcpy(p, mpData, mnSize * sizeof(TYPE));

            if (mnCapacity > SIZE)
            {
                mxAlloc.Free(mpData, mnCapacity * sizeof(TYPE));
            }

            mpData = p;
            mnCapacity = new_size;
        }

        mpData[mnSize++] = data;
    }

    void pop_back()
    {
        assert(mnSize > 0);
        --mnSize;
    }

    TYPE& back()
    {
        assert(mnSize > 0);
        return mpData[mnSize - 1];
    }

    const TYPE& back() const
    {
        assert(mnSize > 0);
        return mpData[mnSize - 1];
    }

    TYPE& operator[](size_t index)
    {
        assert(index < mnSize);
        return mpData[index];
    }

    const TYPE& operator[](size_t index) const
    {
        assert(index < mnSize);
        return mpData[index];
    }

    //预分配
    void reserve(size_t size)
    {
        if (size > mnCapacity)
        {
            TYPE* p = (TYPE*)mxAlloc.Alloc(size * sizeof(TYPE));
            memcpy(p, mpData, mnSize * sizeof(TYPE));

            if (mnCapacity > SIZE)
            {
                mxAlloc.Free(mpData, mnCapacity * sizeof(TYPE));
            }

            mpData = p;
            mnCapacity = size;
        }
    }

    void resize(size_t size)
    {
        if (size > mnCapacity)
        {
            //申请现有容量的两倍，如果还不够，就按照size来申请
            size_t new_size = mnCapacity * 2;
            if (new_size < size)
            {
                new_size = size;
            }

            TYPE* p = (TYPE*)mxAlloc.Alloc(new_size * sizeof(TYPE));
            memcpy(p, mpData, mnSize * sizeof(TYPE)); //把原来真正的mnSize数据赋值给新的空间
            if (mnCapacity > SIZE)
            {
                mxAlloc.Free(mpData, mnCapacity * sizeof(TYPE));
            }

            mpData = p;
            mnCapacity = new_size; //容量改成新的
        }

        mnSize = size;
    }

    void resize(size_t size, const TYPE& value)
    {
        if (size > mnCapacity)
        {
            //申请现有容量的两倍，如果还不够，就按照size来申请
            size_t new_size = mnCapacity * 2;
            if (new_size < size)
            {
                new_size = size;
            }

            TYPE* p = (TYPE*)mxAlloc.Alloc(new_size * sizeof(TYPE));
            memcpy(p, mpData, mnSize * sizeof(TYPE)); //把原来真正的mnSize数据赋值给新的空间
            if (mnCapacity > SIZE)
            {
                mxAlloc.Free(mpData, mnCapacity * sizeof(TYPE));
            }

            mpData = p;
            mnCapacity = new_size; //容量改成新的
        }

        if (size > mnSize)
        {
            for (size_t i = mnSize; i < size; ++i)
            {
                mpData[i] = value;
            }
        }

        mnSize = size;
    }

    void insert(size_t index, const TYPE& data)
    {
        assert(index <= mnSize);

        resize(mnSize + 1);
        TYPE* p = mpData + index;
        memmove(p + 1, p, (mnSize - index - 1) * sizeof(TYPE));
        *p = data;
    }

    void remove(size_t index)
    {
        assert(index <= mnSize);

        TYPE* p = mpData + index;
        memmove(p, p + 1, (mnSize - index - 1) * sizeof(TYPE));
        --mnSize;
    }

    //从start开始移除count个元素
    void remove_some(size_t start, size_t count)
    {
        assert((start <= mnSize) && ((start + cout) <= mnSize));
        TYPE* p = mpData + start;
        memmove(p, p + count, (mnSize - start - count) * sizeof(TYPE));
        mnSize -= count;
    }

    void clear()
    {
        mnSize = 0;
    }

    size_t get_mem_usage() const
    {
        size_t size = sizeof(self_t);
        if (mnCapacity > size)
        {
            size += mnCapacity * sizeof(TYPE);
        }

        return size;
    }
private:
    ALLOC mxAlloc;
    TYPE mxStack[SIZE];
    TYPE* mpData;
    size_t mnCapacity; //容量
    size_t mnSize; //数量
};