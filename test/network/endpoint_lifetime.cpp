// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <type_traits>

#include <arknet/arknet.hpp>

#include "doctest.h"
#include "net/include/AFCRDPClient.hpp"
#include "net/include/AFCRDPServer.hpp"
#include "net/include/AFCTCPClient.hpp"
#include "net/include/AFCTCPServer.hpp"
#include "net/include/AFCWebSocketClient.hpp"
#include "net/include/AFCWebSocketServer.hpp"

namespace {

using namespace std::chrono_literals;

struct Receiver
{
    void OnMessage(const ark::AFNetMsg*, ark::conv_id_t) {}
    void OnWebSocketMessage(const ark::AFNetMsg*, const ark::conv_id_t&) {}
    void OnEvent(const ark::AFNetEvent*) {}
};

class ReleasingHeader final : public ark::AFCSMsgHeader
{
public:
    explicit ReleasingHeader(std::function<void()> release)
        : release_(std::move(release))
    {
    }

    ark::AFIMsgHeader* New() const override
    {
        release_();
        return new ark::AFCSMsgHeader;
    }

private:
    std::function<void()> release_;
};

template<class Server, class Client>
void ReleaseServerFromReceive()
{
    Receiver receiver;
    std::shared_ptr<Server> server = std::make_shared<Server>(&receiver, &Receiver::OnMessage, &Receiver::OnEvent);
    std::atomic<bool> released{};
    auto header = std::make_shared<ReleasingHeader>([&] {
        server.reset();
        released.store(true);
    });
    asio::io_context context;
    asio::ip::tcp::acceptor listener(context, {asio::ip::tcp::v4(), 0});
    const auto port = listener.local_endpoint().port();
    listener.close();
    REQUIRE(server->StartServer(header, 11, "127.0.0.1", port, 2, 10, 30000));
    const std::weak_ptr weak = server;
    Client client;
    client.set_auto_reconnect(false);
    REQUIRE(client.start("127.0.0.1", port));
    header->SetMessageId(7);
    header->SetMessageLength(4);
    std::string frame;
    header->SerializeToString(frame);
    frame += "test";
    REQUIRE(client.async_send(frame));
    const auto deadline = std::chrono::steady_clock::now() + 5s;
    while ((!released.load() || !weak.expired() || client.is_started()) && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(1ms);
    }
    CHECK(released.load());
    CHECK(weak.expired());
    CHECK_FALSE(client.is_started());
    client.stop();
}

template<class Client, class Server>
void ReleaseClientFromReceive()
{
    Receiver receiver;
    std::shared_ptr<Client> client;
    if constexpr (std::is_same_v<Client, ark::AFCWebSocketClient>)
    {
        client = std::make_shared<Client>(&receiver, &Receiver::OnWebSocketMessage, &Receiver::OnEvent);
    }
    else
    {
        client = std::make_shared<Client>(&receiver, &Receiver::OnMessage, &Receiver::OnEvent);
    }
    std::atomic<bool> released{};
    auto header = std::make_shared<ReleasingHeader>([&] {
        client.reset();
        released.store(true);
    });
    header->SetMessageId(7);
    header->SetMessageLength(4);
    std::string frame;
    header->SerializeToString(frame);
    frame += "test";
    Server server(1024, 1024 * 1024, 2);
    REQUIRE(server.start("127.0.0.1", 0));
    REQUIRE(client->StartClient(header, 11, "127.0.0.1", server.listen_port()));
    const std::weak_ptr weak = client;
    const auto connected = std::chrono::steady_clock::now() + 5s;
    while (server.get_session_count() == 0 && std::chrono::steady_clock::now() < connected)
    {
        std::this_thread::sleep_for(1ms);
    }
    REQUIRE(server.get_session_count() == 1);
    server.foreach_session([&](auto& session) { session->async_send(frame); });
    const auto deadline = std::chrono::steady_clock::now() + 5s;
    while ((!released.load() || !weak.expired() || server.get_session_count() != 0) &&
           std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(1ms);
    }
    CHECK(released.load());
    CHECK(weak.expired());
    CHECK(server.get_session_count() == 0);
    server.stop();
}

} // namespace

TEST_CASE("Owned endpoints survive the last ARK owner being released inside receive callbacks")
{
    SUBCASE("TCP server")
    {
        ReleaseServerFromReceive<ark::AFCTCPServer, arknet::tcp_client>();
    }
    SUBCASE("WebSocket server")
    {
        ReleaseServerFromReceive<ark::AFCWebSocketServer, arknet::ws_client>();
    }
    SUBCASE("RUDP server")
    {
        ReleaseServerFromReceive<ark::AFCRDPServer, arknet::rudp_client>();
    }
    SUBCASE("TCP client")
    {
        ReleaseClientFromReceive<ark::AFCTCPClient, arknet::tcp_server>();
    }
    SUBCASE("WebSocket client")
    {
        ReleaseClientFromReceive<ark::AFCWebSocketClient, arknet::ws_server>();
    }
    SUBCASE("RUDP client")
    {
        ReleaseClientFromReceive<ark::AFCRDPClient, arknet::rudp_server>();
    }
}
