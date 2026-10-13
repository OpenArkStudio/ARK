#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <arknet/rudp/rudp_client.hpp>

#include "doctest.h"
#include "net/include/AFCRDPClient.hpp"
#include "net/include/AFCRDPServer.hpp"

namespace {
using namespace std::chrono_literals;

struct rudp_message
{
    ark::msg_id_t id;
    ark::conv_id_t session;
    std::string body;
    ark::guid_t actor;
    ark::bus_id_t source;
    ark::bus_id_t destination;
    bool compressed;
};

struct rudp_event
{
    ark::AFNetEventType type;
    ark::conv_id_t session;
    ark::bus_id_t bus;
    std::string ip;
};

class rudp_receiver
{
public:
    void receive(const ark::AFNetMsg* message, ark::conv_id_t session)
    {
        {
            std::lock_guard lock(mutex_);
            wrong_thread_ |= std::this_thread::get_id() != update_thread_;
            messages_.push_back({message->GetMsgId(), session,
                message->GetMsgLength() ? std::string(message->GetMsgData(), message->GetMsgLength()) : std::string{},
                message->GetActorId(), message->GetSrcBus(), message->GetDstBus(), message->GetCompress() != 0});
        }
        if (on_receive)
        {
            on_receive(message, session);
        }
    }

    void event(const ark::AFNetEvent* event)
    {
        std::lock_guard lock(mutex_);
        wrong_thread_ |= std::this_thread::get_id() != update_thread_;
        events_.push_back({event->GetType(), event->GetId(), event->GetBusId(), event->GetIP()});
    }

    std::vector<rudp_message> messages()
    {
        std::lock_guard lock(mutex_);
        return messages_;
    }

    std::vector<rudp_event> events()
    {
        std::lock_guard lock(mutex_);
        return events_;
    }

    std::size_t event_count(ark::AFNetEventType type)
    {
        std::lock_guard lock(mutex_);
        return static_cast<std::size_t>(std::count_if(
            events_.begin(), events_.end(), [type](const rudp_event& value) { return value.type == type; }));
    }

    bool wrong_thread()
    {
        std::lock_guard lock(mutex_);
        return wrong_thread_;
    }

    std::function<void(const ark::AFNetMsg*, ark::conv_id_t)> on_receive;

private:
    const std::thread::id update_thread_ = std::this_thread::get_id();
    std::mutex mutex_;
    std::vector<rudp_message> messages_;
    std::vector<rudp_event> events_;
    bool wrong_thread_ = false;
};

std::uint16_t reserve_rudp_port()
{
    asio::io_context context;
    asio::ip::udp::socket socket(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0));
    return socket.local_endpoint().port();
}

template<class Predicate, class Update>
bool await_rudp(Predicate&& predicate, Update&& update, std::chrono::milliseconds timeout = 5s)
{
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    do
    {
        update();
        if (predicate())
        {
            return true;
        }
        std::this_thread::sleep_for(1ms);
    } while (std::chrono::steady_clock::now() < deadline);
    return false;
}

std::string rudp_payload(std::size_t size)
{
    std::string result(size, '\0');
    for (std::size_t index = 0; index < size; ++index)
    {
        result[index] = static_cast<char>((index * 19 + 37) & 0xff);
    }
    return result;
}

template<class Header>
std::shared_ptr<Header> rudp_header()
{
    auto header = std::make_shared<Header>();
    header->SetCompressed(true);
    header->SetActorId(0x1234567812345678);
    header->SetSourceBusId(101);
    header->SetDestBusId(202);
    return header;
}

std::shared_ptr<ark::AFCRDPClient> rudp_client(rudp_receiver& receiver)
{
    return std::make_shared<ark::AFCRDPClient>(&receiver, &rudp_receiver::receive, &rudp_receiver::event);
}

std::shared_ptr<ark::AFCRDPServer> rudp_server(rudp_receiver& receiver)
{
    return std::make_shared<ark::AFCRDPServer>(&receiver, &rudp_receiver::receive, &rudp_receiver::event);
}
class rudp_serialization_header : public ark::AFCSMsgHeader
{
public:
    void SerializeToString(std::string& buffer) override
    {
        ark::AFCSMsgHeader::SerializeToString(buffer);
        if (auto callback = std::exchange(on_serialize, {}))
        {
            callback();
        }
    }

