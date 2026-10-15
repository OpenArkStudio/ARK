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

#include "ark/net/tcp_client.hpp"
#include "ark/net/tcp_server.hpp"
#include "ark/net/websocket_client.hpp"
#include "ark/net/websocket_server.hpp"
#include "doctest.h"

namespace {
using namespace std::chrono_literals;

struct message
{
    ark::msg_id_t id;
    ark::conv_id_t session;
    std::string body;
    ark::guid_t actor;
    ark::bus_id_t source;
    ark::bus_id_t destination;
};

struct event
{
    ark::net_event_type type;
    ark::conv_id_t session;
};

struct receiver
{
    const std::thread::id dispatch_thread = std::this_thread::get_id();
    std::atomic<bool> wrong_thread{false};
    std::mutex mutex;
    std::vector<message> messages;
    std::vector<event> events;
    std::function<void(ark::conv_id_t)> on_message;

    void OnMessage(const ark::net_msg* message, ark::conv_id_t session) {
        if (std::this_thread::get_id() != dispatch_thread)
        {
            wrong_thread.store(true);
        }
        {
            std::lock_guard guard(mutex);
            messages.push_back({message->get_msg_id(), session,
                message->get_msg_length() == 0 ? std::string{}
                                               : std::string(message->get_msg_data(), message->get_msg_length()),
                message->get_actor_id(), message->get_src_bus(), message->get_dst_bus()});
        }
        if (on_message)
        {
            on_message(session);
        }
    }

    void OnWebSocketMessage(const ark::net_msg* message, const ark::conv_id_t& session) {
        OnMessage(message, session);
    }

    void OnEvent(const ark::net_event* event) {
        if (std::this_thread::get_id() != dispatch_thread)
        {
            wrong_thread.store(true);
        }
        std::lock_guard guard(mutex);
        events.push_back({event->get_type(), event->get_id()});
    }

    std::vector<message> Messages() {
        std::lock_guard guard(mutex);
        return messages;
    }

    std::vector<event> Events() {
        std::lock_guard guard(mutex);
        return events;
    }

