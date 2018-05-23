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
#define SPDLOG_NO_NAME
#define SPDLOG_DEBUG_ON
#define SPDLOG_TRACE_ON

#include "Example1Module.h"
#include "SDK/Core/Base/AFTimer.hpp"
#include "SDK/Core/Base/AFMacros.hpp"
#include "SDK/Core/Common/cronexpr.h"
#include "spdlog/spdlog.h"
#include "spdlog/contrib/sinks/step_file_sink.h"

bool Example1Module::Init()
{
    std::cout << typeid(Example1Module).name() << ", Init" << std::endl;

    return true;
}

void Example1Module::test_log()
{
    //open async mode
    spdlog::set_async_mode(1000000);
//    std::vector<spdlog::sink_ptr> sinks_vec;
//    //auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(pPluginManager->AppName() + ".log", 209715200, 1000);
//    auto step_sink = std::make_shared<spdlog::sinks::step_file_sink_mt>(pPluginManager->AppName(), 60 * 60, ".log", 209715200);
////#if ARK_PLATFORM == PLATFORM_WIN
////    auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
////#else
////    auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
////#endif
//
//    //sinks_vec.push_back(rotating_sink);
//    sinks_vec.push_back(step_sink);
//    //sinks_vec.push_back(color_sink);
//
//    auto combined_logger = std::make_shared<spdlog::logger>("1", std::begin(sinks_vec), std::end(sinks_vec));
//    combined_logger->set_level(spdlog::level::level_enum::trace);
//    combined_logger->set_pattern("%^[%Y%m%d %H:%M:%S.%e][pid:%P][tid:%t][%l] %v%$");
//    spdlog::register_logger(combined_logger);
//
//
//    combined_logger->trace("spdlog start time = {0}", AFCTimeBase::GetInstance().GetNowMillisecond());
//    for (int i = 0; i < 100000; ++i)
//    {
//        combined_logger->trace("{0} {1} {2} trace", ARK_FILE_FUNCTION_LINE);
//        //combined_logger->debug("debug");
//        //combined_logger->info("info");
//        //combined_logger->warn("warn");
//        //combined_logger->error("error");
//        //combined_logger->critical("critical");
//    }
//    combined_logger->trace("spdlog start time = {0}", AFCTimeBase::GetInstance().GetNowMillisecond());

    auto logger = spdlog::create<spdlog::sinks::simple_file_sink_st>("file_logger", "log/spdlog-bench.log", false);
    logger->set_pattern("[%Y-%m-%d %T.%F]: %L %v");

    logger->info("spdlog sync start time = {0}", AFCTimeBase::GetInstance().GetNowMillisecond());
    for (int i = 0; i < 1000000; ++i)
        logger->info("spdlog message #{} : This is some text for your pleasure", i);
    logger->info("spdlog sync start time = {0}", AFCTimeBase::GetInstance().GetNowMillisecond());

    spdlog::drop_all();
}

bool Example1Module::PostInit()
{
    m_pTimerModule = pPluginManager->FindModule<AFITimerModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pScheduleModule = pPluginManager->FindModule<AFIScheduleModule>();

    ARK_ASSERT_RET_VAL(m_pTimerModule != nullptr, false);
    ARK_ASSERT_RET_VAL(m_pLogModule != nullptr, false);
    ARK_ASSERT_RET_VAL(m_pScheduleModule != nullptr, false);

    AFGUID test_id = AFGUID(0, 1);
    ARK_LOG_INFO("Test log, id = %s", test_id.ToString().c_str());

    std::cout << typeid(Example1Module).name() << ", PostInit" << std::endl;

    AFCData data1(DT_STRING, "test1");
    AFCData data2(DT_STRING, "test2");

    test_log();

    data1 = data2;
    const char* str1 = data1.GetString();

    //////////////////////////////////////////////////////////////////////////
    //test cron expression
    //const char* err_msg = NULL;
    //cron_expr* test_cron = cron_parse_expr("0 0 0 * * *", &err_msg);
    //time_t now;
    //time(&now);
    //std::cout << "Current time is " << now << std::endl;
    //time_t next = cron_next(test_cron, (time_t)now);
    //std::cout << "Next cron time is " << next << std::endl;
    bool result = m_pScheduleModule->AddSchedule(1, 0, "0 * * * * *", this, &Example1Module::TestSchduler);
    if (!result)
    {
        std::cout << "add schedule failed" << std::endl;
    }
    //////////////////////////////////////////////////////////////////////////

    //std::cout << pPluginManager->GetNowTime() << std::endl;

    //m_pTimerModule->AddSingleTimer("test", test_id, 10 * 1000/*ms*/, 2, this, &Example1Module::TestTimer);

    return true;
}

void Example1Module::Update()
{

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