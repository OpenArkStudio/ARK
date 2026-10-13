// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.

#include <algorithm>
#include <atomic>
#include <chrono>
#include <concepts>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#include <arknet/tcp/tcp_client.hpp>
#include <arknet/tcp/tcp_server.hpp>
#include <arknet/websocket/ws_client.hpp>
#include <arknet/websocket/ws_server.hpp>

#include "doctest.h"
#include "net/include/AFCTCPClient.hpp"
#include "net/include/AFCTCPServer.hpp"
#include "net/include/AFCWebSocketClient.hpp"
#include "net/include/AFCWebSocketServer.hpp"

namespace {
using namespace std::chrono_literals;

struct Message
{
    ark::msg_id_t id;
    ark::conv_id_t session;
    std::string body;
    ark::guid_t actor;
    ark::bus_id_t source;
    ark::bus_id_t destination;
};

struct Event
{
    ark::AFNetEventType type;
    ark::conv_id_t session;
};

struct Receiver
{
    const std::thread::id dispatch_thread = std::this_thread::get_id();
    std::atomic<bool> wrong_thread{false};
    std::mutex mutex;
    std::vector<Message> messages;
    std::vector<Event> events;
    std::function<void(ark::conv_id_t)> on_message;

    void OnMessage(const ark::AFNetMsg* message, ark::conv_id_t session)
    {
        if (std::this_thread::get_id() != dispatch_thread)
        {
            wrong_thread.store(true);
        }
        {
            std::lock_guard guard(mutex);
            messages.push_back({message->GetMsgId(), session,
                message->GetMsgLength() == 0 ? std::string{}
                                             : std::string(message->GetMsgData(), message->GetMsgLength()),
                message->GetActorId(), message->GetSrcBus(), message->GetDstBus()});
        }
        if (on_message)
        {
            on_message(session);
        }
    }

    void OnWebSocketMessage(const ark::AFNetMsg* message, const ark::conv_id_t& session)
    {
        OnMessage(message, session);
    }

    void OnEvent(const ark::AFNetEvent* event)
    {
        if (std::this_thread::get_id() != dispatch_thread)
        {
            wrong_thread.store(true);
        }
        std::lock_guard guard(mutex);
        events.push_back({event->GetType(), event->GetId()});
    }

    std::vector<Message> Messages()
    {
        std::lock_guard guard(mutex);
        return messages;
    }

    std::vector<Event> Events()
    {
        std::lock_guard guard(mutex);
        return events;
    }

    size_t Count(ark::AFNetEventType type)
    {
        const auto snapshot = Events();
        return std::count_if(
            snapshot.begin(), snapshot.end(), [type](const Event& event) { return event.type == type; });
    }
};

template<typename Predicate, typename Pump>
requires std::invocable<Pump> bool Wait(Predicate predicate, Pump pump, std::chrono::milliseconds timeout = 3000ms)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    do
    {
        pump();
        if (predicate())
        {
            return true;
        }
        std::this_thread::sleep_for(2ms);
    } while (std::chrono::steady_clock::now() < deadline);
    return false;
}

template<typename Predicate>
bool Wait(Predicate predicate, std::chrono::milliseconds timeout = 3000ms)
{
    return Wait(std::move(predicate), [] {}, timeout);
}

uint16_t AvailablePort()
{
    asio::io_context context;
    asio::ip::tcp::acceptor acceptor(context, {asio::ip::make_address("127.0.0.1"), 0});
    return acceptor.local_endpoint().port();
}

std::string Frame(ark::AFIMsgHeader& header, ark::msg_id_t id, const std::string& body)
{
    header.SetMessageId(id);
    header.SetMessageLength(static_cast<uint32_t>(body.size()));
    std::string bytes;
    header.SerializeToString(bytes);
    bytes += body;
    return bytes;
}

