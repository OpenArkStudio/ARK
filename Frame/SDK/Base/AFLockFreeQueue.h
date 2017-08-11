// -------------------------------------------------------------------------
//    @FileName			:    AFQueue.h
//    @Author      :    Ark Game Tech
//    @Date    :    2011-01-21 21:49
//    @Module    :
//
// -------------------------------------------------------------------------

#ifndef NF_LOCKFREEQUEUE_H
#define NF_LOCKFREEQUEUE_H

#include "SDK/Base/AFPlatform.hpp"
#include "common/readerwriterqueue.h"


template<typename T>
class AFLockFreeQueue
{
public:
    AFLockFreeQueue()
    {
    }

    virtual ~AFLockFreeQueue()
    {
    }

    bool Push(const T& object)
    {
        return mList.enqueue(object);
    }

    bool Pop(T& object)
    {
        //return mList.wait_dequeue_timed(object, std::chrono::milliseconds(5));
        return mList.try_dequeue(object);
    }

    int Count()
    {
        return mList.size_approx();
    }

private:
    moodycamel::BlockingReaderWriterQueue <T> mList;
};

#endif