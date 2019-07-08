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

#include "base/AFPlatform.hpp"
#include "base/AFNoncopyable.hpp"

namespace ark {

#if ARK_PLATFORM == PLATFORM_WIN

class AFCReaderWriterLock : public AFNoncopyable
{
public:
    explicit AFCReaderWriterLock()
    {
        m_Readers = 0;
        InitializeCriticalSection(&m_Writer);
        InitializeCriticalSection(&m_ReaderCount);
        m_ClearReadersEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
    }

    ~AFCReaderWriterLock()
    {
        WaitForSingleObject(m_ClearReadersEvent, INFINITE);
        CloseHandle(m_ClearReadersEvent);
        DeleteCriticalSection(&m_Writer);
        DeleteCriticalSection(&m_ReaderCount);
    }

    /*Read, reset events */
    void ReaderLock(void)
    {
        EnterCriticalSection(&m_Writer);
        EnterCriticalSection(&m_ReaderCount);

        if (++m_Readers == 1)
        {
            ::ResetEvent(m_ClearReadersEvent);
        }

        LeaveCriticalSection(&m_ReaderCount);
        LeaveCriticalSection(&m_Writer);
    }

    void ReaderUnlock(void)
    {
        EnterCriticalSection(&m_ReaderCount);

        if (--m_Readers == 0)
        {
            ::SetEvent(m_ClearReadersEvent);
        }

        LeaveCriticalSection(&m_ReaderCount);
    }

    void WriterLock(void)
    {
        EnterCriticalSection(&m_Writer);
        WaitForSingleObject(m_ClearReadersEvent, INFINITE);
    }

    void WriterUnLock(void)
    {
        LeaveCriticalSection(&m_Writer);
    }

private:
    CRITICAL_SECTION m_Writer;
    CRITICAL_SECTION m_ReaderCount;
    int m_Readers;
    HANDLE m_ClearReadersEvent;
};

#else

class AFCReaderWriterLock : public AFNoncopyable
{
public:
    AFCReaderWriterLock()
    {
        ::pthread_rwlock_init(&rwlock, NULL);
    }

    ~AFCReaderWriterLock()
    {
        ::pthread_rwlock_destroy(&rwlock);
    }

    void ReaderLock()
    {
        ::pthread_rwlock_rdlock(&rwlock);
    }

    void ReaderUnlock(void)
    {
        ::pthread_rwlock_unlock(&rwlock);
    }

    void WriterLock(void)
    {
        ::pthread_rwlock_wrlock(&rwlock);
    }

    void WriterUnLock(void)
    {
        ::pthread_rwlock_unlock(&rwlock);
    }

private:
    pthread_rwlock_t rwlock;
};
#endif

class AFScopeRLock : public AFNoncopyable
{
public:
    explicit AFScopeRLock(AFCReaderWriterLock &lock)
        : rwlock(lock)
    {
        rwlock.ReaderLock();
    }

    ~AFScopeRLock()
    {
        rwlock.ReaderUnlock();
    }

private:
    AFCReaderWriterLock &rwlock;
};

class AFScopeWLock : public AFNoncopyable
{
public:
    explicit AFScopeWLock(AFCReaderWriterLock &lock)
        : rwlock_(lock)
    {
        rwlock_.WriterLock();
    }

    ~AFScopeWLock()
    {
        rwlock_.WriterUnLock();
    }

private:
    AFCReaderWriterLock &rwlock_;
};

} // namespace ark