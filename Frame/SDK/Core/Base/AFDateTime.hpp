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

#include "AFPlatform.hpp"
#include "AFTimespan.hpp"
#include "AFMacros.hpp"

class AFDateTime
{
public:
    enum Months
    {
        JANUARY = 1,
        FEBRUARY,
        MARCH,
        APRIL,
        MAY,
        JUNE,
        JULY,
        AUGUST,
        SEPTEMBER,
        OCTOBER,
        NOVEMBER,
        DECEMBER
    };

    /// Symbolic names for week day numbers (0 to 6).
    enum DaysOfWeek
    {
        SUNDAY = 0,
        MONDAY,
        TUESDAY,
        WEDNESDAY,
        THURSDAY,
        FRIDAY,
        SATURDAY
    };

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
    AFDateTime()
    {
        update();
        computeDaytime();
    }

    /// Creates a timestamp from the given time value
    /// (microseconds since midnight, January 1, 1970).
    AFDateTime(TimeVal tv)
    {
        _ts = tv;
        computeDaytime();
    }

    /// Copy constructor.
    AFDateTime(const AFDateTime& other)
    {
        _ts = other._ts;
    }
    /// Destroys the timestamp
    ~AFDateTime()
    {

    }

    AFDateTime& operator = (const AFDateTime& other)
    {
        _ts = other._ts;
    }

    AFDateTime& operator = (TimeVal& tv)
    {
        _ts = tv;
        return *this;
    }
    /// Swaps the Timestamp with another one.
    void swap(AFDateTime& timestamp)
    {
        std::swap(_ts, timestamp._ts);
        computeDaytime();
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
        _ts = ts.QuadPart / (10 * 1000);
#else
        struct timeval tv;
        if (gettimeofday(&tv, NULL))
        {
            throw SystemException("cannot get time of day");
        }
        _ts = TimeVal(tv.tv_sec) * resolution() + tv.tv_usec / 1000;
#endif

        computeDaytime();
    }

    bool operator == (const AFDateTime& ts) const
    {
        return _ts == ts._ts;
    }

    bool operator != (const AFDateTime& ts) const
    {
        return _ts != ts._ts;
    }

    bool operator >  (const AFDateTime& ts) const
    {
        return _ts > ts._ts;
    }

    bool operator >= (const AFDateTime& ts) const
    {
        return _ts >= ts._ts;
    }

    bool operator <  (const AFDateTime& ts) const
    {
        return _ts < ts._ts;
    }

    bool operator <= (const AFDateTime& ts) const
    {
        return _ts <= ts._ts;
    }

    AFDateTime  operator +  (TimeDiff d) const
    {
        return AFDateTime(_ts + d);
    }

    AFDateTime  operator -  (TimeDiff d) const
    {
        return AFDateTime(_ts - d);
    }

    AFDateTime& operator += (TimeDiff d)
    {
        _ts += d;
        return *this;
    }

    AFDateTime& operator -= (TimeDiff d)
    {
        _ts -= d;
        return *this;
    }

    AFDateTime  operator +  (const AFTimespan& span) const
    {
        return *this + span.totalMilliseconds();
    }

    AFDateTime  operator -  (const AFTimespan& span) const
    {
        return *this - span.totalMilliseconds();
    }

    TimeDiff     operator -  (const AFDateTime& ts) const
    {
        return _ts - ts._ts;
    }

    AFDateTime& operator += (const AFTimespan& span)
    {
        return *this += span.totalMilliseconds();
    }

    AFDateTime& operator -= (const AFTimespan& span)
    {
        return *this -= span.totalMilliseconds();
    }

    /// Returns the timestamp expressed in time_t.
    /// time_t base time is midnight, January 1, 1970.
    /// Resolution is one second.
    std::time_t epochTime() const
    {
        return std::time_t(_ts / resolution());
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
        AFDateTime now;
        return now - *this;
    }

    /// Returns true diff the given interval has passed
    /// since the time denoted by the timestamp.
    bool isElapsed(TimeDiff interval) const
    {
        AFDateTime now;
        TimeDiff diff = now - *this;
        return diff >= interval;
    }

    /// Returns the raw time value.
    /// Same as epochMicroseconds().
    TimeVal raw() const
    {
        return _ts;
    }

    /// return utc tm data
    struct tm* utcTime() const
    {
        time_t time = epochTime();
        return std::gmtime(&time);
    }

    struct tm* localTime() const
    {
        time_t time = epochTime();
        return std::localtime(&time);
    }

    /// Returns the year.
    int year() const
    {
        return _year;
    }

    /// Returns the month (1 to 12).
    int month() const
    {
        return _month;
    }

