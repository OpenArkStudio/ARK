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

//This file is from POCO(https://pocoproject.org/)

#pragma once

#include "AFPlatform.hpp"

class AFTimespan
{
public:
    using TimeDiff = int64_t;

    static const TimeDiff MICROSECONDS = 1;
    static const TimeDiff MILLISECONDS = 1000 * MICROSECONDS;
    static const TimeDiff SECONDS = 1000 * MILLISECONDS;
    static const TimeDiff MINUTES = 60 * SECONDS;
    static const TimeDiff HOURS = 60 * MINUTES;
    static const TimeDiff DAYS = 24 * HOURS;

    AFTimespan()
        : _span(0)
    {}
    
    AFTimespan(TimeDiff microseconds)
        : _span(microseconds)
    {}
    
    AFTimespan(int seconds, int microseconds)
        : _span(TimeDiff(seconds) * SECONDS + microseconds)
    {}

    AFTimespan(int days, int hours, int seconds, int microseconds)
        : _span(TimeDiff(microseconds) + TimeDiff(seconds) * SECONDS + TimeDiff(hours) * HOURS + TimeDiff(days) * DAYS)
    {}

    AFTimespan(const AFTimespan& timespan)
        : _span(timespan._span)
    {}

    ~AFTimespan() {}

    AFTimespan& operator=(const AFTimespan& timespan)
    {
        _span = timespan._span;
        return *this;
    }

    AFTimespan& operator=(TimeDiff microseconds)
    {
        _span = microseconds;
        return *this;
    }

    AFTimespan& assign(int days, int hours, int minutes, int seconds, int microseconds = 0)
    {
        _span = TimeDiff(microseconds) + TimeDiff(seconds) * SECONDS + TimeDiff(hours) * HOURS + TimeDiff(days) * DAYS;
        return *this;
    }

    AFTimespan& assign(int seconds, int microseconds = 0)
    {
        _span = TimeDiff(seconds) * SECONDS + microseconds;
        return *this;
    }

    void swap(AFTimespan& timespan)
    {
        std::swap(_span, timespan._span);
    }

    inline bool operator == (const AFTimespan& ts) const
    {
        return _span == ts._span;
    }

    inline bool operator != (const AFTimespan& ts) const
    {
        return _span != ts._span;
    }

    inline bool operator >  (const AFTimespan& ts) const
    {
        return _span > ts._span;
    }

    inline bool operator >= (const AFTimespan& ts) const
    {
        return _span >= ts._span;
    }

    inline bool operator <  (const AFTimespan& ts) const
    {
        return _span < ts._span;
    }

    inline bool operator <= (const AFTimespan& ts) const
    {
        return _span <= ts._span;
    }

    inline bool operator == (TimeDiff microSeconds) const
    {
        return _span == microSeconds;
    }

    inline bool operator != (TimeDiff microSeconds) const
    {
        return _span != microSeconds;
    }

    inline bool operator > (TimeDiff microSeconds) const
    {
        return _span > microSeconds;
    }

    inline bool operator >= (TimeDiff microSeconds) const
    {
        return _span >= microSeconds;
    }

    inline bool operator <  (TimeDiff microSeconds) const
    {
        return _span < microSeconds;
    }

    inline bool operator <= (TimeDiff microSeconds) const
    {
        return _span <= microSeconds;
    }

    inline AFTimespan operator + (const AFTimespan& d) const
    {
        return AFTimespan(_span + d._span);
    }

    inline AFTimespan operator - (const AFTimespan& d) const
    {
        return AFTimespan(_span - d._span);
    }

    inline AFTimespan& operator += (const AFTimespan& d)
    {
        _span += d._span;
        return *this;
    }

    inline AFTimespan& operator -= (const AFTimespan& d)
    {
        _span -= d._span;
        return *this;
    }

    inline AFTimespan operator + (TimeDiff microSeconds) const
    {
        return AFTimespan(_span + microSeconds);
    }

    inline AFTimespan operator - (TimeDiff microSeconds) const
    {
        return AFTimespan(_span - microSeconds);
    }

    inline AFTimespan& operator += (TimeDiff microSeconds)
    {
        _span += microSeconds;
        return *this;
    }

    inline AFTimespan& operator -= (TimeDiff microSeconds)
    {
        _span -= microSeconds;
        return *this;
    }

    inline int days() const
    {
        return int(_span / DAYS);
    }

    inline int hours() const
    {
        return int((_span / HOURS) % 24);
    }

    inline int totalHours() const
    {
        return int((_span / HOURS));
    }

    inline int minutes() const
    {
        return int((_span / MINUTES) % 24);
    }

    inline int totalMinutes() const
    {
        return int(_span / MINUTES);
    }

    inline int seconds() const
    {
        return int((_span / SECONDS) % 60);
    }

    inline int totalSeconds() const
    {
        return int(_span / SECONDS);
    }

    inline int milliseconds() const
    {
        return int((_span / MILLISECONDS) % 1000);
    }

    inline TimeDiff totalMilliseconds() const
    {
        return _span / MILLISECONDS;
    }

    inline int microseconds() const
    {
        return int(_span % 1000);
    }

    inline int useconds() const
    {
        return int(_span % (1000 * 1000));
    }

    inline TimeDiff totalMicroseconds() const
    {
        return _span;
    }

private:
    //microseconds
    TimeDiff _span;
};