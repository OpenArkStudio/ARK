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

#include "AFMacros.hpp"
#include "AFSingleton.hpp"
#include "AFMisc.hpp"

class AFCTimeBase : public AFSingleton<AFCTimeBase>
{
public:
    static const int64_t NSECOND_MS = 1000L;
    static const int64_t NMINUTE_MS = 60 * 1000L;
    static const int64_t NHOUR_MS = 60 * 60 * 1000L;
    static const int64_t NDAY_MS = 24 * 60 * 60 * 1000L;
    static const int64_t NWEEK_MS = 7 * 24 * 60 * 60 * 1000L;
    //鍚勭绉?
    static const int64_t NSECOND_S = 1L;
    static const int64_t NMINUTE_S = 60L;
    static const int64_t NHOUR_S = 60 * 60L;
    static const int64_t NDAY_S = 24 * 60 * 60L;
    static const int64_t NWEEK_S = 7 * 24 * 60 * 60L;
    const int MONTH_DAY[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    static const int mnDefaultTimeZone = 8;

public:
    AFCTimeBase()
    {
        mnTimeZone = mnDefaultTimeZone;
    }

public:
    int64_t GetUTCTime()//UTC time as seconds
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    int64_t GetNowMillisecond() //ms with timezone
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + GetTimeZoneMillisecond();
    }
    int64_t GetTimeZoneSecond()
    {
        return mnTimeZone * NHOUR_S;
    }
    int64_t GetTimeZoneMillisecond()
    {
        return mnTimeZone * NHOUR_MS;
    }

    void SetTimeZone(int nTimeZone)
    {
        mnTimeZone = nTimeZone;
    }

    int GetTimeZone()
    {
        return mnTimeZone;
    }
private:
    int mnTimeZone;  //UTC+8
};

class AFTime
{
public:
    enum TimeType
    {
        //-----------BASE-----------
        YEAR,
        MONTH,//start by 0
        DAY,
        HOUR,
        MINUTE,
        SECOND,
        MILLISECOND,
        //------------EX-------------
        WEEK_SINCE_EPOCH,//start by 0
        DAY_SINCE_EPOCH,//start by 0
        HOUR_SINCE_EPOCH,//start by 0
        DAY_OF_WEEK,//start by 1 ,1 means Sunday
    };

public:
    AFTime()
    {
        mnTimeZone = AFCTimeBase::GetInstance().GetTimeZone();
        mnTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + AFCTimeBase::GetInstance().GetTimeZoneMillisecond();
    };

    explicit AFTime(int64_t nTime) : mnTime(nTime)
    {
        mnTimeZone = AFCTimeBase::GetInstance().GetTimeZone();
    }

    AFTime(int64_t nTime, int nTimeZone) : mnTimeZone(nTimeZone), mnTime(nTime)
    {
    }

    explicit AFTime(std::string strTime) : mnTimeZone(AFCTimeBase::GetInstance().GetTimeZone()), mnTime(0)
    {
        InitWithYMDHMSM(strTime);
    }

    AFTime(std::string strTime, int nTimeZone) : mnTimeZone(nTimeZone), mnTime(0)
    {
        InitWithYMDHMSM(strTime);
    }

