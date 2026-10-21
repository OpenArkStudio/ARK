// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <concepts>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

#include <ark/application_manifest.hpp>
#include <ark/plugin_manager.hpp>
#include "../../examples/sample1/sample1_module.hpp"
#include "../../examples/sample2/sample2_module.hpp"

// These reflect the old registration macro's update detection for both samples.
static_assert(!std::same_as<decltype(&ark::sample1_module::update), decltype(&ark::module::update)>);
static_assert(std::same_as<decltype(&ark::sample2_module::update), decltype(&ark::module::update)>);
static_assert(std::derived_from<ark::sample1_module, ark::module_base<ark::module>>);
static_assert(std::derived_from<ark::sample2_module, ark::module_base<ark::module>>);

namespace {

void require(bool condition, const std::string& message) {
    if (!condition)
    {
        std::cerr << message << std::endl;
        // Preserve retained startup resources on a failing test process too.
        std::_Exit(1);
    }
}

} // namespace

int main(int argc, char* argv[]) {
    require(argc == 4, "Expected source root, plugin directory and sample name");
    const std::string sample = argv[3];
    require(sample == "sample1" || sample == "sample2", "Unknown sample");
    const auto manifest =
        ark::load_application_manifest((std::filesystem::path(argv[1]) / "config" / (sample + ".app.conf")).string());
    std::ostringstream events;
    auto* original = std::cout.rdbuf(events.rdbuf());
    ark::logger log;
    ark::logger::init(&log);
    log.set_log_func([&events](spdlog::level::level_enum, const std::string& text) { events << text << '\n'; });
    ark::plugin_manager manager;
    try
    {
        const bool started = manager.start(0, std::string(argv[2]) + "/", "conf/", manifest.plugins);
        require(started, "Sample startup failed: " + manager.get_startup_failure());
        for (int i = 1; i <= 5; ++i)
        {
            require(manager.update(i), "Sample owner update failed");
        }
        require(manager.stop(6), "Sample shutdown did not complete");
        require(manager.get_shutdown_state() == ark::plugin_manager::shutdown_state::stopped,
            "Sample resources were retained instead of stopped");
    }
    catch (const std::exception& error)
    {
        require(false,
            "Sample lifecycle exception: " + std::string(error.what()) + "; startup: " + manager.get_startup_failure());
    }
    std::cout.rdbuf(original);
    const auto output = events.str();
    std::cout << output;
    std::size_t previous{};
    for (const std::string event : {sample == "sample1" ? "init" : "Init", "PostInit", "PreShut", "Shut"})
    {
        auto position = output.find(sample + "_module", previous);
        while (position != std::string::npos)
        {
            const auto end = output.find('\n', position);
            if (output.substr(position, end - position).find(", " + event) != std::string::npos)
            {
                break;
            }
            position = output.find(sample + "_module", end);
        }
        require(position != std::string::npos, "Missing/out-of-order sample event: " + event);
        previous = output.find('\n', position);
    }
    std::cout << sample << ": actual DSOs, five manager-thread updates, startup and shutdown passed" << std::endl;
}
