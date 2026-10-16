// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <tuple>
#include <utility>

#include "../network/doctest.h"

#include <ark/logger.hpp>

namespace {

using filename_calculator = spdlog::sinks::default_date_and_hour_file_name_calculator;

spdlog::filename_t native_filename(const std::filesystem::path& file) {
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
    return file.wstring();
#else
    return file.string();
#endif
}

struct filename_case
{
    spdlog::filename_t input;
    spdlog::filename_t directory;
    spdlog::filename_t basename;
    spdlog::filename_t extension;
};

const std::array<filename_case, 12> filename_cases = {{
    {SPDLOG_FILENAME_T("application.log"), {}, SPDLOG_FILENAME_T("application"), SPDLOG_FILENAME_T(".log")},
    {SPDLOG_FILENAME_T("application"), {}, SPDLOG_FILENAME_T("application"), {}},
    {SPDLOG_FILENAME_T("application."), {}, SPDLOG_FILENAME_T("application."), {}},
    {SPDLOG_FILENAME_T(".application"), {}, SPDLOG_FILENAME_T(".application"), {}},
    {SPDLOG_FILENAME_T(".application.log"), {}, SPDLOG_FILENAME_T(".application"), SPDLOG_FILENAME_T(".log")},
    {SPDLOG_FILENAME_T("logs/application.log"), SPDLOG_FILENAME_T("logs/"), SPDLOG_FILENAME_T("application"),
        SPDLOG_FILENAME_T(".log")},
    {SPDLOG_FILENAME_T("logs/application"), SPDLOG_FILENAME_T("logs/"), SPDLOG_FILENAME_T("application"), {}},
    {SPDLOG_FILENAME_T("logs/application."), SPDLOG_FILENAME_T("logs/"), SPDLOG_FILENAME_T("application."), {}},
    {SPDLOG_FILENAME_T("logs/.application"), SPDLOG_FILENAME_T("logs/"), SPDLOG_FILENAME_T(".application"), {}},
    {SPDLOG_FILENAME_T("logs/.application.log"), SPDLOG_FILENAME_T("logs/"), SPDLOG_FILENAME_T(".application"),
        SPDLOG_FILENAME_T(".log")},
    {SPDLOG_FILENAME_T("logs.v1/application"), SPDLOG_FILENAME_T("logs.v1/"), SPDLOG_FILENAME_T("application"), {}},
    {SPDLOG_FILENAME_T("logs.v1/nested/application.log"), SPDLOG_FILENAME_T("logs.v1/nested/"),
        SPDLOG_FILENAME_T("application"), SPDLOG_FILENAME_T(".log")},
}};

spdlog::filename_t expected_filename(const filename_case& entry, const std::tm& time) {
    const auto day =
        fmt::format(SPDLOG_FILENAME_T("{:04d}{:02d}{:02d}"), time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
    const auto name = fmt::format(SPDLOG_FILENAME_T("{}_{:02d}{}"), entry.basename, time.tm_hour, entry.extension);
    return native_filename(std::filesystem::path(entry.directory) / day / name);
}

class log_directory
{
public:
    const std::filesystem::path directory =
        std::filesystem::temp_directory_path() /
        ("ark-log-path-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));

    log_directory() {
        REQUIRE(std::filesystem::create_directory(directory));
    }

    ~log_directory() {
        std::error_code ignored;
        std::filesystem::remove_all(directory, ignored);
    }
};

class temporary_current_directory
{
public:
    explicit temporary_current_directory(const std::filesystem::path& directory)
        : previous_(std::filesystem::current_path()) {
        std::filesystem::current_path(directory);
    }

    ~temporary_current_directory() {
        std::error_code ignored;
        std::filesystem::current_path(previous_, ignored);
    }

private:
    std::filesystem::path previous_;
};

class logger_registration
{
public:
    explicit logger_registration(std::string name)
        : name_(std::move(name)) {
    }

    ~logger_registration() {
        spdlog::drop(name_);
    }

private:
    std::string name_;
};

std::string read_log(const std::filesystem::path& file) {
    std::ifstream input(file, std::ios::binary);
    REQUIRE(input.is_open());
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

} // namespace

TEST_CASE("Logging path splitting keeps directories separate from basenames and extensions") {
    for (const auto& entry : filename_cases)
    {
        const auto [directory, basename, extension] = filename_calculator::split_by_dir_and_extenstion(entry.input);
        CHECK(directory == entry.directory);
        CHECK(basename == entry.basename);
        CHECK(extension == entry.extension);
    }
#ifdef _WIN32
    const auto [directory, basename, extension] =
        filename_calculator::split_by_dir_and_extenstion(SPDLOG_FILENAME_T("logs.v1\\nested\\application.log"));
    CHECK(directory == SPDLOG_FILENAME_T("logs.v1\\nested\\"));
    CHECK(basename == SPDLOG_FILENAME_T("application"));
    CHECK(extension == SPDLOG_FILENAME_T(".log"));
#endif
}

TEST_CASE("Logging filename calculation preserves relative paths without creating directories") {
    for (const auto& entry : filename_cases)
    {
        const auto before = spdlog::details::os::localtime();
        const auto actual = filename_calculator::calc_filename(entry.input);
        const auto after = spdlog::details::os::localtime();
        CHECK((actual == expected_filename(entry, before) || actual == expected_filename(entry, after)));
        CHECK_FALSE(std::filesystem::path(actual).is_absolute());
    }
}

TEST_CASE("Logging hourly sink creates nested directories for filenames without an extension") {
    log_directory files;
    const auto input = files.directory / "logs.v1" / "nested" / "application";
    const auto output = filename_calculator::calc_filename(native_filename(input));
    REQUIRE(std::filesystem::path(output).parent_path().parent_path() == input.parent_path());
    {
        auto sink = std::make_shared<spdlog::sinks::date_and_hour_file_sink_mt>(native_filename(input));
        spdlog::logger logger("direct-log-path", sink);
        logger.set_pattern("%v");
        logger.info("message {literal}");
        logger.flush();
    }
    CHECK(read_log(output) == std::string("message {literal}") + spdlog::details::os::default_eol);
}

TEST_CASE("Logging multi-thread factory forwards the supplied filename") {
    log_directory files;
    const auto input = files.directory / "multi" / "application.log";
    const auto name = "ark-factory-mt-" + files.directory.filename().string();
    logger_registration registration(name);
    auto logger = spdlog::date_and_hour_file_sink_mt(name, native_filename(input));
    logger->set_pattern("%v");
    logger->info("multi-thread factory");
    logger->flush();
    CHECK(spdlog::get(name) == logger);
    CHECK(read_log(filename_calculator::calc_filename(native_filename(input))) ==
          std::string("multi-thread factory") + spdlog::details::os::default_eol);
}

TEST_CASE("Logging single-thread factory forwards the supplied filename") {
    log_directory files;
    const auto input = files.directory / "single" / "application.log";
    const auto name = "ark-factory-st-" + files.directory.filename().string();
    logger_registration registration(name);
    auto logger = spdlog::date_and_hour_file_sink_st(name, native_filename(input));
    logger->set_pattern("%v");
    logger->info("single-thread factory");
    logger->flush();
    CHECK(spdlog::get(name) == logger);
    CHECK(read_log(filename_calculator::calc_filename(native_filename(input))) ==
          std::string("single-thread factory") + spdlog::details::os::default_eol);
}

TEST_CASE("Logging configured output directory retains its nested file path") {
    log_directory files;
    auto config = std::make_shared<ark::log_conf_data>();
    config->output_path = (files.directory / "configured").string();
    config->sink_queue_count = 0;
    config->consol = false;
    config->pattern = "%v";
    ark::spdlogger logger(config);
    logger.init((std::filesystem::path("nested") / "application").string());
    const auto input = std::filesystem::path(config->output_path) / "nested" / "application";
    CHECK(spdlog::get(input.string()) != nullptr);
    logger.log(spdlog::level::info, "configured output");
#ifdef OPEN_FILE_LOG
    CHECK(read_log(filename_calculator::calc_filename(native_filename(input))) ==
          std::string("configured output") + spdlog::details::os::default_eol);
#endif
}

TEST_CASE("Logging empty output directory keeps the logger and file relative to the working directory") {
    log_directory files;
    temporary_current_directory current(files.directory);
    auto config = std::make_shared<ark::log_conf_data>();
    config->output_path.clear();
    config->sink_queue_count = 0;
    config->consol = false;
    config->pattern = "%v";
    ark::spdlogger logger(config);
    logger.init("application.log");
    CHECK(spdlog::get("application.log") != nullptr);
    logger.log(spdlog::level::info, "relative output");
#ifdef OPEN_FILE_LOG
    const auto output = filename_calculator::calc_filename(SPDLOG_FILENAME_T("application.log"));
    CHECK_FALSE(std::filesystem::path(output).is_absolute());
    CHECK(read_log(output) == std::string("relative output") + spdlog::details::os::default_eol);
#endif
}
