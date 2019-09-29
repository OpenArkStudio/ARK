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

#include "net/interface/AFIHttpClient.hpp"
#include "net/interface/AFIHttpClientModule.hpp"

namespace ark {

class AFCHttpClientModule : public AFIHttpClientModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit AFCHttpClientModule();

    bool Update() override;

    ananas::Future<std::pair<bool, std::string>> AsyncRequest(brynet::net::http::HttpRequest::HTTP_METHOD http_method,
        const std::string& ip, const uint16_t port, const std::string& url, std::map<std::string, std::string>& params,
        const std::vector<std::string>& cookies, const google::protobuf::Message& http_msg) override;

    ananas::Future<std::pair<bool, std::string>> AsyncRequest(brynet::net::http::HttpRequest::HTTP_METHOD http_method,
        const std::string& ip, const uint16_t port, const std::string& url,
        const google::protobuf::Message& http_msg) override;

    ananas::Future<std::pair<bool, std::string>> AsyncRequest(brynet::net::http::HttpRequest::HTTP_METHOD http_method,
        const std::string& ip, const uint16_t port, const std::string& url) override;

    ananas::Future<std::pair<bool, std::string>> AsyncRequest(brynet::net::http::HttpRequest::HTTP_METHOD http_method,
        const std::string& ip, const uint16_t port, const std::string& url, std::map<std::string, std::string>& params,
        const std::vector<std::string>& cookies, const std::string& http_body) override;

private:
    std::shared_ptr<AFIHttpClient> http_client_{nullptr};
};

} // namespace ark
