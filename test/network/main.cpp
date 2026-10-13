// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include "base/AFLogger.hpp"
#include "http/include/AFHttpPlugin.hpp"

std::string ark::AFHttpPlugin::plugin_conf_;

int main(int argc, char** argv)
{
    ark::AFLogger logger;
    ark::AFLogger::Init(&logger);
    logger.SetLogFunc([](spdlog::level::level_enum, const std::string&) {});
    return doctest::Context(argc, argv).run();
}