    std::function<void()> on_serialize;
};
} // namespace

TEST_CASE_TEMPLATE(
    "RUDP preserves ARK framing and dispatches on Update", Header, ark::AFCSMsgHeader, ark::AFSSMsgHeader)
{
    rudp_receiver client_receiver;
    rudp_receiver server_receiver;
    auto client = rudp_client(client_receiver);
    auto server = rudp_server(server_receiver);
    auto header = rudp_header<Header>();
    const auto port = reserve_rudp_port();
    REQUIRE(server->StartServer(header, 101, "127.0.0.1", port, 2, 4, 30000));
    REQUIRE(client->StartClient(header, 202, "127.0.0.1", port));
    CHECK(client_receiver.events().empty());
    CHECK(server_receiver.events().empty());
    const auto update = [&] {
        server->Update();
        client->Update();
    };
    REQUIRE(await_rudp(
        [&] { return client_receiver.events().size() == 1 && server_receiver.events().size() == 1; }, update));
    const auto client_session = client_receiver.events().front().session;
    const auto server_session = server_receiver.events().front().session;
    CHECK(client_receiver.events().front().bus == 202);
    CHECK(server_receiver.events().front().bus == 101);
    CHECK(server->GetIP(server_session) == "127.0.0.1");
    CHECK(server->GetIP(server_session + 100) == ark::NULL_STR);

    std::size_t rejected_echoes = 0;
    server_receiver.on_receive = [&](const ark::AFNetMsg* message, ark::conv_id_t session) {
        Header response(*header);
        response.SetMessageId(message->GetMsgId());
        response.SetMessageLength(message->GetMsgLength());
        if (!server->SendMsg(&response, message->GetMsgLength() ? message->GetMsgData() : "", session))
        {
            ++rejected_echoes;
        }
    };
    const std::vector<std::size_t> sizes{0, 1, 23, 511, 1024, 4096};
    for (std::size_t index = 0; index < sizes.size(); ++index)
    {
        Header request(*header);
        request.SetMessageId(static_cast<ark::msg_id_t>(40 + index));
        request.SetMessageLength(static_cast<std::uint32_t>(sizes[index]));
        const auto body = rudp_payload(sizes[index]);
        REQUIRE(client->SendMsg(&request, body.empty() ? "" : body.data(), client_session));
    }
    CHECK(client_receiver.messages().empty());
    CHECK(server_receiver.messages().empty());
    REQUIRE(await_rudp(
        [&] {
            return client_receiver.messages().size() == sizes.size() &&
                   server_receiver.messages().size() == sizes.size();
        },
        update));
    CHECK(rejected_echoes == 0);
    const auto sent = server_receiver.messages();
    const auto echoed = client_receiver.messages();
    for (std::size_t index = 0; index < sizes.size(); ++index)
    {
        CHECK(sent[index].id == 40 + index);
        CHECK(echoed[index].id == sent[index].id);
        CHECK(sent[index].body == rudp_payload(sizes[index]));
        CHECK(echoed[index].body == sent[index].body);
        CHECK(sent[index].session == server_session);
        CHECK(echoed[index].session == client_session);
        CHECK(sent[index].actor == header->ActorId());
        CHECK(sent[index].source == header->SourceBusId());
        CHECK(sent[index].destination == header->DestBusId());
        CHECK(sent[index].compressed);
    }

    Header broadcast(*header);
    broadcast.SetMessageId(99);
    const auto body = rudp_payload(4096);
    broadcast.SetMessageLength(static_cast<std::uint32_t>(body.size()));
    server->BroadcastMsg(&broadcast, body.data());
    REQUIRE(await_rudp([&] { return client_receiver.messages().size() == sizes.size() + 1; }, update));
    CHECK(client_receiver.messages().back().id == 99);
    CHECK(client_receiver.messages().back().body == body);
    server->CloseSession(server_session);
    CHECK(server_receiver.event_count(ark::AFNetEventType::DISCONNECT) == 0);
    REQUIRE(await_rudp(
        [&] {
            return client_receiver.event_count(ark::AFNetEventType::DISCONNECT) == 1 &&
                   server_receiver.event_count(ark::AFNetEventType::DISCONNECT) == 1;
        },
        update));
    CHECK_FALSE(client_receiver.wrong_thread());
    CHECK_FALSE(server_receiver.wrong_thread());
    client->Shutdown();
    server->Shutdown();
}