template<typename Client>
std::shared_ptr<Client> CreateClient(Receiver& receiver)
{
    if constexpr (std::is_same_v<Client, ark::AFCWebSocketClient>)
    {
        return std::make_shared<Client>(&receiver, &Receiver::OnWebSocketMessage, &Receiver::OnEvent);
    }
    else
    {
        return std::make_shared<Client>(&receiver, &Receiver::OnMessage, &Receiver::OnEvent);
    }
}

template<typename Wrapper, typename Transport>
void ReconnectKeepsEvents()
{
    Receiver receiver;
    std::mutex mutex;
    std::shared_ptr<typename Transport::session_type> peer;
    std::atomic<size_t> connections{0};
    Transport server(1024, 1024 * 1024, 2);
    server.bind_connect([&](auto& session) {
        std::lock_guard guard(mutex);
        peer = session;
        ++connections;
    });
    REQUIRE(server.start("127.0.0.1", 0));
    auto client = CreateClient<Wrapper>(receiver);
    REQUIRE(client->StartClient(std::make_shared<ark::AFCSMsgHeader>(), 11, "127.0.0.1", server.listen_port()));
    REQUIRE(Wait([&] { return connections.load() == 1; }));

    std::shared_ptr<typename Transport::session_type> original;
    {
        std::lock_guard guard(mutex);
        original = peer;
    }
    original->stop();
    REQUIRE(Wait([&] { return connections.load() == 2; }, 8000ms));
    CHECK(receiver.Events().empty());
    REQUIRE(Wait([&] { return receiver.Count(ark::AFNetEventType::CONNECT) == 2; }, [&] { client->Update(); }));
    const auto events = receiver.Events();
    REQUIRE(events.size() == 3);
    CHECK(events[0].type == ark::AFNetEventType::CONNECT);
    CHECK(events[1].type == ark::AFNetEventType::DISCONNECT);
    CHECK(events[2].type == ark::AFNetEventType::CONNECT);
    CHECK(events[0].session == events[1].session);
    CHECK(events[2].session != events[0].session);
    CHECK_FALSE(receiver.wrong_thread.load());
    client->Shutdown();
    client->Update();
}

template<typename Wrapper, typename Transport>
void ReentrantClose()
{
    Receiver receiver;
    auto server = std::make_shared<Wrapper>(&receiver, &Receiver::OnMessage, &Receiver::OnEvent);
    receiver.on_message = [weak = std::weak_ptr(server)](ark::conv_id_t session) {
        if (const auto owner = weak.lock())
        {
            owner->CloseSession(session);
        }
    };
    const auto port = AvailablePort();
    REQUIRE(server->StartServer(std::make_shared<ark::AFCSMsgHeader>(), 11, "127.0.0.1", port, 2, 10, 30000));
    Transport client;
    if constexpr (std::is_same_v<Transport, arknet::ws_client>)
    {
        client.bind_connect([&] {
            if (!arknet::get_last_error())
                client.ws_stream().binary(true);
        });
    }
    REQUIRE(client.start("127.0.0.1", port));
    ark::AFCSMsgHeader header;
    const auto bytes = Frame(header, 7, "close in Update callback");
    REQUIRE(client.send(bytes) == bytes.size());
    REQUIRE(Wait([&] { return receiver.Count(ark::AFNetEventType::DISCONNECT) == 1; }, [&] { server->Update(); }));
    REQUIRE(receiver.Messages().size() == 1);
    REQUIRE(Wait([&] { return !client.is_started(); }));
    CHECK_FALSE(receiver.wrong_thread.load());
}

