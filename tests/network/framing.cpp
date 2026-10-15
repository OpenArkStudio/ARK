// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.

#include <memory>
#include <string>

#include "doctest.h"

#include "ark/net/net_session.hpp"

namespace {

std::string make_frame(ark::msg_id_t id, const std::string& body) {
    ark::s_msg_header header;
    header.set_message_id(id);
    header.set_message_length(static_cast<uint32_t>(body.size()));
    std::string bytes;
    header.serialize_to_string(bytes);
    return bytes + body;
}
} // namespace

TEST_CASE("Framing preserves split headers, binary payloads and coalesced messages") {
    ark::tcp_client_session session(std::make_shared<ark::s_msg_header>(), 1);
    const std::string payload("a\0b", 3);
    const auto first = make_frame(7, payload);
    session.add_buffer(first.data(), 2);
    CHECK(session.parse_buffer_to_msg());
    ark::net_msg* message = nullptr;
    CHECK_FALSE(session.pop_net_msg(message));
    session.add_buffer(first.data() + 2, first.size() - 2);
    session.add_buffer(make_frame(8, ""));
    CHECK(session.parse_buffer_to_msg());
    REQUIRE(session.pop_net_msg(message));
    CHECK(message->get_msg_id() == 7);
    CHECK(std::string(message->get_msg_data(), message->get_msg_length()) == payload);
    ark::net_msg::release(message);
    REQUIRE(session.pop_net_msg(message));
    CHECK(message->get_msg_id() == 8);
    CHECK(message->get_msg_length() == 0);
    ark::net_msg::release(message);
    CHECK_FALSE(session.pop_net_msg(message));
    CHECK(session.get_buffer_len() == 0);
}

TEST_CASE("Framing rejects oversized input without preempting disconnect bookkeeping") {
    ark::tcp_client_session session(std::make_shared<ark::s_msg_header>(), 1);
    ark::s_msg_header header;
    header.set_message_length(ark::ark_msg_max_length + 1);
    std::string bytes;
    header.serialize_to_string(bytes);
    session.add_buffer(bytes);
    CHECK_FALSE(session.parse_buffer_to_msg());
    CHECK_FALSE(session.need_remove());
    ark::net_msg* message = nullptr;
    CHECK_FALSE(session.pop_net_msg(message));
}
