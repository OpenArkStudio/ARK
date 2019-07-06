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

#include "base/AFMap.hpp"
#include "log/interface/AFILogModule.h"

namespace ark {

class AFCLogModule : public AFILogModule
{
public:
    using spdlogger = std::shared_ptr<spdlog::async_logger>;
    using spd_threadpool = std::shared_ptr<spdlog::details::thread_pool>;

    bool Shut() override;

    const spdlogger &GetDevLogger() override;

    void OssLog(const google::protobuf::Message &msg) override;

protected:
    void CreateDevLogger();
    spdlogger CreateOssLogger(const std::string &name);

private:
    spdlogger dev_logger_{nullptr};
    spd_threadpool dev_tp_{nullptr};

    // AFNewSmartPtrMap<std::string, spdlog::async_logger> test_loggers_;
    std::map<std::string, spdlogger> oss_loggers_;
    spd_threadpool oss_tp_{nullptr};
};

// class AFCDynamicLogModule : public AFIDynamicLogModule
//{
// public:
//    bool Shut() override;
//
//    const std::shared_ptr<spdlog::async_logger> &GetLogger(const int id, const char *name) override;
//
// protected:
//    void CreateLogger(const int id, const char *name);
//
// private:
//    using dynamic_log_key = std::pair<int, const char *>;
//    using dynamic_log_container = std::map<dynamic_log_key, std::shared_ptr<spdlog::async_logger>>;
//
//    dynamic_log_container _dynamic_loggers;
//    std::shared_ptr<spdlog::details::thread_pool> tp_{nullptr};
//
//    const std::shared_ptr<spdlog::async_logger> &_null_logger = nullptr;
//};

} // namespace ark
