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

#include "ark/bus/bus_module.hpp"
#include "ark/bus/msg_module.hpp"
#include "ark/net/net_service_manager_module.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/proto.hpp"
#include "router/interface/router_net_module.hpp"

namespace ark {

class router_net_module_impl final : public router_net_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;
    bool post_init() override;
    //bool PreUpdate() override;

    std::shared_ptr<net_server_service> GetNetServer() override;

protected:
    int StartServer();
    //int StartClient();

private:
    bus_module* m_pBusModule;
    net_service_manager_module* m_pNetServiceManagerModule;

    std::shared_ptr<net_server_service> m_pNetServer;
};

} // namespace ark
