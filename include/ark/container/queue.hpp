/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 QuadHex ArkNX.
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

#include <atomic>
#include <list>

#include "ark/noncopyable.hpp"

namespace ark {

class queue_lock : public noncopyable
{
public:
    explicit queue_lock() {
        flag.clear();
    }

    ~queue_lock() {}

    void lock() {
        while (flag.test_and_set(std::memory_order_acquire))
            ;
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }

private:
    mutable std::atomic_flag flag;
};

using lock = queue_lock;

template<typename T>
class queue : public lock
{
public:
    queue() {}

    virtual ~queue() {}

    bool push(const T& object) {
        lock();

        mList.push_back(object);

        unlock();

        return true;
    }

    bool pop(T& object) {
        lock();

        if (mList.empty())
        {
            unlock();

            return false;
        }

        object = mList.front();
        mList.pop_front();

        unlock();

        return true;
    }

    int count() {
        return mList.size();
    }

private:
    std::list<T> mList;
};

} // namespace ark
