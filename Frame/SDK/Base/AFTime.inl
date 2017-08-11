#include "AFPlatform.hpp"
#include "AFMacros.hpp"
#include "AFTime.hpp"
#include "common/lexical_cast.hpp"

int AFTime::mnTimeZone = AFTime::mnDefaultTimeZone;

AFTime::AFTime()
{
    mnTimeZone = mnDefaultTimeZone;
    mnTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + GetTimeZoneMillisecond();
}

AFTime::AFTime(int64_t nTime)
    : mnTime(nTime)
{
    mnTimeZone = mnDefaultTimeZone;
}

AFTime::AFTime(int64_t nTime, int nTimeZone)
{
    mnTimeZone = nTimeZone;
    mnTime = nTime;
}

AFTime::AFTime(std::string strTime)
{
    mnTimeZone = mnDefaultTimeZone;
    mnTime = 0;
    InitWithYMDHMSM(strTime);
}

AFTime::AFTime(std::string strTime, int nTimeZone)
{
    mnTimeZone = nTimeZone;
    mnTime = 0;
    InitWithYMDHMSM(strTime);
}

AFTime::AFTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond)
{
    mnTime = 0;
    InitWithYMDHMSM(nYear, nMonth, nDay, nHour, nMinute, nSecond, 0);
}

AFTime::AFTime(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nMilliSecond, int nTimeZone/* = mnDefaultTimeZone*/)
{
    mnTimeZone = nTimeZone;
    mnTime = 0;
    InitWithYMDHMSM(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMilliSecond);
}

int64_t AFTime::GetTime()
{
    return mnTime;//GetFixTime();
}

int64_t AFTime::GetUTCTime()
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

//int64_t NFTime::GetNowSecondTime()
//{
//    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + GetTimeZoneSecond();
//}

int64_t AFTime::GetNowMillisecond()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + GetTimeZoneMillisecond();
}

int64_t AFTime::GetTimeZoneSecond()
{
    return mnTimeZone * NHOUR_S;
}

int64_t AFTime::GetTimeZoneMillisecond()
{
    return mnTimeZone * NHOUR_MS;
}

std::string AFTime::GetStr()
{
    return GetStr_YMDHMS();
}