template<typename Wrapper, typename Transport>
void LimitAndTimeout()
{
    Receiver receiver;
    auto server = std::make_shared<Wrapper>(&receiver, &Receiver::OnMessage, &Receiver::OnEvent);
    const auto port = AvailablePort();
    REQUIRE(server->StartServer(std::make_shared<ark::AFCSMsgHeader>(), 11, "127.0.0.1", port, 2, 1, 750));
    Transport first;
    Transport second;
    REQUIRE(first.start("127.0.0.1", port));
    REQUIRE(Wait([&] { return receiver.Count(ark::AFNetEventType::CONNECT) == 1; }, [&] { server->Update(); }));
    second.start("127.0.0.1", port);
    REQUIRE(Wait([&] { return !second.is_started(); }, [&] { server->Update(); }, 500ms));
    CHECK(receiver.Count(ark::AFNetEventType::CONNECT) == 1);
    REQUIRE(Wait([&] { return receiver.Count(ark::AFNetEventType::DISCONNECT) == 1; }, [&] { server->Update(); }));
    REQUIRE(Wait([&] { return !first.is_started(); }));
    CHECK_FALSE(receiver.wrong_thread.load());
    Transport replacement;
    REQUIRE(replacement.start("127.0.0.1", port));
    REQUIRE(Wait([&] { return receiver.Count(ark::AFNetEventType::CONNECT) == 2; }, [&] { server->Update(); }));
}

template<typename Wrapper, typename Transport>
void RejectOversizedFrame()
{
    Receiver receiver;
    auto server = std::make_shared<Wrapper>(&receiver, &Receiver::OnMessage, &Receiver::OnEvent);
    const auto port = AvailablePort();
    REQUIRE(server->StartServer(std::make_shared<ark::AFCSMsgHeader>(), 11, "127.0.0.1", port, 2, 10, 30000));
    Transport client;
    if constexpr (std::is_same_v<Transport, arknet::ws_client>)
    {
        client.bind_connect([&] {
            if (!arknet::get_last_error())
            {
                client.ws_stream().binary(true);
            }
        });
    }
    REQUIRE(client.start("127.0.0.1", port));
    ark::AFCSMsgHeader header;
    header.SetMessageId(7);
    header.SetMessageLength(ark::ARK_MSG_MAX_LENGTH + 1);
    std::string bytes;
    header.SerializeToString(bytes);
    REQUIRE(client.send(bytes) == bytes.size());
    REQUIRE(Wait([&] { return receiver.Count(ark::AFNetEventType::DISCONNECT) == 1; }, [&] { server->Update(); }));
    CHECK(receiver.Messages().empty());
    REQUIRE(Wait([&] { return !client.is_started(); }));
}

class ReconnectDuringSerialization : public ark::AFCSMsgHeader
{
public:
    explicit ReconnectDuringSerialization(std::function<void()> reconnect)
        : reconnect_(std::move(reconnect))
    {
    }

    void SerializeToString(std::string& bytes) override
    {
        reconnect_();
        ark::AFCSMsgHeader::SerializeToString(bytes);
    }

private:
    std::function<void()> reconnect_;
};

