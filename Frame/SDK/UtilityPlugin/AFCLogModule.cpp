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

#include "AFCLogModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "spdlog/contrib/sinks/date_and_hour_file_sink.h"

AFCLogModule::AFCLogModule(AFIPluginManager* p)
{
    pPluginManager = p;

    //Create spdlog
    try
    {
        CreateLogger();
    }
    catch (std::system_error& error)
    {
        CONSOLE_LOG_NO_FILE << "Create logger failed, error = " << error.what() << std::endl;
        ARK_ASSERT_NO_EFFECT(0);
    }
}

bool AFCLogModule::Shut()
{
    spdlog::drop_all();
    return true;
}

const std::shared_ptr<spdlog::async_logger>& AFCLogModule::GetLogger()
{
    return mxLogger;
}

void AFCLogModule::CreateLogger()
{
    std::vector<spdlog::sink_ptr> sinks_vec;
    std::string log_name;
#if ARK_PLATFORM == PLATFORM_WIN
    log_name = ARK_FORMAT("..\\log\\{}.log", pPluginManager->AppName());
#else
    log_name = ARK_FORMAT("../log/{}.log", pPluginManager->AppName());
#endif

    auto date_and_hour_sink = std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>(log_name);
#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
#if ARK_PLATFORM == PLATFORM_WIN
    auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
    auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif

    sinks_vec.push_back(color_sink);
#endif
    sinks_vec.push_back(date_and_hour_sink);

    mxLogger = std::make_shared<spdlog::async_logger>(pPluginManager->AppName(), std::begin(sinks_vec), std::end(sinks_vec), 1024);

#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
    mxLogger->set_level(spdlog::level::level_enum::trace);
    mxLogger->set_pattern("%^[%Y%m%d %H:%M:%S.%e][%l]%v%$");
#else
    mxLogger->set_pattern("[%Y%m%d %H:%M:%S.%e][%l]%v");
#endif

    mxLogger->flush_on(spdlog::level::err);

    spdlog::register_logger(mxLogger);
}

//////////////////////////////////////////////////////////////////////////

AFCDynamicLogModule::AFCDynamicLogModule(AFIPluginManager* p)
{
    pPluginManager = p;
}

bool AFCDynamicLogModule::Shut()
{
    spdlog::drop_all();
    return true;
}

const std::shared_ptr<spdlog::async_logger>& AFCDynamicLogModule::GetLogger(const int id, const char* name)
{
    auto iter = _dynamic_loggers.find(std::make_pair(id, name));

    if (iter != _dynamic_loggers.end())
    {
        return iter->second;
    }
    else
    {
        //Create spdlog
        try
        {
            CreateLogger(id, name);
        }
        catch (std::system_error& error)
        {
            CONSOLE_LOG_NO_FILE << "Create logger failed, error = " << error.what() << std::endl;
            ARK_ASSERT_NO_EFFECT(0);
        }

        //find twice
        iter = _dynamic_loggers.find(std::make_pair(id, name));

        if (iter != _dynamic_loggers.end())
        {
            return iter->second;
        }
        else
        {
            return _null_logger;
        }
    }
}

void AFCDynamicLogModule::CreateLogger(const int id, const char* name)
{
    std::vector<spdlog::sink_ptr> sinks_vec;
    std::string log_name;
#if ARK_PLATFORM == PLATFORM_WIN
    log_name = fmt::format("..\\log\\{}\\{}.log", id, name);
#else
    log_name = fmt::format("../log/{}/{}.log", id, name);
#endif

    auto date_and_hour_sink = std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>(log_name);
#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
#if ARK_PLATFORM == PLATFORM_WIN
    auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
    auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif

    sinks_vec.push_back(color_sink);
#endif
    sinks_vec.push_back(date_and_hour_sink);

    auto dynamic_logger = std::make_shared<spdlog::async_logger>(name, std::begin(sinks_vec), std::end(sinks_vec), 1024);

#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
    dynamic_logger->set_level(spdlog::level::level_enum::trace);
    dynamic_logger->set_pattern("%^[%Y%m%d %H:%M:%S.%e][%l]%v%$");
#else
    dynamic_logger->set_pattern("[%Y%m%d %H:%M:%S.%e][%l]%v");
#endif
    dynamic_logger->flush_on(spdlog::level::err);

    spdlog::register_logger(dynamic_logger);
    _dynamic_loggers.insert(std::make_pair(std::make_pair(id, name), dynamic_logger));
}