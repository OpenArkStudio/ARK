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

#include "router/include/router_net_module_impl.hpp"

namespace ark {

bool router_net_module_impl::init() {
    m_pBusModule = find_module<bus_module>();
    m_pNetServiceManagerModule = find_module<net_service_manager_module>();

    return true;
}

bool router_net_module_impl::post_init() {
    StartServer();
    return true;
}

int router_net_module_impl::StartServer() {
    auto ret = m_pNetServiceManagerModule->create_server();
    if (!ret)
    {
        ARK_LOG_ERROR("Cannot start server net, busid = {}", m_pBusModule->get_self_bus_name());
        ARK_ASSERT_NO_EFFECT(0);
        exit(0);
    }

    m_pNetServer = m_pNetServiceManagerModule->get_inter_net_server();
    if (m_pNetServer == nullptr)
    {
        ARK_LOG_ERROR("Cannot find server net, busid = {}", m_pBusModule->get_self_bus_name());
        exit(0);
    }

    // add msg process

    return 0;
}

//
//bool AFCRouterNetModule::PreUpdate()
//{
//    int ret = StartClient();
//    return (ret == 0);
//}
//
//int AFCRouterNetModule::StartClient()
//{
//    //创建所有与对端链接的client
//    int ret = m_pNetServiceManagerModule->CreateClusterClients();
//    if (ret != 0)
//    {
//        ARK_LOG_ERROR("Cannot start server net, busid = {}, error = {}", m_pBusModule->GetSelfBusName(), ret);
//        ARK_ASSERT_NO_EFFECT(0);
//        return ret;
//    }
//
//    AFINetClientService* pNetClientWorld =
//        m_pNetServiceManagerModule->GetNetClientService(ARK_APP_TYPE::ARK_APP_MASTER);
//    if (pNetClientWorld == nullptr)
//    {
//        return 0;
//    }
//
//    // add msg process
//
//    return 0;
//}

std::shared_ptr<net_server_service> router_net_module_impl::GetNetServer() {
    return m_pNetServer;
}

} // namespace ark