TEST_CASE("RUDP rejects invalid sends and unstarted endpoints shut down safely")
{
    rudp_receiver receiver;
    auto client = rudp_client(receiver);
    auto server = rudp_server(receiver);
    auto header = rudp_header<ark::AFCSMsgHeader>();
    header->SetMessageId(10);
    header->SetMessageLength(0);
    CHECK_FALSE(client->SendMsg(header.get(), "", 0));
    CHECK_FALSE(server->SendMsg(header.get(), "", 1000));
    CHECK_FALSE(client->SendMsg(nullptr, "", 0));
    CHECK_FALSE(server->SendMsg(header.get(), nullptr, 1000));
    CHECK_FALSE(server->StartServer(header, 1, "127.0.0.1", reserve_rudp_port(), 1, 0, 1000));
    client->Shutdown();
    server->Shutdown();
    client->Update();
    server->Update();
    CHECK_FALSE(client->IsWorking());
    CHECK_FALSE(server->IsWorking());
    CHECK(receiver.events().empty());
}

TEST_CASE("RUDP wrapper rejects stale session IDs and oversized messages")
{
    rudp_receiver client_receiver;
    rudp_receiver server_receiver;
    auto client = rudp_client(client_receiver);
    auto server = rudp_server(server_receiver);
    auto header = rudp_header<ark::AFCSMsgHeader>();
    const auto port = reserve_rudp_port();
    REQUIRE(server->StartServer(header, 1, "127.0.0.1", port, 1, 1, 30000));
    REQUIRE(client->StartClient(header, 2, "127.0.0.1", port));
    const auto update = [&] {
        server->Update();
        client->Update();
    };
    REQUIRE(await_rudp(
        [&] { return client_receiver.events().size() == 1 && server_receiver.events().size() == 1; }, update));
    header->SetMessageId(10);
    header->SetMessageLength(0);
    CHECK_FALSE(client->SendMsg(header.get(), "", client_receiver.events().front().session + 1));
    CHECK_FALSE(server->SendMsg(header.get(), "", server_receiver.events().front().session + 1));
    header->SetMessageLength(4097);
    const auto oversized = rudp_payload(4097);
    CHECK_FALSE(client->SendMsg(header.get(), oversized.data(), 0));
    CHECK_FALSE(server->SendMsg(header.get(), oversized.data(), server_receiver.events().front().session));
    server->BroadcastMsg(header.get(), oversized.data());
    client->Shutdown();
    header->SetMessageLength(0);
    CHECK_FALSE(client->SendMsg(header.get(), "", 0));
    server->Shutdown();
    update();
    CHECK(client_receiver.messages().empty());
    CHECK(server_receiver.messages().empty());
}

TEST_CASE("RUDP reconnects after a server restart and retains queued lifecycle events")
{
    rudp_receiver client_receiver;
    rudp_receiver server_receiver;
    auto client = rudp_client(client_receiver);
    auto server = rudp_server(server_receiver);
    auto header = rudp_header<ark::AFSSMsgHeader>();
    const auto port = reserve_rudp_port();
    REQUIRE(server->StartServer(header, 1, "127.0.0.1", port, 2, 2, 30000));
    REQUIRE(client->StartClient(header, 2, "127.0.0.1", port));
    server->Shutdown();
    REQUIRE(server->StartServer(header, 1, "127.0.0.1", port, 2, 2, 30000));
    const auto update = [&] {
        server->Update();
        client->Update();
    };
    REQUIRE(await_rudp(
        [&] {
            return client_receiver.event_count(ark::AFNetEventType::CONNECT) == 2 &&
                   client_receiver.event_count(ark::AFNetEventType::DISCONNECT) == 1 &&
                   server_receiver.event_count(ark::AFNetEventType::CONNECT) == 2;
        },
        update, 12s));
    const auto client_events = client_receiver.events();
    REQUIRE(client_events.size() == 3);
    CHECK(client_events[0].type == ark::AFNetEventType::CONNECT);
    CHECK(client_events[1].type == ark::AFNetEventType::DISCONNECT);
    CHECK(client_events[2].type == ark::AFNetEventType::CONNECT);
    CHECK(client_events[0].session != client_events[2].session);
    CHECK(server_receiver.event_count(ark::AFNetEventType::DISCONNECT) == 1);
    header->SetMessageId(71);
    header->SetMessageLength(1);
    REQUIRE(client->SendMsg(header.get(), "x", client_events.back().session));
    REQUIRE(await_rudp([&] { return server_receiver.messages().size() == 1; }, update));
    CHECK(server_receiver.messages().front().body == "x");
    CHECK_FALSE(client_receiver.wrong_thread());
    CHECK_FALSE(server_receiver.wrong_thread());
    client->Shutdown();
    server->Shutdown();
}

