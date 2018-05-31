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

#include "AFPlatform.hpp"

class AFSpinLock
{
public:
    AFSpinLock() : _atomic_lock(false) {}
    AFSpinLock(AFSpinLock const &) = default;
    AFSpinLock(AFSpinLock &&) = default;
    AFSpinLock &operator=(AFSpinLock const &) = default;
    AFSpinLock &operator=(AFSpinLock &&) = default;

    ~AFSpinLock(void) = default;

    inline void lock()
    {
        while (_atomic_lock.exchange(true));
    }

    inline void unlock()
    {
        _atomic_lock = false;
    }

    inline bool isLocked() const
    {
        return _atomic_lock;
    }

private:
    std::atomic_bool _atomic_lock;
};