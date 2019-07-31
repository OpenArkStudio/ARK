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

#include <iostream>

#include "brynet/net/http/HttpFormat.h"

#include "net/interface/AFINet.hpp"
#include "net/include/AFNetSession.hpp"

class AFCHttpClient // : public AFINet
{
public:
    AFCHttpClient();

    std::string Request(const std::string& ip, uint16_t port, brynet::net::http::HttpRequest::HTTP_METHOD method,
        const std::string& url, const std::map<std::string, std::string>& url_params,
        const std::string& http_body = "");

private:
    brynet::net::TcpService::Ptr tcp_service_{nullptr};
    brynet::net::AsyncConnector::Ptr async_connector_{nullptr};
};

AFCHttpClient::AFCHttpClient()
{
    brynet::net::base::InitSocket();

    tcp_service_ = brynet::net::TcpService::Create();
    tcp_service_->startWorkerThread(2);

    async_connector_ = brynet::net::AsyncConnector::Create();
    async_connector_->startWorkerThread();
}

std::string AFCHttpClient::Request(const std::string& ip, uint16_t port,
    brynet::net::http::HttpRequest::HTTP_METHOD method, const std::string& url,
    const std::map<std::string, std::string>& url_params, const std::string& http_json_body /* = ""*/)
{
    brynet::net::http::HttpRequest request;
    request.setMethod(method);
    request.setUrl(url);
    request.addHeadValue("Host", std::string(ip) + std::string(":") + ARK_TO_STRING(port));
    // request.setContentType("application/json");
    if (!http_json_body.empty())
    {
        request.setBody(http_json_body);
    }

    brynet::net::http::HttpQueryParameter params;
    for (auto iter : url_params)
    {
        params.add(iter.first, iter.second);
    }

    request.setQuery(params.getResult());
    std::string req_url = request.getResult();

    // start to connect
    brynet::net::wrapper::HttpConnectionBuilder()
        .configureConnector(async_connector_)
        .configureService(tcp_service_)
        .configureConnectOptions({
            brynet::net::AsyncConnector::ConnectOptions::WithAddr(ip, port),
            brynet::net::AsyncConnector::ConnectOptions::WithTimeout(std::chrono::seconds(10)),
            brynet::net::AsyncConnector::ConnectOptions::WithFailedCallback(
                []() { std::cout << "connect failed-------------" << std::endl; }),
        })
        .configureConnectionOptions({brynet::net::TcpService::AddSocketOption::WithMaxRecvBufferSize(1024),
            brynet::net::TcpService::AddSocketOption::AddEnterCallback(
                [](const brynet::net::TcpConnection::Ptr& session) {
                    // do something for session
                    (void)session;
                    std::cout << "enter callback" << std::endl;
                })})
        .configureEnterCallback([req_url](brynet::net::http::HttpSession::Ptr session) {
            //(void)session;
            std::cout << "connect success+++++++++++++" << std::endl;
            session->send(req_url.c_str(), req_url.size());
            std::cout << "after send" << std::endl;
            session->setHttpCallback([req_url](const brynet::net::http::HTTPParser& httpParser,
                                         const brynet::net::http::HttpSession::Ptr& session) {
                //(void)session;
                std::cout << "status" << httpParser.getStatus() << std::endl;
                std::cout << "body=" << httpParser.getBody() << std::endl;
            });
            session->setClosedCallback([req_url](const brynet::net::http::HttpSession::Ptr& session) {
                std::cout << "disconnect" << std::endl;
            });
        })
        .asyncConnect();

    return "";
}

int main()
{
    AFCHttpClient http_client;

    std::map<std::string, std::string> url_params;
    url_params.insert(std::make_pair("tag", "session"));
    url_params.insert(std::make_pair("passing", "1"));
    http_client.Request("172.26.24.163", 8500, brynet::net::http::HttpRequest::HTTP_METHOD::HTTP_METHOD_GET,
        "/v1/health/service/atelier", url_params);

    int a;
    std::cin >> a;
    return 0;
}
