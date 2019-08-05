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
#include "base/AFPluginManager.hpp"
#include "log/include/AFCLogModule.hpp"

namespace ark {

bool AFCLogModule::Shut()
{
    spdlog::drop_all();
    return true;
}

std::shared_ptr<spdlog::async_logger> AFCLogModule::GetDevLogger()
{
    if (dev_logger_ == nullptr)
    {
        try
        {
            CreateDevLogger();
        }
        catch (std::system_error& error)
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
    std::string log_name = ARK_FORMAT("{}{}{}.{}.log", GetPluginManager()->GetLogPath(),
        spdlog::details::os::folder_sep, GetPluginManager()->AppName(), AFMisc::Bus2Str(GetPluginManager()->BusID()));
    auto date_and_hour_sink = std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>(log_name);
#ifdef ARK_RUN_MODE_DEBUG
#ifdef ARK_PLATFORM_WIN
    auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
    auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif
    sinks_vec.push_back(color_sink);
#endif
    sinks_vec.push_back(date_and_hour_sink);

    dev_tp_ = std::make_shared<spdlog::details::thread_pool>(spdlog::details::default_async_q_size, 1);
    dev_logger_ = std::make_shared<spdlog::async_logger>(
        GetPluginManager()->AppName(), std::begin(sinks_vec), std::end(sinks_vec), dev_tp_);

#ifdef ARK_RUN_MODE_DEBUG
    dev_logger_->set_level(spdlog::level::level_enum::trace);
    dev_logger_->set_pattern("%^[%Y%m%d %H:%M:%S.%e][%-8l][%@]%v%$");
#else
    dev_logger_->set_pattern("[%Y%m%d %H:%M:%S.%e][%-8l][%@]%v");
#endif
    // flush when level >= trace
    dev_logger_->flush_on(spdlog::level::trace);

    spdlog::register_logger(dev_logger_);
}

//////////////////////////////////////////////////////////////////////////

AFCLogModule::spdlogger AFCLogModule::CreateOssLogger(const std::string& name)
{
    if (name.empty())
    {
        return nullptr;
    }

    try
    {
        // log/oss/login.log
        std::vector<spdlog::sink_ptr> sinks_vec;
        std::string log_name = ARK_FORMAT("{}{}oss{}{}.log", GetPluginManager()->GetLogPath(),
            spdlog::details::os::folder_sep, spdlog::details::os::folder_sep, name);
        auto rorate_file_sink =
            std::make_shared<spdlog::sinks::rotating_file_with_date_sink_mt>(log_name, 20 * 1024 * 1024, 100000);
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
    catch (std::system_error& error)
    {
        CONSOLE_INFO_LOG << "Create OSS logger failed, error = " << error.what() << std::endl;
    }

    ARK_ASSERT_RET_VAL(0, nullptr);
}

void AFCLogModule::OssLog(const google::protobuf::Message& msg)
{
    const std::string& logger_name = msg.GetDescriptor()->name();
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

} // namespace ark
