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

#include <brynet/net/TCPService.h>
#include <brynet/net/Connector.h>
#include <brynet/net/EventLoop.h>
#include <brynet/net/http/HttpFormat.h>
#include "base/AFPlatform.hpp"
#include "net/interface/AFIHttpClient.hpp"

namespace ark {

class AFCHttpClient final : public AFIHttpClient
{

public:
    AFCHttpClient();
    virtual ~AFCHttpClient();

protected:
    void AsyncPost(const std::string& ip, const uint16_t port, const std::string& url,
        std::map<std::string, std::string>& params, const std::string& post_data, HTTP_CALLBACK&& callback) override;

    void AsyncGet(const std::string& ip, const uint16_t port, const std::string& url,
        std::map<std::string, std::string>& params, HTTP_CALLBACK&& callback) override;

private:
    brynet::net::AsyncConnector::Ptr connector_{nullptr};
    brynet::net::TcpService::Ptr tcp_service_{nullptr};
    brynet::net::wrapper::HttpConnectionBuilder connection_builder_;
};

} // namespace ark
