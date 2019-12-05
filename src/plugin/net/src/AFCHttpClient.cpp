#include <brynet/net/wrapper/HttpConnectionBuilder.hpp>
#include <brynet/net/http/HttpService.hpp>

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

    timer_mgr_ = std::make_shared<brynet::base::TimerMgr>();
}

AFCHttpClient::~AFCHttpClient()
{
    using namespace brynet::net;

    tcp_service_->stopWorkerThread();
    connector_->stopWorkerThread();

    base::DestroySocket();
}

bool AFCHttpClient::Update()
{
    timer_mgr_->schedule();
    return true;
}

ananas::Future<std::pair<bool, std::string>> AFCHttpClient::AsyncRequest(
    brynet::net::http::HttpRequest::HTTP_METHOD http_method, const std::string& ip, const uint16_t port,
    const std::string& url, std::map<std::string, std::string>& params, const std::vector<std::string>& cookies,
    const std::string& http_body)
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
            if (!http_body.empty())
            {
                request.setBody(http_body);
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
        .configureConnectOptions({ConnectOption::WithAddr(ip, port), ConnectOption::WithTimeout(ARK_CONNECT_TIMEOUT),
            ConnectOption::WithFailedCallback(
                [=]() mutable { promise.SetValue(std::make_pair(false, std::string())); })})
        .configureConnectionOptions({AddSocketOption::WithMaxRecvBufferSize(ARK_HTTP_RECV_BUFFER_SIZE),
            AddSocketOption::AddEnterCallback([](const TcpConnection::Ptr& session) {
#ifdef ARK_RUN_MODE_DEBUG
                CONSOLE_INFO_LOG << "http client connected!" << std::endl;
#endif
            })})
        .configureEnterCallback([=](HttpSession::Ptr session) mutable {
            session->send(req_url.c_str(), req_url.size());
            session->setHttpCallback([=](const HTTPParser& httpParser, const HttpSession::Ptr& session) mutable {
                promise.SetValue(std::make_pair(true, httpParser.getBody()));
            });
            session->setClosedCallback([](const HttpSession::Ptr& session) {
#ifdef ARK_RUN_MODE_DEBUG
                CONSOLE_INFO_LOG << "http client closed!" << std::endl;
#endif
            });

            // start a timer to process request timeout and disconnect
            timer_mgr_->addTimer(ARK_CONNECT_TIMEOUT, [=]() mutable { session->postShutdown(); });
        })
        .asyncConnect();

    return promise.GetFuture();
}

} // namespace ark