TEST_CASE("RUDP server admission obeys max_client and releases disconnected slots")
{
    rudp_receiver client_receiver;
    rudp_receiver server_receiver;
    auto client = rudp_client(client_receiver);
    auto server = rudp_server(server_receiver);
    auto header = rudp_header<ark::AFCSMsgHeader>();
    const auto port = reserve_rudp_port();
    REQUIRE(server->StartServer(header, 1, "127.0.0.1", port, 2, 1, 30000));
    REQUIRE(client->StartClient(header, 2, "127.0.0.1", port));
    const auto update = [&] {
        server->Update();
        client->Update();
    };
    REQUIRE(await_rudp([&] { return server_receiver.events().size() == 1; }, update));
    arknet::rudp_client excess;
    excess.set_auto_reconnect(false).set_connect_timeout(300ms);
    CHECK_FALSE(excess.start("127.0.0.1", port));
    CHECK(arknet::get_last_error() == asio::error::timed_out);
    update();
    CHECK(server_receiver.event_count(ark::AFNetEventType::CONNECT) == 1);
    client->Shutdown();
    REQUIRE(await_rudp([&] { return server_receiver.event_count(ark::AFNetEventType::DISCONNECT) == 1; }, update));
    excess.set_connect_timeout(2s);
    REQUIRE(excess.start("127.0.0.1", port));
    REQUIRE(await_rudp([&] { return server_receiver.event_count(ark::AFNetEventType::CONNECT) == 2; }, update));
    excess.stop();
    server->Shutdown();
}

TEST_CASE("RUDP server silence timeout is configured in milliseconds")
{
    rudp_receiver client_receiver;
    rudp_receiver server_receiver;
    auto client = rudp_client(client_receiver);
    auto server = rudp_server(server_receiver);
    auto header = rudp_header<ark::AFCSMsgHeader>();
    const auto port = reserve_rudp_port();
    REQUIRE(server->StartServer(header, 1, "127.0.0.1", port, 1, 1, 80));
    REQUIRE(client->StartClient(header, 2, "127.0.0.1", port));
    const auto update = [&] {
        server->Update();
        client->Update();
    };
    REQUIRE(await_rudp([&] { return server_receiver.event_count(ark::AFNetEventType::DISCONNECT) == 1; }, update, 2s));
    CHECK_FALSE(server_receiver.wrong_thread());
    client->Shutdown();
    server->Shutdown();
}

TEST_CASE("RUDP endpoint callbacks do not keep ARK wrappers alive")
{
    rudp_receiver client_receiver;
    rudp_receiver server_receiver;
    auto client = rudp_client(client_receiver);
    auto server = rudp_server(server_receiver);
    auto header = rudp_header<ark::AFCSMsgHeader>();
    const auto port = reserve_rudp_port();
    REQUIRE(server->StartServer(header, 1, "127.0.0.1", port, 1, 1, 30000));
    REQUIRE(client->StartClient(header, 2, "127.0.0.1", port));
    std::weak_ptr<ark::AFCRDPClient> weak_client = client;
    std::weak_ptr<ark::AFCRDPServer> weak_server = server;
    client.reset();
    server.reset();
    REQUIRE(await_rudp([&] { return weak_client.expired() && weak_server.expired(); }, [] {}, 2s));
    CHECK(client_receiver.events().empty());
    CHECK(server_receiver.events().empty());
}

