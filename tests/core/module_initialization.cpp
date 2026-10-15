// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include "../network/doctest.h"

#include "ark/kernel/class_meta_module_impl.hpp"
#include "ark/kernel/config_module_impl.hpp"
#include "ark/kernel/kernel_module_impl.hpp"
#include "ark/kernel/map_module_impl.hpp"

namespace {

class unused_guid_module final : public ark::guid_module
{
public:
    ark::guid_t create_guid() override {
        return {};
    }

    std::string parse_uid(const ark::guid_t&) override {
        return {};
    }
};

class metadata_files
{
public:
    metadata_files()
        : previous_(std::filesystem::current_path())
        , directory_(
              std::filesystem::temp_directory_path() /
              ("ark-module-init-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()))) {
        std::filesystem::create_directories(directory_ / "resourcemeta");
        write("config_class.config",
            R"(<xml><config id="attribute" meta="meta/attribute.config" res="meta/attribute.data"/></xml>)");
        write("entity_class.config", R"(<xml><config meta="meta/player.config"/></xml>)");
        write("attribute.config", R"(<xml><meta field="id" type="uint64"/><meta field="level" type="int"/></xml>)");
        write("attribute.data", R"(<xml><data id="42" level="7"/></xml>)");
        write("player.config", R"(<xml><meta class="Player" type="class"/></xml>)");
        std::filesystem::current_path(directory_);
    }

    ~metadata_files() {
        std::error_code ignored;
        std::filesystem::current_path(previous_, ignored);
        std::filesystem::remove_all(directory_, ignored);
    }

private:
    void write(const char* name, const char* contents) {
        std::ofstream output(directory_ / "resourcemeta" / name);
        output.exceptions(std::ios::badbit | std::ios::failbit);
        output << contents;
    }

    std::filesystem::path previous_;
    std::filesystem::path directory_;
};

void verify_initialization_order(bool config_init_first, bool config_post_init_first) {
    ark::logger test_logger;
    ark::logger::init(&test_logger);
    test_logger.set_log_func([](spdlog::level::level_enum, const std::string&) {});

    metadata_files files;
    ark::plugin_manager manager;
    ark::class_meta_module_impl metadata;
    ark::config_module_impl config;
    ark::kernel_module_impl kernel;
    ark::map_module_impl map;
    unused_guid_module guid;

    const auto register_module = [&]<typename interface_type>(interface_type* implementation) {
        implementation->set_plugin_manager(&manager);
        REQUIRE(manager.add_module(GET_CLASS_NAME(interface_type), implementation));
    };
    register_module.operator()<ark::class_meta_module>(&metadata);
    register_module.operator()<ark::config_module>(&config);
    register_module.operator()<ark::kernel_module>(&kernel);
    register_module.operator()<ark::map_module>(&map);
    register_module.operator()<ark::guid_module>(&guid);

    REQUIRE(map.init());
    if (config_init_first)
    {
        REQUIRE(config.init());
        REQUIRE(kernel.init());
    }
    else
    {
        REQUIRE(kernel.init());
        REQUIRE(config.init());
    }

    // Consumers have resolved their interfaces, but no schema has been loaded.
    CHECK(config.find_static_entity(42) == nullptr);
    REQUIRE(metadata.init());
    REQUIRE(metadata.find_meta("Player") != nullptr);
    REQUIRE(metadata.find_static_meta("attribute") != nullptr);

    if (config_post_init_first)
    {
        REQUIRE(config.post_init());
        REQUIRE(kernel.post_init());
    }
    else
    {
        REQUIRE(kernel.post_init());
        REQUIRE(config.post_init());
    }

    const auto entity = config.find_static_entity(42);
    REQUIRE(entity != nullptr);
    CHECK(entity->get_config_id() == 42);
    CHECK(entity->get_int32("level") == 7);
    CHECK(kernel.get_static_entity(42) == entity);
    CHECK(kernel.update());
    CHECK(kernel.pre_shut());
    CHECK(config.shut());
    CHECK(metadata.shut());
}

} // namespace

TEST_CASE("kernel and config initialize before schema regardless of consumer order") {
    SUBCASE("kernel init and kernel post_init first") {
        verify_initialization_order(false, false);
    }
    SUBCASE("kernel init and config post_init first") {
        verify_initialization_order(false, true);
    }
    SUBCASE("config init and kernel post_init first") {
        verify_initialization_order(true, false);
    }
    SUBCASE("config init and config post_init first") {
        verify_initialization_order(true, true);
    }
}
