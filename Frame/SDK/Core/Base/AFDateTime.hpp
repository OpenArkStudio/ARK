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
#include "AFTimestamp.hpp"
#include "AFMacros.hpp"

//This file is from POCO(https://pocoproject.org/)

class AFDateTime
{
public:
    /// Symbolic names for month numbers (1 to 12).
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

    /// Creates a DateTime for the current date and time.
    AFDateTime()
    {
        AFTimestamp now;
        _utcTime = now.utcTime();
        computeGregorian(julianDay());
        computeDaytime();
    }
    
    /// Creates a DateTime for the date and time given in
    /// a Timestamp.
    AFDateTime(const AFTimestamp& timestamp)
        : _utcTime(timestamp.utcTime())
    {
        computeGregorian(julianDay());
        computeDaytime();
    }

    /// Creates a DateTime for the given Gregorian date and time.
    ///   * year is from 0 to 9999.
    ///   * month is from 1 to 12.
    ///   * day is from 1 to 31.
    ///   * hour is from 0 to 23.
    ///   * minute is from 0 to 59.
    ///   * second is from 0 to 60 (allowing leap seconds).
    ///   * millisecond is from 0 to 999.
    ///   * microsecond is from 0 to 999.
    AFDateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0) :
        _year(year),
        _month(month),
        _day(day),
        _hour(hour),
        _minute(minute),
        _second(second),
        _millisecond(millisecond),
        _microsecond(microsecond)
    {
        ARK_ASSERT_NO_EFFECT(year >= 0 && year <= 9999);
        ARK_ASSERT_NO_EFFECT(month >= 1 && month <= 12);
        ARK_ASSERT_NO_EFFECT(day >= 1 && day <= daysOfMonth(year, month));
        ARK_ASSERT_NO_EFFECT(hour >= 0 && hour <= 23);
        ARK_ASSERT_NO_EFFECT(minute >= 0 && minute <= 59);
        ARK_ASSERT_NO_EFFECT(second >= 0 && second <= 60); // allow leap seconds
        ARK_ASSERT_NO_EFFECT(millisecond >= 0 && millisecond <= 999);
        ARK_ASSERT_NO_EFFECT(microsecond >= 0 && microsecond <= 999);

        _utcTime = toUtcTime(toJulianDay(year, month, day)) + 10 * (hour*AFTimespan::HOURS + minute * AFTimespan::MINUTES + second * AFTimespan::SECONDS + millisecond * AFTimespan::MILLISECONDS + microsecond);
    }
    
    /// Creates a DateTime for the given Julian day.
    AFDateTime(double julianDay) :
        _utcTime(toUtcTime(julianDay))
    {
        computeGregorian(julianDay);
    }

    /// Creates a DateTime from an UtcTimeVal and a TimeDiff.
    ///
    /// Mainly used internally by DateTime and friends.
    AFDateTime(AFTimestamp::UTCTimeVal utcTime, AFTimestamp::TimeDiff diff) :
        _utcTime(utcTime + diff * 10)
    {
        computeGregorian(julianDay());
        computeDaytime();
    }

    /// Copy constructor. Creates the DateTime from another one.
    AFDateTime(const AFDateTime& dateTime) :
        _utcTime(dateTime._utcTime),
        _year(dateTime._year),
        _month(dateTime._month),
        _day(dateTime._day),
        _hour(dateTime._hour),
        _minute(dateTime._minute),
        _second(dateTime._second),
        _millisecond(dateTime._millisecond),
        _microsecond(dateTime._microsecond)
    {}
    
    /// Destroys the DateTime.
    ~AFDateTime() {}
    
    /// Assigns another DateTime.
    AFDateTime& operator = (const AFDateTime& dateTime)
    {
        if (&dateTime != this)
        {
            _utcTime = dateTime._utcTime;
            _year = dateTime._year;
            _month = dateTime._month;
            _day = dateTime._day;
            _hour = dateTime._hour;
            _minute = dateTime._minute;
            _second = dateTime._second;
            _millisecond = dateTime._millisecond;
            _microsecond = dateTime._microsecond;
        }

        return *this;
    }
    
    /// Assigns a Timestamp.
    AFDateTime& operator = (const AFTimestamp& timestamp)
    {
        _utcTime = timestamp.utcTime();
        computeGregorian(julianDay());
        computeDaytime();
        return *this;
    }
    
    /// Assigns a Julian day.
    AFDateTime& operator = (double julianDay)
    {
        _utcTime = toUtcTime(julianDay);
        computeGregorian(julianDay);
        return *this;
    }
    
    /// Assigns a Gregorian date and time.
    ///   * year is from 0 to 9999.
    ///   * month is from 1 to 12.
    ///   * day is from 1 to 31.
    ///   * hour is from 0 to 23.
    ///   * minute is from 0 to 59.
    ///   * second is from 0 to 60 (allowing leap seconds).
    ///   * millisecond is from 0 to 999.
    ///   * microsecond is from 0 to 999.
    AFDateTime& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0)
    {
        ARK_ASSERT_NO_EFFECT(year >= 0 && year <= 9999);
        ARK_ASSERT_NO_EFFECT(month >= 1 && month <= 12);
        ARK_ASSERT_NO_EFFECT(day >= 1 && day <= daysOfMonth(year, month));
        ARK_ASSERT_NO_EFFECT(hour >= 0 && hour <= 23);
        ARK_ASSERT_NO_EFFECT(minute >= 0 && minute <= 59);
        ARK_ASSERT_NO_EFFECT(second >= 0 && second <= 60); // allow leap seconds
        ARK_ASSERT_NO_EFFECT(millisecond >= 0 && millisecond <= 999);
        ARK_ASSERT_NO_EFFECT(microsecond >= 0 && microsecond <= 999);

        _utcTime = toUtcTime(toJulianDay(year, month, day)) + 10 * (hour * AFTimespan::HOURS + minute * AFTimespan::MINUTES + second * AFTimespan::SECONDS + millisecond * AFTimespan::MILLISECONDS + microsecond);
        _year = year;
        _month = month;
        _day = day;
        _hour = hour;
        _minute = minute;
        _second = second;
        _millisecond = millisecond;
        _microsecond = microsecond;

        return *this;
    }

    /// Swaps the DateTime with another one.
    void swap(AFDateTime& dateTime)
    {
        std::swap(_utcTime, dateTime._utcTime);
        std::swap(_year, dateTime._year);
        std::swap(_month, dateTime._month);
        std::swap(_day, dateTime._day);
        std::swap(_hour, dateTime._hour);
        std::swap(_minute, dateTime._minute);
        std::swap(_second, dateTime._second);
        std::swap(_millisecond, dateTime._millisecond);
        std::swap(_microsecond, dateTime._microsecond);
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
        ARK_ASSERT_NO_EFFECT(firstDayOfWeek >= 0 && firstDayOfWeek <= 6);

        /// find the first firstDayOfWeek.
        int baseDay = 1;
        while (AFDateTime(_year, 1, baseDay).dayOfWeek() != firstDayOfWeek) ++baseDay;

        int doy = dayOfYear();
        int offs = baseDay <= 4 ? 0 : 1;
        if (doy < baseDay)
        {
            return offs;
        }
        else
        {
            return (doy - baseDay) / 7 + 1 + offs;
        }
    }

    /// Returns the day within the month (1 to 31).
    int day() const
    {
        return _day;
    }
    
    /// Returns the weekday (0 to 6, where
    /// 0 = Sunday, 1 = Monday, ..., 6 = Saturday).
    int dayOfWeek() const
    {
        return int((std::floor(julianDay() + 1.5))) % 7;
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
    
    /// Returns the millisecond (0 to 999)
    int millisecond() const
    {
        return _millisecond;
    }
    
    /// Returns the microsecond (0 to 999)
    int microsecond() const
    {
        return _microsecond;
    }
    
    /// Returns the julian day for the date and time.
    double julianDay() const
    {
        return toJulianDay(_utcTime);
    }
    
    /// Returns the date and time expressed as a Timestamp.
    AFTimestamp timestamp() const
    {
        return AFTimestamp::fromUtcTime(_utcTime);
    }
    
    /// Returns the date and time expressed in UTC-based
    /// time. UTC base time is midnight, October 15, 1582.
    /// Resolution is 100 nanoseconds.
    AFTimestamp::UTCTimeVal utcTime() const
    {
        return _utcTime;
    }

    bool operator == (const AFDateTime& dateTime) const
    {
        return _utcTime == dateTime._utcTime;
    }

    bool operator != (const AFDateTime& dateTime) const
    {
        return _utcTime != dateTime._utcTime;
    }

    bool operator <  (const AFDateTime& dateTime) const
    {
        return _utcTime < dateTime._utcTime;
    }

    bool operator <= (const AFDateTime& dateTime) const
    {
        return _utcTime <= dateTime._utcTime;
    }

    bool operator >  (const AFDateTime& dateTime) const
    {
        return _utcTime > dateTime._utcTime;
    }

    bool operator >= (const AFDateTime& dateTime) const
    {
        return _utcTime >= dateTime._utcTime;
    }

    AFDateTime operator + (const AFTimespan& span) const
    {
        return AFDateTime(_utcTime, span.totalMicroseconds());
    }

    AFDateTime operator - (const AFTimespan& span) const
    {
        return AFDateTime(_utcTime, -span.totalMicroseconds());
    }

    AFTimespan operator - (const AFDateTime& dateTime) const
    {
        return AFTimespan((_utcTime - dateTime._utcTime) / 10);
    }

    AFDateTime& operator += (const AFTimespan& span)
    {
        _utcTime += span.totalMicroseconds() * 10;
        computeGregorian(julianDay());
        computeDaytime();
        return *this;
    }

    AFDateTime& operator -= (const AFTimespan& span)
    {
        _utcTime -= span.totalMicroseconds() * 10;
        computeGregorian(julianDay());
        computeDaytime();
        return *this;
    }

    /// Converts a local time into UTC, by applying the given time zone differential.
    void makeUTC(int tzd)
    {
        operator -= (AFTimespan(((AFTimestamp::TimeDiff) tzd)*AFTimespan::SECONDS));
    }
    
    /// Converts a UTC time into a local time, by applying the given time zone differential.
    void makeLocal(int tzd)
    {
        operator += (AFTimespan(((AFTimespan::TimeDiff) tzd)*AFTimespan::SECONDS));
    }
    
    /// Returns true if the given year is a leap year;
    /// false otherwise.
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

protected:
    /// Computes the Julian day for an UTC time.
    static double toJulianDay(AFTimestamp::UTCTimeVal utcTime)
    {
        double utcDays = double(utcTime) / 864000000000.0;
        return utcDays + 2299160.5; // first day of Gregorian reform (Oct 15 1582)
    }
    
    /// Computes the Julian day for a gregorian calendar date and time.
    /// See <http://vsg.cape.com/~pbaum/date/jdimp.htm>, section 2.3.1 for the algorithm.
    static double toJulianDay(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0)
    {
        // lookup table for (153*month - 457)/5 - note that 3 <= month <= 14.
        static int lookup[] = { -91, -60, -30, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337 };

        // day to double
        double dday = double(day) + ((double((hour * 60 + minute) * 60 + second) * 1000 + millisecond) * 1000 + microsecond) / 86400000000.0;
        if (month < 3)
        {
            month += 12;
            --year;
        }
        double dyear = double(year);
        return dday + lookup[month] + 365 * year + std::floor(dyear / 4) - std::floor(dyear / 100) + std::floor(dyear / 400) + 1721118.5;
    }

    /// Computes the UTC time for a Julian day.
    static AFTimestamp::UTCTimeVal toUtcTime(double julianDay)
    {
        return AFTimestamp::UTCTimeVal((julianDay - 2299160.5)*864000000000.0);
    }

    /// Computes the Gregorian date for the given Julian day.
    /// See <http://vsg.cape.com/~pbaum/date/injdimp.htm>, section 3.3.1 for the algorithm.
    void computeGregorian(double julianDay)
    {
        double z = std::floor(julianDay - 1721118.5);
        double r = julianDay - 1721118.5 - z;
        double g = z - 0.25;
        double a = std::floor(g / 36524.25);
        double b = a - std::floor(a / 4);
        _year = short(std::floor((b + g) / 365.25));
        double c = b + z - std::floor(365.25*_year);
        _month = short(std::floor((5 * c + 456) / 153));
        double dday = c - std::floor((153.0*_month - 457) / 5) + r;
        _day = short(dday);
        if (_month > 12)
        {
            ++_year;
            _month -= 12;
        }
        r *= 24;
        _hour = short(std::floor(r));
        r -= std::floor(r);
        r *= 60;
        _minute = short(std::floor(r));
        r -= std::floor(r);
        r *= 60;
        _second = short(std::floor(r));
        r -= std::floor(r);
        r *= 1000;
        _millisecond = short(std::floor(r));
        r -= std::floor(r);
        r *= 1000;
        _microsecond = short(r + 0.5);

        normalize();

        ARK_ASSERT_NO_EFFECT(_month >= 1 && _month <= 12);
        ARK_ASSERT_NO_EFFECT(_day >= 1 && _day <= daysOfMonth(_year, _month));
        ARK_ASSERT_NO_EFFECT(_hour >= 0 && _hour <= 23);
        ARK_ASSERT_NO_EFFECT(_minute >= 0 && _minute <= 59);
        ARK_ASSERT_NO_EFFECT(_second >= 0 && _second <= 59);
        ARK_ASSERT_NO_EFFECT(_millisecond >= 0 && _millisecond <= 999);
        ARK_ASSERT_NO_EFFECT(_microsecond >= 0 && _microsecond <= 999);
    }

    /// Extracts the daytime (hours, minutes, seconds, etc.) from the stored utcTime.
    void computeDaytime()
    {
        AFTimespan span(_utcTime / 10);
        int hour = span.hours();
        // Due to double rounding issues, the previous call to computeGregorian()
        // may have crossed into the next or previous day. We need to correct that.
        if (hour == 23 && _hour == 0)
        {
            _day--;
            if (_day == 0)
            {
                _month--;
                if (_month == 0)
                {
                    _month = 12;
                    _year--;
                }
                _day = daysOfMonth(_year, _month);
            }
        }
        else if (hour == 0 && _hour == 23)
        {
            _day++;
            if (_day > daysOfMonth(_year, _month))
            {
                _month++;
                if (_month > 12)
                {
                    _month = 1;
                    _year++;
                }
                _day = 1;
            }
        }
        _hour = hour;
        _minute = span.minutes();
        _second = span.seconds();
        _millisecond = span.milliseconds();
        _microsecond = span.microseconds();
    }

private:
    ///utility functions used to correct the overflow in computeGregorian
    void checkLimit(short& lower, short& higher, short limit)
    {
        if (lower >= limit)
        {
            higher += short(lower / limit);
            lower = short(lower % limit);
        }
    }

    void normalize()
    {
        checkLimit(_microsecond, _millisecond, 1000);
        checkLimit(_millisecond, _second, 1000);
        checkLimit(_second, _minute, 60);
        checkLimit(_minute, _hour, 60);
        checkLimit(_hour, _day, 24);

        if (_day > daysOfMonth(_year, _month))
        {
            _day -= daysOfMonth(_year, _month);
            if (++_month > 12)
            {
                ++_year;
                _month -= 12;
            }
        }
    }

    AFTimestamp::UTCTimeVal _utcTime{ 0 };
    short  _year{0};
    short  _month{0};
    short  _day{0};
    short  _hour{0};
    short  _minute{0};
    short  _second{0};
    short  _millisecond{0};
    short  _microsecond{0};
};

inline void swap(AFDateTime& d1, AFDateTime& d2)
{
    d1.swap(d2);
}