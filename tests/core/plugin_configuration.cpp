// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

#include "../network/doctest.h"

#include "detail/plugin_configuration.hpp"

namespace {

class configuration_file
{
public:
    configuration_file()
        : path_(
              std::filesystem::temp_directory_path() /
              ("ark-plugin-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".toml")) {
    }

    ~configuration_file() {
        std::error_code ignored;
        std::filesystem::remove(path_, ignored);
    }

    std::string write(std::string_view text) const {
        std::ofstream output(path_, std::ios::binary);
        output.exceptions(std::ios::badbit | std::ios::failbit);
        output.write(text.data(), static_cast<std::streamsize>(text.size()));
        output.close();
        return path_.string();
    }

private:
    std::filesystem::path path_;
};

} // namespace

TEST_CASE("HTTP TOML settings retain IPv6 host and explicit concurrency") {
    configuration_file file;
    const auto config = ark::plugin_config_detail::load_http(file.write("[http]\nhost='::1'\nport=0\nthreads=4\n"));
    CHECK(config.host == "::1");
    CHECK(config.port == 0);
    CHECK(config.threads == 4);
    const auto maximum =
        ark::plugin_config_detail::load_http(file.write("[http]\nhost='localhost'\nport=65535\nthreads=255\n"));
    CHECK(maximum.port == 65535);
    CHECK(maximum.threads == 255);
}

TEST_CASE("HTTP rejects missing mistyped unknown and out-of-range settings") {
    configuration_file file;
    for (const auto text : {"[http]\nhost='localhost'\nport=9090\nthread=4\n",
             "[http]\nhost='localhost'\nport=65536\nthreads=4\n", "[http]\nhost='localhost'\nport=-1\nthreads=4\n",
             "[http]\nhost='localhost'\nport=9090\nthreads=0\n", "[http]\nhost='localhost'\nport=9090\nthreads=256\n",
             "[http]\nhost='localhost'\nport=9090\nthreads=4.0\n", "[http]\nhost='localhost'\nport='9090'\nthreads=4\n",
             "[http]\nhost=''\nport=9090\nthreads=4\n", "[http]\nhost=\"\\u0000\"\nport=9090\nthreads=4\n",
             "[http]\nport=9090\nthreads=4\n", "[wrong]\nhost='localhost'\n", "<xml><conf ip='localhost'/></xml>"})
    {
        CHECK_THROWS_AS(ark::plugin_config_detail::load_http(file.write(text)), std::runtime_error);
    }
}

TEST_CASE("Consul accepts a hostname and validates its port before network access") {
    configuration_file file;
    const auto config = ark::plugin_config_detail::load_consul(file.write("[consul]\nhost='localhost'\nport=8500\n"));
    CHECK(config.host == "localhost");
    CHECK(config.port == 8500);
    for (const auto text : {"[consul]\nhost='localhost'\nport=0\n", "[consul]\nhost='localhost'\nport=65536\n",
             "[consul]\nhost='localhost'\n", "[consul]\nhost='localhost'\nport=8500\npassword='not-logged'\n"})
    {
        CHECK_THROWS_AS(ark::plugin_config_detail::load_consul(file.write(text)), std::runtime_error);
    }
}

TEST_CASE("Plugin configuration diagnostics identify the file and invalid field") {
    configuration_file file;
    const auto path = file.write("[http]\nhost='localhost'\nport=9090\nthreads=false\n");
    try
    {
        static_cast<void>(ark::plugin_config_detail::load_http(path));
        FAIL("invalid threads accepted");
    }
    catch (const std::runtime_error& error)
    {
        CHECK(std::string(error.what()).find(path) != std::string::npos);
        CHECK(std::string(error.what()).find("threads") != std::string::npos);
    }
    std::filesystem::remove(path);
    CHECK_THROWS_AS(ark::plugin_config_detail::load_http(path), std::runtime_error);
}
