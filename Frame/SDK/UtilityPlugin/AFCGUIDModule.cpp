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

#include "AFCGUIDModule.h"
#include "SDK/Core/AFDateTime.hpp"

namespace guid_module
{
//refer:https://github.com/nebula-im/snowflake4cxx

uint64_t WaitUntilNextMillis(uint64_t last_timestamp)
{
    uint64_t timestamp = AFDateTime::GetNowTime();

    while (timestamp <= last_timestamp)
    {
        timestamp = AFDateTime::GetNowTime();
    }

    return timestamp;
}

class IdWorkerThreadUnsafe
{
public:
    IdWorkerThreadUnsafe() = default;

    uint64_t GetNextID()
    {
        uint64_t timestamp = AFDateTime::GetNowTime();

        //in current microsecond
        if (last_timestamp_ == timestamp)
        {
            sequence_ = (sequence_ + 1) & 0xFFFF;

            if (sequence_ == 0)
            {
                timestamp = WaitUntilNextMillis(last_timestamp_);
            }
        }
        else
        {
            sequence_ = 0;
        }

        last_timestamp_ = timestamp;
        return ((timestamp - ARK_EPOCH) * ARK_GUID_POWER + sequence_);
    }

private:
    uint64_t last_timestamp_{ 0 };
    uint32_t sequence_{ 0 };
};

class IdWorkerThreadSafe
{
public:
    IdWorkerThreadSafe() = default;

    uint64_t GetNextID()
    {
        std::lock_guard<std::mutex> g(lock_);
        return id_worker_.GetNextID();
    }

private:
    IdWorkerThreadUnsafe id_worker_;
    mutable std::mutex lock_;
};

}

//////////////////////////////////////////////////////////////////////////

bool AFCGUIDModule::Init()
{
#ifdef AF_THREAD_SAFE
    m_pIDWoker = new guid_module::IdWorkerThreadSafe();
#else
    m_pIDWoker = new guid_module::IdWorkerThreadUnsafe();
#endif // AF_THREAD_SAFE

    return true;
}

bool AFCGUIDModule::PreShut()
{
    if (m_pIDWoker != nullptr)
    {
        delete m_pIDWoker;
        m_pIDWoker = NULL;
    }

    return true;
}

void AFCGUIDModule::SetGUIDMask(uint64_t mask)
{
    mnMask = mask;
}

AFGUID AFCGUIDModule::CreateGUID()
{
    ARK_ASSERT_RET_VAL(m_pIDWoker != nullptr, NULL_GUID);

    uint64_t nLow = m_pIDWoker->GetNextID();
    AFGUID newGUID(mnMask, nLow);

    return newGUID;
}
