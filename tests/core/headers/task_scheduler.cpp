// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/task.hpp>
#include <ark/utility/scheduler.hpp>

ark::task<> task_then_scheduler(ark::scheduler&) {
    co_return;
}
