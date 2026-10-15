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

#include "ark/dep.hpp"
#include "ark/macros.hpp"
#include "ark/time/date_time.hpp"

namespace ark {

#if defined(ARK_USE_GUID_32)

class uid_generator
{
    guid_t last_sequence_{1000};

public:
    virtual ~uid_generator() = default;

    virtual guid_t get_uid() {
        return next_id();
    }

    std::string parse_uid(const guid_t uid) {
        return ARK_FORMAT("ID={}", uid);
    }

protected:
    guid_t next_id() {
        if (last_sequence_ >= pow(2, 22))
        {
            last_sequence_ = 1000;
        }

        return last_sequence_++;
    }
};

class uid_generator_thread_safe : public uid_generator
{
public:
    guid_t get_uid() override {
        std::lock_guard<std::mutex> lock(mutex_);
        return next_id();
    }

private:
    mutable std::mutex mutex_;
};

#elif defined(ARK_USE_GUID_128)

// TODO: 128 bits guid

#else

class bits_allocator
{
public:
    static const int total_bits = 1 << 6;

    bits_allocator(int ts_bits, int worker_bits, int seq_bits)
        : sign_bits_(1)
        , timestamp_bits_(ts_bits)
        , worker_id_bits_(worker_bits)
        , sequence_bits_{seq_bits} {
        int alloc_total_bit = sign_bits_ + ts_bits + worker_bits + seq_bits;
        ARK_ASSERT_NO_EFFECT(alloc_total_bit == total_bits); // allocate not enough 64 bits

        max_delta_seconds_ = ~(uint64_t(-1L) << timestamp_bits_);
        max_worker_id_ = ~(uint64_t(-1L) << worker_id_bits_);
        max_sequence_ = ~(uint64_t(-1L) << sequence_bits_);

        timestamp_shift_ = worker_id_bits_ + sequence_bits_;
        worker_id_shift_ = sequence_bits_;
    }

    int64_t alloc(int64_t delta_seconds, int64_t worker_id, int64_t seq) {
        return (delta_seconds << timestamp_shift_) | (worker_id << worker_id_shift_) | seq;
    }

    int get_total_bits() {
        return total_bits;
    }

    int get_sign_bits() {
        return sign_bits_;
    }

    int get_timestamp_bits() {
        return timestamp_bits_;
    }

    int get_worker_id_bits() {
        return worker_id_bits_;
    }

    int get_sequence_bits() {
        return sequence_bits_;
    }

    int64_t get_max_sequence() {
        return max_sequence_;
    }

    int64_t get_max_delta_seconds() {
        return max_delta_seconds_;
    }

private:
    int sign_bits_{1};
    int timestamp_bits_{0};
    int worker_id_bits_{0};
    int sequence_bits_{0};

    int64_t max_delta_seconds_{0};
    int64_t max_worker_id_{0};
    int64_t max_sequence_{0};

    int timestamp_shift_{0};
    int worker_id_shift_{0};
};

/*
+------+---------------+----------------+----------+
| sign | delta seconds | worker node id | sequence |
+------+---------------+----------------+----------+
| 1bit |     29bits    |     24bits     |  10bits  |
*/

ARK_CONSTEXPR static const int time_bits = 29;
ARK_CONSTEXPR static const int worker_bit = 24;
ARK_CONSTEXPR static const int seq_bits = 10;               // generate pow(2, 10) = 1024 per second
ARK_CONSTEXPR static const int64_t uid_epoch = 1542643200L; // 2018/11/20 00:00:00

// TODO:remove sign and use uint64_t
// then seq_bits is 12 bits, mean 2048/s
class uid_generator
{
public:
    uid_generator() {

        bits_alloc_ = std::make_unique<bits_allocator>(time_bits, worker_bit, seq_bits);
    }

    virtual ~uid_generator() = default;

    virtual int64_t get_uid(const int64_t worker_id) {
        return next_id(worker_id);
    }

    std::string parse_uid(const guid_t uid) {
        int64_t total_bits = bits_alloc_->get_total_bits();
        int64_t sign_bits = bits_alloc_->get_sign_bits();
        int64_t timestamp_bits = bits_alloc_->get_timestamp_bits();
        int64_t worker_id_bits = bits_alloc_->get_worker_id_bits();
        int64_t sequence_bits = bits_alloc_->get_sequence_bits();

        int64_t seq = (uint64_t(uid << (total_bits - sequence_bits))) >> (total_bits - sequence_bits);
        int64_t worker_id = (uint64_t(uid << (sign_bits + timestamp_bits))) >> (total_bits - worker_id_bits);
        int64_t delta_seconds = (uint64_t(uid)) >> (worker_id_bits + sequence_bits);

        // convert to DateTime(ms)
        date_time date((delta_seconds + uid_epoch) * date_time::resolution());

        return ARK_FORMAT("UID={} timestamp={} workerid={} sequence={}", uid, date.to_string(), worker_id, seq);
    }

protected:
    int64_t next_id(const int64_t worker_id) {
        int64_t cur_second = get_current_second();
        ARK_ASSERT_RET_VAL(cur_second >= last_second_, 0);

        if (cur_second == last_second_)
        {
            sequence_ = (sequence_ + 1) & bits_alloc_->get_max_sequence();
            if (sequence_ == 0)
            {
                cur_second = get_next_second(last_second_);
            }
        }
        else
        {
            sequence_ = 0;
        }

        last_second_ = cur_second;
        return bits_alloc_->alloc(cur_second - uid_epoch, worker_id, sequence_);
    }

    int64_t get_current_second() {
        int64_t cur_second = date_time::get_timestamp();
        ARK_ASSERT_RET_VAL((cur_second - uid_epoch) <= bits_alloc_->get_max_delta_seconds(), 0);

        return cur_second;
    }

    int64_t get_next_second(int64_t last_ts) {
        int64_t ts = get_current_second();
        while (ts <= last_ts)
        {
            ts = get_current_second();
        }

        return ts;
    }

private:
    int64_t sequence_{0L};
    int64_t last_second_{-1L};

    std::unique_ptr<bits_allocator> bits_alloc_{nullptr};
};

class uid_generator_thread_safe : public uid_generator
{
public:
    int64_t get_uid(const int64_t worker_id) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return next_id(worker_id);
    }

private:
    mutable std::mutex mutex_;
};

#endif

} // namespace ark
