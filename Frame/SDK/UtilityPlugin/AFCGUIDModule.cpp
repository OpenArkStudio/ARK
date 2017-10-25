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

#include "AFCGUIDModule.h"

namespace GUIDModule
{
//https://github.com/nebula-im/snowflake4cxx
/**
* Twitter_Snowflake
* SnowFlake的结构如下(每部分用-分开):
* 0 - 0000000000 0000000000 0000000000 0000000000 0 - 00000 - 00000 - 000000000000
* 1位标识，由于long基本类型在Java中是带符号的，最高位是符号位，正数是0，负数是1，所以id一般是正数，最高位是0
* 41位时间截(毫秒级)，注意，41位时间截不是存储当前时间的时间截，而是存储时间截的差值（当前时间截 - 开始时间截)
* 得到的值），这里的的开始时间截，一般是我们的id生成器开始使用的时间，由我们程序来指定的（如下下面程序IdWorker类的startTime属性）。41位的时间截，可以使用69年，年T = (1L << 41) / (1000L * 60 * 60 * 24 * 365) = 69
* 10位的数据机器位，可以部署在1024个节点，包括5位datacenterId和5位workerId
* 12位序列，毫秒内的计数，12位的计数顺序号支持每个节点每毫秒(同一机器，同一时间截)产生4096个ID序号
* 加起来刚好64位。
* SnowFlake的优点是，整体上按照时间自增排序，并且整个分布式系统内不会产生ID碰撞(由数据中心ID和机器ID作区分)，并且效率较高。
*/

#ifndef _MSC_VER
# include <sys/time.h>
#else
# include <windows.h>
# include <winsock2.h>
# include <time.h>
#endif

#define ARK_EPOCH 1288834974657L

#ifdef _MSC_VER
int gettimeofday(struct timeval* tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif

uint64_t GetNowInMsec()
{
    struct timeval tv;
    int  nRet = gettimeofday(&tv, 0);
    return uint64_t(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

uint64_t WaitUntilNextMillis(uint64_t last_timestamp)
{
    uint64_t timestamp = GetNowInMsec();
    while(timestamp <= last_timestamp)
    {
        timestamp = GetNowInMsec();
    }
    return timestamp;
}

class IdWorkerUnThreadSafe
{
public:
    IdWorkerUnThreadSafe(uint16_t worker_id, uint16_t data_center_id)
        : worker_id_(worker_id)
        , data_center_id_(data_center_id)
    {}

    uint64_t GetNextID()
    {
        uint64_t timestamp = GetNowInMsec();

        //in current microsecond
        if(last_timestamp_ == timestamp)
        {
            sequence_ = (sequence_ + 1) & 0xFFF;
            if(sequence_ == 0)
            {
                timestamp = WaitUntilNextMillis(last_timestamp_);
            }
        }
        else
        {
            sequence_ = 0;
        }

        last_timestamp_ = timestamp;
        return (
            (timestamp - ARK_EPOCH) << 22
            | data_center_id_ << 17
            | worker_id_ << 12
            | sequence_);
    }

protected:
    uint16_t worker_id_{ 0 };
    uint16_t data_center_id_{ 0 };
    uint64_t last_timestamp_{ 0 };
    uint32_t sequence_{ 0 };
};

class IdWorkerThreadSafe
{
public:
    IdWorkerThreadSafe(uint16_t worker_id, uint16_t data_center_id)
        : id_worker_(worker_id, data_center_id)
    {}

    uint64_t GetNextID()
    {
        std::lock_guard<std::mutex> g(lock_);
        return id_worker_.GetNextID();
    }

protected:
    IdWorkerUnThreadSafe id_worker_;
    std::mutex lock_;
};
}

//////////////////////////////////////////////////////////////////////////

AFCGUIDModule::AFCGUIDModule(AFIPluginManager* p)
{
    pPluginManager = p;
}

bool AFCGUIDModule::Init()
{
    return true;
}

bool AFCGUIDModule::AfterInit()
{
    return true;
}

bool AFCGUIDModule::Execute()
{
    return true;
}

bool AFCGUIDModule::BeforeShut()
{
    if(NULL != m_pIDWoker)
    {
        delete m_pIDWoker;
        m_pIDWoker = NULL;
    }

    return true;
}

bool AFCGUIDModule::Shut()
{
    return true;
}

void AFCGUIDModule::SetWorkerAndDatacenter(uint16_t worker_id, uint16_t data_center_id)
{
#ifdef AF_THREAD_SAFE
    m_pIDWoker = ARK_NEW GUIDModule::IdWorkerThreadSafe(worker_id, data_center_id);
#else
    m_pIDWoker = ARK_NEW GUIDModule::IdWorkerUnThreadSafe(worker_id, data_center_id);
#endif
}

uint64_t AFCGUIDModule::CreateGUID()
{
    assert(NULL != m_pIDWoker);

    return m_pIDWoker->GetNextID();
}
