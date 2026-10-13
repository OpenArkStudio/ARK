// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "doctest.h"
#include "base/AFLogger.hpp"
#include "base/spdlog/sinks/rotating_file_with_date_sink.h"

namespace {

struct LogDirectory
{
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() /
        ("ark-logging-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));

    LogDirectory()
    {
        REQUIRE(std::filesystem::create_directory(path));
    }

    ~LogDirectory()
    {
        std::error_code ignored;
        std::filesystem::remove_all(path, ignored);
    }
};

std::string ReadLog(const std::filesystem::path& path)
{
    std::ifstream stream(path, std::ios::binary);
    REQUIRE(stream.is_open());
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}

} // namespace

TEST_CASE("Logging preserves runtime formats source locations and literal braces")
{
    const std::string format = "value={}";
    CHECK(ARK_FORMAT(format, 7) == "value=7");
    CHECK(ARK_FORMAT("{{literal}}") == "{literal}");
    const std::string with_function = ARK_FORMAT_FUNCTION(format, 7);
    CHECK(with_function.ends_with("]value=7"));

    ark::AFLogger logger;
    std::vector<std::pair<spdlog::level::level_enum, std::string>> messages;
    logger.SetLogFunc([&messages](spdlog::level::level_enum level, const std::string& content) {
        messages.emplace_back(level, content);
    });
    logger.LogT(spdlog::source_loc{"logging.cpp", 42, ""}, spdlog::level::warn, "value={} text={}", 7, "{plain}");
    REQUIRE(messages.size() == 1);
    CHECK(messages.front().first == spdlog::level::warn);
    CHECK(messages.front().second == "[logging.cpp:42]value=7 text={plain}");
}

TEST_CASE("Logging file sinks create nested directories and preserve rotation output")
{
    LogDirectory directory;
    const auto base_file = directory.path / "nested" / "application.log";
    const std::string message = "value={plain}";
    const std::string eol = spdlog::details::os::default_eol;

    SUBCASE("hourly")
    {
        const auto filename =
            spdlog::sinks::default_date_and_hour_file_name_calculator::calc_filename(base_file.string());
        {
            auto sink = std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>(base_file.string());
            spdlog::logger logger("hourly-test", sink);
            logger.set_pattern("%v");
            logger.log(spdlog::level::info, message);
            logger.flush();
        }
        CHECK(ReadLog(filename) == message + eol);
    }

    SUBCASE("size rotation")
    {
        using Sink = spdlog::sinks::rotating_file_with_date_sink_mt;
        const auto filename = Sink::calc_filename(base_file.string(), 0);
        const auto rotated = Sink::calc_filename(base_file.string(), 1);
        {
            auto sink = std::make_shared<Sink>(base_file.string(), 24, 2);
            spdlog::logger logger("rotating-test", sink);
            logger.set_pattern("%v");
            logger.log(spdlog::level::info, message);
            logger.log(spdlog::level::info, message);
            logger.flush();
        }
        CHECK(ReadLog(filename) == message + eol);
        CHECK(ReadLog(rotated) == message + eol);
    }
}
