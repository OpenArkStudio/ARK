/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "base/AFMacros.hpp"
#include "base/AFDateTime.hpp"
#include "base/AFRandom.hpp"
#include "base/AFTimer.hpp"
#include "base/AFCRC.hpp"
#include "base/AFScheduler.hpp"
#include "kernel/include/AFCData.hpp"

#include "proto/cpp/AFOss.pb.h"
#include "Sample1Module.h"

namespace ark {

bool Sample1Module::Init()
{
    m_pTimerModule = FindModule<AFITimerModule>();
    m_pLogModule = FindModule<AFILogModule>();
    m_pGUIDModule = FindModule<AFIGUIDModule>();

    ARK_LOG_INFO("{}, init", GET_CLASS_NAME(Sample1Module));

    return true;
}

void TestBasicData()
{
    // Test basic data
    AFGUID test_id = AFGUID(1);

    // test GUID type
    AFCData guid_data1(ArkDataType::DT_INT64, AFGUID(1));
    AFCData guid_data2(ArkDataType::DT_INT64, AFGUID(0));
    guid_data1 = guid_data2;
    AFGUID guid = guid_data1.GetInt64();

    // test string type
    AFCData data1(ArkDataType::DT_STRING, "test1");
    AFCData data2(ArkDataType::DT_STRING, "test2");
    data1 = data2;
    const char* str1 = data1.GetString();
}

void TestDateTime()
{
    AFDateTime now;
    std::cout << now.ToString() << std::endl;
    std::cout << now.GetYear() << "/" << now.GetMonth() << "/" << now.GetDay() << " " << now.GetHour() << ":"
              << now.GetMinute() << ":" << now.GetSecond() << std::endl;
    std::cout << now.GetTime() << std::endl;

    AFDateTime now2(now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), now.GetSecond());
    ARK_ASSERT_NO_EFFECT(now.GetTime() == now2.GetTime());
}

void TestRandom()
{
    AFRandom rand;
    rand.SetSeed(AFDateTime::GetTimestamp());

    int rand_array[10] = {0};

    for (int i = 0; i < 100000; ++i)
    {
        uint32_t my_rand = rand.Random(0, 100000);

        if (my_rand <= 10000)
        {
            rand_array[0]++;
        }
        else if (my_rand > 10000 && my_rand <= 20000)
        {
            rand_array[1]++;
        }
        else if (my_rand > 20000 && my_rand <= 30000)
        {
            rand_array[2]++;
        }
        else if (my_rand > 30000 && my_rand <= 40000)
        {
            rand_array[3]++;
        }
        else if (my_rand > 40000 && my_rand <= 50000)
        {
            rand_array[4]++;
        }
        else if (my_rand > 50000 && my_rand <= 60000)
        {
            rand_array[5]++;
        }
        else if (my_rand > 60000 && my_rand <= 70000)
        {
            rand_array[6]++;
        }
        else if (my_rand > 70000 && my_rand <= 80000)
        {
            rand_array[7]++;
        }
        else if (my_rand > 80000 && my_rand <= 90000)
        {
            rand_array[8]++;
        }
        else
        {
            rand_array[9]++;
        }
    }

    for (size_t i = 0; i < ARRAY_LENTGH(rand_array); ++i)
    {
        std::cout << rand_array[i] << " int range [" << i * 10000 << ", " << (i + 1) * 10000 << "]" << std::endl;
    }
}

void TestCRC()
{
    std::string s = "hello";
    uint16_t crc16 = AFCRC::crc16(s);
    auto log = ARK_FORMAT("CRC16 [{}] -> [{}]", s, crc16);
    std::cout << log << std::endl;

    s = "world";
    uint32_t crc32 = AFCRC::crc32(s);
    log = ARK_FORMAT("CRC32 [{}] -> [{}]", s, crc32);
    std::cout << log << std::endl;
}

void Sample1Module::TestOssLog()
{
    auto func = [&]() {
        osslog::create_account msg;
        msg.set_cid(1);
        msg.set_sid(1);
        msg.set_account("xxxxx");
        AFDateTime time(GetPluginManager()->GetNowTime());
        msg.set_time(static_cast<int>(time.GetTime()));
        msg.set_ms(static_cast<double>(time.Raw()));
        msg.set_ts(time.ToISO8601String());
        msg.set_ip("127.0.0.1");
        msg.set_adv("aaa");
        msg.set_ad("bbb");
        msg.set_guest(0);
        msg.set_device("iPhone");

        ARK_OSS_LOG(msg);
    };

    for (int i = 0; i < 100000; ++i)
    {
        func();
    }
}

void Sample1Module::TestCronScheduler()
{
    static AFScheduler scheduler;
    AFDateTime* now = ARK_NEW AFDateTime();

    scheduler.cron("* * * * *", [now]() {
        AFDateTime cur;
        AFDateTime::TimeDiff diff = (cur - *now);

        std::cout << diff << std::endl;
        //if ((diff / AFDateTime::Resolution()) == 60)
        //{
        //    std::cout << "test OK" << std::endl;
        //}
        //else
        //{
        //    std::cout << "test failed" << std::endl;
        //}

        *now = cur;
    });
}

bool Sample1Module::PostInit()
{
    ARK_LOG_INFO("{}, PostInit", GET_CLASS_NAME(Sample1Module));

    TestCronScheduler();

    return true;
}

bool Sample1Module::Update()
{
    return true;
}

bool Sample1Module::PreShut()
{
    ARK_LOG_INFO("{}, PreShut", GET_CLASS_NAME(Sample1Module));
    return true;
}

bool Sample1Module::Shut()
{
    ARK_LOG_INFO("{}, Shut", GET_CLASS_NAME(Sample1Module));
    return true;
}

} // namespace ark
