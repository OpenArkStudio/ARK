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

#include "base/AFPlatform.hpp"

namespace ark {

template<typename K, typename V>
class AFLruMap
{
public:
    using iterator = typename std::unordered_map<K, V>::iterator;

    explicit AFLruMap(size_t capacity = 1024)
    {
        capacity_ = capacity > 0 ? capacity : 1024;
    }

    ~AFLruMap() = default;

    bool empty() const
    {
        return kv_.size() == 0;
    }

    size_t size() const
    {
        return kv_.size();
    }

    iterator begin()
    {
        return kv_.begin();
    }

    iterator end()
    {
        return kv_.end();
    }

    iterator find(const K& key)
    {
        iterator it = kv_.find(key);
        if (it != kv_.end() && kl_.front() != key)
        {
            auto ki = ki_.find(key);
            kl_.splice(kl_.begin(), kl_, ki->second); // move key to the front
            ki->second = kl_.begin();
        }
        return it;
    }

    // The key is not inserted if it already exists.
    void insert(const K& key, const V& value)
    {
        auto r = kv_.insert(std::make_pair(key, value));
        if (!r.second)
            return;

        kl_.push_front(key);
        ki_[key] = kl_.begin();

        if (kv_.size() > capacity_)
        {
            K k = kl_.back();
            kl_.pop_back();
            kv_.erase(k);
            ki_.erase(k);
        }
    }

    void erase(iterator it)
    {
        if (it != kv_.end())
        {
            auto ki = ki_.find(it->first);
            kl_.erase(ki->second);
            ki_.erase(ki);
            kv_.erase(it);
        }
    }

    void erase(const K& key)
    {
        this->erase(kv_.find(key));
    }

    void clear()
    {
        kv_.clear();
        ki_.clear();
        kl_.clear();
    }

    void swap(AFLruMap& x) noexcept
    {
        kv_.swap(x.kv_);
        ki_.swap(x.ki_);
        kl_.swap(x.kl_);
        std::swap(capacity_, x.capacity_);
    }

private:
    std::unordered_map<K, V> kv_;
    std::unordered_map<K, typename std::list<K>::iterator> ki_;
    std::list<K> kl_; // key list
    size_t capacity_; // max capacity
};

} // namespace ark