    //with default(UTC+8) timezone
    AFTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond) : mnTimeZone(AFCTimeBase::GetInstance().GetTimeZone()), mnTime(0)
    {
        InitWithYMDHMSM(nYear, nMonth, nDay, nHour, nMinute, nSecond, 0);
    }

    AFTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nMilliSecond, int nTimeZone = -1) //with particular timezone
    {
        if(nTimeZone == -1)
        {
            mnTimeZone = AFCTimeBase::GetInstance().GetTimeZone();
        }

        mnTime = 0;
        InitWithYMDHMSM(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMilliSecond);
    }

    std::string GetStr()
    {
        return GetStr_YMDHMS();
    }

    std::string GetStr_YMD()
    {
        time_t t;
        t = mnTime / AFCTimeBase::NSECOND_MS;
        struct tm* p = gmtime(&t);

        char buf[255] = { 0 };
        if(p)
        {
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", p);
        }
        std::string strTime;
        strTime.assign(buf, strlen(buf));
        size_t pos = strTime.find(" ");
        if(pos == std::string::npos)
        {
            return "";
        }

        strTime = strTime.substr(0, pos - 1);
        return strTime;
    }
    std::string GetStr_HMS()
    {
        time_t t;
        t = mnTime / AFCTimeBase::NSECOND_MS;
        struct tm* p = gmtime(&t);

        char buf[255] = { 0 };
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", p);
        std::string strTime;
        strTime.assign(buf, strlen(buf));
        size_t pos = strTime.find(" ");
        if(pos == std::string::npos)
        {
            return "";
        }

        strTime = strTime.substr(pos + 1, strTime.length() - pos - 1);
        return strTime;
    }

    std::string GetStr_YMDHMS()
    {
        time_t t;
        t = mnTime / AFCTimeBase::NSECOND_MS;
        struct tm* p = gmtime(&t);

        char buf[255] = { 0 };
        if(p)
        {
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", p);
        }

        std::string strTime;
        strTime.assign(buf, strlen(buf));
        return strTime;
    }

    std::string GetStr_YMDHMSM() //YYYY_MM_DD_hh_mm_ss_ms
    {
        std::stringstream ss;
        ss << Get(YEAR) << "_" << Get(MONTH) << "_" << Get(DAY) << "_" << Get(HOUR) << "_" << Get(MINUTE) << "_" << Get(SECOND) << "_" << Get(MILLISECOND);
        return ss.str();
    }

    bool FromString(const std::string& strTime)
    {
        mnTime = 0;
        mnTimeZone = AFCTimeBase::GetInstance().GetTimeZone();
        InitWithYMDHMSM(strTime);
        return true;
    }

    int64_t GetTime()
    {
        return mnTime;//GetFixTime();
    }

    int64_t Get(TimeType nType)
    {
        int year = 1970;
        int64_t nFixTime = GetTime();//GetFixTime();
        switch(nType)
        {
        case YEAR:
            {
                while(true)
                {
                    int nDays = 365;
                    if(IsLeapYear(year))
                    {
                        nDays++;
                    }

                    if(nFixTime < nDays * AFCTimeBase::NDAY_MS)
                    {
                        break;
                    }

                    nFixTime -= nDays * AFCTimeBase::NDAY_MS;
                    year++;
                }

                return year;
            }
            break;
        case MONTH:
            {
                while(true)
                {
                    int nDays = 365;
                    if(IsLeapYear(year))
                    {
                        nDays++;
                    }

                    for(int month = 1; month <= 12; month++)
                    {
                        int64_t nMonthDays;
                        switch(month)
                        {
                        case 2:
                            {
                                if(nDays == 365)
                                {
                                    nMonthDays = 28;
                                }
                                else
                                {
                                    nMonthDays = 29;
                                }
                            }
                            break;
                        case 1:
                        case 3:
                        case 5:
                        case 7:
                        case 8:
                        case 10:
                        case 12:
                            {
                                nMonthDays = 31;
                            }
                            break;
                        default:
                            {
                                nMonthDays = 30;
                            }
                            break;
                        }

                        if(nFixTime < nMonthDays * AFCTimeBase::NDAY_MS)
                        {
                            return month;
                        }

                        nFixTime -= nMonthDays * AFCTimeBase::NDAY_MS;
                    }

                    year++;
                }
            }
            break;
        case DAY:
            {
                while(true)
                {
                    int nDays = 365;
                    if(IsLeapYear(year))
                    {
                        nDays++;
                    }
                    for(int month = 1; month <= 12; month++)
                    {
                        int64_t nMothDays;
                        switch(month)
                        {
                        case 2:
                            {
                                if(nDays == 365)
                                {
                                    nMothDays = 28;
                                }
                                else
                                {
                                    nMothDays = 29;
                                }
                            }
                            break;
                        case 1:
                        case 3:
                        case 5:
                        case 7:
                        case 8:
                        case 10:
                        case 12:
                            {
                                nMothDays = 31;
                            }
                            break;
                        default:
                            {
                                nMothDays = 30;
                            }
                        }

                        if(nFixTime < nMothDays * AFCTimeBase::NDAY_MS)
                        {
                            return nFixTime / AFCTimeBase::NDAY_MS + 1;
                        }

                        nFixTime -= nMothDays * AFCTimeBase::NDAY_MS;
                    }
                    year++;
                }
            }
            break;
        case HOUR:
            {
                return (nFixTime % AFCTimeBase::NDAY_MS) / AFCTimeBase::NHOUR_MS;
            }
            break;
        case MINUTE:
            {
                return (nFixTime % AFCTimeBase::NHOUR_MS) / AFCTimeBase::NMINUTE_MS;
            }
            break;
        case SECOND:
            {
                return (nFixTime % AFCTimeBase::NMINUTE_MS) / AFCTimeBase::NSECOND_MS;
            }
            break;
        case MILLISECOND:
            {
                return (nFixTime % AFCTimeBase::NSECOND_MS);
            }
            break;
        case WEEK_SINCE_EPOCH:
            {
                return (mnTime + AFCTimeBase::NDAY_MS * 3) / AFCTimeBase::NWEEK_MS;// 1970.1.1 is thursday , monday is first day of week
            }
            break;
        case DAY_SINCE_EPOCH:
            {
                return mnTime / AFCTimeBase::NDAY_MS;
            }
            break;
        case HOUR_SINCE_EPOCH:
            {
                return mnTime / AFCTimeBase::NHOUR_MS;
            }
            break;
        case DAY_OF_WEEK:
            {
                return ((nFixTime + AFCTimeBase::NDAY_MS * 3) % AFCTimeBase::NWEEK_MS) / AFCTimeBase::NDAY_MS + 1;// 1970.1.1 is thursday , Monday is first day of week ,start by 1
            }
            break;
        default:
            {
                return 0;
            }
            break;
        }

        return 0;
    }



    void SetTime(int64_t nTime)
    {
        mnTime = nTime;
    }

    //甯︽椂鍖?
    int64_t GetYear()
    {
        return Get(YEAR);
    }

    int64_t GetMonth()
    {
        return Get(MONTH);
    }
    int64_t GetDay()
    {
        return Get(DAY);
    }
    int64_t GetHour()
    {
        return Get(HOUR);
    }
    int64_t GetMinute()
    {
        return Get(MINUTE);
    }
    int64_t GetSecond()
    {
        return Get(SECOND);
    }

    bool SameMinute(int64_t nTime)
    {
        AFTime xTime(nTime);
        if(xTime.Get(MINUTE) == Get(MINUTE)
                && xTime.Get(HOUR) == Get(HOUR)
                && xTime.Get(DAY) == Get(DAY)
                && xTime.Get(MONTH) == Get(MONTH)
                && xTime.Get(YEAR) == Get(YEAR))
        {
            return true;
        }

        return false;
    }
    bool SameHour(int64_t nTime)
    {
        AFTime xTime(nTime);
        if(xTime.Get(HOUR) == Get(HOUR)
                && xTime.Get(DAY) == Get(DAY)
                && xTime.Get(MONTH) == Get(MONTH)
                && xTime.Get(YEAR) == Get(YEAR))
        {
            return true;
        }

        return false;
    }

    bool SameDay(int64_t nTime)
    {
        AFTime xTime(nTime);
        if(xTime.Get(DAY) == Get(DAY)
                && xTime.Get(MONTH) == Get(MONTH)
                && xTime.Get(YEAR) == Get(YEAR))
        {
            return true;
        }

        return false;
    }
    bool SameMonth(int64_t nTime)
    {
        AFTime xTime(nTime);
        if(xTime.Get(MONTH) == Get(MONTH)
                && xTime.Get(YEAR) == Get(YEAR))
        {
            return true;
        }

        return false;
    }
    bool SameYear(int64_t nTime)
    {
        AFTime xTime(nTime);
        if(xTime.Get(YEAR) == Get(YEAR))
        {
            return true;
        }

        return false;
    }
    bool SameWeek(int64_t nTime)
    {
        if(SameDay(nTime))
        {
            return true;
        }

        if(!SameYear(nTime))
        {
            return false;
        }

        AFTime xTime(nTime);
        if(GetWeekOfYear() == xTime.GetWeekOfYear())
        {
            return true;
        }

        return false;
    }

    int GetDayOfWeek()
    {
        return (int)Get(DAY_OF_WEEK);
    }

    int GetWeekOfMonth()
    {
        AFTime xTime((int)Get(YEAR), (int)Get(MONTH), 1, 0, 0, 0, 0);
        int nFirstDayOfWeek = xTime.GetDayOfWeek();
        int64_t nDaySpan = Get(DAY) - 1;
        return int((nFirstDayOfWeek + nDaySpan) / 7);
    }

    int GetDayOfYear()
    {
        int nDayOfYear = 0;
        int nYear = (int)Get(YEAR);
        int nMonth = (int)Get(MONTH);
        int nDay = (int)Get(DAY);
        for(int i = 0; i < nMonth - 1; ++i)
        {
            nDayOfYear += AFCTimeBase::GetInstance().MONTH_DAY[i];

            if(IsLeapYear(nYear) && i == 2)
            {
                nDayOfYear += 1;
            }
        }

        //add this month
        nDayOfYear += nDay;

        return nDayOfYear;
    }

    int GetWeekOfYear()
    {
        int nJulian = GetDayOfYear();
        int nDayOfWeek = GetDayOfWeek();
        AFTime xTime(Get(YEAR), 1, 1, 0, 0, 0, 0);
        int nFirstDayOfWeek = xTime.GetDayOfWeek();
        int nWeekNum = (nJulian + 6) / 7;
        if(nDayOfWeek < nFirstDayOfWeek)
        {
            ++nWeekNum;
        }

        return nWeekNum;
    }


