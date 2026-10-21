// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/logger.hpp>

// Keep the second inclusion separate to retain the include-guard check.
#include <ark/logger.hpp>

static_assert(ark::detail::log_basename("directory/application.cpp") == "application.cpp");
static_assert(ark::detail::log_basename(R"(directory\application.cpp)") == "application.cpp");
static_assert(ark::detail::log_basename(R"(directory\mixed/application.cpp)") == "application.cpp");
static_assert(ark::detail::log_basename(R"(directory/mixed\application.cpp)") == "application.cpp");
static_assert(ark::detail::log_basename("application.cpp") == "application.cpp");
static_assert(ark::detail::log_basename("").empty());
static_assert(ark::detail::log_basename("directory/").empty());
static_assert(ark::detail::log_basename("directory\\").empty());
static_assert(noexcept(ark::detail::log_basename(std::string_view{})));
static_assert(noexcept(ark::detail::log_source_location()));
constexpr auto header_location = ark::detail::log_source_location();
static_assert(std::string_view(header_location.filename) == "logger.cpp");
static_assert(header_location.line == __LINE__ - 2);
static_assert(std::string_view(header_location.funcname).empty());

namespace ark {

void check_logger_variadic_header() {
    ARK_LOG_INFO("header-only logging");
    ARK_LOG_INFO("header-only logging {}", 42);
}

} // namespace ark
