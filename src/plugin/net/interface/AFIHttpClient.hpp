/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
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

#include "base/AFPlatform.hpp"
#include "proto/AFProtoCPP.hpp"

namespace ark {

// http client without DNS resolver
class AFIHttpClient
{
public:
    virtual ~AFIHttpClient() = default;

    using HTTP_CALLBACK = std::function<void(const std::string&)>;

    virtual bool AsyncPost(const std::string& ip, const uint16_t port, const std::string& url,
        std::map<std::string, std::string>& params,
        const std::string& post_data, HTTP_CALLBACK&& callback) = 0;

    // Response - the type pf response protobuf message
    template<typename Response>
    bool AsyncJsonPost(const std::string& ip, const uint16_t port, const std::string& url,
        std::map<std::string, std::string>& params,
        const google::protobuf::Message& post_msg, std::function<void(const Response&)>&& callback)
    {
        std::string post_body;
        google::protobuf::util::MessageToJsonString(post_msg, &post_body);
        return AsyncPost(ip, port, url, params, post_body, [=](const std::string& response_body) {
            Response response;
            auto status = google::protobuf::util::JsonStringToMessage(response_body, response);
            if (!status.ok())
            {
                CONSOLE_ERROR_LOG << "google::protobuf::util::JsonStringToMessage failed, " << status.error_message().ToString() << std::endl;
                return;
            }

            auto tcp_service = GetTcpService();
            if (tcp_service == nullptr)
            {
                return;
            }

            auto event_loop = tcp_service->getRandomEventLoop();
            event_loop->runAsyncFunctor([=]() {
                if (callback != nullptr)
                {
                    callback(response);
                }
            });
        });
    }

    virtual bool AsyncGet(
        const std::string& ip, const uint16_t port, const std::string& url, std::map<std::string, std::string>& params, HTTP_CALLBACK&& callback) = 0;

    // Response - the type pf response protobuf message
    template<typename Response>
    bool AsyncJsonGet(const std::string& ip, const uint16_t port, const std::string& url,
        std::map<std::string, std::string>& params,
        std::function<void(const Response&)>&& callback)
    {
        return AsyncGet(ip, port, url, params, [=](const std::string& response_body) {
            Response response;
            auto status = google::protobuf::util::JsonStringToMessage(response_body, response);
            if (!status.ok())
            {
                CONSOLE_ERROR_LOG << "google::protobuf::util::JsonStringToMessage failed, " << status.error_message().ToString() << std::endl;
                return;
            }

            auto tcp_service = GetTcpService();
            if (tcp_service == nullptr)
            {
                return;
            }

            auto event_loop = tcp_service->getRandomEventLoop();
            event_loop->runAsyncFunctor([=]() {
                if (callback != nullptr)
                {
                    callback(response);
                }
            });
        });
    }

    virtual bool AsyncPut(const std::string& ip, const uint16_t port, const std::string& url,
        std::map<std::string, std::string>& params,
        const std::string& put_data, HTTP_CALLBACK&& callback) = 0;

protected:
    virtual brynet::net::TcpService::Ptr GetTcpService() = 0;
};

} // namespace ark
