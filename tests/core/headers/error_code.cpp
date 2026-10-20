// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/error_code.hpp>

// Keep the second inclusion separate to retain the include-guard check.
#include <type_traits>

#include <ark/error_code.hpp>

static_assert(std::is_same_v<std::underlying_type_t<ark::error_code>, std::uint32_t>);
static_assert(static_cast<std::uint32_t>(ark::error_code::success) == 0);
static_assert(static_cast<std::uint32_t>(ark::error_code::kcp_server_disconnect) == 1000);
static_assert(static_cast<std::uint32_t>(ark::error_code::kcp_time_out_disconnect) == 1001);
static_assert(static_cast<std::uint32_t>(ark::error_code::kcp_force_disconnect) == 1002);
static_assert(static_cast<std::uint32_t>(ark::error_code::kcp_connect_timeout) == 1003);
static_assert(static_cast<std::uint32_t>(ark::error_code::peer_disconnect) == 1004);
static_assert(static_cast<std::uint32_t>(ark::error_code::logic_error_code_start) == 10000);
