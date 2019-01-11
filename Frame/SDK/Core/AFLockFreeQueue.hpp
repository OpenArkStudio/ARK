/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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
#include "Common/readerwriterqueue.h"

namespace ark
{

    template<typename T>
    class AFLockFreeQueue
    {
    public:
        AFLockFreeQueue() = default;
        ~AFLockFreeQueue() = default;

        bool Push(const T& object)
        {
            return queue_.enqueue(object);
        }

        bool Pop(T& object)
        {
            return queue_.try_dequeue(object);
        }

        size_t Count()
        {
            return queue_.size_approx();
        }

    private:
        moodycamel::BlockingReaderWriterQueue<T> queue_;
    };

}