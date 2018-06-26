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

#include "Example1Module.h"
#include "SDK/Core/Base/AFTimer.hpp"
#include "SDK/Core/Base/AFMacros.hpp"
#include "SDK/Core/Common/cronexpr.h"
#include "SDK/Core/Base/AFDateTime.hpp"
#include "SDK/Core/Base/AFRandom.hpp"

bool Example1Module::Init()
{
    std::cout << typeid(Example1Module).name() << ", Init" << std::endl;

    return true;
}

void TestDateTime()
{
    AFDateTime now;
    std::cout << now.ToString() << std::endl;
    std::cout << now.GetYear() << "/" << now.GetMonth() << "/" << now.GetDay() << " " << now.GetHour() << ":" << now.GetMinute() << ":" << now.GetSecond() << std::endl;
    std::cout << now.GetTime() << std::endl;

    AFDateTime now2(now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), now.GetSecond());
    ARK_ASSERT_NO_EFFECT(now.GetTime() == now2.GetTime());
}

void TestRandom()
{
    AFRandom rand;
    rand.SetSeed(AFDateTime::GetTimestamp());

    int rand_array[10] = { 0 };
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

    for (int i = 0; i < ARRAY_LENTGH(rand_array); ++i)
    {
        std::cout << rand_array[i] << " int range [" << i * 10000 << ", " << (i + 1) * 10000 << "]" << std::endl;
    }
}

bool Example1Module::PostInit()
{
    std::cout << typeid(Example1Module).name() << ", PostInit" << std::endl;

    m_pTimerModule = pPluginManager->FindModule<AFITimerModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pScheduleModule = pPluginManager->FindModule<AFIScheduleModule>();

    ARK_ASSERT_RET_VAL(m_pTimerModule != nullptr, false);
    ARK_ASSERT_RET_VAL(m_pLogModule != nullptr, false);
    ARK_ASSERT_RET_VAL(m_pScheduleModule != nullptr, false);

    AFGUID test_id = AFGUID(0, 1);

    //test GUID type
    AFCData guid_data1(DT_OBJECT, AFGUID(0, 1));
    AFCData guid_data2(DT_OBJECT, AFGUID(1, 0));
    guid_data1 = guid_data2;
    AFGUID guid = guid_data1.GetObject();


    //test string type
    AFCData data1(DT_STRING, "test1");
    AFCData data2(DT_STRING, "test2");
    data1 = data2;
    const char* str1 = data1.GetString();

    //////////////////////////////////////////////////////////////////////////
    //Test AFDateTime
    TestDateTime();
    //////////////////////////////////////////////////////////////////////////
    //Test Random
    TestRandom();
    //////////////////////////////////////////////////////////////////////////
    //test cron expression
    //const char* err_msg = NULL;
    //cron_expr* test_cron = cron_parse_expr("0 0 0 * * *", &err_msg);
    //time_t now;
    //time(&now);
    //std::cout << "Current time is " << now << std::endl;
    //time_t next = cron_next(test_cron, (time_t)now);
    //std::cout << "Next cron time is " << next << std::endl;
    //bool result = m_pScheduleModule->AddSchedule(1, 0, "0 * * * * *", this, &Example1Module::TestSchduler);
    //if (!result)
    //{
    //    std::cout << "add schedule failed" << std::endl;
    //}
    //////////////////////////////////////////////////////////////////////////

    //std::cout << pPluginManager->GetNowTime() << std::endl;

    //m_pTimerModule->AddSingleTimer("test", test_id, 10 * 1000/*ms*/, 2, this, &Example1Module::TestTimer);

    return true;
}

bool Example1Module::PreShut()
{
    std::cout << typeid(Example1Module).name() << ", PreShut" << std::endl;
    return true;
}

bool Example1Module::Shut()
{
    std::cout << typeid(Example1Module).name() << ", Shut" << std::endl;
    return true;
}

void Example1Module::TestTimer(const std::string& name, const AFGUID& entity_id)
{
    std::cout << pPluginManager->GetNowTime() << std::endl;
    std::cout << "Test Timer: " << name << " id = " << entity_id.ToString() << std::endl;
}

bool Example1Module::TestSchduler(const int id, const int arg)
{
    std::cout << pPluginManager->GetNowTime() << std::endl;
    std::cout << "Test Scheduler: id = " << id << ", arg = " << arg << std::endl;
    return true;
}