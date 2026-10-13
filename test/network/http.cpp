// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <arknet/http/http_server.hpp>
#include <arknet/http/request.hpp>

#include "doctest.h"
#include "http/include/AFCHttpServerModule.hpp"
#include "net/include/AFCHttpClientModule.hpp"

namespace {
using namespace std::chrono_literals;
using HttpRequest = arknet::http_request::request_type;
using HttpResponse = arknet::http_request::response_type;
using HttpResult = std::pair<bool, std::string>;

template<class Predicate, class Pump>
bool WaitHttp(Predicate predicate, Pump pump, std::chrono::milliseconds timeout = 5s)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    do
    {
        pump();
        if (predicate())
        {
            return true;
        }
        std::this_thread::sleep_for(1ms);
    } while (std::chrono::steady_clock::now() < deadline);
    return false;
}

template<class Predicate>
bool WaitHttp(Predicate predicate, std::chrono::milliseconds timeout = 5s)
{
    return WaitHttp(std::move(predicate), [] {}, timeout);
}

HttpResult AwaitHttp(std::future<HttpResult>& future)
{
    REQUIRE(future.wait_for(5s) == std::future_status::ready);
    return future.get();
}

class HttpPeer
{
public:
    explicit HttpPeer(bool reply = true)
        : reply_(reply)
    {
        server_.bind_recv([this](auto& session, HttpRequest& request) {
            {
                std::lock_guard lock(mutex_);
                requests_.push_back(request);
            }
            if (reply_)
            {
                HttpResponse response(arknet::http::status::ok, request.version());
                response.keep_alive(request.keep_alive());
                if (respond)
                {
                    respond(request, response);
                }
                else
                {
                    response.body() = request.body();
                }
                response.prepare_payload();
                session->async_send(std::move(response));
            }
        });
    }

    ~HttpPeer()
    {
        server_.stop();
    }

    bool Start()
    {
        return server_.start("127.0.0.1", 0);
    }

    std::uint16_t Port() const
    {
        return server_.get_listen_port();
    }

    std::vector<HttpRequest> Requests()
    {
        std::lock_guard lock(mutex_);
        return requests_;
    }

    std::size_t Count()
    {
        std::lock_guard lock(mutex_);
        return requests_.size();
    }

    std::function<void(const HttpRequest&, HttpResponse&)> respond;

private:
    const bool reply_;
    std::mutex mutex_;
    std::vector<HttpRequest> requests_;
    arknet::http_server server_{1024, 16 * 1024 * 1024, 2};
};

struct HttpCallbacks
{
    void Receive(HttpResult result)
    {
        if (std::this_thread::get_id() != update_thread)
        {
            wrong_thread.store(true);
        }
        std::lock_guard lock(mutex);
        results.push_back(std::move(result));
    }

    std::vector<HttpResult> Results()
    {
        std::lock_guard lock(mutex);
        return results;
    }

    const std::thread::id update_thread = std::this_thread::get_id();
    std::atomic<bool> wrong_thread{false};
    std::mutex mutex;
    std::vector<HttpResult> results;
};

std::uint16_t HttpPort()
{
    asio::io_context context;
    asio::ip::tcp::acceptor acceptor(context, {asio::ip::make_address("127.0.0.1"), 0});
    return acceptor.local_endpoint().port();
}
} // namespace

TEST_CASE("HTTP concurrent futures keep each request and response independent")
{
    HttpPeer peer;
    peer.respond = [](const HttpRequest& request, HttpResponse& response) {
        response.body() =
            std::string(request.method_string()) + ":" + std::string(request.target()) + ":" + request.body();
    };
    REQUIRE(peer.Start());
    ark::AFCHttpClientModule client;
    std::vector<std::future<HttpResult>> futures;
    std::vector<std::string> expected;
    for (int index = 0; index < 48; ++index)
    {
        const auto target = "/request/" + std::to_string(index);
        if (index % 2 == 0)
        {
            const auto body = "body-" + std::to_string(index);
            futures.push_back(client.Post("localhost", peer.Port(), target, nullptr, &body));
            expected.push_back("POST:" + target + ":" + body);
        }
        else
        {
            futures.push_back(client.Get("localhost", peer.Port(), target));
            expected.push_back("GET:" + target + ":");
        }
    }
    for (std::size_t index = 0; index < futures.size(); ++index)
    {
        const auto result = AwaitHttp(futures[index]);
        CHECK(result.first);
        CHECK(result.second == expected[index]);
    }
    CHECK(peer.Count() == futures.size());
}

