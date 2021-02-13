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

#include "base/AFMacros.hpp"
#include "base/time/AFDateTime.hpp"
#include "utility/include/AFScheduler.hpp"
#include "kernel/include/AFCData.hpp"
#include "Sample1Module.h"

namespace ark {

bool Sample1Module::Init()
{
    m_pTimerModule = FindModule<AFITimerModule>();
    m_pGUIDModule = FindModule<AFIGUIDModule>();

    ARK_LOG_INFO("{}, init", GET_CLASS_NAME(Sample1Module));

    return true;
}

void TestBasicData()
{
    // Test basic data
    guid_t test_id = guid_t(1);

    // test GUID type
    AFCData guid_data1(ArkDataType::DT_INT64, guid_t(1));
    AFCData guid_data2(ArkDataType::DT_INT64, guid_t(0));
    guid_data1 = guid_data2;
    guid_t guid = guid_data1.GetInt64();

    // test string type
    AFCData data1(ArkDataType::DT_STRING, "test1");
    AFCData data2(ArkDataType::DT_STRING, "test2");
    data1 = data2;
    const char* str1 = data1.GetString();
}

void Sample1Module::TestCronScheduler()
{
    static AFScheduler scheduler;
    AFDateTime* now = ARK_NEW AFDateTime();

    scheduler.cron("* * * * *", [now]() {
        AFDateTime cur;
        AFDateTime::TimeDiff diff = (cur - *now);

        std::cout << diff << std::endl;
        *now = cur;
    });
}

void Sample1Module::TimerTest(uint64_t timer_id, const guid_t& entity_id)
{
    ARK_LOG_INFO("timer log, id = {} entity_id = {}", timer_id, entity_id);
}

bool Sample1Module::PostInit()
{
    ARK_LOG_INFO("{}, PostInit", GET_CLASS_NAME(Sample1Module));

    m_pTimerModule->AddSingleTimer(guid_t(100), std::chrono::seconds(10), 5, this, &Sample1Module::TimerTest);
    //TestCronScheduler();

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
