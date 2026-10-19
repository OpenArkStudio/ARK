// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/container/thread_safe_queue.hpp>

// Keep the second inclusion separate so formatting preserves this include-guard regression.
#include <ark/container/thread_safe_queue.hpp>

void compile_thread_safe_queue_header() {
    ark::thread_safe_queue<int> values;
    const auto& observed = values;
    static_cast<void>(observed.empty());
    static_cast<void>(values.size());
    values.push(1);
    int value = 0;
    static_cast<void>(values.try_pop(value));
    static_cast<void>(values.grab_all());
}
