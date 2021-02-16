/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "base/time/AFDateTime.hpp"

TEST_CASE("GetMilliseconds() should return now_UTC_time", "[GetMilliseconds()]")
{
    ark::AFDateTime date_time_instance;
    REQUIRE(date_time_instance.GetMilliseconds() ==
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count());
}
TEST_CASE("default constructor should return AFDateTime instance with now_UTC_time", " AFDateTime()")
{
    ark::AFDateTime date_time_instance{};
    REQUIRE(date_time_instance.GetMilliseconds() ==
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count());
}

TEST_CASE("using TimeVal constructor should return instance with the same TimeVal", " AFDateTime(TimeVal tv)")
{
    auto time_val =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    ark::AFDateTime date_time_instance{time_val};
    REQUIRE(date_time_instance.GetMilliseconds() == time_val);
}

TEST_CASE("using year_month_day_hour_minute_second_tzd constructor should return instance with same value",
    "AFDateTime(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int tzd )")
{
    ark::AFDateTime date_time_provider{};
    int year = date_time_provider.GetYear();
    int month = date_time_provider.GetMonth();
    int day = date_time_provider.GetDay();
    int hour = date_time_provider.GetHour();
    int minute = date_time_provider.GetMinute();
    int second = date_time_provider.GetSecond();

    ark::AFDateTime date_time_instance(year, month, day, hour, minute, second);
    REQUIRE(date_time_instance.GetTime() == date_time_provider.GetTime()); //accurate to seconds
}

TEST_CASE("same AFdateTime instance, == should return true", "bool operator==(const AFDateTime& ts) const")
{
    ark::AFDateTime date_time_instance(5);
    ark::AFDateTime date_time_control_group(5);
    REQUIRE(date_time_instance == date_time_control_group);
}

TEST_CASE("copy constructor should return instance same as copied instance", "AFDateTime(const AFDateTime& other)")
{
    ark::AFDateTime date_time_provider;
    ark::AFDateTime date_time_instance{date_time_provider};
    REQUIRE(date_time_provider == date_time_instance);
}

TEST_CASE("= constructor with AFDateTime instance should return another instance same as assigned instance",
    "AFDateTime& operator=(const AFDateTime& other)")
{
    ark::AFDateTime date_time_provider;
    ark::AFDateTime date_time_instance = date_time_provider;
    REQUIRE(date_time_instance == date_time_provider);
}

TEST_CASE("= constructor with timeval should return instance with same timeval", "AFDateTime& operator=(TimeVal& tv)")
{
    ark::AFDateTime::TimeVal tv_provider =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();

    ark::AFDateTime date_time_instance = tv_provider;
    REQUIRE(date_time_instance.GetMilliseconds() == tv_provider);
}

TEST_CASE("two AFdateTime instances should be swapped", "void swap(AFDateTime& timestamp)")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime date_time_control_group(date_time_instance.GetMilliseconds() + 5);
    REQUIRE(date_time_instance < date_time_control_group);
    date_time_instance.swap(date_time_control_group);
    REQUIRE(date_time_instance > date_time_control_group);
}

TEST_CASE("two different AFdateTime should return false", "bool operator!=(const AFDateTime& ts) const")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime date_time_control_group(date_time_instance.GetMilliseconds() + 5);
    REQUIRE(date_time_instance != date_time_control_group);
}

TEST_CASE("instance with larger timeval > instance with smaller timeval", "bool operator>(const AFDateTime& ts) const")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime date_time_control_group(date_time_instance.GetMilliseconds() + 5);
    REQUIRE(date_time_control_group > date_time_instance);
}

TEST_CASE("instance with larger or same timeval >= instance with smaller or same timeval",
    "bool operator>=(const AFDateTime& ts) const")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime date_time_control_group(date_time_instance.GetMilliseconds() + 5);
    REQUIRE(date_time_control_group >= date_time_instance);
    REQUIRE(date_time_control_group >= date_time_control_group);
}

