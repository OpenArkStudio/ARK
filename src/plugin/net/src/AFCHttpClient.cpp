#include <brynet/net/Wrapper.h>
#include <brynet/net/http/HttpService.h>

#include "base/AFMacros.hpp"
#include "net/include/AFCHttpClient.hpp"
#include "net/include/AFNetMsg.hpp"

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

aom::Future<bool> AFCHttpClient::AsyncPost(const std::string& ip, const uint16_t port, const std::string& url, std::map<std::string, std::string>& params,
    const std::string& post_data, HTTP_CALLBACK&& callback)
{
    using namespace brynet::net;
    using namespace brynet::net::http;

    http::HttpRequest request;
    request.setMethod(HttpRequest::HTTP_METHOD::HTTP_METHOD_POST);
    request.setUrl(url);
    request.addHeadValue("Host", std::string(ip) + std::string(":") + ARK_TO_STRING(port));
    request.setContentType("application/json");
    if (!post_data.empty())
    {
        request.setBody(post_data);
    }

    HttpQueryParameter query_params;
    for (auto iter : params)
    {
        query_params.add(iter.first, iter.second);
    }

    request.setQuery(query_params.getResult());
    std::string req_url = request.getResult();

    aom::Promise<bool> promise;

    // start to connect
    connection_builder_.configureConnector(connector_)
        .configureService(tcp_service_)
        .configureConnectOptions({AsyncConnector::ConnectOptions::WithAddr(ip, port), AsyncConnector::ConnectOptions::WithTimeout(ARK_CONNECT_TIMEOUT),
            AsyncConnector::ConnectOptions::WithFailedCallback([&]() {
                std::cout << "Connect failed" << std::endl;
                promise.set_value(false);
            })})
        .configureConnectionOptions({TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_HTTP_RECV_BUFFER_SIZE),
            TcpService::AddSocketOption::AddEnterCallback([](const TcpConnection::Ptr& session) { std::cout << "Connect successfully" << std::endl; })})
        .configureEnterCallback([service = tcp_service_, callback, req_url, &promise](HttpSession::Ptr session) {
            session->send(req_url.c_str(), req_url.size());
            session->setHttpCallback([&](const HTTPParser& httpParser, const HttpSession::Ptr& session) {
                promise.set_value(true);
                auto event_loop = service->getRandomEventLoop();
                event_loop->runAsyncFunctor([=] {
                    if (callback != nullptr)
                    {
                        callback(httpParser.getBody());
                    }
                });
            });
            session->setClosedCallback([req_url](const HttpSession::Ptr& session) { std::cout << "Disconnect" << std::endl; });
        })
        .asyncConnect();

    return promise.get_future();
}

aom::Future<std::string> AFCHttpClient::AsyncGet(
    const std::string& ip, const uint16_t port, const std::string& url, std::map<std::string, std::string>& params, HTTP_CALLBACK&& callback)
{
    using namespace brynet::net;
    using namespace brynet::net::http;

    http::HttpRequest request;
    request.setMethod(HttpRequest::HTTP_METHOD::HTTP_METHOD_GET);
    request.setUrl(url);
    request.addHeadValue("Host", std::string(ip) + std::string(":") + ARK_TO_STRING(port));

    HttpQueryParameter query_params;
    for (auto iter : params)
    {
        query_params.add(iter.first, iter.second);
    }

    request.setQuery(query_params.getResult());
    std::string req_url = request.getResult();

    aom::Promise<std::string> promise;

    // start to connect
    connection_builder_.configureConnector(connector_)
        .configureService(tcp_service_)
        .configureConnectOptions({AsyncConnector::ConnectOptions::WithAddr(ip, port), AsyncConnector::ConnectOptions::WithTimeout(ARK_CONNECT_TIMEOUT),
            AsyncConnector::ConnectOptions::WithFailedCallback([&]() {
                std::cout << "Connect failed" << std::endl;
                promise.set_value("");
            })})
        .configureConnectionOptions({TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_HTTP_RECV_BUFFER_SIZE),
            TcpService::AddSocketOption::AddEnterCallback([](const TcpConnection::Ptr& session) { std::cout << "Connect successfully" << std::endl; })})
        .configureEnterCallback([&](HttpSession::Ptr session) {
            session->send(req_url.c_str(), req_url.size());
            session->setHttpCallback([&](const HTTPParser& httpParser, const HttpSession::Ptr& session) {
                promise.set_value(httpParser.getBody());
                auto event_loop = tcp_service_->getRandomEventLoop();
                event_loop->runAsyncFunctor([=] {
                    if (callback != nullptr)
                    {
                        callback(httpParser.getBody());
                    }
                });
            });
            session->setClosedCallback([&](const HttpSession::Ptr& session) {
                std::cout << "Disconnect" << std::endl;
                promise.set_value(std::string());
            });
        })
        .asyncConnect();

    return promise.get_future();
}

aom::Future<bool> AFCHttpClient::AsyncPut(const std::string& ip, const uint16_t port, const std::string& url, std::map<std::string, std::string>& params,
    const std::string& put_data, HTTP_CALLBACK&& callback)
{
    using namespace brynet::net;
    using namespace brynet::net::http;

    http::HttpRequest request;
    request.setMethod(HttpRequest::HTTP_METHOD::HTTP_METHOD_PUT);
    request.setUrl(url);
    request.addHeadValue("Host", std::string(ip) + std::string(":") + ARK_TO_STRING(port));

    HttpQueryParameter query_params;
    for (auto iter : params)
    {
        query_params.add(iter.first, iter.second);
    }

    if (!put_data.empty())
    {
        request.setBody(put_data);
    }

    request.setQuery(query_params.getResult());
    std::string req_url = request.getResult();

    aom::Promise<bool> promise;

    // start to connect
    connection_builder_.configureConnector(connector_)
        .configureService(tcp_service_)
        .configureConnectOptions({AsyncConnector::ConnectOptions::WithAddr(ip, port), AsyncConnector::ConnectOptions::WithTimeout(ARK_CONNECT_TIMEOUT),
            AsyncConnector::ConnectOptions::WithFailedCallback([&]() {
                std::cout << "Connect failed" << std::endl;
                promise.set_value(false);
            })})
        .configureConnectionOptions({TcpService::AddSocketOption::WithMaxRecvBufferSize(ARK_HTTP_RECV_BUFFER_SIZE),
            TcpService::AddSocketOption::AddEnterCallback([](const TcpConnection::Ptr& session) { std::cout << "Connect successfully" << std::endl; })})
        .configureEnterCallback([&](HttpSession::Ptr session) {
            session->send(req_url.c_str(), req_url.size());
            session->setHttpCallback([&](const HTTPParser& httpParser, const HttpSession::Ptr& session) {
                promise.set_value(true);
                auto event_loop = tcp_service_->getRandomEventLoop();
                event_loop->runAsyncFunctor([=] {
                    if (callback != nullptr)
                    {
                        callback(httpParser.getBody());
                    }
                });
            });
            session->setClosedCallback([req_url](const HttpSession::Ptr& session) { std::cout << "Disconnect" << std::endl; });
        })
        .asyncConnect();

    return promise.get_future();
}

} // namespace ark