TEST_CASE("HTTP requests own binary bodies query parameters and headers")
{
    HttpPeer peer;
    REQUIRE(peer.Start());
    ark::AFCHttpClientModule client;
    std::string body("first\0second", 12);
    const auto original_body = body;
    std::map<std::string, std::string> parameters{{"reserved", "a&= b"}, {"utf8", "\xe4\xb8\xad"}};
    std::vector<std::string> cookies{"session=original", "locale=en"};
    std::string content_type = "application/octet-stream";
    auto future =
        client.Post("localhost", peer.Port(), "/echo?existing=yes", &parameters, &body, &content_type, &cookies);
    body = "changed";
    parameters.clear();
    cookies.clear();
    content_type.clear();
    const auto result = AwaitHttp(future);
    CHECK(result.first);
    CHECK(result.second == original_body);
    const auto requests = peer.Requests();
    REQUIRE(requests.size() == 1);
    CHECK(requests.front().target() == "/echo?existing=yes&reserved=a%26%3D%20b&utf8=%E4%B8%AD");
    CHECK(requests.front().body() == original_body);
    CHECK(requests.front()[arknet::http::field::content_type] == "application/octet-stream");
    CHECK(requests.front()[arknet::http::field::cookie] == "session=original; locale=en");
    CHECK(requests.front()[arknet::http::field::host] == "localhost:" + std::to_string(peer.Port()));
    CHECK(requests.front()[arknet::http::field::content_length] == "12");
    CHECK_FALSE(requests.front().keep_alive());

    const std::map<std::string, std::string> empty_parameters;
    auto empty_query = client.Get("127.0.0.1", peer.Port(), "/unchanged", &empty_parameters);
    CHECK(AwaitHttp(empty_query).first);
    CHECK(peer.Requests().back().target() == "/unchanged");
    auto empty_target = client.Get("127.0.0.1", peer.Port(), "");
    CHECK(AwaitHttp(empty_target).first);
    CHECK(peer.Requests().back().target() == "/");
}

TEST_CASE("HTTP callbacks run once on the Update thread for both service overloads")
{
    HttpPeer peer;
    REQUIRE(peer.Start());
    HttpCallbacks callbacks;
    ark::AFCHttpClientModule client;
    const std::string body = "callback-body";
    for (int index = 0; index < 12; ++index)
    {
        auto callback = [&](HttpResult result) { callbacks.Receive(std::move(result)); };
        if (index % 2 == 0)
        {
            client.Post(callback, "localhost", peer.Port(), "/callback", nullptr, &body);
        }
        else
        {
            client.Post(callback, "localhost", std::to_string(peer.Port()), "/callback", nullptr, &body);
        }
    }
    REQUIRE(WaitHttp([&] { return peer.Count() == 12; }));
    std::this_thread::sleep_for(100ms);
    CHECK(callbacks.Results().empty());
    REQUIRE(WaitHttp([&] { return callbacks.Results().size() == 12; }, [&] { client.Update(); }));
    for (const auto& result : callbacks.Results())
    {
        CHECK(result.first);
        CHECK(result.second == body);
    }
    CHECK_FALSE(callbacks.wrong_thread.load());
    for (int index = 0; index < 3; ++index)
    {
        client.Update();
    }
    CHECK(callbacks.Results().size() == 12);
}

