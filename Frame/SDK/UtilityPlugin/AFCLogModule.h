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

#pragma once

#include "SDK/Interface/AFILogModule.h"

namespace ark
{

    class AFCLogModule : public AFILogModule
    {
    public:
        explicit AFCLogModule() = default;

        bool Init() override;
        bool Shut() override;

        const std::shared_ptr<spdlog::async_logger>& GetLogger() override;

    protected:
        void CreateLogger();

    private:
        std::shared_ptr<spdlog::async_logger> mxLogger;
    };

    class AFCDynamicLogModule : public AFIDynamicLogModule
    {
    public:
        explicit AFCDynamicLogModule() = default;

        bool Shut() override;

        const std::shared_ptr<spdlog::async_logger>& GetLogger(const int id, const char* name) override;

    protected:
        void CreateLogger(const int id, const char* name);

    private:
        using dynamic_log_key = std::pair<int, const char*>;
        using dynamic_log_container = std::map<dynamic_log_key, std::shared_ptr<spdlog::async_logger>>;

        dynamic_log_container _dynamic_loggers;
        const std::shared_ptr<spdlog::async_logger>& _null_logger = nullptr;
    };

}