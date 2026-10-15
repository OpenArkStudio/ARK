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

#include <memory>
#include <string>

#include <arknet/http/http_server.hpp>

#include "ark/http/http_server_module.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

class http_server_module_impl final : public http_server_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;
    bool pre_shut() override;

    bool start_server() override;
    bool start_server(std::string const& ip = "0.0.0.0", uint16_t port = 8080, uint8_t thread_num = 1) override;

    arknet::http_router& get_router() override {
        return router_;
    }

protected:
    bool load_config();
    std::string http_ip_{"0.0.0.0"};
    uint16_t http_port_{9090};
    uint8_t http_thread_num_{1};

private:
    arknet::http_router router_;
    std::unique_ptr<arknet::http_server> server_;
    std::string listen_key_;
};

} // namespace ark
