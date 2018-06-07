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

/// A Timestamp stores a monotonic* time value
/// with (theoretical) microseconds resolution.
/// Timestamps can be compared with each other
/// and simple arithmetics are supported.
///
/// [*] Note that Timestamp values are only monotonic as
/// long as the system's clock is monotonic as well
/// (and not, e.g. set back due to time synchronization
/// or other reasons).
///
/// Timestamps are UTC (Coordinated Universal Time)
/// based and thus independent of the timezone
/// in effect on the system.
///
/// The internal reference time is the Unix epoch, 
/// midnight, January 1, 1970.
class AFTimestamp
{
public:
    /// Monotonic UTC time value in microsecond resolution,
    /// with base time midnight, January 1, 1970.
    using TimeVal = int64_t;
    /// Monotonic UTC time value in 100 nanosecond resolution,
    /// with base time midnight, October 15, 1582.
    using UTCTimeVal = int64_t;
    /// Difference between two TimeVal values in microseconds.
    using TimeDiff = int64_t;

    static const TimeVal TIMEVAL_MIN = std::numeric_limits<int64_t>::min();
    static const TimeVal TIMEVAL_MAX = std::numeric_limits<int64_t>::max();

    /// Creates a timestamp with the current time.
    AFTimestamp()
    {
        update();
    }

    /// Creates a timestamp from the given time value
    /// (microseconds since midnight, January 1, 1970).
    AFTimestamp(TimeVal tv)
    {
        _ts = tv;
    }

    /// Copy constructor.
    AFTimestamp(const AFTimestamp& other)
    {
        _ts = other._ts;
    }
    /// Destroys the timestamp
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
    /// Swaps the Timestamp with another one.
    void swap(AFTimestamp& timestamp)
    {
        std::swap(_ts, timestamp._ts);
    }
    /// Updates the Timestamp with the current time.
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

    /// Returns the timestamp expressed in time_t.
    /// time_t base time is midnight, January 1, 1970.
    /// Resolution is one second.
    std::time_t epochTime() const
    {
        return std::time_t(_ts / resolution());
    }

    /// Returns the timestamp expressed in UTC-based
    /// time. UTC base time is midnight, October 15, 1582.
    /// Resolution is 100 nanoseconds.
    UTCTimeVal utcTime() const
    {
        return _ts * 10 + (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
    }

    /// Returns the timestamp expressed in milliseconds
    /// since the Unix epoch, midnight, January 1, 1970.
    TimeVal epochMilliseconds()
    {
        return _ts / 1000;
    }

    /// Returns the timestamp expressed in microseconds
    /// since the Unix epoch, midnight, January 1, 1970.
    TimeVal epochMicroseconds() const
    {
        return _ts;
    }

    /// Returns the time elapsed since the time denoted by
    /// the timestamp. Equivalent to Timestamp() - *this.
    TimeDiff elapsed() const
    {
        AFTimestamp now;
        return now - *this;
    }

    /// Returns true diff the given interval has passed
    /// since the time denoted by the timestamp.
    bool isElapsed(TimeDiff interval) const
    {
        AFTimestamp now;
        TimeDiff diff = now - *this;
        return diff >= interval;
    }

    /// Returns the raw time value.
    /// Same as epochMicroseconds().
    TimeVal raw() const
    {
        return _ts;
    }

    /// Creates a timestamp from a std::time_t.
    static AFTimestamp fromEpochTime(std::time_t t)
    {
        return AFTimestamp(TimeVal(t) * resolution());
    }

    /// Creates a timestamp from a UTC time value
    /// (100 nanosecond intervals since midnight,
    /// October 15, 1582).
    static AFTimestamp fromUtcTime(UTCTimeVal val)
    {
        val -= (TimeDiff(0x01b21dd2) << 32) + 0x13814000;
        val /= 10;
        return AFTimestamp(val);
    }

    /// Returns the resolution in units per second.
    /// Since the timestamp has microsecond resolution,
    /// the returned value is always 1000000.
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