template<typename Wrapper, typename Transport>
void StaleSessionCannotSendOrCloseReplacement()
{
    Receiver receiver;
    std::mutex peer_mutex;
    std::shared_ptr<typename Transport::session_type> peer;
    std::mutex wire_mutex;
    std::string wire;
    std::atomic<size_t> connections{0};
    Transport server(1024, 1024 * 1024, 2);
    server.bind_connect([&](auto& session) {
        if constexpr (std::is_same_v<Transport, arknet::ws_server>)
        {
            session->ws_stream().binary(true);
        }
        std::lock_guard guard(peer_mutex);
        peer = session;
        ++connections;
    });
    server.bind_recv([&](auto& session, std::string_view data) {
        {
            std::lock_guard guard(wire_mutex);
            wire += data;
        }
        session->async_send(data);
    });
    REQUIRE(server.start("127.0.0.1", 0));
    auto client = CreateClient<Wrapper>(receiver);
    REQUIRE(client->StartClient(std::make_shared<ark::AFCSMsgHeader>(), 11, "127.0.0.1", server.listen_port()));
    REQUIRE(Wait([&] { return receiver.Count(ark::AFNetEventType::CONNECT) == 1; }, [&] { client->Update(); }));
    REQUIRE(Wait([&] { return connections.load() == 1; }));
    const auto old_id = receiver.Events().front().session;
    std::shared_ptr<typename Transport::session_type> original;
    {
        std::lock_guard guard(peer_mutex);
        original = peer;
    }
    REQUIRE(original != nullptr);

    bool replaced = false;
    ReconnectDuringSerialization stale_header([&] {
        original->stop();
        replaced =
            Wait([&] { return receiver.Count(ark::AFNetEventType::CONNECT) == 2; }, [&] { client->Update(); }, 8000ms);
    });
    const std::string stale_body = "belongs to the closed session";
    stale_header.SetMessageId(71);
    stale_header.SetMessageLength(static_cast<uint32_t>(stale_body.size()));
    CHECK_FALSE(client->SendMsg(&stale_header, stale_body.data(), old_id));
    REQUIRE(replaced);
    REQUIRE(connections.load() == 2);
    const auto current_id = receiver.Events().back().session;
    REQUIRE(current_id != old_id);
    client->CloseSession(old_id);
    client->CloseSession(0);

    ark::AFCSMsgHeader current_header;
    const std::string current_body = "belongs to the current session";
    const auto expected = Frame(current_header, 72, current_body);
    REQUIRE(client->SendMsg(&current_header, current_body.data(), 0));
    REQUIRE(Wait([&] { return receiver.Messages().size() == 1; }, [&] { client->Update(); }));
    CHECK(receiver.Messages().front().body == current_body);
    CHECK(receiver.Messages().front().session == current_id);
    {
        std::lock_guard guard(wire_mutex);
        CHECK(wire == expected);
    }
    client->CloseSession(current_id);
    REQUIRE(Wait([&] { return receiver.Count(ark::AFNetEventType::DISCONNECT) == 2; }, [&] { client->Update(); }));
    CHECK_FALSE(client->SendMsg(&current_header, current_body.data(), current_id));
    CHECK_FALSE(receiver.wrong_thread.load());
}
} // namespace

TEST_CASE("TCP server preserves fragmented and coalesced ARK frames")
{
    std::shared_ptr<ark::AFIMsgHeader> header;
    SUBCASE("7-byte client header")
    {
        header = std::make_shared<ark::AFCSMsgHeader>();
    }
    SUBCASE("23-byte server header")
    {
        header = std::make_shared<ark::AFSSMsgHeader>();
        header->SetActorId(12345678);
        header->SetSourceBusId(11);
        header->SetDestBusId(22);
    }
    Receiver receiver;
    auto server = std::make_shared<ark::AFCTCPServer>(&receiver, &Receiver::OnMessage, &Receiver::OnEvent);
    const auto port = AvailablePort();
    REQUIRE(server->StartServer(header, 11, "127.0.0.1", port, 2, 10, 30000));
    std::mutex wire_mutex;
    std::string wire;
    arknet::tcp_client client;
    client.bind_recv([&](std::string_view data) {
        std::lock_guard guard(wire_mutex);
        wire += data;
    });
    REQUIRE(client.start("127.0.0.1", port));
    const std::string body(4096, '\xA5');
    const auto first = Frame(*header, 7, body);
    const auto second = Frame(*header, 8, "");
    REQUIRE(client.send(first.data(), 3) == 3);
    const auto rest = first.substr(3) + second;
    REQUIRE(client.send(rest) == rest.size());
    std::this_thread::sleep_for(30ms);
    CHECK(receiver.Events().empty());
    CHECK(receiver.Messages().empty());
    REQUIRE(Wait([&] { return receiver.Messages().size() == 2; }, [&] { server->Update(); }));
    const auto messages = receiver.Messages();
    CHECK(messages[0].body == body);
    CHECK(messages[0].id == 7);
    CHECK(messages[1].body.empty());
    CHECK(messages[1].id == 8);
    CHECK(messages[0].actor == header->ActorId());
    CHECK(messages[0].source == header->SourceBusId());
    CHECK(messages[0].destination == header->DestBusId());
    CHECK_FALSE(receiver.wrong_thread.load());
    header->SetMessageId(9);
    header->SetMessageLength(static_cast<uint32_t>(body.size()));
    std::string expected;
    header->SerializeToString(expected);
    expected += body;
    REQUIRE(server->SendMsg(header.get(), body.data(), messages[0].session));
    REQUIRE(Wait([&] {
        std::lock_guard guard(wire_mutex);
        return wire.size() >= expected.size();
    }));
    {
        std::lock_guard guard(wire_mutex);
        CHECK(wire == expected);
    }
    CHECK_FALSE(server->SendMsg(header.get(), body.data(), messages[0].session + 1));
}

