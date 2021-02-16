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

#pragma once

#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <spdlog/async_logger.h>
#include <spdlog/async.h>
#include <spdlog/fmt/fmt.h>

// custom sinks
#include "spdlog/sinks/date_and_hour_file_sink.h"
//#include "spdlog/sinks/rotating_file_with_date_sink.h"

#include "base/AFMacros.hpp"
#include "base/container/AFMap.hpp"
#include "base/AFXml.hpp"

#ifndef ARK_PLATFORM_WIN
#include <execinfo.h>
#endif

namespace ark {

// Get the basename of __FILE__ (at compile time if possible)
#if FMT_HAS_FEATURE(__builtin_strrchr)
#define SPDLOG_STRRCHR(str, sep) __builtin_strrchr(str, sep)
#else
#define SPDLOG_STRRCHR(str, sep) strrchr(str, sep)
#endif //__builtin_strrchr not defined

#ifndef SPDLOG_FILE_BASENAME
#ifdef PLATFORM_WIN
#define SPDLOG_FILE_BASENAME(file) SPDLOG_STRRCHR("\\" file, '\\') + 1
#else
#define SPDLOG_FILE_BASENAME(file) SPDLOG_STRRCHR("/" file, '/') + 1
#endif
#endif // SPDLOG_FILE_BASENAME

//SPDLOG_TRACE();

#define ARK_LOG_TRACE(my_fmt, ...)                                                                                     \
    AFLogger::instance()->LogT(                                                                                        \
        spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""}, spdlog::level::trace, my_fmt, ##__VA_ARGS__)
#define ARK_LOG_DEBUG(my_fmt, ...)                                                                                     \
    AFLogger::instance()->LogT(                                                                                        \
        spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""}, spdlog::level::debug, my_fmt, ##__VA_ARGS__)
#define ARK_LOG_INFO(my_fmt, ...)                                                                                      \
    AFLogger::instance()->LogT(                                                                                        \
        spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""}, spdlog::level::info, my_fmt, ##__VA_ARGS__)
#define ARK_LOG_WARN(my_fmt, ...)                                                                                      \
    AFLogger::instance()->LogT(                                                                                        \
        spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""}, spdlog::level::warn, my_fmt, ##__VA_ARGS__)
#define ARK_LOG_ERROR(my_fmt, ...)                                                                                     \
    AFLogger::instance()->LogT(                                                                                        \
        spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""}, spdlog::level::err, my_fmt, ##__VA_ARGS__)
#define ARK_LOG_CRITICAL(my_fmt, ...)                                                                                  \
    AFLogger::instance()->LogT(spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""},                       \
        spdlog::level::critical, my_fmt, ##__VA_ARGS__);

#define ARK_LOG_OSS(name, content) AFLogger::instance()->LogOSS(spdlog::level::info, name, content)

enum class kLogSinkType : uint8_t
{
    Date_and_hour_sink = 0, // default, rotate by hour with specific date path
    Size_step_sink,         // rotate by file size, unused for now
    // others
};

class AFLogConfData final
{
public:
    std::string output_path_{"./binlog"};
    kLogSinkType sink_type_{kLogSinkType::Date_and_hour_sink};
    uint16_t sink_queue_count_{1024};

