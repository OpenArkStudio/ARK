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

//This file is from POCO(https://pocoproject.org/)

#include "AFPlatform.hpp"
#include "AFTimespan.hpp"

class AFTimestamp
{
public:
    using TimeVal = int64_t;
    using UTCTimeVal = int64_t;
    using TimeDiff = int64_t;

    static const TimeVal TIMEVAL_MIN = std::numeric_limits<int64_t>::min();
    static const TimeVal TIMEVAL_MAX = std::numeric_limits<int64_t>::max();

    AFTimestamp()
    {
        update();
    }

    AFTimestamp(TimeVal tv)
    {
        _ts = tv;
    }

    AFTimestamp(const AFTimestamp& other)
    {
        _ts = other._ts;
    }

    ~AFTimestamp()
    {

    }

    AFTimestamp& operator = (const AFTimestamp& other)
    {
        _ts = other._ts;
    }

    AFTimestamp& operator = (TimeVal& tv)
    {
        _ts = tv;
        return *this;
    }

    void swap(AFTimestamp& timestamp)
    {
        std::swap(_ts, timestamp._ts);
    }

    void update()
    {
#if ARK_PLATFORM == PLATFORM_WIN
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);

        ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
        epoch.LowPart = 0xD53E8000;
        epoch.HighPart = 0x019DB1DE;

        ULARGE_INTEGER ts;
        ts.LowPart = ft.dwLowDateTime;
        ts.HighPart = ft.dwHighDateTime;
        ts.QuadPart -= epoch.QuadPart;
        _ts = ts.QuadPart / 10;
#else
        struct timeval tv;
        if (gettimeofday(&tv, NULL))
        {
            throw SystemException("cannot get time of day");
        }
        _ts = TimeVal(tv.tv_sec) * resolution() + tv.tv_usec;
#endif
    }

    bool operator == (const AFTimestamp& ts) const
    {
        return _ts == ts._ts;
    }

    bool operator != (const AFTimestamp& ts) const
    {
        return _ts != ts._ts;
    }

    bool operator >  (const AFTimestamp& ts) const
    {
        return _ts > ts._ts;
    }

    bool operator >= (const AFTimestamp& ts) const
    {
        return _ts >= ts._ts;
    }

    bool operator <  (const AFTimestamp& ts) const
    {
        return _ts < ts._ts;
    }

    bool operator <= (const AFTimestamp& ts) const
    {
        return _ts <= ts._ts;
    }

    AFTimestamp  operator +  (TimeDiff d) const
    {
        return AFTimestamp(_ts + d);
    }

    AFTimestamp  operator -  (TimeDiff d) const
    {
        return AFTimestamp(_ts - d);
    }

    AFTimestamp& operator += (TimeDiff d)
    {
        _ts += d;
        return *this;
    }

    AFTimestamp& operator -= (TimeDiff d)
    {
        _ts -= d;
        return *this;
    }

    AFTimestamp  operator +  (const AFTimespan& span) const
    {
        return *this + span.totalMicroseconds();
    }

    AFTimestamp  operator -  (const AFTimespan& span) const
    {
        return *this - span.totalMicroseconds();
    }

    TimeDiff     operator -  (const AFTimestamp& ts) const
    {
        return _ts - ts._ts;
    }

    AFTimestamp& operator += (const AFTimespan& span)
    {
        return *this += span.totalMicroseconds();
    }

    AFTimestamp& operator -= (const AFTimespan& span)
    {
        return *this -= span.totalMicroseconds();
    }

    std::time_t epochTime() const
    {
        return std::time_t(_ts / resolution());
    }

    UTCTimeVal utcTime() const
    {
        return _ts * 10 + (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
    }

    TimeVal epochMicroseconds() const
    {
        return _ts;
    }

    TimeDiff elapsed() const
    {
        AFTimestamp now;
        return now - *this;
    }

    bool isElapsed(TimeDiff interval) const
    {
        AFTimestamp now;
        TimeDiff diff = now - *this;
        return diff >= interval;
    }

    TimeVal raw() const
    {
        return _ts;
    }

    static AFTimestamp fromEpochTime(std::time_t t)
    {
        return AFTimestamp(TimeVal(t) * resolution());
    }

    static AFTimestamp fromUtcTime(UTCTimeVal val)
    {
        val -= (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
        val /= 10;
        return AFTimestamp(val);
    }

    static TimeDiff resolution()
    {
        return 1000 * 1000; //1s = 1000 * 1000 microseconds
    }

#if ARK_PLATFORM == PLATFORM_WIN
    static AFTimestamp fromFileTimeNP(uint32_t fileTimeLow, uint32_t fileTimeHigh)
    {
        ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
        epoch.LowPart = 0xD53E8000;
        epoch.HighPart = 0x019DB1DE;

        ULARGE_INTEGER ts;
        ts.LowPart = fileTimeLow;
        ts.HighPart = fileTimeHigh;
        ts.QuadPart -= epoch.QuadPart;

        return AFTimestamp(ts.QuadPart / 10);
    }

    void toFileTimeNP(uint32_t& fileTimeLow, uint32_t& fileTimeHigh) const
    {
        ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
        epoch.LowPart = 0xD53E8000;
        epoch.HighPart = 0x019DB1DE;

        ULARGE_INTEGER ts;
        ts.QuadPart = _ts * 10;
        ts.QuadPart += epoch.QuadPart;
        fileTimeLow = ts.LowPart;
        fileTimeHigh = ts.HighPart;
    }
#endif

private:
    //microseconds
    TimeVal _ts;
};

inline void swap(AFTimestamp& s1, AFTimestamp& s2)
{
    s1.swap(s2);
}