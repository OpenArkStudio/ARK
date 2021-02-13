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

#include "base/AFPluginManager.hpp"
#include "http/interface/AFIHttpServerModule.hpp"

namespace ark {

using namespace _0xdead4ead;

class AFCHttpServerModule final : public AFIHttpServerModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool PreShut() override;

    bool StartServer() override;
    bool StartServer(std::string const& ip = "0.0.0.0", uint16_t port = 8080, uint8_t thread_num = 1) override;

    http::basic_router<http_session>& GetRouter() override
    {
        return router_;
    }

protected:
    bool LoadConfig();
    std::string http_ip_{"0.0.0.0"};
    uint16_t http_port_{9090};
    uint8_t http_thread_num_{1};

private:
    // TODO: 替换为cinatra
    boost::asio::io_context ioc_;
    boost::asio::posix::stream_descriptor out_{ioc_, STDERR_FILENO};
    boost::asio::signal_set sig_set_{ioc_, SIGINT, SIGTERM};
    http::basic_router<http_session> router_{std::regex::ECMAScript};
    std::vector<std::thread> thread_pool_;
};

} // namespace ark