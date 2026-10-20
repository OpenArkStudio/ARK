// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <ark/plugin_manager.hpp>

namespace {

class startup_failure_module final : public ark::module
{
public:
    void fail_at(std::string phase) {
        const auto separator = phase.find(':');
        behavior_ = separator == std::string::npos ? "false" : phase.substr(0, separator);
        failed_phase_ = separator == std::string::npos ? std::move(phase) : phase.substr(separator + 1);
    }

    const std::string& get_name() const override {
        static const std::string name = "app_startup_failure_module";
        return name;
    }

    bool init() override {
        return record("init");
    }

    bool post_init() override {
        return record("post_init");
    }

    bool check_config() override {
        return record("check_config");
    }

    bool pre_update() override {
        return record("pre_update");
    }

    bool update() override {
        // Record forbidden entry only once, keeping failure output bounded.
        if (!updated_)
        {
            updated_ = true;
            record("update");
        }
        return true;
    }

    bool pre_shut() override {
        return record("pre_shut");
    }

    bool shut() override {
        return record("shut");
    }

private:
    bool record(std::string_view phase) {
        std::cerr << "app-startup-test:" << phase << '\n';
        if (phase == failed_phase_)
        {
            if (behavior_ == "throw")
            {
                throw std::runtime_error("test startup exception in " + std::string(phase));
            }
            if (behavior_ == "unknown")
            {
                throw 7;
            }
            return false;
        }
        return true;
    }

    std::string failed_phase_;
    std::string behavior_;
    bool updated_{};
};

startup_failure_module fixture;

} // namespace

// The real application loads this test-only module through its normal DLL API.
ARK_EXPORT_FUNC void DllEntryPlugin(
    ark::plugin_manager* manager, const std::string& plugin_name, ark::logger* shared_logger) {
    ark::logger::init(shared_logger);
    const auto& mode = manager->get_plugin_conf(plugin_name);
    fixture.fail_at(mode);
    if (!manager->add_module(fixture.get_name(), &fixture) || !manager->add_update_module(&fixture))
    {
        std::cerr << "Startup test module registration failed\n";
        std::abort();
    }
    if (mode == "throw:load_plugin")
    {
        std::cerr << "app-startup-test:load_plugin\n";
        throw std::runtime_error("test plugin entry exception");
    }
}

ARK_EXPORT_FUNC void DllExitPlugin(ark::plugin_manager* manager) {
    std::cerr << "app-startup-test:uninstall\n";
    manager->remove_update_module(fixture.get_name());
    manager->remove_module(fixture.get_name());
}
