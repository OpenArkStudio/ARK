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

#pragma once

#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <spdlog/async_logger.h>
#include <spdlog/async.h>
#include <spdlog/fmt/fmt.h>
#include "interface/AFIModule.hpp"
#include "proto/AFProtoCPP.hpp"

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

namespace ark {

#define ARK_LOG_TRACE(my_fmt, ...)                                                                                     \
    m_pLogModule->Log(spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""}, spdlog::level::trace, my_fmt,  \
        ##__VA_ARGS__);
#define ARK_LOG_DEBUG(my_fmt, ...)                                                                                     \
    m_pLogModule->Log(spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""}, spdlog::level::debug, my_fmt,  \
        ##__VA_ARGS__);
#define ARK_LOG_INFO(my_fmt, ...)                                                                                      \
    m_pLogModule->Log(                                                                                                 \
        spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""}, spdlog::level::info, my_fmt, ##__VA_ARGS__);
#define ARK_LOG_WARN(my_fmt, ...)                                                                                      \
    m_pLogModule->Log(                                                                                                 \
        spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""}, spdlog::level::warn, my_fmt, ##__VA_ARGS__);
#define ARK_LOG_ERROR(my_fmt, ...)                                                                                     \
    m_pLogModule->Log(                                                                                                 \
        spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""}, spdlog::level::err, my_fmt, ##__VA_ARGS__);
#define ARK_LOG_CRITICAL(my_fmt, ...)                                                                                  \
    m_pLogModule->Log(spdlog::source_loc{SPDLOG_FILE_BASENAME(__FILE__), __LINE__, ""}, spdlog::level::critical,       \
        my_fmt, ##__VA_ARGS__);

#define ARK_OSS_LOG(msg) m_pLogModule->OssLog(msg);

class AFILogModule : public AFIModule
{
public:
    template<typename... ARGS>
    void Log(spdlog::source_loc&& loc, spdlog::level::level_enum log_level, const char* my_fmt, const ARGS&... args)
    {
        const std::shared_ptr<spdlog::async_logger>& logger = GetDevLogger();

        if (logger == nullptr)
        {
            return;
        }

        logger->log(loc, log_level, my_fmt, args...);
    }

    virtual std::shared_ptr<spdlog::async_logger> GetDevLogger() = 0;
    //////////////////////////////////////////////////////////////////////////
    // oss log
    virtual void OssLog(const google::protobuf::Message& msg) = 0;
};

} // namespace ark
