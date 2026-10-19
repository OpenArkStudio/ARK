// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <memory>

#include "ark/plugin_manager.hpp"
#include "ark/redis/redis_module.hpp"

namespace ark {

class redis_module_impl final : public redis_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    redis_module_impl();
    ~redis_module_impl() override;

    bool init() override;
    bool update() override;
    bool ready_to_shut() const override;
    bool pre_shut() override;
    bool shut() override;

    bool configure(std::string connection, redis_options options) override;
    task<arknet::redis::result> command(std::string connection, arknet::redis::command command,
        std::chrono::milliseconds timeout = std::chrono::seconds(5)) override;
    task<arknet::redis::result> pipeline(std::string connection, std::vector<arknet::redis::command> commands,
        std::chrono::milliseconds timeout = std::chrono::seconds(5)) override;

private:
    struct state;
    std::shared_ptr<state> state_;
    void ensure_state();
};

} // namespace ark
