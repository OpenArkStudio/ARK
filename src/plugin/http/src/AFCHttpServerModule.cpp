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

#include "base/AFXml.hpp"
#include "http/include/AFHttpPlugin.hpp"
#include "http/include/AFCHttpServerModule.hpp"

namespace ark {

bool AFCHttpServerModule::Init()
{
    auto ret = LoadConfig();
    ARK_ASSERT_RET_VAL(ret, false);

    return true;
}

bool AFCHttpServerModule::PreShut()
{
    for (auto& t : thread_pool_)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    return true;
}

bool AFCHttpServerModule::StartServer()
{
    return StartServer(http_ip_, http_port_, http_thread_num_);
}

bool AFCHttpServerModule::StartServer(
    std::string const& ip /* = "0.0.0.0"*/, uint16_t port /* = 8080 */, uint8_t thread_num /* = 1*/)
{
    // handle errors and warnings
    const auto& on_error = [this](boost::system::error_code ec, beast::string_view from) {
        // log
        if (ec == boost::system::errc::address_in_use || ec == boost::system::errc::permission_denied)
        {
            this->ioc_.stop();
        }
    };

    // handle connections
    const auto& on_accept = [&](asio_socket sock) {
        auto endpoint = sock.remote_endpoint();
        // log
        http_session::recv(std::move(sock), router_, on_error);
    };

    //auto const address = boost::asio::ip::address_v4::any();
    auto const address = boost::asio::ip::address_v4::from_string(ip);

    // accept
    http_listener::launch(ioc_, {address, port}, on_accept, on_error);

    // capture SIGINT & SIGTERM
    sig_set_.async_wait([this](boost::system::error_code const&, int sig) {
        // log
        this->ioc_.stop();
    });

    // Run io service on the number of the thread
    thread_pool_.reserve(thread_num > 0 ? thread_num : 4);
    for (uint8_t i = 0; i < thread_num; ++i)
    {
        thread_pool_.emplace_back(std::bind(
            static_cast<std::size_t (boost::asio::io_context::*)()>(&boost::asio::io_context::run), std::ref(ioc_)));
    }

    return true;
}

bool AFCHttpServerModule::LoadConfig()
{
    AFXml config(AFHttpPlugin::GetPluginConf());
    auto root = config.GetRootNode();
    ARK_ASSERT_RET_VAL(root.IsValid(), false);

    auto conf_node = root.FindNode("conf");
    ARK_ASSERT_RET_VAL(conf_node.IsValid(), false);

    http_ip_ = conf_node.GetString("ip");
    http_port_ = conf_node.GetUint32("port");
    http_thread_num_ = conf_node.GetUint32("thread_num");

    return true;
}

} // namespace ark