std::string AFTime::GetStr_YMD()
{
    time_t t;
    t = mnTime / NSECOND_MS; //转成秒
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

std::string AFTime::GetStr_HMS()
{
    time_t t;
    t = mnTime / NSECOND_MS; //转成秒
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

std::string AFTime::GetStr_YMDHMS()
{
    time_t t;
    t = mnTime / NSECOND_MS; //转成秒
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

std::string AFTime::GetStr_YMDHMSM()
{
    std::stringstream ss;
    ss << Get(YEAR) << "_" << Get(MONTH) << "_" << Get(DAY) << "_" << Get(HOUR) << "_" << Get(MINUTE) << "_" << Get(SECOND) << "_" << Get(MILLISECOND);
    return ss.str();
}

bool AFTime::FromString(const std::string& strTime)
{
    mnTime = 0;
    mnTimeZone = mnDefaultTimeZone;
    InitWithYMDHMSM(strTime);
    return true;
}

int64_t AFTime::Get(TimeType nType)
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

                if(nFixTime < nDays * AFTime::NDAY_MS)
                {
                    break;
                }

                nFixTime -= nDays * AFTime::NDAY_MS;
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

                    if(nFixTime < nMonthDays * AFTime::NDAY_MS)
                    {
                        return month;
                    }

                    nFixTime -= nMonthDays * AFTime::NDAY_MS;
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

                    if(nFixTime < nMothDays * AFTime::NDAY_MS)
                    {
                        return nFixTime / AFTime::NDAY_MS + 1;
                    }

                    nFixTime -= nMothDays * AFTime::NDAY_MS;
                }
                year++;
            }
        }
        break;
    case HOUR:
        {
            return (nFixTime % AFTime::NDAY_MS) / AFTime::NHOUR_MS;
        }
        break;
    case MINUTE:
        {
            return (nFixTime % AFTime::NHOUR_MS) / AFTime::NMINUTE_MS;
        }
        break;
    case SECOND:
        {
            return (nFixTime % AFTime::NMINUTE_MS) / AFTime::NSECOND_MS;
        }
        break;
    case MILLISECOND:
        {
            return (nFixTime % AFTime::NSECOND_MS);
        }
        break;
    case WEEK_SINCE_EPOCH:
        {
            return (mnTime + AFTime::NDAY_MS * 3) / AFTime::NWEEK_MS;// 1970.1.1 is thursday , monday is first day of week
        }
        break;
    case DAY_SINCE_EPOCH:
        {
            return mnTime / AFTime::NDAY_MS;
        }
        break;
    case HOUR_SINCE_EPOCH:
        {
            return mnTime / AFTime::NHOUR_MS;
        }
        break;
    case DAY_OF_WEEK:
        {
            return ((nFixTime + AFTime::NDAY_MS * 3) % AFTime::NWEEK_MS) / AFTime::NDAY_MS + 1;// 1970.1.1 is thursday , monday is first day of week ,start by 1
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

int AFTime::GetTimeZone()
{
    return mnTimeZone;
}

void AFTime::SetTime(int64_t nTime)
{
    mnTime = nTime;
}

void AFTime::SetTimeZone(int nTimeZone)
{
    mnTimeZone = nTimeZone;
}

int64_t AFTime::GetYear()
{
    return Get(YEAR);
}

int64_t AFTime::GetMonth()
{
    return Get(MONTH);
}

int64_t AFTime::GetDay()
{
    return Get(DAY);
}

int64_t AFTime::GetHour()
{
    return Get(HOUR);
}

int64_t AFTime::GetMinute()
{
    return Get(MINUTE);
}

int64_t AFTime::GetSecond()
{
    return Get(SECOND);
}

void AFTime::InitWithYMDHMSM(std::string strTime)
{
    //time format is YYYY-MM-DD HH:mm:ss
    std::vector<std::string> cells;
    Split(strTime, cells, " ");
    if(cells.size() < 2)
    {
        return;
    }
    std::vector<std::string> cellsYMD;
    Split(cells.at(0), cellsYMD, "-");
    if(cellsYMD.size() != 3)
    {
        return;
    }
    std::vector<std::string> cellsHMS;
    Split(cells.at(1), cellsHMS, ":");
    if(cellsHMS.size() != 3)
    {
        return;
    }
    int nYear = ARK_LEXICAL_CAST<int>(cellsYMD[0]);
    int nMonth = ARK_LEXICAL_CAST<int>(cellsYMD[1]);
    int nDay = ARK_LEXICAL_CAST<int>(cellsYMD[2]);
    int nHour = ARK_LEXICAL_CAST<int>(cellsHMS[0]);
    int nMinute = ARK_LEXICAL_CAST<int>(cellsHMS[1]);
    int nSecond = ARK_LEXICAL_CAST<int>(cellsHMS[2]);
    int nMilliSecond = 0;
    if(cells.size() == 3)
    {
        nMilliSecond = ARK_LEXICAL_CAST<int>(cells[2]);
    }

    if(nYear < 1970)
    {
        return;
    }

    int nTmpYear = 1970;
    while(nTmpYear < nYear)
    {
        mnTime += AFTime::NDAY_MS * 365;
        if(IsLeapYear(nTmpYear))
        {
            mnTime += AFTime::NDAY_MS;
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

        mnTime += AFTime::NDAY_MS * nMothDays;
    }

    mnTime += AFTime::NDAY_MS * (nDay - 1);
    mnTime += AFTime::NHOUR_MS * nHour;
    mnTime += AFTime::NMINUTE_MS * nMinute;
    mnTime += AFTime::NSECOND_MS * nSecond;
    mnTime += nMilliSecond;

    //mnTime -= NFTime::NHOUR_MS * mnTimeZone;
}

void AFTime::InitWithYMDHMSM(int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, int nMilliSecond)
{
    if(nYear < 1970)
    {
        return;
    }

    int nTmpYear = 1970;
    while(nTmpYear < nYear)
    {
        mnTime += AFTime::NDAY_MS * 365;
        if(IsLeapYear(nTmpYear))
        {
            mnTime += AFTime::NDAY_MS;
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

        mnTime += AFTime::NDAY_MS * nMothDays;
    }

    mnTime += AFTime::NDAY_MS * (nDay - 1);
    mnTime += AFTime::NHOUR_MS * nHour;
    mnTime += AFTime::NMINUTE_MS * nMinute;
    mnTime += AFTime::NSECOND_MS * nSecond;
    mnTime += nMilliSecond;

    //mnTime -= NFTime::NHOUR_MS * mnTimeZone;
}

//int64_t NFTime::GetFixTime()
//{
//    return mnTime + mnTimeZone * NFTime::NHOUR_MS;
//}

bool AFTime::IsLeapYear(int nYear)
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
int AFTime::Split(const std::string& str, std::vector<std::string>& ret_, std::string sep)
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

    return 0;
}

bool AFTime::SameMinute(int64_t nTime)
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

bool AFTime::SameHour(int64_t nTime)
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

bool AFTime::SameDay(int64_t nTime)
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

bool AFTime::SameMonth(int64_t nTime)
{
    AFTime xTime(nTime);
    if(xTime.Get(MONTH) == Get(MONTH)
            && xTime.Get(YEAR) == Get(YEAR))
    {
        return true;
    }

    return false;
}

bool AFTime::SameYear(int64_t nTime)
{
    AFTime xTime(nTime);
    if(xTime.Get(YEAR) == Get(YEAR))
    {
        return true;
    }

    return false;
}

bool AFTime::SameWeek(int64_t nTime)
{
    //同一天，可定是同一周
    if(SameDay(nTime))
    {
        return true;
    }

    //不是同一年，直接不做比较
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

int AFTime::GetDayOfWeek()
{
    return (int)Get(DAY_OF_WEEK);
}

int AFTime::GetWeekOfMonth()
{
    AFTime xTime((int)Get(YEAR), (int)Get(MONTH), 1, 0, 0, 0, 0);
    int nFirstDayOfWeek = xTime.GetDayOfWeek();
    int nDaySpan = Get(DAY) - 1;
    return (nFirstDayOfWeek + nDaySpan) / 7;
}

int AFTime::GetDayOfYear()
{
    int nDayOfYear = 0;
    int nYear = (int)Get(YEAR);
    int nMonth = (int)Get(MONTH);
    int nDay = (int)Get(DAY);
    for(int i = 0; i < nMonth - 1; ++i)
    {
        nDayOfYear += MONTH_DAY[i];

        //闰年2月是29，所以多加1
        if(IsLeapYear(nYear) && i == 2)
        {
            nDayOfYear += 1;
        }
    }

    //add this month
    nDayOfYear += nDay;

    return nDayOfYear;
}

int AFTime::GetWeekOfYear()
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
