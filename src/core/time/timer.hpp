/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "ark/define.hpp"
#include "ark/platform.hpp"
#include "ark/time/date_time.hpp"

namespace ark {

const uint32_t free_timer_size = 1000; // free timer count, predefined
const uint32_t timer_precision = 10;   // 10 ms per step
const uint32_t tv_bits = 8;
const uint32_t tv_size = 1 << tv_bits; // 256 slots each time_vec
const uint32_t tv_mask = tv_size - 1;
const uint32_t tv_num = 6; // 6 levels

class time_wheel_data
{
public:
    uint64_t idx{0};
    uint32_t count{0};    // =0 means forever
    uint64_t interval{0}; // interval count by step
    uint64_t expires{0};
    timer_functor callback;

    // callback data
    guid_t entity_id{null_guid};
};

class time_wheel_vec
{
public:
    int index{0};
    std::vector<std::list<time_wheel_data*>> vec;
};

class time_wheel_manager
{
public:
    void init(uint64_t now_time) {
        now_slot_ = 0;
        timer_idx_ = 0;
        last_update_time_ = now_time;

        for (uint32_t i = 0; i < free_timer_size; ++i)
        {
            time_wheel_data* data = ARK_NEW time_wheel_data();
            ARK_ASSERT_RET_NONE(data != nullptr);
            free_timers_.push_back(data);
        }

        for (uint32_t i = 0; i < tv_num; ++i)
        {
            time_wheel_vec* tv = ARK_NEW time_wheel_vec();
            init_timer_vec(tv, tv_size);
            tw_vecs_[i] = tv;
        }
    }

    void update(int64_t now_time) {
        update_timer(now_time);
    }

    void shut() {
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

    uint64_t add_timer(
        const guid_t& entity_id, uint32_t count, const std ::chrono::milliseconds interval, timer_functor&& callback) {
        time_wheel_data* timer = get_free_timer();
        timer->idx = ++timer_idx_;
        timer->expires = (interval.count() / timer_precision) + now_slot_;
        timer->count = count;
        timer->interval = (interval.count() / timer_precision);
        timer->callback = callback;
        timer->entity_id = entity_id;

        add_timer_internal(timer);
        timers_.insert(std::make_pair(timer->idx, timer));

        return timer->idx;
    }

    bool remove_timer(uint64_t idx) {
        auto iter = timers_.find(idx);
        if (iter == timers_.end())
        {
            return false;
        }

        time_wheel_data* timer = iter->second;
        timers_.erase(iter);

        timer->callback = nullptr;
        return true;
    }

protected:
    void init_timer_vec(time_wheel_vec* tv, int32_t size) {
        tv->index = 0;
        tv->vec.resize(size);
        for (int i = 0; i < size; ++i)
        {
            tv->vec[i] = std::list<time_wheel_data*>();
        }
    }

    time_wheel_data* get_free_timer() {
        time_wheel_data* timer = nullptr;
        if (free_timers_.empty())
        {
            timer = ARK_NEW time_wheel_data;
            ARK_ASSERT_RET_VAL(timer != nullptr, nullptr);
        }
        else
        {
            timer = free_timers_.front();
            free_timers_.pop_front();
        }

        return timer;
    }

    void add_to_free_timer(time_wheel_data* timer) {
        free_timers_.push_back(timer);
    }

    void update_timer(int64_t now_time) {
        uint64_t passed_slot = (now_time - last_update_time_) / timer_precision;

        if (passed_slot == 0)
        {
            return;
        }

        last_update_time_ += passed_slot * timer_precision;

        for (uint64_t i = 0; i < passed_slot; ++i)
        {
            update_slot_timer();
        }
    }

    void add_timer_internal(time_wheel_data* timer_data) {
        uint64_t expires = timer_data->expires;
        int64_t idx = expires - now_slot_;
        if (idx <= 0)
        {
            time_wheel_vec* tv = tw_vecs_[0];
            tv->vec[tv->index].push_back(timer_data);
            return;
        }

        uint32_t index = 0;
        uint64_t max_tv_num = 0;
        for (uint32_t i = 0; i < tv_num; ++i)
        {
            max_tv_num = ((uint64_t)1 << ((i + 1) * tv_bits));
            if ((uint64_t)idx < max_tv_num)
            {
                index = ((expires >> (i * tv_bits)) & tv_mask);
                tw_vecs_[i]->vec[index].push_back(timer_data);
                return;
            }
        }

        ARK_ASSERT_NO_EFFECT(0);
    }

    void update_slot_timer() {
        if (tw_vecs_[0]->index == 0)
        {
            uint32_t n = 1;
            do
            {
                carry_timers(tw_vecs_[n]);
            } while (tw_vecs_[n]->index == 1 && (++n < tv_num));
        }

        int32_t index = tw_vecs_[0]->index;
        while (!tw_vecs_[0]->vec[index].empty())
        {
            time_wheel_data* timer = tw_vecs_[0]->vec[index].front();
            tw_vecs_[0]->vec[index].pop_front();

            if (timer->callback == nullptr)
            {
                add_to_free_timer(timer);
                continue;
            }

            // timer handler
            (timer->callback)(timer->idx, timer->entity_id);

            // forever timer
            if (timer->count == 0 && timer->interval > 0)
            {
                timer->expires = static_cast<uint64_t>(now_slot_) + timer->interval;
                add_timer_internal(timer);
                continue;
            }

            timer->count--;
            if (timer->count > 0 && timer->interval > 0)
            {
                timer->expires = static_cast<uint64_t>(now_slot_) + timer->interval;
                add_timer_internal(timer);
            }
            else
            {
                timers_.erase(timer->idx);
                add_to_free_timer(timer);
            }
        }

        now_slot_++;
        tw_vecs_[0]->index = (tw_vecs_[0]->index + 1) & tv_mask;
    }

    void carry_timers(time_wheel_vec* tv) {
        int32_t index = tv->index;
        while (!tv->vec[index].empty())
        {
            time_wheel_data* timer = tv->vec[index].front();
            add_timer_internal(timer);
            tv->vec[index].pop_front();
        }

        tv->index = (tv->index + 1) & tv_mask;
    }

private:
    uint32_t now_slot_{0};
    time_wheel_vec* tw_vecs_[tv_num];
    uint64_t last_update_time_{0};
    uint64_t timer_idx_{0};
    std::unordered_map<uint64_t, time_wheel_data*> timers_;
    std::list<time_wheel_data*> free_timers_;
};

} // namespace ark