TEST_CASE("instance with smaller timeval < instance with larger timeval", "bool operator<(const AFDateTime& ts) const")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime date_time_control_group(date_time_instance.GetMilliseconds() + 5);
    REQUIRE(date_time_instance < date_time_control_group);
}

TEST_CASE("instance with smaller  or same timeval < instance with larger or same timeval",
    "bool operator<=(const AFDateTime& ts) const")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime date_time_control_group(date_time_instance.GetMilliseconds() + 5);
    REQUIRE(date_time_instance <= date_time_control_group);
    REQUIRE(date_time_instance <= date_time_instance);
}

TEST_CASE("old instance plus Tiemdiff should return plused timeval", "AFDateTime operator+(TimeDiff d) const")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime::TimeVal old_tv = date_time_instance.GetMilliseconds();

    date_time_instance = date_time_instance + (ark::AFDateTime::TimeDiff)2;
    REQUIRE(date_time_instance.GetMilliseconds() == old_tv + (ark::AFDateTime::TimeDiff)2);
}

TEST_CASE("old instance minus Timediff should return minused timeval", "AFDateTime operator-(TimeDiff d) const")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime::TimeVal old_tv = date_time_instance.GetMilliseconds();

    date_time_instance = date_time_instance - (ark::AFDateTime::TimeDiff)2;
    REQUIRE(date_time_instance.GetMilliseconds() == old_tv - (ark::AFDateTime::TimeDiff)2);
}

TEST_CASE("overload operator +=(Timediff)", "AFDateTime& operator+=(TimeDiff d)")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime::TimeVal old_tv = date_time_instance.GetMilliseconds();

    date_time_instance += (ark::AFDateTime::TimeDiff)2;
    REQUIRE(date_time_instance.GetMilliseconds() == old_tv + (ark::AFDateTime::TimeDiff)2);
}

TEST_CASE("overload operator -=(Timediff)", "AFDateTime& operator-=(TimeDiff d)")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime::TimeVal old_tv = date_time_instance.GetMilliseconds();

    date_time_instance -= (ark::AFDateTime::TimeDiff)2;
    REQUIRE(date_time_instance.GetMilliseconds() == old_tv - (ark::AFDateTime::TimeDiff)2);
}

TEST_CASE("overload operator +(AFTimespan)", "AFDateTime operator+(const AFTimespan& span) const")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime::TimeVal old_tv = date_time_instance.GetMilliseconds();

    date_time_instance = date_time_instance + ark::AFTimespan(2);
    REQUIRE(date_time_instance.GetMilliseconds() == old_tv + (ark::AFDateTime::TimeDiff)2);
}

TEST_CASE("overload operator -(AFTimespan)", " AFDateTime operator-(const AFTimespan& span) const")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime::TimeVal old_tv = date_time_instance.GetMilliseconds();

    date_time_instance = date_time_instance - ark::AFTimespan(2);
    REQUIRE(date_time_instance.GetMilliseconds() == old_tv - (ark::AFDateTime::TimeDiff)2);
}

TEST_CASE("overload operator -(AFdateTime)", " TimeDiff operator-(const AFDateTime& ts) const")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime date_time_control_group(date_time_instance.GetMilliseconds() + 5);
    REQUIRE(date_time_control_group - date_time_instance ==
            date_time_control_group.GetMilliseconds() - date_time_instance.GetMilliseconds());
}

TEST_CASE("overload operator +=(AFTimeSpan)", " AFDateTime& operator+=(const AFTimespan& span)")
{
    ark::AFDateTime date_time_instance;

    ark::AFDateTime date_time_control_group(date_time_instance.GetMilliseconds() + 5);

    date_time_instance += ark::AFTimespan(date_time_control_group - date_time_instance);
    REQUIRE(date_time_instance == date_time_control_group);
}

