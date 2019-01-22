/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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
#include "AFMacros.hpp"

namespace ark
{

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

        explicit AFTimespan(TimeDiff milliseconds)
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
            if (this != &timespan)
            {
                this->_span = timespan._span;
            }
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

        inline bool operator > (const AFTimespan& ts) const
        {
            return _span > ts._span;
        }

        inline bool operator >= (const AFTimespan& ts) const
        {
            return _span >= ts._span;
        }

        inline bool operator < (const AFTimespan& ts) const
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

        inline bool operator < (TimeDiff milliseconds) const
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

    ////////////////////////////////////////////////////////////////////////

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

        //Monotonic UTC time value in microsecond resolution,
        //with base time midnight, January 1, 1970.
        using TimeVal = int64_t;
        //Difference between two TimeVal values in milliseconds.
        using TimeDiff = int64_t;

        static const TimeVal TIMEVAL_MIN = std::numeric_limits<int64_t>::min();
        static const TimeVal TIMEVAL_MAX = std::numeric_limits<int64_t>::max();

        //Creates a timestamp with the current time.
        AFDateTime()
        {
            update();
        }

        //Creates a timestamp from the given time value
        //(milliseconds since midnight, January 1, 1970).
        explicit AFDateTime(TimeVal tv)
            : _ts(tv)
        {
        }

        AFDateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int tzd = 8)
        {
            struct tm tmp_tm;
            tmp_tm.tm_year = year - 1900;
            tmp_tm.tm_mon = month - 1;
            tmp_tm.tm_mday = day;
            tmp_tm.tm_hour = hour;
            tmp_tm.tm_min = minute;
            tmp_tm.tm_sec = second;
            tmp_tm.tm_isdst = -1;
            time_t time = std::mktime(&tmp_tm);
            _ts = TimeVal(time * Resolution()) + tzd * AFTimespan::HOUR_MS;
        }

        //Copy constructor
        AFDateTime(const AFDateTime& other)
        {
            _ts = other._ts;
        }
        //Destroys the timestamp
        ~AFDateTime()
        {

        }

        AFDateTime& operator = (const AFDateTime& other)
        {
            if (this != &other)
            {
                _ts = other._ts;
            }

            return *this;
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
            _ts = ts.QuadPart / (10 * Resolution());
#else
            struct timeval tv;

            if (gettimeofday(&tv, NULL))
            {
                throw std::invalid_argument("cannot get time of day");
            }

            _ts = TimeVal(tv.tv_sec) * Resolution() + tv.tv_usec / Resolution();
#endif
        }

        bool operator == (const AFDateTime& ts) const
        {
            return _ts == ts._ts;
        }

        bool operator != (const AFDateTime& ts) const
        {
            return _ts != ts._ts;
        }

        bool operator > (const AFDateTime& ts) const
        {
            return _ts > ts._ts;
        }

        bool operator >= (const AFDateTime& ts) const
        {
            return _ts >= ts._ts;
        }

        bool operator < (const AFDateTime& ts) const
        {
            return _ts < ts._ts;
        }

        bool operator <= (const AFDateTime& ts) const
        {
            return _ts <= ts._ts;
        }

        AFDateTime  operator + (TimeDiff d) const
        {
            return AFDateTime(_ts + d);
        }

        AFDateTime  operator - (TimeDiff d) const
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

        AFDateTime operator + (const AFTimespan& span) const
        {
            return *this + span.totalMilliseconds();
        }

        AFDateTime operator - (const AFTimespan& span) const
        {
            return *this - span.totalMilliseconds();
        }

        TimeDiff operator - (const AFDateTime& ts) const
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

        //Returns the timestamp expressed in time_t.
        //ime_t base time is midnight, January 1, 1970.
        //Resolution is one second.
        std::time_t GetTime() const
        {
            return std::time_t(_ts / Resolution());
        }

        //Returns the timestamp expressed in milliseconds
        //since the Unix epoch, midnight, January 1, 1970.
        TimeVal GetMilliseconds()
        {
            return _ts;
        }

        //Returns the time elapsed since the time denoted by
        //the timestamp. Equivalent to Timestamp() - *this.
        TimeDiff GetElapsed() const
        {
            AFDateTime now;
            return now - *this;
        }

        //Returns true diff the given interval has passed
        //since the time denoted by the timestamp.
        bool IsElapsed(TimeDiff interval) const
        {
            AFDateTime now;
            TimeDiff diff = now - *this;
            return diff >= interval;
        }

        //Returns the raw time value.
        //Same as epochMicroseconds().
        TimeVal Raw() const
        {
            return _ts;
        }

        //Returns utc tm data
        struct tm* GetUTCTime() const
        {
            time_t time = GetTime();
            return std::gmtime(&time);
        }

        //Return local tm data
        struct tm* GetLocalTime() const
        {
            time_t time = GetTime();
            return std::localtime(&time);
        }

        //Returns the year.
        int GetYear() const
        {
            struct tm* ptm = GetUTCTime();
            return ptm->tm_year + 1900;
        }

        //Returns the month (1 to 12).
        int GetMonth() const
        {
            struct tm* ptm = GetUTCTime();
            return ptm->tm_mon + 1;
        }

        int GetWeekOfYear(int firstDayOfWeek = MONDAY) const
        {
            ARK_ASSERT_NO_EFFECT(firstDayOfWeek >= SUNDAY && firstDayOfWeek <= SATURDAY);

            //find the first firstDayOfWeek.
            int baseDay = 1;

            while (AFDateTime(GetYear(), 1, baseDay).GetDayOfWeek() != firstDayOfWeek) ++baseDay;

            int day = GetDayOfYear();
            int offs = baseDay <= 4 ? 0 : 1;

            if (day < baseDay)
            {
                return offs;
            }
            else
            {
                return (day - baseDay) / 7 + 1 + offs;
            }

            return 0;
        }

        //Returns the day within the month (1 to 31).
        int GetDay(bool is_monday_first_day = true) const
        {
            struct tm* ptm = GetUTCTime();
            return ptm->tm_mday;
        }

        //Returns the weekday (0 to 6, where
        //0 = Sunday, 1 = Monday, ..., 6 = Saturday).
        int GetDayOfWeek(int firstDayOfWeek = MONDAY) const
        {
            struct tm* ptm = GetUTCTime();

            if (firstDayOfWeek == MONDAY)
            {
                return ((ptm->tm_wday == 0) ? 7 : ptm->tm_wday);
            }
            else
            {
                return ptm->tm_wday;
            }
        }

        //Returns the number of the day in the year.
        //January 1 is 1, February 1 is 32, etc.
        int GetDayOfYear() const
        {
            struct tm* ptm = GetUTCTime();
            return ptm->tm_yday;
        }

        //Returns the hour (0 to 23).
        int GetHour() const
        {
            struct tm* ptm = GetUTCTime();
            return ptm->tm_hour;
        }

        //Returns the hour (0 to 12).
        int GethourAMPM() const
        {
            int hour = GetHour();

            if (hour < 1)
                return 12;
            else if (hour > 12)
                return hour - 12;
            else
                return hour;
        }

        //Returns true if hour < 12;
        bool IsAM() const
        {
            return GetHour() < 12;
        }

        //Returns true if hour >= 12.
        bool IsPM() const
        {
            return GetHour() >= 12;
        }

        /// Returns the minute (0 to 59).
        int GetMinute() const
        {
            struct tm* ptm = GetUTCTime();
            return ptm->tm_min;
        }

        //Returns the second (0 to 59).
        int GetSecond() const
        {
            struct tm* ptm = GetUTCTime();
            return ptm->tm_sec;
        }

        bool IsLeapYear() const
        {
            int year = GetYear();
            return IsLeapYear(year);
        }

        bool SameMinute(TimeVal time)
        {
            AFDateTime xTime(time);
            return (xTime.GetMinute() == GetMinute() &&
                    xTime.GetHour() == GetHour() &&
                    xTime.GetDay() == GetDay() &&
                    xTime.GetMonth() == GetMonth() &&
                    xTime.GetYear() == GetYear());
        }

        bool SameHour(TimeVal time)
        {
            AFDateTime xTime(time);
            return (xTime.GetHour() == GetHour() &&
                    xTime.GetDay() == GetDay() &&
                    xTime.GetMonth() == GetMonth() &&
                    xTime.GetYear() == GetYear());
        }

        bool SameDay(TimeVal time)
        {
            AFDateTime xTime(time);
            return (xTime.GetDay() == GetDay() &&
                    xTime.GetMonth() == GetMonth() &&
                    xTime.GetYear() == GetYear());
        }

        bool SameMonth(TimeVal time)
        {
            AFDateTime xTime(time);
            return (xTime.GetMonth() == GetMonth() &&
                    xTime.GetYear() == GetYear());
        }

        bool SameYear(TimeVal time)
        {
            AFDateTime xTime(time);
            return (xTime.GetYear() == GetYear());
        }

        bool SameWeek(TimeVal time)
        {
            if (!SameYear(time))
            {
                return false;
            }

            if (SameDay(time))
            {
                return true;
            }

            AFDateTime xTime(time);
            return (GetWeekOfYear() == xTime.GetWeekOfYear());
        }

        std::string ToString()
        {
            struct tm* ptm = GetLocalTime(); //local time
            static char timeBuff[30] = { 0 };
            std::strftime(timeBuff, sizeof(timeBuff), "%Y/%m/%d %H:%M:%S", ptm);
            return std::string(timeBuff);
        }

        static bool IsLeapYear(int year)
        {
            return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
        }

        //Returns the number of days in the given month
        //and year. Month is from 1 to 12.
        static int GetDaysOfMonth(int year, int month)
        {
            ARK_ASSERT_NO_EFFECT(month >= 1 && month <= 12);

            if (month < 1 || month > 12)
            {
                return -1;
            }

            static int daysOfMonthTable[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

            if (month == 2 && IsLeapYear(year))
                return 29;
            else
                return daysOfMonthTable[month];
        }

        //Checks if the given date and time is valid
        //all arguments are within a proper range).
        //
        //Returns true if all arguments are valid, false otherwise.
        static bool isValid(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0)
        {
            return
                (year >= 0 && year <= 9999) &&
                (month >= 1 && month <= 12) &&
                (day >= 1 && day <= GetDaysOfMonth(year, month)) &&
                (hour >= 0 && hour <= 23) &&
                (minute >= 0 && minute <= 59) &&
                (second >= 0 && second <= 60) &&
                (millisecond >= 0 && millisecond <= 999);
        }

        //Creates a timestamp static
        //Returns milliseconds
        static TimeVal GetNowTime()
        {
            AFDateTime now;
            return now.Raw();
        }

        //Creates a timestamp static
        //Returns seconds
        static int GetTimestamp()
        {
            AFDateTime now;
            return (int)now.GetTime();
        }

        static TimeDiff Resolution()
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

            return AFDateTime(ts.QuadPart / (10 * Resolution()));
        }

        void toFileTimeNP(uint32_t& fileTimeLow, uint32_t& fileTimeHigh) const
        {
            ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
            epoch.LowPart = 0xD53E8000;
            epoch.HighPart = 0x019DB1DE;

            ULARGE_INTEGER ts;
            ts.QuadPart = _ts * 10 * Resolution();
            ts.QuadPart += epoch.QuadPart;
            fileTimeLow = ts.LowPart;
            fileTimeHigh = ts.HighPart;
        }
#endif

    private:
        //milliseconds
        TimeVal _ts;
    };

    inline void swap(AFDateTime& d1, AFDateTime& d2)
    {
        d1.swap(d2);
    }

}