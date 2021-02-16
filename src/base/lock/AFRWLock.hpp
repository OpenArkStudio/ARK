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
#include "base/AFNoncopyable.hpp"

namespace ark {

class AFReaderWriterLock : public AFNoncopyable
{
    mutable std::shared_mutex mtx_;

public:
    void ReadLock()
    {
        mtx_.lock_shared();
    }

    void ReadUnlock()
    {
        mtx_.unlock_shared();
    }

    void WriteLock()
    {
        mtx_.lock();
    }

    void WriteUnlock()
    {
        mtx_.unlock();
    }
};

class AFScopeRLock : public AFNoncopyable
{
public:
    explicit AFScopeRLock(AFReaderWriterLock& lock)
        : rwlock(lock)
    {
        rwlock.ReadLock();
    }

    ~AFScopeRLock()
    {
        rwlock.ReadUnlock();
    }

private:
    AFReaderWriterLock& rwlock;
};

class AFScopeWLock : public AFNoncopyable
{
public:
    explicit AFScopeWLock(AFReaderWriterLock& lock)
        : rwlock_(lock)
    {
        rwlock_.WriteLock();
    }

    ~AFScopeWLock()
    {
        rwlock_.WriteUnlock();
    }

private:
    AFReaderWriterLock& rwlock_;
};

} // namespace ark
