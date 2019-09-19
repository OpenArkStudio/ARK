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

#include "net/include/AFNetMsg.hpp"
#include "consul/include/AFConsulClient.hpp"

namespace ark {

static const std::string REGISTER_API("/v1/agent/service/register");
static const std::string DEREGISTER_API("/v1/agent/service/deregister/");
static const std::string KV_API("/v1/kv/");
static const std::string HEALTH_CHECK_API("/v1/health/service/");

AFConsulClient::AFConsulClient(const std::string& ip, const uint16_t port)
{
    using namespace brynet::net;

    base::InitSocket();

    tcp_service_ = TcpService::Create();
    connector_ = AsyncConnector::Create();

    tcp_service_->startWorkerThread(1);
    connector_->startWorkerThread();

    SetConsulCenter(ip, port);
}

AFConsulClient::~AFConsulClient()
{
    using namespace brynet::net;
    base::DestroySocket();
}

void AFConsulClient::SetConsulCenter(const std::string& ip, const uint16_t port)
{
    consul_ip_ = ip;
    consul_port_ = port;
}

ananas::Future<std::pair<bool, std::string>> AFConsulClient::RegisterService(const consul::service_data& service)
{
    std::map<std::string, std::string> params;
    return _AsyncRequest(brynet::net::http::HttpRequest::HTTP_METHOD::HTTP_METHOD_PUT, consul_ip_, consul_port_,
        REGISTER_API, params, service);
}

ananas::Future<std::pair<bool, std::string>> AFConsulClient::DeregisterService(const std::string& service_id)
{
    ananas::Promise<std::pair<bool, std::string>> promise;
    if (service_id.empty())
    {
        promise.SetValue(std::make_pair(false, std::string()));
        return promise.GetFuture();
    }

    auto url = DEREGISTER_API + service_id;
    return _AsyncRequest(brynet::net::http::HttpRequest::HTTP_METHOD::HTTP_METHOD_PUT, consul_ip_, consul_port_, url);
}

ananas::Future<std::pair<bool, std::string>> AFConsulClient::SetKeyValue(
    const std::string& key, const std::string& value)
{
    ananas::Promise<std::pair<bool, std::string>> promise;
    if (key.empty())
    {
        promise.SetValue(std::make_pair(false, std::string()));
        return promise.GetFuture();
    }

    std::map<std::string, std::string> params;
    return _AsyncRequest(
        brynet::net::http::HttpRequest::HTTP_METHOD::HTTP_METHOD_PUT, consul_ip_, consul_port_, KV_API, params, value);
}

std::string AFConsulClient::GetValue(const std::string& key)
{
    if (key.empty())
    {
        return "";
    }

    return "";
}

bool AFConsulClient::DeleteValue(const std::string& key)
{
    //TODO

    return true;
}

bool AFConsulClient::HealthCheck(
    const std::string& service_name, const std::string& tag_filter, consul::service_set& services)
{
    //TODO

    return true;
}

bool AFConsulClient::MultiHealthCheck(
    const std::string& service_name, const std::vector<std::string>& tag_filter_list, consul::service_set& services)
{
    //TODO

    return true;
}

//////////////////////////////////////////////////////////////////////////
ananas::Future<std::pair<bool, std::string>> AFConsulClient::_AsyncRequest(
    brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
    const std::string& url, std::map<std::string, std::string>& params, const std::string& http_doby)
{
    using namespace brynet::net;
    using namespace brynet::net::http;

    http::HttpRequest request;
    request.setMethod(http_method);
    request.setUrl(url);
    request.addHeadValue("Host", std::string(ip) + std::string(":") + ARK_TO_STRING(port));
    request.setContentType("application/json");

    switch (http_method)
    {
        case brynet::net::http::HttpRequest::HTTP_METHOD::HTTP_METHOD_POST:
        case brynet::net::http::HttpRequest::HTTP_METHOD::HTTP_METHOD_PUT:
            if (!http_doby.empty())
            {
                request.setBody(http_doby);
            }
            break;
        default:
            break;
    }

    HttpQueryParameter query_params;
    for (auto iter : params)
    {
        query_params.add(iter.first, iter.second);
    }

    request.setQuery(query_params.getResult());
    std::string req_url = request.getResult();

    // promise
    ananas::Promise<std::pair<bool, std::string>> promise;

    // start to connect
    connection_builder_.configureConnector(connector_)
        .configureService(tcp_service_)
        .configureConnectOptions({AsyncConnector::ConnectOptions::WithAddr(ip, port),
            AsyncConnector::ConnectOptions::WithTimeout(ARK_CONNECT_TIMEOUT),
            AsyncConnector::ConnectOptions::WithFailedCallback(
                [=]() mutable { promise.SetValue(std::make_pair(false, std::string())); })})
        .configureConnectionOptions({TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_HTTP_RECV_BUFFER_SIZE),
            TcpService::AddSocketOption::AddEnterCallback([](const TcpConnection::Ptr& session) { /*DO NOTHING*/ })})
        .configureEnterCallback([=](HttpSession::Ptr session) mutable {
            session->send(req_url.c_str(), req_url.size());
            session->setHttpCallback([=](const HTTPParser& httpParser, const HttpSession::Ptr& session) mutable {
                promise.SetValue(std::make_pair(true, httpParser.getBody()));
            });
            session->setClosedCallback([](const HttpSession::Ptr& session) { /*DO NOTHING*/ });
        })
        .asyncConnect();

    return promise.GetFuture();
}

ananas::Future<std::pair<bool, std::string>> AFConsulClient::_AsyncRequest(
    brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
    const std::string& url, std::map<std::string, std::string>& params, const google::protobuf::Message& http_msg)
{
    ananas::Promise<std::pair<bool, std::string>> promise;

    std::string http_body;
    auto status = google::protobuf::util::MessageToJsonString(http_msg, &http_body);
    if (!status.ok())
    {
        promise.SetValue(std::make_pair(false, std::string()));
        return promise.GetFuture();
    }

    return _AsyncRequest(http_method, ip, port, url, params, http_body);
}

ananas::Future<std::pair<bool, std::string>> AFConsulClient::_AsyncRequest(
    brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
    const std::string& url)
{
    std::map<std::string, std::string> params;
    return _AsyncRequest(http_method, ip, port, url, params, "");
}

} // namespace ark