TEST_CASE("WebSocket server sends binary ARK frames")
{
    Receiver receiver;
    auto server = std::make_shared<ark::AFCWebSocketServer>(&receiver, &Receiver::OnMessage, &Receiver::OnEvent);
    const auto port = AvailablePort();
    auto header = std::make_shared<ark::AFCSMsgHeader>();
    REQUIRE(server->StartServer(header, 11, "127.0.0.1", port, 2, 10, 30000));
    std::atomic<bool> binary{false};
    std::mutex mutex;
    std::string reply;
    arknet::ws_client client;
    client.bind_connect([&] {
        if (!arknet::get_last_error())
            client.ws_stream().binary(true);
    });
    client.bind_recv([&](std::string_view data) {
        binary.store(client.ws_stream().got_binary());
        std::lock_guard guard(mutex);
        reply.assign(data);
    });
    REQUIRE(client.start("127.0.0.1", port));
    const std::string body(4096, '\xFF');
    const auto bytes = Frame(*header, 17, body);
    REQUIRE(client.send(bytes) == bytes.size());
    CHECK(receiver.Messages().empty());
    REQUIRE(Wait([&] { return receiver.Messages().size() == 1; }, [&] { server->Update(); }));
    const auto message = receiver.Messages().front();
    CHECK(message.body == body);
    REQUIRE(server->SendMsg(header.get(), body.data(), message.session));
    REQUIRE(Wait([&] {
        std::lock_guard guard(mutex);
        return reply.size() == bytes.size();
    }));
    CHECK(binary.load());
    {
        std::lock_guard guard(mutex);
        CHECK(reply == bytes);
    }
    CHECK_FALSE(receiver.wrong_thread.load());
}

TEST_CASE("TCP and WebSocket clients preserve ARK frames and use Update dispatch")
{
    SUBCASE("TCP")
    {
        Receiver receiver;
        arknet::tcp_server server(1024, 1024 * 1024, 2);
        server.bind_recv([](auto& session, std::string_view data) { session->async_send(data); });
        REQUIRE(server.start("127.0.0.1", 0));
        auto client = CreateClient<ark::AFCTCPClient>(receiver);
        auto header = std::make_shared<ark::AFSSMsgHeader>();
        header->SetActorId(987654321);
        header->SetSourceBusId(11);
        header->SetDestBusId(22);
        REQUIRE(client->StartClient(header, 11, "127.0.0.1", server.listen_port()));
        const std::string body(4096, '\xFE');
        Frame(*header, 71, body);
        REQUIRE(client->SendMsg(header.get(), body.data(), 0));
        CHECK(receiver.Events().empty());
        REQUIRE(Wait([&] { return receiver.Messages().size() == 1; }, [&] { client->Update(); }));
        const auto message = receiver.Messages().front();
        CHECK(message.body == body);
        CHECK(message.actor == 987654321);
        CHECK(message.source == 11);
        CHECK(message.destination == 22);
        CHECK_FALSE(receiver.wrong_thread.load());
        const std::weak_ptr weak = client;
        client.reset();
        CHECK(weak.expired());
    }
    SUBCASE("WebSocket")
    {
        Receiver receiver;
        std::atomic<bool> binary{false};
        arknet::ws_server server(1024, 1024 * 1024, 2);
        server.bind_connect([](auto& session) { session->ws_stream().binary(true); });
        server.bind_recv([&](auto& session, std::string_view data) {
            binary.store(session->ws_stream().got_binary());
            session->async_send(data);
        });
        REQUIRE(server.start("127.0.0.1", 0));
        auto client = CreateClient<ark::AFCWebSocketClient>(receiver);
        auto header = std::make_shared<ark::AFCSMsgHeader>();
        REQUIRE(client->StartClient(header, 11, "127.0.0.1", server.listen_port()));
        const std::string body(4096, '\xFE');
        Frame(*header, 71, body);
        REQUIRE(client->SendMsg(header.get(), body.data(), 0));
        CHECK(receiver.Events().empty());
        REQUIRE(Wait([&] { return receiver.Messages().size() == 1; }, [&] { client->Update(); }));
        CHECK(receiver.Messages().front().body == body);
        CHECK(binary.load());
        CHECK_FALSE(receiver.wrong_thread.load());
        const std::weak_ptr weak = client;
        client.reset();
        CHECK(weak.expired());
    }
}

