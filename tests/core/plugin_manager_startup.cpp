// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <array>
#include <cstddef>
#include <string>

#include "../network/doctest.h"

#include <ark/plugin_manager.hpp>

namespace {

enum class startup_phase : std::size_t
{
    init,
    post_init,
    check_config,
    pre_update,
    none
};

constexpr std::array phase_names{"init", "post_init", "check_config", "pre_update"};

class counted_module final : public ark::module
{
public:
    explicit counted_module(startup_phase failed_phase = startup_phase::none)
        : failed_phase_(failed_phase) {
    }

    bool init() override {
        return record(startup_phase::init);
    }

    bool post_init() override {
        return record(startup_phase::post_init);
    }

    bool check_config() override {
        return record(startup_phase::check_config);
    }

    bool pre_update() override {
        return record(startup_phase::pre_update);
    }

    std::array<unsigned, phase_names.size()> calls{};

private:
    bool record(startup_phase phase) {
        ++calls[static_cast<std::size_t>(phase)];
        return phase != failed_phase_;
    }

    startup_phase failed_phase_;
};

class unconfigured_logger_scope
{
public:
    unconfigured_logger_scope() {
        ark::logger::set_instance(&logger_);
    }

    ~unconfigured_logger_scope() {
        ark::logger::set_instance(previous_);
    }

private:
    ark::logger* previous_{ark::logger::instance()};
    ark::logger logger_;
};

bool start(ark::plugin_manager& manager) {
    return manager.start(42, {}, {}, {});
}

} // namespace

TEST_CASE("Plugin startup calls each successful module phase once") {
    ark::plugin_manager manager;
    std::array<counted_module, 3> modules;
    for (std::size_t index = 0; index < modules.size(); ++index)
    {
        REQUIRE(manager.add_module("counted-module-" + std::to_string(index), &modules[index]));
    }

    CHECK(start(manager));
    for (const auto& module : modules)
    {
        CHECK(module.calls == std::array<unsigned, 4>{1, 1, 1, 1});
    }
}

TEST_CASE("Plugin startup returns false without repeating a failed phase or running later phases") {
    for (std::size_t failed = 0; failed < phase_names.size(); ++failed)
    {
        CAPTURE(std::string(phase_names[failed]));
        ark::plugin_manager manager;
        counted_module module(static_cast<startup_phase>(failed));
        REQUIRE(manager.add_module("counted-module-single", &module));

        CHECK_FALSE(start(manager));
        for (std::size_t phase = 0; phase < phase_names.size(); ++phase)
        {
            CHECK(module.calls[phase] == (phase <= failed ? 1u : 0u));
        }
    }
}

TEST_CASE("Plugin startup stops a phase at its first failure independently of module iteration order") {
    for (std::size_t failed = 0; failed < phase_names.size(); ++failed)
    {
        CAPTURE(std::string(phase_names[failed]));
        ark::plugin_manager manager;
        std::array modules{counted_module(static_cast<startup_phase>(failed)),
            counted_module(static_cast<startup_phase>(failed)), counted_module(static_cast<startup_phase>(failed))};
        for (std::size_t index = 0; index < modules.size(); ++index)
        {
            REQUIRE(manager.add_module("counted-module-" + std::to_string(index), &modules[index]));
        }

        CHECK_FALSE(start(manager));
        for (std::size_t phase = 0; phase < phase_names.size(); ++phase)
        {
            unsigned phase_calls = 0;
            for (const auto& module : modules)
            {
                CHECK(module.calls[phase] <= 1);
                phase_calls += module.calls[phase];
            }
            const auto expected = phase < failed ? modules.size() : phase == failed ? 1u : 0u;
            CHECK(phase_calls == expected);
        }
    }
}

TEST_CASE("Plugin startup returns false without a configured logger") {
    unconfigured_logger_scope logger;
    ark::plugin_manager manager;
    counted_module module(startup_phase::init);
    REQUIRE(manager.add_module("counted-module-no-logger", &module));

    bool started = true;
    CHECK_NOTHROW(started = start(manager));
    CHECK_FALSE(started);
    CHECK(module.calls == std::array<unsigned, 4>{1, 0, 0, 0});
}
