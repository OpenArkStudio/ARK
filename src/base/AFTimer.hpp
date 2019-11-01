/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "base/AFPlatform.hpp"
#include "base/AFDefine.hpp"
#include "base/AFSingleton.hpp"
#include "base/AFDateTime.hpp"

namespace ark {

const uint32_t FREE_TIMER_SIZE = 1000; // free timer count, predefined
const uint32_t TIMER_PRECISION = 10;   // 10 ms per step
const uint32_t TV_BITS = 8;
const uint32_t TV_SIZE = 1 << TV_BITS; // 256 slots each time_vec
const uint32_t TV_MASK = TV_SIZE - 1;
const uint32_t TV_NUM = 6; // 6 levels

class AFTimeWheelData
{
public:
    uint64_t idx{0};
    uint32_t count{0};    // =0 means forever
    uint64_t interval{0}; // interval count by step
    uint64_t expires{0};
    TIMER_FUNCTOR callback;

    // callback data
    AFGUID entity_id{NULL_GUID};
};

class AFTimeWheelVec
{
public:
    int index{0};
    std::vector<std::list<AFTimeWheelData*>> vec;
};

class AFTimeWheelManager
{
public:
    void Init(uint64_t now_time)
    {
        now_slot_ = 0;
        timer_idx_ = 0;
        last_update_time_ = now_time;

        for (int i = 0; i < FREE_TIMER_SIZE; ++i)
        {
            AFTimeWheelData* data = ARK_NEW AFTimeWheelData();
            ARK_ASSERT_RET_NONE(data != nullptr);
            free_timers_.push_back(data);
        }

        for (int i = 0; i < TV_NUM; ++i)
        {
            AFTimeWheelVec* tv = ARK_NEW AFTimeWheelVec();
            InitTimerVec(tv, TV_SIZE);
            tw_vecs_[i] = tv;
        }
    }

    void Update(int64_t now_time)
    {
        UpdateTimer(now_time);
    }

    void Shut()
    {
        for (auto iter : timers_)
        {
            ARK_DELETE(iter.second);
        }

        for (auto iter : free_timers_)
        {
            ARK_DELETE(iter);
        }

        for (auto iter : tw_vecs_)
        {
            ARK_DELETE(iter);
        }

        timers_.clear();
        free_timers_.clear();
    }

    uint64_t AddTimer(
        const AFGUID& entity_id, uint32_t count, const std ::chrono::milliseconds interval, TIMER_FUNCTOR&& callback)
    {
        AFTimeWheelData* timer = GetFreeTimer();
        timer->idx = ++timer_idx_;
        timer->expires = (interval.count() / TIMER_PRECISION) + now_slot_;
        timer->count = count;
        timer->interval = (interval.count() / TIMER_PRECISION);
        timer->callback = callback;
        timer->entity_id = entity_id;

        AddTimerInternal(timer);
        timers_.insert(make_pair(timer->idx, timer));

        return timer->idx;
    }

    bool RemoveTimer(uint64_t idx)
    {
        auto iter = timers_.find(idx);
        if (iter == timers_.end())
        {
            return false;
        }

        AFTimeWheelData* timer = iter->second;
        timers_.erase(iter);

        // timer->callback = nullptr;
        return true;
    }

protected:
    void InitTimerVec(AFTimeWheelVec* tv, int32_t size)
    {
        tv->index = 0;
        tv->vec.resize(size);
        for (int i = 0; i < size; ++i)
        {
            tv->vec[i] = std::list<AFTimeWheelData*>();
        }
    }

    AFTimeWheelData* GetFreeTimer()
    {
        AFTimeWheelData* timer = nullptr;
        if (free_timers_.empty())
        {
            timer = ARK_NEW AFTimeWheelData;
            ARK_ASSERT_RET_VAL(timer != nullptr, nullptr);
        }
        else
        {
            timer = free_timers_.front();
            free_timers_.pop_front();
        }

        memset(timer, 0, sizeof(AFTimeWheelData));
        return timer;
    }

    void AddToFreeTimer(AFTimeWheelData* timer)
    {
        free_timers_.push_back(timer);
    }

    void UpdateTimer(int64_t now_time)
    {
        uint64_t passedSlot = (now_time - last_update_time_) / TIMER_PRECISION;

        if (passedSlot == 0)
        {
            return;
        }

        last_update_time_ += passedSlot * TIMER_PRECISION;

        for (uint64_t i = 0; i < passedSlot; ++i)
        {
            UpdateSlotTimer();
        }
    }

    void AddTimerInternal(AFTimeWheelData* timer_data)
    {
        uint64_t expires = timer_data->expires;
        int64_t idx = expires - now_slot_;
        if (idx <= 0)
        {
            AFTimeWheelVec* tv = tw_vecs_[0];
            tv->vec[tv->index].push_back(timer_data);
            return;
        }

        uint32_t index = 0;
        uint64_t max_tv_num = 0;
        for (int32_t i = 0; i < TV_NUM; ++i)
        {
            max_tv_num = ((uint64_t)1 << ((i + 1) * TV_BITS));
            if ((uint64_t)idx < max_tv_num)
            {
                index = ((expires >> (i * TV_BITS)) & TV_MASK);
                tw_vecs_[i]->vec[index].push_back(timer_data);
                return;
            }
        }

        ARK_ASSERT_NO_EFFECT(0);
    }

    void UpdateSlotTimer()
    {
        if (tw_vecs_[0]->index == 0)
        {
            int32_t n = 1;
            do
            {
                CarryTimers(tw_vecs_[n]);
            } while (tw_vecs_[n]->index == 1 && ++n < TV_NUM);
        }

        int32_t index = tw_vecs_[0]->index;
        while (!tw_vecs_[0]->vec[index].empty())
        {
            AFTimeWheelData* timer = tw_vecs_[0]->vec[index].front();
            tw_vecs_[0]->vec[index].pop_front();

            if (timer->callback == nullptr)
            {
                AddToFreeTimer(timer);
                continue;
            }

            // timer handler
            (timer->callback)(timer->idx, timer->entity_id);

            // forever timer
            if (timer->count < 0 && timer->interval > 0)
            {
                timer->expires = static_cast<uint64_t>(now_slot_) + timer->interval;
                AddTimerInternal(timer);
                continue;
            }

            timer->count--;
            if (timer->count > 0 && timer->interval > 0)
            {
                timer->expires = static_cast<uint64_t>(now_slot_) + timer->interval;
                AddTimerInternal(timer);
            }
            else
            {
                timers_.erase(timer->idx);
                AddToFreeTimer(timer);
            }
        }

        now_slot_++;
        tw_vecs_[0]->index = (tw_vecs_[0]->index + 1) & TV_MASK;
    }

    void CarryTimers(AFTimeWheelVec* tv)
    {
        int32_t index = tv->index;
        while (!tv->vec[index].empty())
        {
            AFTimeWheelData* timer = tv->vec[index].front();
            AddTimerInternal(timer);
            tv->vec[index].pop_front();
        }

        tv->index = (tv->index + 1) & TV_MASK;
    }

private:
    uint32_t now_slot_{0};
    AFTimeWheelVec* tw_vecs_[TV_NUM];
    uint64_t last_update_time_{0};
    uint64_t timer_idx_{0};
    std::unordered_map<uint64_t, AFTimeWheelData*> timers_;
    std::list<AFTimeWheelData*> free_timers_;
};

} // namespace ark
