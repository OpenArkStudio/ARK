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

    static const TimeDiff MILLISECONDS = 1L;
    static const TimeDiff SECOND_MS = 1000 * MILLISECONDS;
    static const TimeDiff MINUTE_MS = 60 * SECOND_MS;
    static const TimeDiff HOUR_MS = 60 * MINUTE_MS;
    static const TimeDiff DAY_MS = 24 * HOUR_MS;
    static const TimeDiff WEEK_MS = 7 * DAY_MS;
    const int MONTH_DAY[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    AFTimespan()
        : _span(0)
    {}
    
    AFTimespan(TimeDiff milliseconds)
        : _span(milliseconds)
    {}
    
    AFTimespan(int seconds, int milliseconds)
        : _span(TimeDiff(seconds) * SECOND_MS + milliseconds)
    {}

    AFTimespan(int days, int hours, int seconds, int milliseconds)
        : _span(TimeDiff(milliseconds) + TimeDiff(seconds) * SECOND_MS + TimeDiff(hours) * HOUR_MS + TimeDiff(days) * DAY_MS)
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

    AFTimespan& assign(int days, int hours, int minutes, int seconds, int milliseconds = 0)
    {
        _span = TimeDiff(milliseconds) + TimeDiff(seconds) * SECOND_MS + TimeDiff(hours) * HOUR_MS + TimeDiff(days) * DAY_MS;
        return *this;
    }

    AFTimespan& assign(int seconds, int milliseconds = 0)
    {
        _span = TimeDiff(milliseconds) + TimeDiff(seconds) * SECOND_MS;
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

    inline bool operator == (TimeDiff milliseconds) const
    {
        return _span == milliseconds;
    }

    inline bool operator != (TimeDiff milliseconds) const
    {
        return _span != milliseconds;
    }

    inline bool operator > (TimeDiff milliseconds) const
    {
        return _span > milliseconds;
    }

    inline bool operator >= (TimeDiff milliseconds) const
    {
        return _span >= milliseconds;
    }

    inline bool operator <  (TimeDiff milliseconds) const
    {
        return _span < milliseconds;
    }

    inline bool operator <= (TimeDiff milliseconds) const
    {
        return _span <= milliseconds;
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

    inline AFTimespan operator + (TimeDiff milliseconds) const
    {
        return AFTimespan(_span + milliseconds);
    }

    inline AFTimespan operator - (TimeDiff milliseconds) const
    {
        return AFTimespan(_span - milliseconds);
    }

    inline AFTimespan& operator += (TimeDiff milliseconds)
    {
        _span += milliseconds;
        return *this;
    }

    inline AFTimespan& operator -= (TimeDiff milliseconds)
    {
        _span -= milliseconds;
        return *this;
    }

    inline int days() const
    {
        return int(_span / DAY_MS);
    }

    inline int hours() const
    {
        return int((_span / HOUR_MS) % 24);
    }

    inline int totalHours() const
    {
        return int((_span / HOUR_MS));
    }

    inline int minutes() const
    {
        return int((_span / MINUTE_MS) % 60);
    }

    inline int totalMinutes() const
    {
        return int(_span / MINUTE_MS);
    }

    inline int seconds() const
    {
        return int((_span / SECOND_MS) % 60);
    }

    inline int totalSeconds() const
    {
        return int(_span / SECOND_MS);
    }

    inline int milliseconds() const
    {
        return int(_span % 1000);
    }

    inline TimeDiff totalMilliseconds() const
    {
        return _span;
    }

private:
    //milliseconds
    TimeDiff _span;
};