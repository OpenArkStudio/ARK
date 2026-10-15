// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <atomic>

namespace ark::app_detail {

inline constexpr int command_stop_reason = -1;
inline constinit std::atomic<int> stop_reason{0};
static_assert(std::atomic<int>::is_always_lock_free, "Application signal handling requires lock-free atomic<int>");

inline void request_stop(int reason) noexcept {
    int expected = 0;
    stop_reason.compare_exchange_strong(expected, reason, std::memory_order_relaxed);
}

inline int requested_stop_reason() noexcept {
    return stop_reason.load(std::memory_order_relaxed);
}

extern "C" inline void ark_app_stop_signal_handler(int reason) noexcept {
    request_stop(reason);
}

} // namespace ark::app_detail
