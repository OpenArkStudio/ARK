// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.

#include <memory>
#include <string>

#include "doctest.h"
#include "net/include/AFNetSession.hpp"

namespace {
std::string MakeFrame(ark::msg_id_t id, const std::string& body)
{
    ark::AFCSMsgHeader header;
    header.SetMessageId(id);
    header.SetMessageLength(static_cast<uint32_t>(body.size()));
    std::string bytes;
    header.SerializeToString(bytes);
    return bytes + body;
}
} // namespace

TEST_CASE("Framing preserves split headers, binary payloads and coalesced messages")
{
    ark::AFTCPClientSession session(std::make_shared<ark::AFCSMsgHeader>(), 1);
    const std::string payload("a\0b", 3);
    const auto first = MakeFrame(7, payload);
    session.AddBuffer(first.data(), 2);
    CHECK(session.ParseBufferToMsg());
    ark::AFNetMsg* message = nullptr;
    CHECK_FALSE(session.PopNetMsg(message));
    session.AddBuffer(first.data() + 2, first.size() - 2);
    session.AddBuffer(MakeFrame(8, ""));
    CHECK(session.ParseBufferToMsg());
    REQUIRE(session.PopNetMsg(message));
    CHECK(message->GetMsgId() == 7);
    CHECK(std::string(message->GetMsgData(), message->GetMsgLength()) == payload);
    ark::AFNetMsg::Release(message);
    REQUIRE(session.PopNetMsg(message));
    CHECK(message->GetMsgId() == 8);
    CHECK(message->GetMsgLength() == 0);
    ark::AFNetMsg::Release(message);
    CHECK_FALSE(session.PopNetMsg(message));
    CHECK(session.GetBufferLen() == 0);
}

TEST_CASE("Framing rejects oversized input without preempting disconnect bookkeeping")
{
    ark::AFTCPClientSession session(std::make_shared<ark::AFCSMsgHeader>(), 1);
    ark::AFCSMsgHeader header;
    header.SetMessageLength(ark::ARK_MSG_MAX_LENGTH + 1);
    std::string bytes;
    header.SerializeToString(bytes);
    session.AddBuffer(bytes);
    CHECK_FALSE(session.ParseBufferToMsg());
    CHECK_FALSE(session.NeedRemove());
    ark::AFNetMsg* message = nullptr;
    CHECK_FALSE(session.PopNetMsg(message));
}

TEST_CASE("Receive buffers preserve unread bytes across compaction and growth")
{
    ark::AFBuffer buffer(4);
    CHECK(buffer.write("abc", 3));
    buffer.remove_data(2);
    CHECK(buffer.write("defg", 4));
    CHECK(buffer.get_length() == 5);
    CHECK(std::string(buffer.get_data(), buffer.get_length()) == "cdefg");
    buffer.remove_data(5);
    CHECK(buffer.get_length() == 0);
    CHECK(buffer.write("123456789", 9));
    CHECK(std::string(buffer.get_data(), buffer.get_length()) == "123456789");
    buffer.remove_data(8);
    CHECK(buffer.write("abcdef", 6));
    CHECK(std::string(buffer.get_data(), buffer.get_length()) == "9abcdef");
}

TEST_CASE("Framing retains a partial body after complete frames are consumed")
{
    ark::AFTCPClientSession session(std::make_shared<ark::AFCSMsgHeader>(), 1);
    const auto first = MakeFrame(7, "first");
    const std::string payload(ark::ARK_MSG_MAX_LENGTH, 'x');
    const auto second = MakeFrame(8, payload);
    session.AddBuffer(first + second.substr(0, second.size() - 3));
    CHECK(session.ParseBufferToMsg());
    ark::AFNetMsg* message{};
    REQUIRE(session.PopNetMsg(message));
    CHECK(message->GetMsgId() == 7);
    ark::AFNetMsg::Release(message);
    CHECK_FALSE(session.PopNetMsg(message));
    for (std::size_t i = second.size() - 3; i < second.size(); ++i)
    {
        session.AddBuffer(second.data() + i, 1);
        CHECK(session.ParseBufferToMsg());
    }
    REQUIRE(session.PopNetMsg(message));
    CHECK(message->GetMsgId() == 8);
    CHECK(std::string(message->GetMsgData(), message->GetMsgLength()) == payload);
    ark::AFNetMsg::Release(message);
    CHECK(session.GetBufferLen() == 0);
    session.AddBuffer(MakeFrame(9, "next"));
    CHECK(session.ParseBufferToMsg());
    REQUIRE(session.PopNetMsg(message));
    CHECK(message->GetMsgId() == 9);
    ark::AFNetMsg::Release(message);
    CHECK_FALSE(session.PopNetMsg(message));
}

TEST_CASE("Discarding a partial frame also discards its decoded header")
{
    ark::AFTCPClientSession session(std::make_shared<ark::AFCSMsgHeader>(), 1);
    const auto discarded = MakeFrame(7, std::string(100, 'x'));
    session.AddBuffer(discarded.substr(0, discarded.size() - 5));
    CHECK(session.ParseBufferToMsg());
    const auto remaining = session.GetBufferLen();
    CHECK(session.RemoveBuffer(remaining) == 0);
    session.AddBuffer(MakeFrame(8, "next"));
    CHECK(session.ParseBufferToMsg());
    ark::AFNetMsg* message{};
    REQUIRE(session.PopNetMsg(message));
    CHECK(message->GetMsgId() == 8);
    CHECK(std::string(message->GetMsgData(), message->GetMsgLength()) == "next");
    ark::AFNetMsg::Release(message);
    CHECK(session.GetBufferLen() == 0);
}
