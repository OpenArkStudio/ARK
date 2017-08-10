// -------------------------------------------------------------------------
//    @FileName         ??    AFRWLock.cpp
//    @Author           ??    Chaunbo.Guo
//    @Date             ??    2017-07-15
//    @Module           ??    AFRWLock
//    @Desc             :     AFRWLock
// -------------------------------------------------------------------------
#ifndef AF_RW_LOCK_H
#define AF_RW_LOCK_H

#ifndef NF_PLATFORM
#if defined( __WIN32__ ) || defined( _WIN32 ) || defined(_WINDOWS) || defined(WIN) || defined(_WIN64) || defined( __WIN64__ )||defined(WIN32) || defined(WIN) ||defined( _MSC_VER )
#   define NF_USE_WINDOWS 1
#endif
#elif NF_PLATFORM == NF_PLATFORM_WIN
#   define NF_USE_WINDOWS 1
#endif

#if NF_USE_WINDOWS == 1
#include <windows.h>
#else
#include <pthread.h>
#endif


class AFNonCopyAble
{
protected:
    AFNonCopyAble() {}
    ~AFNonCopyAble() {}
private:
    AFNonCopyAble(const AFNonCopyAble&);
    const AFNonCopyAble& operator=(const AFNonCopyAble&);
};

#if NF_USE_WINDOWS == 1
class AFCReaderWriterLock : private AFNonCopyAble
{
public:
    AFCReaderWriterLock()
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
        if(++m_Readers == 1)
        {
            ::ResetEvent(m_ClearReadersEvent);
        }
        LeaveCriticalSection(&m_ReaderCount);
        LeaveCriticalSection(&m_Writer);
    }

    void ReaderUnlock(void)
    {
        EnterCriticalSection(&m_ReaderCount);
        if(--m_Readers == 0)
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
class AFCReaderWriterLock : private AFNonCopyAble
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

class AFScopeRdLock : private AFNonCopyAble
{
public:
    AFScopeRdLock(AFCReaderWriterLock &lock) : rwlock(lock)
    {
        rwlock.ReaderLock();
    }

    ~AFScopeRdLock()
    {
        rwlock.ReaderUnlock();
    }
private:
    AFCReaderWriterLock & rwlock;
};

class AFScopeWrLock : private AFNonCopyAble
{
public:
    AFScopeWrLock(AFCReaderWriterLock &lock) : rwlock(lock)
    {
        rwlock.WriterLock();
    }

    ~AFScopeWrLock()
    {
        rwlock.WriterUnLock();
    }
private:
    AFCReaderWriterLock & rwlock;
};

#endif //AF_RW_LOCK_H