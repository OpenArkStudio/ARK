/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include "AFMacros.hpp"

namespace ark
{

    template <typename VALUE, bool is_smart_ptr>
    class AFMapValueType
    {
    public:
        using value_type = VALUE*;
    };

    template <typename VALUE>
    class AFMapValueType<VALUE, false>
    {
    public:
        using value_type = VALUE*;
    };

    template <typename VALUE>
    class AFMapValueType<VALUE, true>
    {
    public:
        using value_type = std::shared_ptr<VALUE>;
    };

    template <typename KEY, typename VALUE, bool is_smart_ptr>
    class AFMapBase
    {
    public:
        using k_type = KEY;
        using v_type = typename AFMapValueType<VALUE, is_smart_ptr>::value_type;
        using map_type = std::map<k_type, v_type>;
        using value_type = typename map_type::value_type;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = typename map_type::iterator;
        using const_iterator = typename map_type::const_iterator;
        using reverse_iterator = typename map_type::reverse_iterator;
        using const_reverse_iterator = typename map_type::const_reverse_iterator;

        AFMapBase() = default;
        ~AFMapBase() = default;

        std::size_t size() const
        {
            return nodes_.size();
        }

        bool empty() const
        {
            return nodes_.empty();
        }

        std::pair<iterator, bool> insert(const k_type& key, const v_type& value)
        {
            return nodes_.insert(value_type(key, value));
        }

        iterator erase(iterator it)
        {
            return nodes_.erase(it);
        }

        std::size_t erase(const k_type& key)
        {
            return nodes_.erase(key);
        }

        iterator find(const k_type& key)
        {
            return nodes_.find(key);
        }

        iterator begin()
        {
            return nodes_.begin();
        }

        iterator end()
        {
            return nodes_.end();
        }

        //const_iterator begin() const
        //{
        //    return nodes_.cbegin();
        //}

        //const_iterator end() const
        //{
        //    return nodes_.cend();
        //}

        reverse_iterator rbegin()
        {
            return nodes_.rbegin();
        }

        reverse_iterator rend()
        {
            return nodes_.rend();
        }

        //const_reverse_iterator rbegin() const
        //{
        //    return nodes_.crbegin();
        //}

        //const_reverse_iterator rend() const
        //{
        //    return nodes_.crend();
        //}

        void clear()
        {
            nodes_.clear();
        }

        v_type find_value(const k_type& key)
        {
            auto iter = find(key);
            if (iter != end())
            {
                return iter->second;
            }

            return nullptr;
        }

    private:
        map_type nodes_;
    };

    template <typename KEY, typename VALUE>
    using AFMap = AFMapBase<KEY, VALUE, false>;

    template <typename KEY, typename VALUE>
    using AFMapEx = AFMapBase<KEY, VALUE, true>;

}