    bool consol_{true};
    spdlog::level::level_enum level_{spdlog::level::trace};
    std::string pattern_{};
};

class AFLogConf final
{
    AFSmartPtrHashmap<std::string, AFLogConfData> conf_datas_;
    std::string default_log_name_{"default"};

public:
    void LoadLogConf(std::string const& filename)
    {
        AFXml xml(filename);
        auto root_node = xml.GetRootNode();

        for (auto logger_node = root_node.FindNode("logger"); logger_node.IsValid(); logger_node.NextNode())
        {
            // create AFLogConfData
            auto conf_data = std::make_shared<AFLogConfData>();
            auto name = logger_node.GetString("name");
            conf_data->sink_queue_count_ = logger_node.GetUint32("queue_count");
            conf_data->output_path_ = logger_node.GetString("path");

#ifdef ARK_RUN_MODE_DEBUG
            auto mode_node = logger_node.FindNode("debug");
#else
            auto mode_node = logger_node.FindNode("release");
#endif //ARK_RUN_MODE_DEBUG

            conf_data->consol_ = mode_node.GetBool("console");
            conf_data->level_ = static_cast<spdlog::level::level_enum>(mode_node.GetUint32("level"));
            conf_data->pattern_ = mode_node.GetString("pattern");

            conf_datas_.insert(name, conf_data);
        }
    }

    std::shared_ptr<AFLogConfData> GetConfData(std::string const& name)
    {
        return conf_datas_.find_value(name);
    }

    std::string const& GetDefault() const
    {
        return default_log_name_;
    }
};

class AFSpdlogger final
{
    std::shared_ptr<AFLogConfData> log_conf_{nullptr};
    std::string log_name_{};

    std::shared_ptr<spdlog::logger> logger_{nullptr};
    std::shared_ptr<spdlog::details::thread_pool> tp_{nullptr};

public:
    explicit AFSpdlogger(std::shared_ptr<AFLogConfData> conf)
        : log_conf_(conf)
    {
    }

    ~AFSpdlogger()
    {
        spdlog::drop(log_name_);
    }

    void Init(std::string const& filename)
    {
        log_name_ = ARK_FORMAT("{}{}{}", log_conf_->output_path_, spdlog::details::os::folder_sep, filename);
        std::vector<spdlog::sink_ptr> sinks_vec;

#ifdef OPEN_FILE_LOG
        switch (log_conf_->sink_type_)
        {
        case kLogSinkType::Date_and_hour_sink:
        {
            auto date_and_hour_sink = std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>(log_name_);
            sinks_vec.emplace_back(date_and_hour_sink);
        }
        break;
        default:
            ARK_ASSERT_BREAK(0);
            break;
        }
#endif

        if (log_conf_->consol_)
        {
#ifdef ARK_PLATFORM_WIN
            auto color_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#else
            auto color_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#endif // ARK_PLATFORM_WIN
            sinks_vec.push_back(color_sink);
        }

        if (log_conf_->sink_queue_count_ > 0)
        {
            tp_ = std::make_shared<spdlog::details::thread_pool>(log_conf_->sink_queue_count_, 1);
            logger_ =
                std::make_shared<spdlog::async_logger>(log_name_, std::begin(sinks_vec), std::end(sinks_vec), tp_);
        }
        else
        {
            logger_ = std::make_shared<spdlog::logger>(log_name_, std::begin(sinks_vec), std::end(sinks_vec));
        }

        logger_->set_pattern(log_conf_->pattern_);
        logger_->set_level(log_conf_->level_);
        logger_->flush_on(log_conf_->level_);

        spdlog::register_logger(logger_);
    }

    void Log(spdlog::level::level_enum level, std::string const& content)
    {
        logger_->log(level, content);
    }
};

using LOG_FUNCTOR = std::function<void(spdlog::level::level_enum, std::string const&)>;

class AFLogger final : public AFSingleton<AFLogger>
{
    using logger_t = std::shared_ptr<AFSpdlogger>;

    // singleton
    //inline static AFLogger* singleton_logger_{nullptr};

    // default logger
    logger_t default_logger_{nullptr};

    // oss logger
    logger_t oss_logger_{nullptr};

    // created logger list
    std::unordered_map<bus_id_t, logger_t> loggers_;

    // log function
    LOG_FUNCTOR log_func_;

    // log configuration
    std::unique_ptr<AFLogConf> log_conf_{nullptr};

public:
    ~AFLogger()
    {
        loggers_.clear();
        //ARK_DELETE(AFLogger::singleton_logger_);
    }

