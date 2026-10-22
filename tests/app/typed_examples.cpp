// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <concepts>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

#include <ark/application_manifest.hpp>
#include <ark/plugin_manager.hpp>
#include <ark/utility/compress_module.hpp>
#include <ark/utility/guid_module.hpp>
#include <ark/utility/schedule_module.hpp>
#include <ark/utility/timer_module.hpp>
#include "../../examples/module_lifecycle/module_lifecycle_module.hpp"
#include "../../examples/timers/timers_module.hpp"

// These reflect the old registration macro's update detection for both samples.
static_assert(!std::same_as<decltype(&ark::timers_module::update), decltype(&ark::module::update)>);
static_assert(std::same_as<decltype(&ark::module_lifecycle_module::update), decltype(&ark::module::update)>);
static_assert(std::derived_from<ark::timers_module, ark::module_base<ark::module>>);
static_assert(std::derived_from<ark::module_lifecycle_module, ark::module_base<ark::module>>);

namespace {

struct timer_receiver
{
    void on_timer(uint64_t id, const ark::guid_t& entity) {
        ++calls;
        timer_id = id;
        entity_id = entity;
    }

    int calls{};
    uint64_t timer_id{};
    ark::guid_t entity_id{};
};

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
    require(sample == "timers" || sample == "module_lifecycle", "Unknown sample");
    const auto manifest =
        ark::load_application_manifest((std::filesystem::path(argv[1]) / "config" / (sample + ".app.toml")).string());
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
        auto* guids = manager.find_module<ark::guid_module>();
        auto* timers = manager.find_module<ark::timer_module>();
        auto* schedules = manager.find_module<ark::schedule_module>();
        auto* compression = manager.find_module<ark::compress_module>();
        for (auto* service : {static_cast<ark::module*>(guids), static_cast<ark::module*>(timers),
                 static_cast<ark::module*>(schedules), static_cast<ark::module*>(compression)})
        {
            require(service && service->get_plugin_manager() == &manager, "Utility interface lost manager ownership");
        }
        require(guids->get_name() == typeid(ark::guid_module).name() &&
                    timers->get_name() == typeid(ark::timer_module).name() &&
                    schedules->get_name() == typeid(ark::schedule_module).name() &&
                    compression->get_name() == typeid(ark::compress_module).name(),
            "Utility registration must retain interface RTTI keys");
        const auto first_id = guids->create_guid();
        const auto second_id = guids->create_guid();
        require(first_id != ark::null_guid && second_id != first_id && !guids->parse_uid(first_id).empty(),
            "Utility GUID generation or parsing failed");
        const std::string input("utility binary\0round trip", 25);
        for (auto algorithm : {ark::compress_type::compress_type_zlib, ark::compress_type::compress_type_zstd})
        {
            std::string packed;
            std::string unpacked;
            require(compression->compress(algorithm, input.data(), input.size(), packed), "Utility compression failed");
            require(compression->decompress(algorithm, packed.data(), packed.size(), unpacked) && unpacked == input,
                "Utility decompression did not preserve binary bytes");
        }
        timer_receiver timer;
        const auto timer_id =
            timers->add_single_timer(first_id, std::chrono::milliseconds(20), 1, &timer, &timer_receiver::on_timer);
        require(timer_id != 0 && timer.calls == 0, "Utility timer fired without manager updates");
        for (int i = 1; i <= 5; ++i)
        {
            require(manager.update(i * 10), "Sample owner update failed");
        }
        require(timer.calls == 1 && timer.timer_id == timer_id && timer.entity_id == first_id,
            "Utility timer must fire once through owner-thread updates with original callback arguments");
        require(manager.stop(60), "Sample shutdown did not complete");
        require(manager.stop(61), "Repeated stop must not unregister Utility modules twice");
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
    for (const std::string event : {sample == "timers" ? "init" : "Init", "PostInit", "PreShut", "Shut"})
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
    std::cout << sample
              << ": actual DSOs, four Utility interfaces, GUID, zlib/zstd binary round trips, owner-driven timer, "
                 "startup and shutdown passed"
              << std::endl;
}