TEST_CASE("HTTP preserves non-200 failure reasons and arbitrary request verbs")
{
    HttpPeer peer;
    peer.respond = [](const HttpRequest& request, HttpResponse& response) {
        response.result(arknet::http::status::not_found);
        response.reason("Missing business route");
        response.body() = std::string(request.method_string()) + " error body";
    };
    REQUIRE(peer.Start());
    ark::AFCHttpClientModule client;
    auto missing = client.Get("127.0.0.1", peer.Port(), "/missing");
    auto put = client.AsyncRequest(arknet::http::verb::put, "127.0.0.1", peer.Port(), "/put");
    auto remove = client.AsyncRequest(arknet::http::verb::delete_, "127.0.0.1", peer.Port(), "/delete");
    for (auto* future : {&missing, &put, &remove})
    {
        const auto result = AwaitHttp(*future);
        CHECK_FALSE(result.first);
        CHECK(result.second == "Missing business route");
    }
    REQUIRE(peer.Count() == 3);
}

TEST_CASE("HTTP IO timeout completes futures and queues callback failure exactly once")
{
    HttpPeer peer(false);
    REQUIRE(peer.Start());
    HttpCallbacks callbacks;
    ark::AFCHttpClientModule client;
    const auto start = std::chrono::steady_clock::now();
    auto future = client.Get("127.0.0.1", peer.Port(), "/stall", nullptr, nullptr, 1, 1);
    client.Post([&](HttpResult result) { callbacks.Receive(std::move(result)); }, "127.0.0.1", peer.Port(), "/stall",
        nullptr, nullptr, nullptr, nullptr, 1, 1);
    REQUIRE(WaitHttp([&] { return peer.Count() == 2; }));
    const auto result = AwaitHttp(future);
    CHECK_FALSE(result.first);
    CHECK(result.second == arknet::error_code(asio::error::timed_out).message());
    CHECK(std::chrono::steady_clock::now() - start >= 900ms);
    CHECK(callbacks.Results().empty());
    REQUIRE(WaitHttp([&] { return callbacks.Results().size() == 1; }, [&] { client.Update(); }));
    CHECK_FALSE(callbacks.Results().front().first);
    CHECK(callbacks.Results().front().second == result.second);
    CHECK_FALSE(callbacks.wrong_thread.load());
    client.PreShut();
    client.Update();
    CHECK(callbacks.Results().size() == 1);
}

TEST_CASE("HTTP shutdown cancels all pending requests and settles futures without Update")
{
    HttpPeer peer(false);
    REQUIRE(peer.Start());
    HttpCallbacks callbacks;
    ark::AFCHttpClientModule client;
    std::vector<std::future<HttpResult>> futures;
    for (int index = 0; index < 24; ++index)
    {
        futures.push_back(client.Get("127.0.0.1", peer.Port(), "/pending"));
    }
    for (int index = 0; index < 8; ++index)
    {
        client.Post(
            [&](HttpResult result) { callbacks.Receive(std::move(result)); }, "127.0.0.1", peer.Port(), "/pending");
    }
    REQUIRE(WaitHttp([&] { return peer.Count() == 32; }));
    const auto start = std::chrono::steady_clock::now();
    REQUIRE(client.PreShut());
    CHECK(std::chrono::steady_clock::now() - start < 2s);
    CHECK(callbacks.Results().empty());
    const auto aborted = arknet::error_code(asio::error::operation_aborted).message();
    for (auto& future : futures)
    {
        CHECK(future.wait_for(0ms) == std::future_status::ready);
        const auto result = AwaitHttp(future);
        CHECK_FALSE(result.first);
        CHECK(result.second == aborted);
    }
    client.Update();
    REQUIRE(callbacks.Results().size() == 8);
    for (const auto& result : callbacks.Results())
    {
        CHECK_FALSE(result.first);
        CHECK(result.second == aborted);
    }
    CHECK_FALSE(callbacks.wrong_thread.load());
    CHECK(client.PreShut());
    auto rejected = client.Get("127.0.0.1", peer.Port(), "/after-shutdown");
    CHECK(rejected.wait_for(0ms) == std::future_status::ready);
    CHECK(AwaitHttp(rejected).second == aborted);
    client.Post(
        [&](HttpResult result) { callbacks.Receive(std::move(result)); }, "127.0.0.1", peer.Port(), "/after-shutdown");
    CHECK(callbacks.Results().size() == 8);
    client.Update();
    REQUIRE(callbacks.Results().size() == 9);
    CHECK(callbacks.Results().back().second == aborted);
    CHECK(peer.Count() == 32);

    std::future<HttpResult> destroyed;
    {
        auto owned = std::make_unique<ark::AFCHttpClientModule>();
        destroyed = owned->Get("127.0.0.1", peer.Port(), "/destroyed");
        REQUIRE(WaitHttp([&] { return peer.Count() == 33; }));
    }
    CHECK(destroyed.wait_for(0ms) == std::future_status::ready);
    CHECK(AwaitHttp(destroyed).second == aborted);
}

