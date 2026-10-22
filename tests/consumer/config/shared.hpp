// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <string>

#ifdef _WIN32
#ifdef ARK_CONFIG_PROBE_BUILD
#define ARK_CONFIG_PROBE_API __declspec(dllexport)
#else
#define ARK_CONFIG_PROBE_API __declspec(dllimport)
#endif
#else
#define ARK_CONFIG_PROBE_API
#endif

ARK_CONFIG_PROBE_API bool read_config_from_shared_library(const std::string& path, bool application);