    size_t count(ark::net_event_type type) {
        const auto snapshot = Events();
        return std::count_if(
            snapshot.begin(), snapshot.end(), [type](const event& event) { return event.type == type; });
    }
};

template<typename Predicate, typename Pump>
    requires std::invocable<Pump>
bool Wait(Predicate predicate, Pump pump, std::chrono::milliseconds timeout = 3000ms) {
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
bool Wait(Predicate predicate, std::chrono::milliseconds timeout = 3000ms) {
    return Wait(std::move(predicate), [] {}, timeout);
}

uint16_t AvailablePort() {
    asio::io_context context;
    asio::ip::tcp::acceptor acceptor(context, {asio::ip::make_address("127.0.0.1"), 0});
    return acceptor.local_endpoint().port();
}

std::string Frame(ark::msg_header& header, ark::msg_id_t id, const std::string& body) {
    header.set_message_id(id);
    header.set_message_length(static_cast<uint32_t>(body.size()));
    std::string bytes;
    header.serialize_to_string(bytes);
    bytes += body;
    return bytes;
}

template<typename Client>
std::shared_ptr<Client> CreateClient(receiver& receiver) {
    if constexpr (std::is_same_v<Client, ark::websocket_client>)
    {
        return std::make_shared<Client>(&receiver, &receiver::OnWebSocketMessage, &receiver::OnEvent);
    }
    else
    {
        return std::make_shared<Client>(&receiver, &receiver::OnMessage, &receiver::OnEvent);
    }
}

template<typename Wrapper, typename Transport>
void ReconnectKeepsEvents() {
    receiver receiver;
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
    REQUIRE(client->start_client(std::make_shared<ark::s_msg_header>(), 11, "127.0.0.1", server.listen_port()));
    REQUIRE(Wait([&] { return connections.load() == 1; }));

    std::shared_ptr<typename Transport::session_type> original;
    {
        std::lock_guard guard(mutex);
        original = peer;
    }
    original->stop();
    REQUIRE(Wait([&] { return connections.load() == 2; }, 8000ms));
    CHECK(receiver.Events().empty());
    REQUIRE(Wait([&] { return receiver.count(ark::net_event_type::CONNECT) == 2; }, [&] { client->update(); }));
    const auto events = receiver.Events();
    REQUIRE(events.size() == 3);
    CHECK(events[0].type == ark::net_event_type::CONNECT);
    CHECK(events[1].type == ark::net_event_type::DISCONNECT);
    CHECK(events[2].type == ark::net_event_type::CONNECT);
    CHECK(events[0].session == events[1].session);
    CHECK(events[2].session != events[0].session);
    CHECK_FALSE(receiver.wrong_thread.load());
    client->shutdown();
    client->update();
}

template<typename Wrapper, typename Transport>
void ReentrantClose() {
    receiver receiver;
    auto server = std::make_shared<Wrapper>(&receiver, &receiver::OnMessage, &receiver::OnEvent);
    receiver.on_message = [weak = std::weak_ptr(server)](ark::conv_id_t session) {
        if (const auto owner = weak.lock())
        {
            owner->close_session(session);
        }
    };
    const auto port = AvailablePort();
    REQUIRE(server->start_server(std::make_shared<ark::s_msg_header>(), 11, "127.0.0.1", port, 2, 10, 30000));
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
    ark::s_msg_header header;
    const auto bytes = Frame(header, 7, "close in Update callback");
    REQUIRE(client.send(bytes) == bytes.size());
    REQUIRE(Wait([&] { return receiver.count(ark::net_event_type::DISCONNECT) == 1; }, [&] { server->update(); }));
    REQUIRE(receiver.Messages().size() == 1);
    REQUIRE(Wait([&] { return !client.is_started(); }));
    CHECK_FALSE(receiver.wrong_thread.load());
}

template<typename Wrapper, typename Transport>
void LimitAndTimeout() {
    receiver receiver;
    auto server = std::make_shared<Wrapper>(&receiver, &receiver::OnMessage, &receiver::OnEvent);
    const auto port = AvailablePort();
    REQUIRE(server->start_server(std::make_shared<ark::s_msg_header>(), 11, "127.0.0.1", port, 2, 1, 750));
    Transport first;
    Transport second;
    REQUIRE(first.start("127.0.0.1", port));
    REQUIRE(Wait([&] { return receiver.count(ark::net_event_type::CONNECT) == 1; }, [&] { server->update(); }));
    second.start("127.0.0.1", port);
    REQUIRE(Wait([&] { return !second.is_started(); }, [&] { server->update(); }, 500ms));
    CHECK(receiver.count(ark::net_event_type::CONNECT) == 1);
    REQUIRE(Wait([&] { return receiver.count(ark::net_event_type::DISCONNECT) == 1; }, [&] { server->update(); }));
    REQUIRE(Wait([&] { return !first.is_started(); }));
    CHECK_FALSE(receiver.wrong_thread.load());
    Transport replacement;
    REQUIRE(replacement.start("127.0.0.1", port));
    REQUIRE(Wait([&] { return receiver.count(ark::net_event_type::CONNECT) == 2; }, [&] { server->update(); }));
}

template<typename Wrapper, typename Transport>
void RejectOversizedFrame() {
    receiver receiver;
    auto server = std::make_shared<Wrapper>(&receiver, &receiver::OnMessage, &receiver::OnEvent);
    const auto port = AvailablePort();
    REQUIRE(server->start_server(std::make_shared<ark::s_msg_header>(), 11, "127.0.0.1", port, 2, 10, 30000));
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
    ark::s_msg_header header;
    header.set_message_id(7);
    header.set_message_length(ark::ARK_MSG_MAX_LENGTH + 1);
    std::string bytes;
    header.serialize_to_string(bytes);
    REQUIRE(client.send(bytes) == bytes.size());
    REQUIRE(Wait([&] { return receiver.count(ark::net_event_type::DISCONNECT) == 1; }, [&] { server->update(); }));
    CHECK(receiver.Messages().empty());
    REQUIRE(Wait([&] { return !client.is_started(); }));
}

class reconnect_during_serialization : public ark::s_msg_header
{
public:
    explicit reconnect_during_serialization(std::function<void()> reconnect)
        : reconnect_(std::move(reconnect)) {}

    void serialize_to_string(std::string& bytes) override {
        reconnect_();
        ark::s_msg_header::serialize_to_string(bytes);
    }

private:
    std::function<void()> reconnect_;
};

template<typename Wrapper, typename Transport>
void StaleSessionCannotSendOrCloseReplacement() {
    receiver receiver;
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
    REQUIRE(client->start_client(std::make_shared<ark::s_msg_header>(), 11, "127.0.0.1", server.listen_port()));
    REQUIRE(Wait([&] { return receiver.count(ark::net_event_type::CONNECT) == 1; }, [&] { client->update(); }));
    REQUIRE(Wait([&] { return connections.load() == 1; }));
    const auto old_id = receiver.Events().front().session;
    std::shared_ptr<typename Transport::session_type> original;
    {
        std::lock_guard guard(peer_mutex);
        original = peer;
    }
    REQUIRE(original != nullptr);

    bool replaced = false;
    reconnect_during_serialization stale_header([&] {
        original->stop();
        replaced =
            Wait([&] { return receiver.count(ark::net_event_type::CONNECT) == 2; }, [&] { client->update(); }, 8000ms);
    });
    const std::string stale_body = "belongs to the closed session";
    stale_header.set_message_id(71);
    stale_header.set_message_length(static_cast<uint32_t>(stale_body.size()));
    CHECK_FALSE(client->send_msg(&stale_header, stale_body.data(), old_id));
    REQUIRE(replaced);
    REQUIRE(Wait([&] { return connections.load() == 2; }));
    const auto current_id = receiver.Events().back().session;
    REQUIRE(current_id != old_id);
    client->close_session(old_id);
    client->close_session(0);

    ark::s_msg_header current_header;
    const std::string current_body = "belongs to the current session";
    const auto expected = Frame(current_header, 72, current_body);
    REQUIRE(client->send_msg(&current_header, current_body.data(), 0));
    REQUIRE(Wait([&] { return receiver.Messages().size() == 1; }, [&] { client->update(); }));
    CHECK(receiver.Messages().front().body == current_body);
    CHECK(receiver.Messages().front().session == current_id);
    {
        std::lock_guard guard(wire_mutex);
        CHECK(wire == expected);
    }
    client->close_session(current_id);
    REQUIRE(Wait([&] { return receiver.count(ark::net_event_type::DISCONNECT) == 2; }, [&] { client->update(); }));
    CHECK_FALSE(client->send_msg(&current_header, current_body.data(), current_id));
    CHECK_FALSE(receiver.wrong_thread.load());
}
} // namespace

TEST_CASE("TCP server preserves fragmented and coalesced ARK frames") {
    std::shared_ptr<ark::msg_header> header;
    SUBCASE("7-byte client header") {
        header = std::make_shared<ark::s_msg_header>();
    }
    SUBCASE("23-byte server header") {
        header = std::make_shared<ark::ss_msg_header>();
        header->set_actor_id(12345678);
        header->set_source_bus_id(11);
        header->set_dest_bus_id(22);
    }
    receiver receiver;
    auto server = std::make_shared<ark::tcp_server>(&receiver, &receiver::OnMessage, &receiver::OnEvent);
    const auto port = AvailablePort();
    REQUIRE(server->start_server(header, 11, "127.0.0.1", port, 2, 10, 30000));
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
    REQUIRE(Wait([&] { return receiver.Messages().size() == 2; }, [&] { server->update(); }));
    const auto messages = receiver.Messages();
    CHECK(messages[0].body == body);
    CHECK(messages[0].id == 7);
    CHECK(messages[1].body.empty());
    CHECK(messages[1].id == 8);
    CHECK(messages[0].actor == header->actor_id());
    CHECK(messages[0].source == header->source_bus_id());
    CHECK(messages[0].destination == header->dest_bus_id());
    CHECK_FALSE(receiver.wrong_thread.load());
    header->set_message_id(9);
    header->set_message_length(static_cast<uint32_t>(body.size()));
    std::string expected;
    header->serialize_to_string(expected);
    expected += body;
    REQUIRE(server->send_msg(header.get(), body.data(), messages[0].session));
    REQUIRE(Wait([&] {
        std::lock_guard guard(wire_mutex);
        return wire.size() >= expected.size();
    }));
    {
        std::lock_guard guard(wire_mutex);
        CHECK(wire == expected);
    }
    CHECK_FALSE(server->send_msg(header.get(), body.data(), messages[0].session + 1));
}

TEST_CASE("WebSocket server sends binary ARK frames") {
    receiver receiver;
    auto server = std::make_shared<ark::websocket_server>(&receiver, &receiver::OnMessage, &receiver::OnEvent);
    const auto port = AvailablePort();
    auto header = std::make_shared<ark::s_msg_header>();
    REQUIRE(server->start_server(header, 11, "127.0.0.1", port, 2, 10, 30000));
    std::atomic<bool> binary{false};
    std::mutex mutex;
    std::string reply;
    arknet::ws_client client;
    client.bind_connect([&] {
        if (!arknet::get_last_error())
        {
            client.ws_stream().binary(true);
        }
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
    REQUIRE(Wait([&] { return receiver.Messages().size() == 1; }, [&] { server->update(); }));
    const auto message = receiver.Messages().front();
    CHECK(message.body == body);
    REQUIRE(server->send_msg(header.get(), body.data(), message.session));
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

TEST_CASE("TCP and WebSocket clients preserve ARK frames and use Update dispatch") {
    SUBCASE("TCP") {
        receiver receiver;
        arknet::tcp_server server(1024, 1024 * 1024, 2);
        server.bind_recv([](auto& session, std::string_view data) { session->async_send(data); });
        REQUIRE(server.start("127.0.0.1", 0));
        auto client = CreateClient<ark::tcp_client>(receiver);
        auto header = std::make_shared<ark::ss_msg_header>();
        header->set_actor_id(987654321);
        header->set_source_bus_id(11);
        header->set_dest_bus_id(22);
        REQUIRE(client->start_client(header, 11, "127.0.0.1", server.listen_port()));
        const std::string body(4096, '\xFE');
        Frame(*header, 71, body);
        REQUIRE(client->send_msg(header.get(), body.data(), 0));
        CHECK(receiver.Events().empty());
        REQUIRE(Wait([&] { return receiver.Messages().size() == 1; }, [&] { client->update(); }));
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
    SUBCASE("WebSocket") {
        receiver receiver;
        std::atomic<bool> binary{false};
        arknet::ws_server server(1024, 1024 * 1024, 2);
        server.bind_connect([](auto& session) { session->ws_stream().binary(true); });
        server.bind_recv([&](auto& session, std::string_view data) {
            binary.store(session->ws_stream().got_binary());
            session->async_send(data);
        });
        REQUIRE(server.start("127.0.0.1", 0));
        auto client = CreateClient<ark::websocket_client>(receiver);
        auto header = std::make_shared<ark::s_msg_header>();
        REQUIRE(client->start_client(header, 11, "127.0.0.1", server.listen_port()));
        const std::string body(4096, '\xFE');
        Frame(*header, 71, body);
        REQUIRE(client->send_msg(header.get(), body.data(), 0));
        CHECK(receiver.Events().empty());
        REQUIRE(Wait([&] { return receiver.Messages().size() == 1; }, [&] { client->update(); }));
        CHECK(receiver.Messages().front().body == body);
        CHECK(binary.load());
        CHECK_FALSE(receiver.wrong_thread.load());
        const std::weak_ptr weak = client;
        client.reset();
        CHECK(weak.expired());
    }
}

TEST_CASE("TCP and WebSocket callbacks can close their own session") {
    SUBCASE("TCP") {
        ReentrantClose<ark::tcp_server, arknet::tcp_client>();
    }
    SUBCASE("WebSocket") {
        ReentrantClose<ark::websocket_server, arknet::ws_client>();
    }
}

TEST_CASE("TCP and WebSocket enforce connection limits and idle timeouts") {
    SUBCASE("TCP") {
        LimitAndTimeout<ark::tcp_server, arknet::tcp_client>();
    }
    SUBCASE("WebSocket") {
        LimitAndTimeout<ark::websocket_server, arknet::ws_client>();
    }
}

TEST_CASE("TCP and WebSocket retain disconnect events when reconnect precedes Update") {
    SUBCASE("TCP") {
        ReconnectKeepsEvents<ark::tcp_client, arknet::tcp_server>();
    }
    SUBCASE("WebSocket") {
        ReconnectKeepsEvents<ark::websocket_client, arknet::ws_server>();
    }
}

TEST_CASE("TCP and WebSocket close oversized ARK frames through normal disconnect dispatch") {
    SUBCASE("TCP") {
        RejectOversizedFrame<ark::tcp_server, arknet::tcp_client>();
    }
    SUBCASE("WebSocket") {
        RejectOversizedFrame<ark::websocket_server, arknet::ws_client>();
    }
}

TEST_CASE("TCP and WebSocket reject old session operations after reconnect during serialization") {
    SUBCASE("TCP") {
        StaleSessionCannotSendOrCloseReplacement<ark::tcp_client, arknet::tcp_server>();
    }
    SUBCASE("WebSocket") {
        StaleSessionCannotSendOrCloseReplacement<ark::websocket_client, arknet::ws_server>();
    }
}

TEST_CASE("TCP and WebSocket wrappers can be destroyed before Start") {
    receiver receiver;
    auto tcp_client = CreateClient<ark::tcp_client>(receiver);
    auto ws_client = CreateClient<ark::websocket_client>(receiver);
    auto tcp_server = std::make_shared<ark::tcp_server>(&receiver, &receiver::OnMessage, &receiver::OnEvent);
    auto ws_server = std::make_shared<ark::websocket_server>(&receiver, &receiver::OnMessage, &receiver::OnEvent);
    tcp_client.reset();
    ws_client.reset();
    tcp_server.reset();
    ws_server.reset();
    CHECK(receiver.Events().empty());
}