TEST_CASE("HTTP validates host headers target and timeout before network IO")
{
    HttpPeer peer;
    REQUIRE(peer.Start());
    ark::AFCHttpClientModule client;
    const std::string invalid_header = "application/json\r\nInjected: yes";
    auto host = client.Get("invalid/host", peer.Port(), "/");
    auto header = client.Post("127.0.0.1", peer.Port(), "/", nullptr, nullptr, &invalid_header);
    auto target = client.Get("127.0.0.1", peer.Port(), "/invalid path");
    auto timeout = client.Get("127.0.0.1", peer.Port(), "/", nullptr, nullptr, 0, 1);
    const std::string trace_body = "trace-body";
    auto trace =
        client.AsyncRequest(arknet::http::verb::trace, "127.0.0.1", peer.Port(), "/", nullptr, nullptr, &trace_body);
    for (auto* future : {&host, &header, &target, &timeout, &trace})
    {
        const auto result = AwaitHttp(*future);
        CHECK_FALSE(result.first);
        CHECK(result.second == arknet::error_code(asio::error::invalid_argument).message());
    }
    CHECK(peer.Count() == 0);
}

TEST_CASE("ARK HTTP server uses native routing and stops without signal delivery")
{
    ark::AFCHttpServerModule server;
    server.GetRouter()
        .add(arknet::http::verb::get, "/users/:id",
            [](const auto& context, auto& response) {
                response.set(arknet::http::field::content_type, "text/plain");
                response.body() = "user:" + std::string(context.param("id")) + ":" + std::string(context.query);
            })
        .add(arknet::http::verb::post, "/echo",
            [](const auto& context, auto& response) { response.body() = context.request.body(); });
    const auto port = HttpPort();
    REQUIRE(server.StartServer("127.0.0.1", port, 2));
    CHECK_FALSE(server.StartServer("127.0.0.1", port, 2));
    ark::AFCHttpClientModule client;
    auto get = client.Get("127.0.0.1", port, "/users/42?active=1");
    const auto result = AwaitHttp(get);
    CHECK(result.first);
    CHECK(result.second == "user:42:active=1");
    const std::string body("route\0body", 10);
    auto post = client.Post("127.0.0.1", port, "/echo", nullptr, &body);
    CHECK(AwaitHttp(post).second == body);
    auto missing = client.Get("127.0.0.1", port, "/missing");
    const auto missing_result = AwaitHttp(missing);
    CHECK_FALSE(missing_result.first);
    CHECK(missing_result.second == "Not Found");
    auto method = client.AsyncRequest(arknet::http::verb::delete_, "127.0.0.1", port, "/users/42");
    const auto method_result = AwaitHttp(method);
    CHECK_FALSE(method_result.first);
    CHECK(method_result.second == "Method Not Allowed");

    HttpRequest request(arknet::http::verb::get, "/", 11);
    request.keep_alive(true);
    const auto response = server.make_200<arknet::http::string_body>(request, "ready", "text/plain");
    CHECK(response.result() == arknet::http::status::ok);
    CHECK(response.body() == "ready");
    CHECK(response[arknet::http::field::server] == "ARK");
    CHECK(response[arknet::http::field::content_length] == "5");
    CHECK(response.keep_alive());

    ark::AFCHttpServerModule duplicate;
    CHECK_FALSE(duplicate.StartServer("127.0.0.1", port, 1));
    CHECK(duplicate.PreShut());
    CHECK(server.PreShut());
    REQUIRE(server.StartServer("127.0.0.1", port, 1));
    auto restarted = client.Get("127.0.0.1", port, "/users/7");
    CHECK(AwaitHttp(restarted).second == "user:7:");
    CHECK(server.PreShut());
}