    static void Init(AFLogger* logger)
    {
        if (logger == nullptr)
        {
            logger = ARK_NEW AFLogger();
        }

        // init singleton
        AFSingleton<AFLogger>::SetInstance(logger);
    }

    bool InitLogger(std::string const& conf_file, std::string const& log_filename)
    {
        if (log_conf_ == nullptr)
        {
            log_conf_ = std::make_unique<AFLogConf>();
            log_conf_->LoadLogConf(conf_file);
        }

        // init local logger
        auto conf_data = log_conf_->GetConfData(log_conf_->GetDefault());
        ARK_ASSERT_RET_VAL(conf_data != nullptr, false);

        default_logger_ = std::make_shared<AFSpdlogger>(conf_data);
        default_logger_->Init(log_filename);

        // set log function for local logger
        auto func = std::bind(&AFLogger::Log, this, std::placeholders::_1, std::placeholders::_2);
        SetLogFunc(std::move(func));

        // init oss logger
        auto oss_conf_data = log_conf_->GetConfData("oss");
        ARK_ASSERT_RET_VAL(oss_conf_data != nullptr, false);

        oss_logger_ = std::make_shared<AFSpdlogger>(oss_conf_data);
        oss_logger_->Init(log_filename);

        return true;
    }

    logger_t CreateLogger(bus_id_t id, std::string const& name, std::string const& filename)
    {
        auto iter = loggers_.find(id);
        if (iter != loggers_.end())
        {
            return iter->second;
        }

        // find log configuration
        auto log_conf_data = log_conf_->GetConfData(name);
        if (log_conf_data == nullptr)
        {
            return nullptr;
        }

        // create logger
        auto new_logger = std::make_shared<AFSpdlogger>(log_conf_data);
        new_logger->Init(filename);
        loggers_.insert(std::make_pair(id, new_logger));
        return new_logger;
    }

    void RemoveLogger(bus_id_t id)
    {
        auto iter = loggers_.find(id);
        if (iter == loggers_.end())
        {
            return;
        }

        loggers_.erase(iter);
    }

    void SetLogFunc(LOG_FUNCTOR&& func)
    {
        if (func != nullptr)
        {
            log_func_ = func;
        }
        else
        {
            auto func = std::bind(&AFLogger::Log, this, std::placeholders::_1, std::placeholders::_2);
            log_func_ = func;
        }
    }

    template<typename... ARGS>
    void LogT(spdlog::source_loc&& loc, spdlog::level::level_enum log_level, const char* my_fmt, ARGS&&... args)
    {
        static const std::string func_line_fmt = "[{}:{}]";
        auto new_fmt = func_line_fmt + my_fmt;
        auto content = ARK_FORMAT(new_fmt, loc.filename, loc.line, std::forward<ARGS>(args)...);
        log_func_(log_level, content);
    }

    void LogOSS(spdlog::level::level_enum log_level, const std::string& name, const std::string& content)
    {
        static const std::string oss_fmt = "{}:{}";
        auto final_content = ARK_FORMAT(oss_fmt, name, content);
        oss_logger_->Log(log_level, final_content);
    }

    void LogStackTrace(std::string const& error)
    {
#ifndef ARK_PLATFORM_WIN
        Log(spdlog::level::err, "StackTrace Begin =============================");
        Log(spdlog::level::err, error);

        static const uint16_t default_size = 256;
        void* default_data[default_size] = {0};
        auto stack_num = backtrace(default_data, default_size);
        auto stack_trace = backtrace_symbols(default_data, stack_num);
        for (auto i = 0; i < stack_num; i++)
        {
            Log(spdlog::level::err, stack_trace[i]);
        }
        free(stack_trace);

        Log(spdlog::level::err, "StackTrace End ===============================");
#endif
    }

protected:
    void Log(spdlog::level::level_enum log_level, std::string const content)
    {
        default_logger_->Log(log_level, content);
    }
};

} // namespace ark