TEST_CASE("TCP and WebSocket callbacks can close their own session")
{
    SUBCASE("TCP")
    {
        ReentrantClose<ark::AFCTCPServer, arknet::tcp_client>();
    }
    SUBCASE("WebSocket")
    {
        ReentrantClose<ark::AFCWebSocketServer, arknet::ws_client>();
    }
}

TEST_CASE("TCP and WebSocket enforce connection limits and idle timeouts")
{
    SUBCASE("TCP")
    {
        LimitAndTimeout<ark::AFCTCPServer, arknet::tcp_client>();
    }
    SUBCASE("WebSocket")
    {
        LimitAndTimeout<ark::AFCWebSocketServer, arknet::ws_client>();
    }
}

TEST_CASE("TCP and WebSocket retain disconnect events when reconnect precedes Update")
{
    SUBCASE("TCP")
    {
        ReconnectKeepsEvents<ark::AFCTCPClient, arknet::tcp_server>();
    }
    SUBCASE("WebSocket")
    {
        ReconnectKeepsEvents<ark::AFCWebSocketClient, arknet::ws_server>();
    }
}

TEST_CASE("TCP and WebSocket close oversized ARK frames through normal disconnect dispatch")
{
    SUBCASE("TCP")
    {
        RejectOversizedFrame<ark::AFCTCPServer, arknet::tcp_client>();
    }
    SUBCASE("WebSocket")
    {
        RejectOversizedFrame<ark::AFCWebSocketServer, arknet::ws_client>();
    }
}

TEST_CASE("TCP and WebSocket reject old session operations after reconnect during serialization")
{
    SUBCASE("TCP")
    {
        StaleSessionCannotSendOrCloseReplacement<ark::AFCTCPClient, arknet::tcp_server>();
    }
    SUBCASE("WebSocket")
    {
        StaleSessionCannotSendOrCloseReplacement<ark::AFCWebSocketClient, arknet::ws_server>();
    }
}

TEST_CASE("TCP and WebSocket wrappers can be destroyed before Start")
{
    Receiver receiver;
    auto tcp_client = CreateClient<ark::AFCTCPClient>(receiver);
    auto ws_client = CreateClient<ark::AFCWebSocketClient>(receiver);
    auto tcp_server = std::make_shared<ark::AFCTCPServer>(&receiver, &Receiver::OnMessage, &Receiver::OnEvent);
    auto ws_server = std::make_shared<ark::AFCWebSocketServer>(&receiver, &Receiver::OnMessage, &Receiver::OnEvent);
    tcp_client.reset();
    ws_client.reset();
    tcp_server.reset();
    ws_server.reset();
    CHECK(receiver.Events().empty());
}