private:
    void InitWithYMDHMSM(std::string strTime)
    {
        //time format is YYYY-MM-DD HH:mm:ss
        std::vector<std::string> cells;
        int nCount = Split(strTime, cells, " ");
        ARK_ASSERT_RET_NONE(cells.size() == 2 && nCount == 2);

        std::vector<std::string> cellsYMD;
        nCount = Split(cells.at(0), cellsYMD, "-");
        ARK_ASSERT_RET_NONE(cellsYMD.size() == 3 && nCount == 3);

        std::vector<std::string> cellsHMS;
        nCount = Split(cells.at(1), cellsHMS, ":");
        ARK_ASSERT_RET_NONE(cellsHMS.size() == 3 && nCount == 3);

        int nYear(0);
        int nMonth(0);
        int nDay(0);
        int nHour(0);
        int nMinute(0);
        int nSecond(0);

        bool bRet = AFMisc::ARK_FROM_STR(cellsYMD[0], nYear);
        bRet = AFMisc::ARK_FROM_STR(cellsYMD[1], nMonth);
        bRet = AFMisc::ARK_FROM_STR(cellsYMD[2], nDay);
        bRet = AFMisc::ARK_FROM_STR(cellsHMS[0], nHour);
        bRet = AFMisc::ARK_FROM_STR(cellsHMS[1], nMinute);
        bRet = AFMisc::ARK_FROM_STR(cellsHMS[2], nSecond);

        int nMilliSecond = 0;
        if(cells.size() == 3)
        {
            bRet = AFMisc::ARK_FROM_STR(cells[2], nMilliSecond);
            ARK_ASSERT_NO_EFFECT(bRet);
        }

        InitWithYMDHMSM(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMilliSecond);
    }

    void InitWithYMDHMSM(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nMilliSecond)
    {
        if(nYear < 1970)
        {
            return;
        }

        int nTmpYear = 1970;
        while(nTmpYear < nYear)
        {
            mnTime += AFCTimeBase::NDAY_MS * 365;
            if(IsLeapYear(nTmpYear))
            {
                mnTime += AFCTimeBase::NDAY_MS;
            }
            nTmpYear++;
        }

        bool bIsLeapYear = IsLeapYear(nYear);
        for(int month = 1; month < nMonth; month++)
        {
            int64_t nMothDays;
            switch(month)
            {
            case 2:
                {
                    if(!bIsLeapYear)
                    {
                        nMothDays = 28;
                    }
                    else
                    {
                        nMothDays = 29;
                    }
                }
                break;
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                {
                    nMothDays = 31;
                }
                break;
            default:
                {
                    nMothDays = 30;
                }
                break;
            }

            mnTime += AFCTimeBase::NDAY_MS * nMothDays;
        }

        mnTime += AFCTimeBase::NDAY_MS * (nDay - 1);
        mnTime += AFCTimeBase::NHOUR_MS * nHour;
        mnTime += AFCTimeBase::NMINUTE_MS * nMinute;
        mnTime += AFCTimeBase::NSECOND_MS * nSecond;
        mnTime += nMilliSecond;
    }

    //int64_t GetFixTime();//add timeZone value
    bool IsLeapYear(int nYear)
    {
        if((nYear % 4 == 0) && (nYear % 100 != 0))
        {
            return true;
        }
        if(nYear % 400 == 0)
        {
            return true;
        }
        return false;
    }

    int Split(const std::string& str, std::vector<std::string>& ret_, std::string sep)
    {
        if(str.empty())
        {
            return 0;
        }

        std::string tmp;
        std::string::size_type pos_begin = str.find_first_not_of(sep);
        std::string::size_type comma_pos = 0;

        while(pos_begin != std::string::npos)
        {
            comma_pos = str.find(sep, pos_begin);
            if(comma_pos != std::string::npos)
            {
                tmp = str.substr(pos_begin, comma_pos - pos_begin);
                pos_begin = comma_pos + sep.length();
            }
            else
            {
                tmp = str.substr(pos_begin);
                pos_begin = comma_pos;
            }

            if(!tmp.empty())
            {
                ret_.push_back(tmp);
                tmp.clear();
            }
        }

        return (int)ret_.size();
    }

private:
    int64_t mnTime;
    int mnTimeZone;  //UTC+8
};