TEST_CASE("overload operator -=(AFTimeSpan)", " AFDateTime& operator-=(const AFTimespan& span)")
{
    ark::AFDateTime date_time_instance;

    ark::AFDateTime date_time_control_group(date_time_instance.GetMilliseconds() + 5);

    date_time_control_group -= ark::AFTimespan(date_time_control_group - date_time_instance);
    REQUIRE(date_time_instance == date_time_control_group);
}

TEST_CASE("should return std::time_t", "  std::time_t GetTime() const")
{
    ark::AFDateTime date_time_instance;
    REQUIRE(date_time_instance.GetTime() == std::time(0));
}

TEST_CASE("should return timediff between current time", "  TimeDiff GetElapsed() const")
{
    ark::AFDateTime date_time_instance;
    ark::AFDateTime date_time_control_group(date_time_instance.GetMilliseconds() + 5);
    REQUIRE(date_time_instance.GetElapsed() == ark::AFDateTime::TimeDiff(ark::AFDateTime() - date_time_instance));
}

TEST_CASE("return interval not equal with timediff between current time", " bool IsElapsed(TimeDiff interval) const")
{
    ark::AFDateTime date_time_instance;
    date_time_instance -= 20;
    REQUIRE(date_time_instance.IsElapsed(19) == true);
}

TEST_CASE("return time ago", " TimeVal Raw() const")
{
    ark::AFDateTime date_time_instance;
    date_time_instance -= 5;

    REQUIRE(date_time_instance.Raw() == date_time_instance.GetMilliseconds());
    REQUIRE(date_time_instance.Raw() !=
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count());
}

TEST_CASE("return time in 0 time_zone", "std::tm GetUTCTime() const")
{
    time_t control_group;
    time(&control_group);

    ark::AFDateTime date_time_instance;
    std::tm current_time = date_time_instance.GetUTCTime();
    REQUIRE(mktime(&current_time) == mktime(gmtime(&control_group)));
}

TEST_CASE("return time in system_time_zone", " std::tm GetLocalTime() const")
{
    time_t control_group;
    time(&control_group);

    ark::AFDateTime date_time_instance;
    std::tm current_time = date_time_instance.GetLocalTime();
    REQUIRE(mktime(&current_time) == mktime(localtime(&control_group)));
}

TEST_CASE("return year", "int GetYear() const")
{
    ark::AFDateTime date_time_instance;

    time_t control_group;
    time(&control_group);
    std::tm* p = localtime(&control_group);
    REQUIRE(date_time_instance.GetYear() == p->tm_year + 1900);
}

TEST_CASE("return month", "int GetMonth() const")
{
    ark::AFDateTime date_time_instance;

    time_t control_group;
    time(&control_group);
    std::tm* p = localtime(&control_group);
    REQUIRE(date_time_instance.GetMonth() == p->tm_mon + 1);
}

TEST_CASE("return day_of_week", "int GetDayOfWeek(int firstDayOfWeek = MONDAY) const")
{
    ark::AFDateTime date_time_instance;

    time_t control_group;
    time(&control_group);
    std::tm* p = localtime(&control_group);
    REQUIRE(date_time_instance.GetDayOfWeek() == p->tm_wday);
}

TEST_CASE("return day_of_year", "int GetDayOfYear() const")
{
    ark::AFDateTime date_time_instance;

    time_t control_group;
    time(&control_group);
    REQUIRE(date_time_instance.GetDayOfYear() == localtime(&control_group)->tm_yday);
}

void test_weekOfYear(int y, int m, int d)
{
    ark::AFDateTime date_time_instance(y, m, d);
    auto week = date_time_instance.GetWeekOfYear();
    auto day = date_time_instance.GetDayOfWeek();
    day = day > 0 ? day : 7;
    auto day_of_year = date_time_instance.GetDayOfYear(); //Jan_1 is 0
    day_of_year++;
    auto _day = ark::AFDateTime(date_time_instance.GetYear(), 1, 4).GetDayOfWeek();
    _day = _day > 0 ? _day : 7;
    int day_reduce;
    day_reduce = (week - 1) * 7 + 4 + day - _day;
    REQUIRE(day_of_year == day_reduce);
}

