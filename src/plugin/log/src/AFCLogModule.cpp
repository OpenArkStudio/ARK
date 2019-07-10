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

#include "spdlog/sinks/date_and_hour_file_sink.h"
#include "spdlog/sinks/rotating_file_with_date_sink.h"
#include "base/AFMacros.hpp"
#include "base/AFMisc.hpp"
#include "interface/AFIPluginManager.h"
#include "log/include/AFCLogModule.h"

namespace ark {

bool AFCLogModule::Shut()
{
    spdlog::drop_all();
    return true;
}

const std::shared_ptr<spdlog::async_logger> &AFCLogModule::GetDevLogger()
{
    if (dev_logger_ == nullptr)
    {
        try
        {
            CreateDevLogger();
        }
        catch (std::system_error &error)
        {
            CONSOLE_INFO_LOG << "Create logger failed, error = " << error.what() << std::endl;
            ARK_ASSERT_RET_VAL(0, nullptr);
        }
    }

    return dev_logger_;
}

void AFCLogModule::CreateDevLogger()
{
    std::vector<spdlog::sink_ptr> sinks_vec;
    std::string log_name = ARK_FORMAT("{}{}{}.{}.log", pPluginManager->GetLogPath(), spdlog::details::os::folder_sep,
        pPluginManager->AppName(), AFMisc::Bus2Str(pPluginManager->BusID()));
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

    dev_tp_ = std::make_shared<spdlog::details::thread_pool>(spdlog::details::default_async_q_size, 1);
    dev_logger_ = std::make_shared<spdlog::async_logger>(pPluginManager->AppName(), std::begin(sinks_vec), std::end(sinks_vec), dev_tp_);

#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
    dev_logger_->set_level(spdlog::level::level_enum::trace);
    dev_logger_->set_pattern("%^[%Y%m%d %H:%M:%S.%e][%-8l][%@]%v%$");
#else
    logger_->set_pattern("[%Y%m%d %H:%M:%S.%e][%-8l][%@]%v");
#endif
    // flush when level >= trace
    dev_logger_->flush_on(spdlog::level::trace);

    spdlog::register_logger(dev_logger_);
}

//////////////////////////////////////////////////////////////////////////

AFCLogModule::spdlogger AFCLogModule::CreateOssLogger(const std::string &name)
{
    if (name.empty())
    {
        return nullptr;
    }

    try
    {
        // log/oss/login.log
        std::vector<spdlog::sink_ptr> sinks_vec;
        std::string log_name = ARK_FORMAT(
            "{}{}oss{}{}.log", pPluginManager->GetLogPath(), spdlog::details::os::folder_sep, spdlog::details::os::folder_sep, name);
        auto rorate_file_sink = std::make_shared<spdlog::sinks::rotating_file_with_date_sink_mt>(log_name, 20 * 1024 * 1024, 100000);
        sinks_vec.push_back(rorate_file_sink);

        if (oss_tp_ == nullptr)
        {
            oss_tp_ = std::make_shared<spdlog::details::thread_pool>(spdlog::details::default_async_q_size, 1);
        }

        auto logger = std::make_shared<spdlog::async_logger>(name, std::begin(sinks_vec), std::end(sinks_vec), oss_tp_);
        logger->set_pattern("%v");
        // flush when level >= info
        logger->flush_on(spdlog::level::info);

        spdlog::register_logger(logger);
        oss_loggers_.insert(std::make_pair(name, logger));
        return logger;
    }
    catch (std::system_error &error)
    {
        CONSOLE_INFO_LOG << "Create OSS logger failed, error = " << error.what() << std::endl;
        ARK_ASSERT_RET_VAL(0, nullptr);
    }
}

void AFCLogModule::OssLog(const google::protobuf::Message &msg)
{
    const std::string &logger_name = msg.GetDescriptor()->name();
    auto iter = oss_loggers_.find(logger_name);
    spdlogger logger;
    if (iter == oss_loggers_.end())
    {
        logger = CreateOssLogger(logger_name);
    }
    else
    {
        logger = iter->second;
    }

    google::protobuf::util::JsonOptions option;
    option.add_whitespace = false;
    option.always_print_primitive_fields = true;
    option.always_print_enums_as_ints = true;
    option.preserve_proto_field_names = true;

    std::string pb_json;
    google::protobuf::util::MessageToJsonString(msg, &pb_json, option);
    logger->log(spdlog::level::info, pb_json);
}
//////////////////////////////////////////////////////////////////////////
// bool AFCDynamicLogModule::Shut()
//{
//    spdlog::drop_all();
//    return true;
//}
//
// const std::shared_ptr<spdlog::async_logger> &AFCDynamicLogModule::GetLogger(const int id, const char *name)
//{
//    auto iter = _dynamic_loggers.find(std::make_pair(id, name));
//
//    if (iter != _dynamic_loggers.end())
//    {
//        return iter->second;
//    }
//    else
//    {
//         Create spdlog
//        try
//        {
//            CreateLogger(id, name);
//        }
//        catch (std::system_error &error)
//        {
//            CONSOLE_INFO_LOG << "Create logger failed, error = " << error.what() << std::endl;
//            ARK_ASSERT_NO_EFFECT(0);
//        }
//
//         find twice
//        iter = _dynamic_loggers.find(std::make_pair(id, name));
//
//        if (iter != _dynamic_loggers.end())
//        {
//            return iter->second;
//        }
//        else
//        {
//            return _null_logger;
//        }
//    }
//}
//
// void AFCDynamicLogModule::CreateLogger(const int id, const char *name)
//{
//    std::vector<spdlog::sink_ptr> sinks_vec;
//    std::string log_name;
//#if ARK_PLATFORM == PLATFORM_WIN
//    log_name = fmt::format("..\\binlog\\{}\\{}.log", id, name);
//#else
//    log_name = fmt::format("../binlog/{}/{}.log", id, name);
//#endif
//
//    auto date_and_hour_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_name, 0, 0);
//#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
//#if ARK_PLATFORM == PLATFORM_WIN
//    auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
//#else
//    auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
//#endif
//    sinks_vec.push_back(color_sink);
//#endif
//    sinks_vec.push_back(date_and_hour_sink);
//
//    tp_ = std::make_shared<spdlog::details::thread_pool>(1024, 1);
//    auto dynamic_logger = std::make_shared<spdlog::async_logger>(name, std::begin(sinks_vec), std::end(sinks_vec), tp_);
//
//#if ARK_RUN_MODE == ARK_RUN_MODE_DEBUG
//    dynamic_logger->set_level(spdlog::level::level_enum::trace);
//    dynamic_logger->set_pattern("%^[%Y%m%d %H:%M:%S.%e][%l]%v%$");
//#else
//    dynamic_logger->set_pattern("[%Y%m%d %H:%M:%S.%e][%l]%v");
//#endif
//    dynamic_logger->flush_on(spdlog::level::err);
//
//    spdlog::register_logger(dynamic_logger);
//    _dynamic_loggers.insert(std::make_pair(std::make_pair(id, name), dynamic_logger));
//}

} // namespace ark
