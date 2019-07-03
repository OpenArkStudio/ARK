/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2019 ArkNX authors.
*
* Licensed under the Apache License, Version 2.0 (the "License").
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

#include "AFMacros.hpp"
#include "AFDateTime.hpp"

namespace ark
{

    class AFBitsAllocator
    {
    public:
        static const int TOTAL_BITS = 1 << 6;

        AFBitsAllocator(int ts_bits, int worker_bits, int seq_bits) :
            sign_bits_(1),
            timestamp_bits_(ts_bits),
            worker_id_bits_(worker_bits),
            sequence_bits_{ seq_bits }
        {
            int alloc_total_bit = sign_bits_ + ts_bits + worker_bits + seq_bits;
            ARK_ASSERT_NO_EFFECT(alloc_total_bit == TOTAL_BITS); //allocate not enough 64 bits

            max_delta_seconds_ = ~(-1L << timestamp_bits_);
            max_worker_id_ = ~(-1L << worker_id_bits_);
            max_sequence_ = ~(-1L << sequence_bits_);

            timestamp_shift_ = worker_id_bits_ + sequence_bits_;
            worker_id_shift_ = sequence_bits_;
        }

        int64_t Alloc(int64_t delta_seconds, int64_t worker_id, int64_t seq)
        {
            return (delta_seconds << timestamp_shift_) | (worker_id << worker_id_shift_) | seq;
        }

        int GetTotalBits()
        {
            return TOTAL_BITS;
        }

        int GetSignBits()
        {
            return sign_bits_;
        }

        int GetTimestampBits()
        {
            return timestamp_bits_;
        }

        int GetWorkerIdBits()
        {
            return worker_id_bits_;
        }

        int GetSequenceBits()
        {
            return sequence_bits_;
        }

        int64_t GetMaxSequence()
        {
            return max_sequence_;
        }

        int64_t GetMaxDeltaSeconds()
        {
            return max_delta_seconds_;
        }

    private:
        int sign_bits_{ 1 };
        int timestamp_bits_{ 0 };
        int worker_id_bits_{ 0 };
        int sequence_bits_{ 0 };

        int64_t max_delta_seconds_{ 0 };
        int64_t max_worker_id_{ 0 };
        int64_t max_sequence_{ 0 };

        int timestamp_shift_{ 0 };
        int worker_id_shift_{ 0 };
    };

    /*
    +------+---------------+----------------+----------+
    | sign | delta seconds | worker node id | sequence |
    +------+---------------+----------------+----------+
    | 1bit |     29bits    |     24bits     |  10bits  |
    */

    //TODO:remove sign and use uint64_t
    //then seq_bits is 12 bits, mean 2048/s
    class AFUidGenerator
    {
    public:
        static const int time_bits = 29;
        static const int worker_bit = 24;
        static const int seq_bits = 10; //generate pow(2, 10) = 1024 per second
        static const int64_t uid_epoch = 1542643200L; //2018/11/20 00:00:00

        AFUidGenerator()
        {
#ifdef ARK_HAVE_LANG_CXX14
            bits_alloc_ = std::make_unique<AFBitsAllocator>(time_bits, worker_bit, seq_bits);
#else
            bits_alloc_.reset(ARK_NEW AFBitsAllocator(time_bits, worker_bit, seq_bits));
#endif

        }

        virtual ~AFUidGenerator() = default;

        virtual int64_t GetUID(const int64_t worker_id)
        {
            return NextID(worker_id);
        }

        std::string ParseUID(const int64_t uid)
        {
            int64_t total_bits = bits_alloc_->GetTotalBits();
            int64_t sign_bits = bits_alloc_->GetSignBits();
            int64_t timestamp_bits = bits_alloc_->GetTimestampBits();
            int64_t worker_id_bits = bits_alloc_->GetWorkerIdBits();
            int64_t sequence_bits = bits_alloc_->GetSequenceBits();

            int64_t seq = (uint64_t(uid << (total_bits - sequence_bits))) >> (total_bits - sequence_bits);
            int64_t worker_id = (uint64_t(uid << (sign_bits + timestamp_bits))) >> (total_bits - worker_id_bits);
            int64_t delta_seconds = (uint64_t(uid)) >> (worker_id_bits + sequence_bits);

            //convert to DateTime(ms)
            AFDateTime date((delta_seconds + uid_epoch) * AFDateTime::Resolution());

            return ARK_FORMAT("UID={} timestamp={} workerid={} sequence={}", uid, date.ToString(), worker_id, seq);
        }

    protected:
        int64_t NextID(const int64_t worker_id)
        {
            int64_t cur_second = GetCurrentSecond();
            ARK_ASSERT_RET_VAL(cur_second >= last_second_, 0);

            if (cur_second == last_second_)
            {
                sequence_ = (sequence_ + 1) & bits_alloc_->GetMaxSequence();
                if (sequence_ == 0)
                {
                    cur_second = GetNextSecond(last_second_);
                }
            }
            else
            {
                sequence_ = 0;
            }

            last_second_ = cur_second;
            return bits_alloc_->Alloc(cur_second - uid_epoch, worker_id, sequence_);
        }


        int64_t GetCurrentSecond()
        {
            int64_t cur_second = AFDateTime::GetTimestamp();
            ARK_ASSERT_RET_VAL((cur_second - uid_epoch) <= bits_alloc_->GetMaxDeltaSeconds(), 0);

            return cur_second;
        }

        int64_t GetNextSecond(int64_t last_ts)
        {
            int64_t ts = GetCurrentSecond();
            while (ts <= last_ts)
            {
                ts = GetCurrentSecond();
            }

            return ts;
        }

    private:
        int64_t sequence_{ 0L };
        int64_t last_second_{ -1L };

        std::unique_ptr<AFBitsAllocator> bits_alloc_{ nullptr };
    };

    class AFUidGeneratorThreadSafe : public AFUidGenerator
    {
    public:
        int64_t GetUID(const int64_t worker_id) override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return NextID(worker_id);
        }

    private:
        mutable std::mutex mutex_;
    };

}