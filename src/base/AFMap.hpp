/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "base/AFMacros.hpp"

namespace ark {

template<typename VALUE>
class AFMapValueType
{
public:
    using value_type = VALUE*;
};

template<typename KEY, typename VALUE, typename MAP_TYPE>
class AFMapBase
{
public:
    using k_type = KEY;
    using v_type = typename AFMapValueType<VALUE>::value_type;
    using map_type = MAP_TYPE;
    using value_type = typename map_type::value_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = typename map_type::iterator;
    using const_iterator = typename map_type::const_iterator;

    AFMapBase() = default;
    ~AFMapBase()
    {
        clear();
    }

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

    // will delete allocated memory
    iterator erase(iterator it)
    {
        if (it != nodes_.end())
        {
            ARK_DELETE(it->second);

            return nodes_.erase(it);
        }

        return it;
    }

    // will delete allocated memory
    bool erase(const k_type& key)
    {
        auto iter = nodes_.find(key);
        if (iter == nodes_.end())
        {
            return false;
        }

        ARK_DELETE(iter->second);

        erase(iter);
        return true;
    }

    iterator find(const k_type& key)
    {
        return nodes_.find(key);
    }

    iterator begin() noexcept
    {
        return nodes_.begin();
    }

    iterator end() noexcept
    {
        return nodes_.end();
    }

    const_iterator begin() const noexcept
    {
        return nodes_.cbegin();
    }

    const_iterator end() const noexcept
    {
        return nodes_.cend();
    }

    const_iterator cbegin() const noexcept
    {
        return nodes_.cbegin();
    }

    const_iterator cend() const noexcept
    {
        return nodes_.cbegin();
    }

    // will delete allocated memory
    void clear()
    {
        for (auto iter : nodes_)
        {
            ARK_DELETE(iter.second);
        }

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

//////////////////////////////////////////////////////////////////////////
// smart ptr map
template<typename VALUE>
class AFSmartPtrMapValueType
{
public:
    using value_type = std::shared_ptr<VALUE>;
};

template<typename KEY, typename VALUE, typename MAP_TYPE>
class AFSmartPtrMapBase
{
public:
    using k_type = KEY;
    using v_type = typename AFSmartPtrMapValueType<VALUE>::value_type;
    using map_type = MAP_TYPE;
    using value_type = typename map_type::value_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = typename map_type::iterator;
    using const_iterator = typename map_type::const_iterator;

    AFSmartPtrMapBase() = default;
    ~AFSmartPtrMapBase()
    {
        clear();
    }

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

    iterator begin() noexcept
    {
        return nodes_.begin();
    }

    iterator end() noexcept
    {
        return nodes_.end();
    }

    const_iterator begin() const noexcept
    {
        return nodes_.cbegin();
    }

    const_iterator end() const noexcept
    {
        return nodes_.cend();
    }

    const_iterator cbegin() const noexcept
    {
        return nodes_.cbegin();
    }

    const_iterator cend() const noexcept
    {
        return nodes_.cbegin();
    }

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

template<typename KEY, typename VALUE>
using AFMap = AFMapBase<KEY, VALUE, std::map<KEY, VALUE*>>;

template<typename KEY, typename VALUE>
using AFSmartPtrMap = AFSmartPtrMapBase<KEY, VALUE, std::map<KEY, std::shared_ptr<VALUE>>>;

template<typename KEY, typename VALUE>
using AFHashmap = AFMapBase<KEY, VALUE, std::unordered_map<KEY, VALUE*>>;

template<typename KEY, typename VALUE>
using AFSmartPtrHashmap = AFSmartPtrMapBase<KEY, VALUE, std::unordered_map<KEY, std::shared_ptr<VALUE>>>;

} // namespace ark
