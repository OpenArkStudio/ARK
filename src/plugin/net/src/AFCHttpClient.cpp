#include <brynet/net/Wrapper.h>
#include <brynet/net/http/HttpService.h>

#include "base/AFMacros.hpp"
#include "net/include/AFCHttpClient.hpp"
#include "net/include/AFNetMsg.hpp"

#include <google/protobuf/util/json_util.h>

namespace ark {

AFCHttpClient::AFCHttpClient()
{
    using namespace brynet::net;

    base::InitSocket();

    tcp_service_ = TcpService::Create();
    connector_ = AsyncConnector::Create();

    tcp_service_->startWorkerThread(1);
    connector_->startWorkerThread();
}

AFCHttpClient::~AFCHttpClient()
{
    using namespace brynet::net;

    base::DestroySocket();
}

ananas::Future<std::pair<bool, std::string>> AFCHttpClient::AsyncRequest(
    brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
    const std::string& url, std::map<std::string, std::string>& params, const std::vector<std::string>& cookies,
    const std::string& http_doby)
{
    using namespace brynet::net;
    using namespace brynet::net::http;

    http::HttpRequest request;
    request.setMethod(http_method);
    request.setUrl(url);
    request.addHeadValue("Host", std::string(ip) + std::string(":") + ARK_TO_STRING(port));

    for (auto cookie : cookies)
    {
        request.setCookie(cookie);
    }

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

            // TODO: timer for request timeout and disconnection
        })
        .asyncConnect();

    return promise.GetFuture();
}

ananas::Future<std::pair<bool, std::string>> AFCHttpClient::AsyncRequest(
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

    return AsyncRequest(http_method, ip, port, url, params, cookies, http_body);
}

ananas::Future<std::pair<bool, std::string>> AFCHttpClient::AsyncRequest(
    brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
    const std::string& url, const google::protobuf::Message& http_msg)
{
    std::map<std::string, std::string> params;
    std::vector<std::string> cookies;
    return AsyncRequest(http_method, ip, port, url, params, cookies, http_msg);
}

ananas::Future<std::pair<bool, std::string>> AFCHttpClient::AsyncRequest(
    brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
    const std::string& url)
{
    std::map<std::string, std::string> params;
    std::vector<std::string> cookies;
    return AsyncRequest(http_method, ip, port, url, params, cookies, std::string(""));
}

} // namespace ark
