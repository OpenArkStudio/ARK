// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.

#define DOCTEST_CONFIG_IMPLEMENT
#include "ark/http/http_plugin.hpp"
#include "ark/logger.hpp"
#include "doctest.h"

std::string ark::http_plugin::plugin_conf_;

int main(int argc, char** argv) {
    ark::logger logger;
    ark::logger::init(&logger);
    logger.set_log_func([](spdlog::level::level_enum, const std::string&) {});
    return doctest::Context(argc, argv).run();
}
