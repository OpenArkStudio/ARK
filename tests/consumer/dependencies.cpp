// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/container/lock_free_queue.hpp>
#include <ark/macros.hpp>

bool check_packaged_dependencies() {
    ark::lock_free_queue<int> values;
    int value = 0;
    return values.push(42) && values.pop(value) && ARK_FORMAT("value={}", value) == "value=42" && values.count() == 0;
}
