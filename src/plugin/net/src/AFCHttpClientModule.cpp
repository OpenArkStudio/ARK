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

#include "net/include/AFCHttpClient.hpp"
#include "net/include/AFCHttpClientModule.hpp"

#include <google/protobuf/util/json_util.h>

namespace ark {

AFCHttpClientModule::AFCHttpClientModule()
{
    http_client_ = std::make_shared<AFCHttpClient>();
}

bool AFCHttpClientModule::Update()
{
    return http_client_->Update();
}

ananas::Future<std::pair<bool, std::string>> AFCHttpClientModule::AsyncRequest(
    brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
    const std::string& url, std::map<std::string, std::string>& params, const std::vector<std::string>& cookies,
    const google::protobuf::Message& http_msg)
{
    ananas::Promise<std::pair<bool, std::string>> promise;

    std::string http_body;
    auto status = google::protobuf::util::MessageToJsonString(http_msg, &http_body);
    if (!status.ok())
    {
        promise.SetValue(std::make_pair(false, std::string()));
        return promise.GetFuture();
    }

    return http_client_->AsyncRequest(http_method, ip, port, url, params, cookies, http_body);
}

ananas::Future<std::pair<bool, std::string>> AFCHttpClientModule::AsyncRequest(
    brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
    const std::string& url, const google::protobuf::Message& http_msg)
{
    ananas::Promise<std::pair<bool, std::string>> promise;

    std::string http_body;
    auto status = google::protobuf::util::MessageToJsonString(http_msg, &http_body);
    if (!status.ok())
    {
        promise.SetValue(std::make_pair(false, std::string()));
        return promise.GetFuture();
    }

    std::map<std::string, std::string> params;
    std::vector<std::string> cookies;
    return http_client_->AsyncRequest(http_method, ip, port, url, params, cookies, http_body);
}

ananas::Future<std::pair<bool, std::string>> AFCHttpClientModule::AsyncRequest(
    brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
    const std::string& url)
{
    std::map<std::string, std::string> params;
    std::vector<std::string> cookies;

    return http_client_->AsyncRequest(http_method, ip, port, url, params, cookies, std::string(""));
}

ananas::Future<std::pair<bool, std::string>> AFCHttpClientModule::AsyncRequest(
    brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
    const std::string& url, std::map<std::string, std::string>& params, const std::vector<std::string>& cookies,
    const std::string& http_body)
{
    return http_client_->AsyncRequest(http_method, ip, port, url, params, cookies, http_body);
}

} // namespace ark
