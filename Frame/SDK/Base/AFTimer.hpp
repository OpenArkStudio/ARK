/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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
#include "AFMacros.hpp"

template<typename TimerHandler>
class TimerContext
{
public:
    TimerContext(const TimerHandler& callback, int32_t duration, int32_t repeattimes, bool forever = false)
        : mbRemoved(false)
        , mbForever(forever)
        , mnID(0)
        , mnRepeattimes(repeattimes)
        , mnDuration(duration)
        , mnEndTime(0)
        , mxHandler(callback)
    {
    }

    TimerContext(TimerHandler&& callback, int32_t duration, int32_t repeattimes, bool forever = false)
        : mbRemoved(false)
        , mbForever(forever)
        , mnID(0)
        , mnRepeattimes(repeattimes)
        , mnDuration(duration)
        , mnEndTime(0)
        , mxHandler(std::forward<TimerHandler>(callback))
    {
    }

    void Init(TimerHandler&& callback, int32_t duration, int32_t repeattimes, bool forever = false)
    {
        mbRemoved(false);
        mbForever(forever);
        mnID(0);
        mnRepeattimes(repeattimes);
        mnDuration(duration);
        mnEndTime(0);
        mxHandler = std::forward<TimerHandler>(callback);
    }

    bool Removed() const { return mbRemoved; }
    void SetRemoved(bool val) { mbRemoved = val; }

    bool Forever() const { return mbForever; }
    void SetForever(bool val) { mbForever = val; }

    uint32_t ID() const { return mnID; }
    void SetID(uint32_t val) { mnID = val; }

    int32_t Repeattimes() const { return mnRepeattimes; }
    void SetRepeattimes(int32_t val) { mnRepeattimes = val; }

    int32_t Duration() const { return mnDuration; }
    void SetDuration(int32_t val) { mnDuration = val; }

    int64_t EndTime() const { return mnEndTime; }
    void SetEndTime(int64_t val) { mnEndTime = val; }

    void Expired(uint32_t id)
    {
        ARK_ASSERT_RET_NONE(mxHandler != nullptr);
        mxHandler(id);
    }

private:
    bool mbRemoved;
    bool mbForever;
    uint32_t mnID;
    int32_t mnRepeattimes;
    int32_t mnDuration;
    int64_t mnEndTime;
    TimerHandler mxHandler;
};

template<typename TContainer, uint8_t TSize>
class TimerWheel
{
public:
    TimerWheel()
        : mnHead(0)
    {
    }

    TContainer& operator[](uint8_t pos)
    {
        ARK_ASSERT_NO_EFFECT(pos < TSize);
        return mxArray[pos];
    }

    TContainer& Front()
    {
        ARK_ASSERT_NO_EFFECT(mnHead < TSize);
        return mxArray[mnHead];
    }

    void PopFront()
    {
        uint32_t tmp = ++mnHead;
        mnHead = tmp % TSize;
    }

    void Size()
    {
        return TSize;
    }

    size_t NextSlot()
    {
        return mnHead;
    }

    bool Round()
    {
        return (mnHead == 0);
    }

private:
    TContainer mxArray[TSize];
    uint32_t mnHead;
};

template<typename TimerHandler>
class Timer
{
private:
    using TimerContextPtr = std::shared_ptr<TimerContext<TimerHandler>>;
    constexpr uint8_t WHEEL_SIZE = 255; //wheel size
    constexpr int32_t PRECISION = 10; //ms
    constexpr int32_t TIMER_ID_SHIFT = 32;
    constexpr int32_t MAX_TIMER_NUM = (1 << 16) - 1;
    using TimerWheelType = TimerWheel<std::list<uint64_t>, WHEEL_SIZE>;

public:
    Timer()
    {
        mxWheels.emplace_back();
        mxWheels.emplace_back();
        mxWheels.emplace_back();
        mxWheels.emplace_back();
    }

    ~Timer() {}

    uint32_t ExpiredOnce(int32_t duration, const TimerHandler& callback)
    {
        if (duration < PRECISION)
        {
            duration = PRECISION;
        }

        auto ptr = std::make_shared<TimerContext<TimerHandler>>(callback, duration, 1, false);
        return AddNewTimer(ptr);
    }

    uint32_t ExpiredOnce(int32_t duration, TimerHandler&& callback)
    {
        if (duration < PRECISION)
        {
            duration = PRECISION;
        }

        auto ptr = std::make_shared<TimerContext<TimerHandler>>(std::forward<TimerHandler>(callback), duration, 1, false);
        return AddNewTimer(ptr);
    }

    uint32_t Repeat(int32_t duration, int32_t repeattimes, const TimerHandler& callback)
    {
        if (duration < PRECISION)
        {
            duration = PRECISION;
        }

        auto ptr = std::make_shared<TimerContext<TimerHandler>>(callback, duration, repeattimes, false);
        return AddNewTimer(ptr);
    }

    uint32_t Repeat(int32_t duration, int32_t repeattimes, TimerHandler&& callback)
    {
        if (duration < PRECISION)
        {
            duration = PRECISION;
        }

        auto ptr = std::make_shared<TimerContext<TimerHandler>>(std::forward<TimerHandler>(callback), duration, repeattimes, false);
        return AddNewTimer(ptr);
    }

