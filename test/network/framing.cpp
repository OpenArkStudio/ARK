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
