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

class AFTimerManager;

class AFTimer
{
public:
    using TimerPtr = std::shared_ptr<AFTimer>;
    using TimerWeakPtr = std::weak_ptr<AFTimer>;
    using TimerCallback = std::function<void(void)>;
    using stready_time_point = std::chrono::steady_clock::time_point;

public:
    AFTimer(stready_time_point start_time, std::chrono::nanoseconds last_time, TimerCallback func) noexcept
        : mxStartTime(std::move(start_time))
        , mxLastTime(std::move(last_time))
        , mxCallback(std::move(func))
        , mbActive(true)
    {
    }

    const stready_time_point& GetStartTime() const
    {
        return mxStartTime;
    }

    const std::chrono::nanoseconds& GetLastTime() const
    {
        return mxLastTime;
    }

    std::chrono::nanoseconds& GetLeftTime() const
    {
        return GetLastTime() - (std::chrono::steady_clock::now() - GetStartTime());
    }

    void Cancel()
    {
        mbActive = false;
    }

protected:
    void operator() ()
    {
        if (mbActive)
        {
            mxCallback();
        }
    }
private:
    bool mbActive;
    TimerCallback mxCallback;
    const stready_time_point mxStartTime;
    std::chrono::nanoseconds mxLastTime;
    
    //need add count or forever
    //need add param

    friend class AFTimerManager;
};

class AFTimerManager
{
public:
    using TimerManagerPtr = std::shared_ptr<AFTimerManager>;
    
    template<typename F, typename ...TArgs>
    AFTimer::TimerWeakPtr AddTimer(std::chrono::nanoseconds timeout, F callback, TArgs&& ... args)
    {
        auto timer = std::make_shared<AFTimer>(std::chrono::steady_clock::now(), 
            std::chrono::nanoseconds(timeout),
            std::bind(std::move(callback), std::forward<TArgs>(args)...));
        mxTimers.push(timer);
        return timer;
    }

    void Update()
    {
        while (!mxTimers.empty())
        {
            auto tmp = mxTimers.top();
            if (tmp->GetLeftTime() > std::chrono::nanoseconds::zero())
            {
                break;
            }

            mxTimers.pop();
            (*tmp)();
        }
    }

    bool Empty() const
    {
        return mxTimers.empty();
    }

    std::chrono::nanoseconds GetLeftTime() const
    {
        if (mxTimers.empty())
        {
            return std::chrono::nanoseconds::zero();
        }

        auto result = mxTimers.top()->GetLastTime();
        if (result < std::chrono::nanoseconds::zero())
        {
            return std::chrono::nanoseconds::zero();
        }

        return result;
    }

    void Clear()
    {
        while (!mxTimers.empty())
        {
            mxTimers.pop();
        }
    }

private:
    class CompareTimer
    {
    public:
        bool operator()(const AFTimer::TimerPtr& left, const AFTimer::TimerPtr& right)
        {
            auto start_diff = left->GetStartTime() - right->GetStartTime();
            auto last_diff = left->GetLastTime() - right->GetLastTime();
            auto diff = start_diff.count() - last_diff.count();
            return (diff > 0);
        }
    };

    std::priority_queue<AFTimer::TimerPtr, std::vector<AFTimer::TimerPtr>, CompareTimer> mxTimers;
};