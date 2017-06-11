#include "AFCGUIDModule.h"
#include <mutex>

namespace GUIDModule
{
    //此段参考 https://github.com/nebula-im/snowflake4cxx
    // 结构为：
    //
    // 0---0000000000 0000000000 0000000000 0000000000 0 --- 00000 ---00000 ---0000000000 00
    // 在上面的字符串中，第一位为未使用（实际上也可作为long的符号位），接下来的41位为毫秒级时间，然后5位datacenter标识位，
    // 5位机器ID（并不算标识符，实际是为线程标识），然后12位该毫秒内的当前毫秒内的计数，加起来刚好64位，为一个Long型。
    //
    // 这样的好处是，整体上按照时间自增排序，并且整个分布式系统内不会产生ID碰撞（由datacenter和机器ID作区分），
    // 并且效率较高，经测试，snowflake每秒能够产生26万ID左右，完全满足需要。
    //

#ifndef _MSC_VER
# include <sys/time.h>
#else
# include <windows.h>
# include <winsock2.h>
# include <time.h>
#endif

#ifdef _MSC_VER
    int gettimeofday(struct timeval* tp, void *tzp)
    {
        time_t clock;
        struct tm tm;
        SYSTEMTIME wtm;
        GetLocalTime(&wtm);
        tm.tm_year = wtm.wYear - 1900;
        tm.tm_mon = wtm.wMonth - 1;
        tm.tm_mday = wtm.wDay;
        tm.tm_hour = wtm.wHour;
        tm.tm_min = wtm.wMinute;
        tm.tm_sec = wtm.wSecond;
        tm.tm_isdst = -1;
        clock = mktime(&tm);
        tp->tv_sec = clock;
        tp->tv_usec = wtm.wMilliseconds * 1000;
        return (0);
    }
#endif

    uint64_t GetNowInMsec()
    {
        struct timeval tv;
        gettimeofday(&tv, 0);
        return uint64_t(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
    }

    uint64_t WaitUntilNextMillis(uint64_t last_timestamp)
    {
        uint64_t timestamp = GetNowInMsec();
        while (timestamp <= last_timestamp)
        {
            timestamp = GetNowInMsec();
        }
        return timestamp;
    }

    class IdWorkerUnThreadSafe
    {
    public:
        IdWorkerUnThreadSafe(uint16_t worker_id, uint16_t data_center_id)
            : worker_id_(worker_id)
            , data_center_id_(data_center_id)
        {}

        uint64_t GetNextID()
        {
            uint64_t timestamp = GetNowInMsec();

            // 在当前秒内
            if (last_timestamp_ == timestamp)
            {
                sequence_ = (sequence_ + 1) & 0xFFF;
                if (sequence_ == 0)
                {
                    timestamp = WaitUntilNextMillis(last_timestamp_);
                }
            }
            else
            {
                sequence_ = 0;
            }

            last_timestamp_ = timestamp;
            return ((timestamp & 0x1FFFFFF) << 22 |
                (data_center_id_ & 0x1F) << 17 |
                (worker_id_ & 0x1F) << 12 |
                (sequence_ & 0xFFF));
        }

    protected:
        uint16_t worker_id_{ 0 };
        uint16_t data_center_id_{ 0 };
        uint64_t last_timestamp_{ 0 };
        uint32_t sequence_{ 0 };
    };

    class IdWorkerThreadSafe
    {
    public:
        IdWorkerThreadSafe(uint16_t worker_id, uint16_t data_center_id)
            : id_worker_(worker_id, data_center_id)
        {}

        uint64_t GetNextID()
        {
            std::lock_guard<std::mutex> g(lock_);
            return id_worker_.GetNextID();
        }

    protected:
        IdWorkerUnThreadSafe id_worker_;
        std::mutex lock_;
    };
}

//////////////////////////////////////////////////////////////////////////

AFCGUIDModule::AFCGUIDModule(NFIPluginManager* p)
{
    pPluginManager = p;
}

bool AFCGUIDModule::Init()
{
    return true;
}

bool AFCGUIDModule::AfterInit()
{
    return true;
}

bool AFCGUIDModule::Execute()
{
    return true;
}

bool AFCGUIDModule::BeforeShut()
{
    if (NULL != m_pIDWoker)
    {
        delete m_pIDWoker;
        m_pIDWoker = NULL;
    }

    return true;
}

bool AFCGUIDModule::Shut()
{
    return true;
}

void AFCGUIDModule::SetWorkerAndDatacenter(uint16_t worker_id, uint16_t data_center_id)
{
#ifdef AF_THREAD_SAFE
    m_pIDWoker = NF_NEW GUIDModule::IdWorkerThreadSafe(worker_id, data_center_id);
#else
    m_pIDWoker = NF_NEW GUIDModule::IdWorkerUnThreadSafe(worker_id, data_center_id);
#endif
}

uint64_t AFCGUIDModule::CreateGUID()
{
    assert(NULL != m_pIDWoker);

    return m_pIDWoker->GetNextID();
}