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

#include "dir_module_factory.hpp"
#include "dir_net_module_impl.hpp"

namespace ark {

std::unique_ptr<dir_net_module> detail::make_dir_net_module() {
    return std::make_unique<dir_net_module_impl>();
}

bool dir_net_module_impl::init() {
    bus_module_ = find_module<bus_module>();
    net_service_manager_module_ = find_module<net_service_manager_module>();

    return true;
}

bool dir_net_module_impl::post_init() {
    start_server();
    return true;
}

int dir_net_module_impl::start_server() {
    auto ret = net_service_manager_module_->create_server();
    if (!ret)
    {
        ARK_LOG_ERROR("Cannot start server net, busid = {}", bus_module_->get_self_bus_name());
        ARK_ASSERT_NO_EFFECT(0);
        exit(0);
    }

    net_server_ = net_service_manager_module_->get_inter_net_server();
    if (net_server_ == nullptr)
    {
        ARK_LOG_ERROR("Cannot find server net, busid = {}", bus_module_->get_self_bus_name());
        exit(0);
    }

    return 0;
}

std::shared_ptr<net_server_service> dir_net_module_impl::get_net_server() {
    return net_server_;
}

} // namespace ark