    void Romove(uint32_t timer_id)
    {
        auto iter = mxTimers.find(timer_id);
        if (iter != mxTimers.end())
        {
            iter->second->SetRemoved(true);
            return;
        }

        for (auto& new_iter : mxNewTimers)
        {
            if (new_iter->GetID() == timer_id)
            {
                new_iter->SetRemoved(true);
                break;
            }
        }
    }

    //Call this function in your logic loop
    void Update()
    {
        auto now = AFCTimeBase::GetInstance().GetNowMillisecond();
        for (auto& iter : mxNewTimers)
        {
            if (iter->Removed())
            {
                continue;
            }

            AddNewTimer(now, iter);
        }

        mxNewTimers.clear();

        if (mnPreTick == 0)
        {
            mnPreTick = now;
        }

        mnTick += (now - mnPreTick);
        mnPreTick = now;

        auto& wheels = mxWheels;
        while (mnTick >= PRECISION)
        {
            mnTick -= PRECISION;
            if (mbStop)
            {
                continue;
            }

            do 
            {
                auto& timers = wheels[0].Front();
                if (!timers.empty())
                {
                    Expired(timers);
                }
                wheels[0].PopFront();
            } while (0);
        }

        int index = 0;
        for (auto wheel = wheels.begin(); wheel != wheels.end(); ++wheel, ++index)
        {
            auto nextWheel = wheel;
            if (wheels.end() == (++nextWheel))
            {
                break;
            }

            if (!wheel->Round())
            {
                break;
            }

            auto& timers = nextWheel->Front();
            while (!timers.empty())
            {
                auto key = timers.front();
                timers.pop_front();
                auto slot = GetSlot(key, index);
                (*wheel)[slot].push_front(key);
            }

            nextWheel->PopFront();
        }
    }

    void StopAll()
    {
        mbStop = true;
    }

    void StartAll()
    {
        mbStop = false;
    }

private:
    //slots:
    //|   00000000   |   00000000   |   00000000   |   00000000   |
    //|    not use   |  wheel3_slot |  wheel2_slot |  wheel1_slot |
    uint32_t MakeKey(uint32_t timer_id, uint32_t slots)
    {
        return ((static_cast<uint64_t>(timer_id) << TIMER_ID_SHIFT) | slots);
    }

    uint8_t GetSlot(uint64_t key, int which_wheel)
    {
        return (key >> (which_wheel * 8)) & 0xFF;
    }

    uint32_t AddNewTimer(const TimerContextPtr& t)
    {
        t->SetEndTime(t->GetDuration() + AFCTimeBase::GetInstance().GetNowMillisecond());
        if (t->GetID() == 0)
        {
            if (mnUUID == 0 || mnUUID == MAX_TIMER_NUM)
            {
                mnUUID = 1;
            }

            auto& timers = mxTimers;
            while (timers.find(mnUUID) != timers.end())
            {
                ++mnUUID;
            }

            t->SetID(mnUUID);
            mxTimers.emplace(t->ID(), t);
        }

        mxNewTimers.push_back(t);
        return t->ID();
    }

    void AddTimer(int64_t now, const TimerContextPtr& t)
    {
        if (t->EndTime() > now)
        {
            auto diff = t->Duration();
            auto offset = diff % PRECISION;
            if (offset > 0)
            {
                diff += PRECISION;
            }

            auto slotCount = diff / PRECISION;
            slotCount = ((slotCount > 0) ? slotCount : 1);
            uint64_t key = 0;
            
            do 
            {
                int index = 0;
                uint32_t slots = 0;
                for (auto iter = mxWheels.begin(); it != mxWheels.end(); ++iter, ++index)
                {
                    auto& wheel = *it;
                    slotCount += wheel.NextSlot();
                    uint8_t slot = (slotCount - 1) % wheel.Size();
                    slotCount -= slot;
                    slots |= (static_cast<uint32_t>(slot) << (index * 8));
                    key = MakeKey(t->ID(), slots);
                    if (slotCount < wheel.Size())
                    {
                        wheel[slot].push_back(key);
                        break;
                    }

                    slotCount /= wheel.Size();
                }
            } while (0);
        }
        else
        {
            mxWheels[0].Front().push_back(MakeKey(t->ID(), 0));
        }
    }

    void Expired(std::list<uint64_t>& expires)
    {
        while (!expires.empty())
        {
            auto key = expires.front();
            expires.pop_front();
            uint32_t timer_id = static_cast<uint32_t>(key >> TIMER_ID_SHIFT);

            auto iter = mxTimers.find(timer_id);
            if (iter == mxTimers.end())
            {
                continue;
            }

            auto& contextPtr = iter->second;
            if (!contextPtr->Removed())
            {
                contextPtr->Expired(timer_id);
                contextPtr->SetRepeattimes(contextPtr->Repeattimes() - 1);
                if (contextPtr->Repeattimes() > 0 || contextPtr->Forever())
                {
                    AddNewTimer(contextPtr);
                    continue;
                }
            }
            
            mxTimers.erase(iter);
        }
    }

private:
    int64_t mnTick{ 0 };
    int64_t mnPreTick{ 0 };
    uint32_t mnUUID{ 1 };
    bool mbStop{ false };
    std::vector<TimerWheelType> mxWheels;
    std::unordered_map<uint32_t, TimerContextPtr> mxTimers;
    std::vector<TimerContextPtr> mxNewTimers;
};