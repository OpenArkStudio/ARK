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

#include <ananas/future/Future.h>
#include <brynet/net/TCPService.h>
#include <brynet/net/Connector.h>
#include <brynet/net/Wrapper.h>
#include <brynet/net/http/HttpFormat.h>
#include <brynet/net/http/HttpService.h>

#include "base/AFPlatform.hpp"
#include "base/AFEnum.hpp"
#include "proto/AFProtoCPP.hpp"

namespace ark {

class AFConsulClient final : public std::enable_shared_from_this<AFConsulClient>
{
public:
    AFConsulClient() = delete;
    AFConsulClient(const std::string& ip, const uint16_t port);
    ~AFConsulClient();

    void Update();

    void SetConsulCenter(const std::string& ip, const uint16_t port);

    ananas::Future<std::pair<bool, std::string>> RegisterService(const consul::service_data& service);
    ananas::Future<std::pair<bool, std::string>> DeregisterService(const std::string& service_id);

    ananas::Future<std::pair<bool, std::string>> SetKeyValue(const std::string& key, const std::string& value);
    ananas::Future<std::pair<bool, std::string>> GetValue(const std::string& key);
    ananas::Future<std::pair<bool, std::string>> DeleteValue(const std::string& key);

    ananas::Future<std::pair<bool, std::string>> HealthCheck(
        const std::string& service_name, const std::string& tag_filter);
    bool MultiHealthCheck(const std::string& service_name, const std::vector<std::string>& tag_filter_list,
        consul::service_set& services);

protected:
    ananas::Future<std::pair<bool, std::string>> _AsyncRequest(brynet::net::http::HttpRequest::HTTP_METHOD http_method,
        const std::string& ip, const uint16_t port, const std::string& url, std::map<std::string, std::string>& params,
        const google::protobuf::Message& http_msg);
    ananas::Future<std::pair<bool, std::string>> _AsyncRequest(brynet::net::http::HttpRequest::HTTP_METHOD http_method,
        const std::string& ip, const uint16_t port, const std::string& url);
    ananas::Future<std::pair<bool, std::string>> _AsyncRequest(brynet::net::http::HttpRequest::HTTP_METHOD http_method,
        const std::string& ip, const uint16_t port, const std::string& url, std::map<std::string, std::string>& params,
        const std::string& http_doby);

private:
    brynet::net::AsyncConnector::Ptr connector_{nullptr};
    brynet::net::TcpService::Ptr tcp_service_{nullptr};
    brynet::net::wrapper::HttpConnectionBuilder connection_builder_;

private:
    std::string consul_ip_{};
    uint16_t consul_port_{8500};

    brynet::timer::TimerMgr::Ptr timer_mgr_;
};

} // namespace ark
