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

namespace ark
{

#define ARK_LOG_TRACE(my_fmt, ...)     m_pLogModule->Log(spdlog::level::trace, ARK_FUNCTION_LINE, my_fmt, ##__VA_ARGS__);
#define ARK_LOG_DEBUG(my_fmt, ...)     m_pLogModule->Log(spdlog::level::debug, ARK_FUNCTION_LINE, my_fmt, ##__VA_ARGS__);
#define ARK_LOG_INFO(my_fmt, ...)      m_pLogModule->Log(spdlog::level::info, ARK_FUNCTION_LINE, my_fmt, ##__VA_ARGS__);
#define ARK_LOG_WARN(my_fmt, ...)      m_pLogModule->Log(spdlog::level::warn, ARK_FUNCTION_LINE, my_fmt, ##__VA_ARGS__);
#define ARK_LOG_ERROR(my_fmt, ...)     m_pLogModule->Log(spdlog::level::err, ARK_FUNCTION_LINE, my_fmt, ##__VA_ARGS__);
#define ARK_LOG_CRITICAL(my_fmt, ...)  m_pLogModule->Log(spdlog::level::critical, ARK_FUNCTION_LINE, my_fmt, ##__VA_ARGS__);

    class AFILogModule : public AFIModule
    {
    public:
        template<typename... ARGS>
        void Log(spdlog::level::level_enum log_level, const char* function, int line, const char* my_fmt, const ARGS& ... args)
        {
            const std::shared_ptr<spdlog::async_logger>& logger = GetLogger();

            if (logger == nullptr)
            {
                return;
            }

            std::string new_fmt = std::string("[{}:{}]") + my_fmt;
            logger->log(log_level, new_fmt.c_str(), function, line, args...);
        }

        virtual const std::shared_ptr<spdlog::async_logger>& GetLogger() = 0;
    };

    //////////////////////////////////////////////////////////////////////////

#define ARK_DYNAMIC_LOG_TRACE(id, name, info)       m_pDynamicLogModule->Log(spdlog::level::trace, id, name, fmt::format("[{}:{}]", ARK_FUNCTION_LINE) + info);
#define ARK_DYNAMIC_LOG_DEBUG(id, name, info)       m_pDynamicLogModule->Log(spdlog::level::debug, id, name, fmt::format("[{}:{}]", ARK_FUNCTION_LINE) + info);
#define ARK_DYNAMIC_LOG_INFO(id, name, info)        m_pDynamicLogModule->Log(spdlog::level::info, id, name, fmt::format("[{}:{}]", ARK_FUNCTION_LINE) + info);
#define ARK_DYNAMIC_LOG_WARN(id, name, info)        m_pDynamicLogModule->Log(spdlog::level::warn, id, name, fmt::format("[{}:{}]", ARK_FUNCTION_LINE) + info);
#define ARK_DYNAMIC_LOG_ERROR(id, name, info)       m_pDynamicLogModule->Log(spdlog::level::err, id, name, fmt::format("[{}:{}]", ARK_FUNCTION_LINE) + info);
#define ARK_DYNAMIC_LOG_CRITICAL(id, name, info)    m_pDynamicLogModule->Log(spdlog::level::critical, id, name, fmt::format("[{}:{}]", ARK_FUNCTION_LINE) + info);

    class AFIDynamicLogModule : public AFIModule
    {
    public:
        template<typename... ARGS>
        void Log(spdlog::level::level_enum log_level, const int id, const std::string& name, const std::string& info)
        {
            auto& logger = GetLogger(id, name.c_str());

            if (logger == nullptr)
            {
                return;
            }

            logger->log(log_level, "[{}][{}]{}", id, name, info);
        }

        virtual const std::shared_ptr<spdlog::async_logger>& GetLogger(const int id, const char* name) = 0;
    };

}