TEST_CASE("return week_of_year", "int GetWeekOfYear(int firstDayOfWeek = MONDAY) const")
{
    int daysOfMonthTable[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    for (int y = 2017; y <= 2019; y++)
    {
        for (int i = 1; i <= 12; i++)
        {
            for (int j = 1; j <= daysOfMonthTable[i]; j++)
            {
                test_weekOfYear(y, i, j);
            }
        }
    }
}

TEST_CASE("return day of month", "int GetDay(bool is_monday_first_day = true) const")
{
    ark::AFDateTime date_time_instance;

    time_t control_group;
    time(&control_group);
    REQUIRE(date_time_instance.GetDay() == localtime(&control_group)->tm_mday);
}

TEST_CASE("return hour of day", "int GetHour() const")
{
    ark::AFDateTime date_time_instance;

    time_t control_group;
    time(&control_group);

    auto tzd = 8;
    REQUIRE(date_time_instance.GetHour() == localtime(&control_group)->tm_hour - tzd);
}

TEST_CASE("return hour of day in am/pm format", "int GethourAMPM() const")
{
    ark::AFDateTime date_time_instance;

    time_t control_group;
    time(&control_group);
    std::tm* p = localtime(&control_group);

    int tzd = 8;
    p->tm_hour -= tzd;
    if (p->tm_hour < 1)
    {
        p->tm_hour = 12;
    }
    else if (p->tm_hour > 12)
    {
        p->tm_hour -= 12;
    }
    REQUIRE(date_time_instance.GetHour() == p->tm_hour);
}

TEST_CASE("return hour of day whether in am", "bool IsAM() const")
{
    ark::AFDateTime date_time_instance;

    time_t control_group;
    time(&control_group);
    std::tm* p = localtime(&control_group);

    int tzd = 8;
    p->tm_hour -= tzd;
    bool isam = false;
    if (p->tm_hour < 12)
    {
        isam = true;
    }
    REQUIRE(date_time_instance.IsAM() == isam);
}

TEST_CASE("return hour of day whether in pm", "bool IsPM() const")
{
    ark::AFDateTime date_time_instance;

    time_t control_group;
    time(&control_group);
    std::tm* p = localtime(&control_group);

    int tzd = 8;
    p->tm_hour -= tzd;
    bool ispm = false;
    if (p->tm_hour >= 12)
    {
        ispm = true;
    }
    REQUIRE(date_time_instance.IsPM() == ispm);
}

TEST_CASE("return minute of hour", "int GetMinute() const")
{
    ark::AFDateTime date_time_instance;

    time_t control_group;
    time(&control_group);

    REQUIRE(date_time_instance.GetMinute() == localtime(&control_group)->tm_min);
}

TEST_CASE("return second of minute", "int GetSecond() const")
{
    ark::AFDateTime date_time_instance;

    time_t control_group;
    time(&control_group);

    REQUIRE(date_time_instance.GetSecond() == localtime(&control_group)->tm_sec);
}

TEST_CASE("return whether LeapYear", "bool IsLeapYear() const")
{
    ark::AFDateTime date_time_instance;
    auto year = date_time_instance.GetYear();
    REQUIRE(date_time_instance.IsLeapYear() == ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0)));
}

TEST_CASE("return whether same minute", "bool SameMinute(TimeVal time)")
{
    ark::AFDateTime date_time_instance;
    REQUIRE(date_time_instance.SameMinute(date_time_instance.GetMilliseconds()) == true);
}

TEST_CASE("return whether same hour", "bool SameHour(TimeVal time)")
{
    ark::AFDateTime date_time_instance;
    REQUIRE(date_time_instance.SameHour(date_time_instance.GetMilliseconds()) == true);
}

TEST_CASE("return whether same day", "bool SameDay(TimeVal time)")
{
    ark::AFDateTime date_time_instance;
    REQUIRE(date_time_instance.SameDay(date_time_instance.GetMilliseconds()) == true);
}

