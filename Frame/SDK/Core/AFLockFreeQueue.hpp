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

#include "SDK/Core/AFPlatform.hpp"
#include "common/readerwriterqueue.h"

namespace ark
{

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

        size_t Count()
        {
            return mList.size_approx();
        }

    private:
        moodycamel::BlockingReaderWriterQueue<T> mList;
    };

}