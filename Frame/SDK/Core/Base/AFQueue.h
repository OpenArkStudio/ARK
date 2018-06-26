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

#include "SDK/Core/Base/AFPlatform.hpp"
#include "SDK/Core/Base/AFNoncopyable.hpp"

class AFLock : public AFNoncopyable
{
public:
    explicit AFLock()
    {
        flag.clear();
    }

    ~AFLock()
    {
    }

    void lock()
    {
        while (flag.test_and_set(std::memory_order_acquire));
    }

    void unlock()
    {
        flag.clear(std::memory_order_release);
    }

private:
    mutable std::atomic_flag flag;
};

template<typename T>
class AFQueue : public AFLock
{
public:
    AFQueue()
    {
    }

    virtual ~AFQueue()
    {
    }

    bool Push(const T& object)
    {
        lock();

        mList.push_back(object);

        unlock();

        return true;
    }

    bool Pop(T& object)
    {
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

    int Count()
    {
        return mList.size();
    }

private:
    std::list<T> mList;
};

