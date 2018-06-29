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

#define ARK_LOG_TRACE(fmt, ...)     m_pLogModule->Log(spdlog::level::trace, ARK_FUNCTION_LINE, fmt, ##__VA_ARGS__);
#define ARK_LOG_DEBUG(fmt, ...)     m_pLogModule->Log(spdlog::level::debug, ARK_FUNCTION_LINE, fmt, ##__VA_ARGS__);
#define ARK_LOG_INFO(fmt, ...)      m_pLogModule->Log(spdlog::level::info, ARK_FUNCTION_LINE, fmt, ##__VA_ARGS__);
#define ARK_LOG_WARN(fmt, ...)      m_pLogModule->Log(spdlog::level::warn, ARK_FUNCTION_LINE, fmt, ##__VA_ARGS__);
#define ARK_LOG_ERROR(fmt, ...)     m_pLogModule->Log(spdlog::level::err, ARK_FUNCTION_LINE, fmt, ##__VA_ARGS__);
#define ARK_LOG_CRITICAL(fmt, ...)  m_pLogModule->Log(spdlog::level::critical, ARK_FUNCTION_LINE, fmt, ##__VA_ARGS__);

class AFILogModule : public AFIModule
{
public:
    template<typename... Args>
    void Log(spdlog::level::level_enum log_level, const char* function, int line, const char* fmt, const Args& ... args)
    {
        std::shared_ptr<spdlog::logger>& logger = GetLogger();

        if (logger == nullptr)
        {
            return;
        }

        std::string new_fmt = std::string("[") + function + ":" + ARK_TO_STRING(line) + "] " + fmt;
        logger->log(log_level, new_fmt.c_str(), args...);
    }

    virtual std::shared_ptr<spdlog::logger>& GetLogger() = 0;
};