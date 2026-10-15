// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/container/queue.hpp>

void compile_queue_header() {
    ark::lock lock;
    lock.lock();
    lock.unlock();
    ark::queue<int> values;
    values.push(1);
    int value = 0;
    values.pop(value);
    static_cast<void>(values.count());
}
