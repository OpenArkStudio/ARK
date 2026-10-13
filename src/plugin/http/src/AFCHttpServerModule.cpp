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

#include <algorithm>
#include <mutex>
#include <set>

#include "base/AFXml.hpp"
#include "http/include/AFCHttpServerModule.hpp"
#include "http/include/AFHttpPlugin.hpp"

namespace ark {

namespace {

std::mutex http_endpoint_mutex;
std::set<std::string> http_endpoints;

std::string EndpointKey(const std::string& ip, uint16_t port)
{
    return ip + '\0' + std::to_string(port);
}

bool ReserveEndpoint(const std::string& key)
{
    std::lock_guard lock(http_endpoint_mutex);
    return http_endpoints.insert(key).second;
}

void ReleaseEndpoint(std::string& key)
{
    if (key.empty())
    {
        return;
    }
    std::lock_guard lock(http_endpoint_mutex);
    http_endpoints.erase(key);
    key.clear();
}

} // namespace

bool AFCHttpServerModule::Init()
{
    auto ret = LoadConfig();
    ARK_ASSERT_RET_VAL(ret, false);

    return true;
}

bool AFCHttpServerModule::PreShut()
{
    if (server_)
    {
        server_->stop();
        server_.reset();
    }
    ReleaseEndpoint(listen_key_);

    return true;
}

bool AFCHttpServerModule::StartServer()
{
    return StartServer(http_ip_, http_port_, http_thread_num_);
}

bool AFCHttpServerModule::StartServer(
    std::string const& ip /* = "0.0.0.0"*/, uint16_t port /* = 8080 */, uint8_t thread_num /* = 1*/)
{
    if (server_ && server_->is_started())
    {
        return false;
    }
    const auto endpoint = port == 0 ? std::string{} : EndpointKey(ip, port);
    if (!endpoint.empty() && !ReserveEndpoint(endpoint))
    {
        return false;
    }
    auto server = std::make_unique<arknet::http_server>(
        arknet::detail::http_frame_size, arknet::detail::max_buffer_size, std::max<unsigned>(thread_num, 1));
    server->bind_recv([this](std::shared_ptr<arknet::http_session>& session, arknet::http_route_request& request) {
        auto response = router_(request);
        response.set(arknet::http::field::server, "ARK");
        session->async_send(std::move(response));
    });
    if (!server->start(ip, port))
    {
        auto reserved_endpoint = endpoint;
        ReleaseEndpoint(reserved_endpoint);
        return false;
    }
    server_ = std::move(server);
    listen_key_ = endpoint;
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
