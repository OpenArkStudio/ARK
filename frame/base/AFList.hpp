/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
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

namespace ark
{

    template <typename T>
    class AFList
    {
    public:
        using list_type = std::list<T>;
        using reference = T&;
        using const_reference = const T&;
        using iterator = typename list_type::iterator;
        using const_iterator = typename list_type::const_iterator;
        using reverse_iterator = typename list_type::reverse_iterator;
        using const_reverse_iterator = typename list_type::const_reverse_iterator;

        AFList() = default;
        ~AFList() = default;

        std::size_t size() const
        {
            return values_.size();
        }

        bool empty() const
        {
            return values_.empty();
        }

        iterator erase(const_iterator& it)
        {
            return values_.erase(it);
        }

        iterator emplace(iterator it, const_reference value)
        {
            return values_.emplace(it, value);
        }

        void emplace_front(const_reference value)
        {
            values_.emplace_front(value);
        }

        void emplace_back(const_reference value)
        {
            values_.emplace_back(value);
        }

        void pop_front()
        {
            values_.pop_front();
        }

        void pop_back()
        {
            values_.pop_back();
        }

        void push_front(const_reference value)
        {
            return values_.push_front(value);
        }

        void push_back(const_reference value)
        {
            return values_.push_back(value);
        }

        void clear()
        {
            values_.clear();
        }

        void remove(const_reference value)
        {
            return values_.remove(value);
        }

        bool get_value(const std::size_t index, reference value)
        {
            if (index >= values_.size())
            {
                return false;
            }

            auto iter = values_.begin();
            std::advance(iter, index);
            value = *iter;
            return true;
        }

        bool exist(const_reference value)
        {
            auto iter = std::find(values_.begin(), values_.end(), value);
            return (iter != values_.end());
        }

        iterator begin()
        {
            return values_.begin();
        }

        iterator end()
        {
            return values_.end();
        }

        reverse_iterator rbegin()
        {
            return values_.rbegin();
        }

        reverse_iterator rend()
        {
            return values_.end();
        }

    private:
        list_type values_;
    };

    //template <typename T>
    //class AFList
    //{
    //public:
    //    virtual ~AFList() = default;
    //    bool Add(const T& id);
    //    bool Find(const T& id);
    //    bool Remove(const T& id);
    //    bool ClearAll();

    //    bool First(T& id);
    //    bool Next(T& id);
    //    bool Get(const int32_t index, T& id);
    //    int Count();

    //private:
    //    using TList = std::list<T>;
    //    TList mtObjConfigList;
    //    typename std::list<T>::iterator mCurIter;
    //};

    //template<typename T>
    //bool AFList<T>::Add(const T& id)
    //{
    //    mtObjConfigList.push_back(id);
    //    return true;
    //}

    //template<typename T>
    //bool AFList<T>::Remove(const T& id)
    //{
    //    if (Find(id))
    //    {
    //        mtObjConfigList.remove(id);
    //        return true;
    //    }

    //    return false;
    //}

    //template < typename T >
    //bool AFList<T>::ClearAll()
    //{
    //    mtObjConfigList.clear();
    //    return true;
    //}

    //template < typename T >
    //bool AFList<T>::First(T& id)
    //{
    //    if (mtObjConfigList.size() <= 0)
    //    {
    //        return false;
    //    }

    //    mCurIter = mtObjConfigList.begin();

    //    if (mCurIter != mtObjConfigList.end())
    //    {
    //        id = *mCurIter;
    //        return true;
    //    }

    //    return false;
    //}

    //template < typename T >
    //bool AFList<T>::Next(T& id)
    //{
    //    if (mCurIter == mtObjConfigList.end())
    //    {
    //        return false;
    //    }

    //    ++mCurIter;

    //    if (mCurIter != mtObjConfigList.end())
    //    {
    //        id = *mCurIter;
    //        return true;
    //    }

    //    return false;
    //}

    //template < typename T >
    //bool AFList<T>::Find(const T& id)
    //{
    //    auto it = std::find(mtObjConfigList.begin(), mtObjConfigList.end(), id);

    //    if (it != mtObjConfigList.end())
    //    {
    //        return true;
    //    }

    //    return false;
    //}

    //template < typename T >
    //bool AFList<T>::Get(const int32_t index, T& id)
    //{
    //    if (index >= mtObjConfigList.size())
    //    {
    //        return false;
    //    }

    //    auto it = this->mtObjConfigList.begin();
    //    std::advance(it, index);

    //    id = *it;

    //    return true;
    //}

    //template < typename T >
    //int AFList<T>::Count()
    //{
    //    return (int)(mtObjConfigList.size());
    //}

}
