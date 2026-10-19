// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/plugin_manager.hpp>
#include <ark/utility/scheduler.hpp>

ark::task<> scheduler_then_task(ark::scheduler&) {
    co_return;
}