TEST_CASE("return whether same month", "bool SameMonth(TimeVal time)")
{
    ark::AFDateTime date_time_instance;
    REQUIRE(date_time_instance.SameMonth(date_time_instance.GetMilliseconds()) == true);
}

TEST_CASE("return whether same year", "bool SameYear(TimeVal time)")
{
    ark::AFDateTime date_time_instance;
    REQUIRE(date_time_instance.SameYear(date_time_instance.GetMilliseconds()) == true);
}

TEST_CASE("return whether same week", "bool SameWeek(TimeVal time)")
{
    ark::AFDateTime date_time_instance;
    REQUIRE(date_time_instance.SameWeek(date_time_instance.GetMilliseconds()) == true);
}

TEST_CASE("return time in %Y/%m/%d %H:%M:%S format", "std::string ToString()")
{
    time_t control_group;
    time(&control_group);
    char timeBuff[32] = {0};
    std::strftime(timeBuff, sizeof(timeBuff), "%Y/%m/%d %H:%M:%S", localtime(&control_group));

    ark::AFDateTime date_time_instance;
    REQUIRE(date_time_instance.ToString() == (std::string)(timeBuff));
}

TEST_CASE("return time in %FT%TZ format", "std::string ToISO8601String()")
{
    time_t control_group;
    time(&control_group);
    char timeBuff[64] = {0};
    std::strftime(timeBuff, sizeof(timeBuff), "%FT%TZ", gmtime(&control_group));

    ark::AFDateTime date_time_instance;
    REQUIRE(date_time_instance.ToISO8601String() == (std::string)(timeBuff));
}

TEST_CASE("return parameter year whether leapYear", "static bool IsLeapYear(int year)")
{
    auto year = ark::AFDateTime().GetYear();
    REQUIRE(ark::AFDateTime::IsLeapYear(year) == ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0)));
}

TEST_CASE("return days of month by parameter year and month", "static int GetDaysOfMonth(int year, int month)")
{
    auto date_time_provider = ark::AFDateTime();
    auto year = date_time_provider.GetYear();
    auto month = date_time_provider.GetMonth();
    int daysOfMonthTable[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && date_time_provider.IsLeapYear())
    {
        daysOfMonthTable[2] = 29;
    }
    REQUIRE(ark::AFDateTime::GetDaysOfMonth(year, month) == daysOfMonthTable[month]);
}

TEST_CASE("return y/m/d/h/m/s parameter whether valid",
    "static bool isValid(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int "
    "millisecond = 0)")
{
    REQUIRE(ark::AFDateTime::isValid(1000, 0, 1) == false);
    REQUIRE(ark::AFDateTime::isValid(10000, 1, 1) == false);
    REQUIRE(ark::AFDateTime::isValid(1000, 19, 1) == false);
    REQUIRE(ark::AFDateTime::isValid(1000, 2, 30) == false);
    REQUIRE(ark::AFDateTime::isValid(1000, 1, 1, 80) == false);
    REQUIRE(ark::AFDateTime::isValid(1000, 9, 1, 16, 90) == false);
    REQUIRE(ark::AFDateTime::isValid(1000, 9, 1, 16, 30, 90) == false);
    REQUIRE(ark::AFDateTime::isValid(1000, 9, 1, 10, 30, 30, 2000) == false);
    REQUIRE(ark::AFDateTime::isValid(2019, 9, 24, 20, 6, 30, 300) == true);
}

TEST_CASE("return current time", "static TimeVal GetNowTime()")
{
    REQUIRE(ark::AFDateTime::GetNowTime() ==
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                .count());
}

TEST_CASE("return timestamp", "static int GetTimestamp()")
{
    REQUIRE(
        ark::AFDateTime::GetTimestamp() ==
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

TEST_CASE("inline void swap(AFDateTime& d1, AFDateTime& d2)", "[swap]")
{
    ark::AFDateTime date_time_instance;
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    ark::AFDateTime date_time_control_group;
    swap(date_time_instance, date_time_control_group);
    REQUIRE(date_time_instance > date_time_control_group);
}
