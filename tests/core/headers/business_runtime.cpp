// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/plugin_manager.hpp>

auto consume_business_runtime(ark::plugin_manager& manager, ark::module& owner) {
    auto runtime = manager.get_task_runtime();
    auto& tasks = manager.create_task_group(owner, 1);
    return &tasks == &manager.get_tasks(owner) && bool(runtime);
}
