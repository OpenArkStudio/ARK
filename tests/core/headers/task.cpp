// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/task.hpp>

ark::task<int> compile_task_header() {
    co_return 42;
}
