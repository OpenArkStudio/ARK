// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include "ark/plugin_manager.hpp"
#include "ark/redis/redis_module.hpp"

namespace ark {

class redis_sample_module final : public module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;
    bool post_init() override;

private:
    task<> run();
    redis_module* redis_module_{};
    task_group* tasks_{};
};

} // namespace ark