TEST_CASE("RUDP closes malformed business frames through the Update event path")
{
    auto header = rudp_header<ark::AFCSMsgHeader>();
    ark::AFCSMsgHeader invalid;
    invalid.SetMessageId(100);
    invalid.SetMessageLength(4097);
    std::string wire;
    invalid.SerializeToString(wire);
    const auto port = reserve_rudp_port();

    SUBCASE("ARK server")
    {
        rudp_receiver receiver;
        auto server = rudp_server(receiver);
        REQUIRE(server->StartServer(header, 1, "127.0.0.1", port, 2, 1, 30000));
        arknet::rudp_client client;
        client.set_auto_reconnect(false);
        REQUIRE(client.start("127.0.0.1", port));
        REQUIRE(client.async_send(wire));
        REQUIRE(await_rudp(
            [&] { return receiver.event_count(ark::AFNetEventType::DISCONNECT) == 1; }, [&] { server->Update(); }));
        CHECK(receiver.event_count(ark::AFNetEventType::CONNECT) == 1);
        CHECK(receiver.messages().empty());
        CHECK_FALSE(receiver.wrong_thread());
        client.stop();
        server->Shutdown();
    }

    SUBCASE("ARK client")
    {
        rudp_receiver receiver;
        auto client = rudp_client(receiver);
        arknet::rudp_server server;
        std::atomic<bool> rejected = false;
        server.bind_connect([&](auto& session) {
            if (!session->async_send(wire))
            {
                rejected = true;
            }
        });
        REQUIRE(server.start("127.0.0.1", port));
        REQUIRE(client->StartClient(header, 2, "127.0.0.1", port));
        REQUIRE(await_rudp(
            [&] { return receiver.event_count(ark::AFNetEventType::DISCONNECT) == 1; }, [&] { client->Update(); }));
        CHECK_FALSE(rejected.load());
        CHECK(receiver.event_count(ark::AFNetEventType::CONNECT) == 1);
        CHECK(receiver.messages().empty());
        CHECK_FALSE(receiver.wrong_thread());
        client->Shutdown();
        server.stop();
    }
}

TEST_CASE("RUDP rejects an old-session send after reconnect during serialization")
{
    std::mutex peer_mutex;
    std::shared_ptr<arknet::rudp_session> peer;
    std::vector<std::string> messages;
    arknet::rudp_server server;
    rudp_receiver receiver;
    auto client = rudp_client(receiver);
    auto header = rudp_header<ark::AFCSMsgHeader>();
    server.bind_connect([&](auto& session) {
        std::lock_guard lock(peer_mutex);
        peer = session;
    });
    server.bind_recv([&](auto&, std::string_view bytes) {
        std::lock_guard lock(peer_mutex);
        messages.emplace_back(bytes);
    });
    REQUIRE(server.start("127.0.0.1", 0));
    REQUIRE(client->StartClient(header, 2, "127.0.0.1", server.listen_port()));
    REQUIRE(
        await_rudp([&] { return receiver.event_count(ark::AFNetEventType::CONNECT) == 1; }, [&] { client->Update(); }));
    const auto old_id = receiver.events().back().session;

    rudp_serialization_header request;
    request.SetMessageId(71);
    request.SetMessageLength(1);
    bool reconnected = false;
    request.on_serialize = [&] {
        std::shared_ptr<arknet::rudp_session> old_peer;
        {
            std::lock_guard lock(peer_mutex);
            old_peer = peer;
        }
        if (!old_peer)
        {
            return;
        }
        old_peer->stop();
        reconnected = await_rudp(
            [&] { return receiver.event_count(ark::AFNetEventType::CONNECT) == 2; }, [&] { client->Update(); }, 12s);
    };
    CHECK_FALSE(client->SendMsg(&request, "x", old_id));
    REQUIRE(reconnected);
    const auto new_id = receiver.events().back().session;
    REQUIRE(new_id != old_id);

    client->CloseSession(old_id);
    request.SetMessageId(72);
    REQUIRE(client->SendMsg(&request, "n", new_id));
    std::string expected;
    request.SerializeToString(expected);
    expected.append("n");
    REQUIRE(await_rudp(
        [&] {
            std::lock_guard lock(peer_mutex);
            return std::find(messages.begin(), messages.end(), expected) != messages.end();
        },
        [&] { client->Update(); }));
    {
        std::lock_guard lock(peer_mutex);
        REQUIRE(messages.size() == 1);
        CHECK(messages.front() == expected);
    }
    client->CloseSession(0);
    REQUIRE(await_rudp(
        [&] { return receiver.event_count(ark::AFNetEventType::DISCONNECT) == 2; }, [&] { client->Update(); }));
    CHECK_FALSE(client->SendMsg(&request, "n", new_id));
    client->Shutdown();
    server.stop();
}