    /// Returns the week number within the year.
    /// FirstDayOfWeek should be either SUNDAY (0) or MONDAY (1).
    /// The returned week number will be from 0 to 53. Week number 1 is the week 
    /// containing January 4. This is in accordance to ISO 8601.
    /// 
    /// The following example assumes that firstDayOfWeek is MONDAY. For 2005, which started
    /// on a Saturday, week 1 will be the week starting on Monday, January 3.
    /// January 1 and 2 will fall within week 0 (or the last week of the previous year).
    ///
    /// For 2007, which starts on a Monday, week 1 will be the week starting on Monday, January 1.
    /// There will be no week 0 in 2007.
    int week(int firstDayOfWeek = MONDAY) const
    {
        //ARK_ASSERT_NO_EFFECT(firstDayOfWeek >= SUNDAY && firstDayOfWeek <= SATURDAY);

        ///// find the first firstDayOfWeek.
        //int baseDay = 1;
        //while (AFDateTime(_year, 1, baseDay).dayOfWeek() != firstDayOfWeek) ++baseDay;

        //int doy = dayOfYear();
        //int offs = baseDay <= 4 ? 0 : 1;
        //if (doy < baseDay)
        //{
        //    return offs;
        //}
        //else
        //{
        //    return (doy - baseDay) / 7 + 1 + offs;
        //}

        return 0;
    }

    /// Returns the day within the month (1 to 31).
    int day() const
    {
        return _day;
    }

    /// Returns the weekday (0 to 6, where
    /// 0 = Sunday, 1 = Monday, ..., 6 = Saturday).
    int dayOfWeek(int firstDayOfWeek = MONDAY) const
    {
        if (firstDayOfWeek == MONDAY)
        {
            return ((_wday == 0) ? 7 : _wday);
        }
        else
        {
            return _wday;
        }
    }

    /// Returns the number of the day in the year.
    /// January 1 is 1, February 1 is 32, etc.
    int dayOfYear() const
    {
        int day = 0;
        for (int month = 1; month < _month; ++month)
        {
            day += daysOfMonth(_year, month);
        }
        day += _day;
        return day;
    }

    /// Returns the hour (0 to 23).
    int hour() const
    {
        return _hour;
    }

    /// Returns the hour (0 to 12).
    int hourAMPM() const
    {
        if (_hour < 1)
            return 12;
        else if (_hour > 12)
            return _hour - 12;
        else
            return _hour;
    }

    /// Returns true if hour < 12;
    bool isAM() const
    {
        return _hour < 12;
    }

    /// Returns true if hour >= 12.
    bool isPM() const
    {
        return _hour >= 12;
    }

    /// Returns the minute (0 to 59).
    int minute() const
    {
        return _minute;
    }

    /// Returns the second (0 to 59).
    int second() const
    {
        return _second;
    }

    static bool isLeapYear(int year)
    {
        return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
    }

    /// Returns the number of days in the given month
    /// and year. Month is from 1 to 12.
    static int daysOfMonth(int year, int month)
    {
        ARK_ASSERT_NO_EFFECT(month >= 1 && month <= 12);

        static int daysOfMonthTable[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        if (month == 2 && isLeapYear(year))
            return 29;
        else
            return daysOfMonthTable[month];
    }

    /// Checks if the given date and time is valid
    /// (all arguments are within a proper range).
    ///
    /// Returns true if all arguments are valid, false otherwise.
    static bool isValid(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0)
    {
        return
            (year >= 0 && year <= 9999) &&
            (month >= 1 && month <= 12) &&
            (day >= 1 && day <= daysOfMonth(year, month)) &&
            (hour >= 0 && hour <= 23) &&
            (minute >= 0 && minute <= 59) &&
            (second >= 0 && second <= 60) &&
            (millisecond >= 0 && millisecond <= 999) &&
            (microsecond >= 0 && microsecond <= 999);
    }

    /// Creates a timestamp static
    /// Returns milliseconds
    static TimeVal getTimestamp()
    {
        AFDateTime now;
        return now.epochMilliseconds();
    }

    /// Creates a timestamp from a std::time_t.
    static AFDateTime fromEpochTime(std::time_t t)
    {
        return AFDateTime(TimeVal(t) * resolution());
    }

    /// Returns the resolution in units per second.
    /// Since the timestamp has microsecond resolution,
    /// the returned value is always 1000000.
    static TimeDiff resolution()
    {
        return 1000; //1s = 1000 milliseconds
    }

#if ARK_PLATFORM == PLATFORM_WIN
    static AFDateTime fromFileTimeNP(uint32_t fileTimeLow, uint32_t fileTimeHigh)
    {
        ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
        epoch.LowPart = 0xD53E8000;
        epoch.HighPart = 0x019DB1DE;

        ULARGE_INTEGER ts;
        ts.LowPart = fileTimeLow;
        ts.HighPart = fileTimeHigh;
        ts.QuadPart -= epoch.QuadPart;

        return AFDateTime(ts.QuadPart / 10);
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

protected:
    void computeDaytime()
    {
        struct tm* ptm = utcTime();
        _year = ptm->tm_year + 1970;
        _month = ptm->tm_mon + 1;
        _day = ptm->tm_mday;
        _hour = ptm->tm_hour;
        _minute = ptm->tm_min;
        _second = ptm->tm_sec;
        _wday = ptm->tm_wday;
    }

private:
    //microseconds
    TimeVal _ts;

    short  _year{ 0 };
    short  _month{ 0 };
    short  _day{ 0 };
    short  _hour{ 0 };
    short  _minute{ 0 };
    short  _second{ 0 };
    short  _wday{ 0 }; //day of week
};

inline void swap(AFDateTime& d1, AFDateTime& d2)
{
    d1.swap(d2);
}