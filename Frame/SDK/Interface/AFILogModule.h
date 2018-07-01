/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#pragma once

#include "AFIModule.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"

#define ARK_FORMAT(format, ...)             fmt::format(format, ##__VA_ARGS__);
#define ARK_FORMAT_FUNCTION(format, ...)    fmt::format(std::string("[{}:{}]{}") + format, ARK_FUNCTION_LINE, ##__VA_ARGS__);

#define ARK_LOG_TRACE(format, ...)     m_pLogModule->Log(spdlog::level::trace, ARK_FUNCTION_LINE, format, ##__VA_ARGS__);
#define ARK_LOG_DEBUG(format, ...)     m_pLogModule->Log(spdlog::level::debug, ARK_FUNCTION_LINE, format, ##__VA_ARGS__);
#define ARK_LOG_INFO(format, ...)      m_pLogModule->Log(spdlog::level::info, ARK_FUNCTION_LINE, format, ##__VA_ARGS__);
#define ARK_LOG_WARN(format, ...)      m_pLogModule->Log(spdlog::level::warn, ARK_FUNCTION_LINE, format, ##__VA_ARGS__);
#define ARK_LOG_ERROR(format, ...)     m_pLogModule->Log(spdlog::level::err, ARK_FUNCTION_LINE, format, ##__VA_ARGS__);
#define ARK_LOG_CRITICAL(format, ...)  m_pLogModule->Log(spdlog::level::critical, ARK_FUNCTION_LINE, format, ##__VA_ARGS__);

class AFILogModule : public AFIModule
{
public:
    template<typename... ARGS>
    void Log(spdlog::level::level_enum log_level, const char* function, int line, const char* format, const ARGS&... args)
    {
        std::shared_ptr<spdlog::logger>& logger = GetLogger();

        if (logger == nullptr)
        {
            return;
        }

        std::string new_fmt = fmt::format("[{}:{}]", function, line) + format;
        logger->log(log_level, new_fmt.c_str(), args...);
    }

    virtual std::shared_ptr<spdlog::logger>& GetLogger() = 0;
};


//#define ARK_DYNAMIC_LOG_TRACE(id, name, info)       m_pDynamicLogModule->Log(spdlog::level::trace, id, name, info);
//#define ARK_DYNAMIC_LOG_DEBUG(id, name, info)       m_pDynamicLogModule->Log(spdlog::level::debug, id, name, info);
//#define ARK_DYNAMIC_LOG_INFO(id, name, info)        m_pDynamicLogModule->Log(spdlog::level::info, id, name, info);
//#define ARK_DYNAMIC_LOG_WARN(id, name, info)        m_pDynamicLogModule->Log(spdlog::level::warn, id, name, info);
//#define ARK_DYNAMIC_LOG_ERROR(id, name, info)       m_pDynamicLogModule->Log(spdlog::level::err, id, name, info);
//#define ARK_DYNAMIC_LOG_CRITICAL(id, name, info)    m_pDynamicLogModule->Log(spdlog::level::critical, id, name, info);
//
//class AFIDynamicLogModule : public AFIModule
//{
//public:
//    template<typename... Args>
//    void Log(spdlog::level::level_enum log_level, const int id, const char* name, const char* info)
//    {
//        std::shared_ptr<spdlog::logger>& logger = GetLogger();
//
//        if (logger == nullptr)
//        {
//            return;
//        }
//
//        std::string new_fmt = fmt::format("[{}:{}]{}", function, line, fmt);
//        logger->log(log_level, new_fmt.c_str(), args...);
//    }
//
//    virtual std::shared_ptr<spdlog::logger>& GetLogger(const int id, const char* name) = 0;
//};