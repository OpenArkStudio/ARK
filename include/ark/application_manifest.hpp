// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace ark {

struct application_manifest
{
    std::string plugin_library_dir;
    std::string plugin_config_dir;
    std::unordered_map<std::string, std::string> plugins;
};

class application_manifest_error : public std::runtime_error
{
public:
    application_manifest_error(
        const std::string& path, const std::string& reason, std::size_t line = 0, std::size_t column = 0);

    const std::string& path() const noexcept;
    std::size_t line() const noexcept;
    std::size_t column() const noexcept;

private:
    std::string path_;
    std::size_t line_;
    std::size_t column_;
};

// A .toml suffix selects TOML; other paths retain the legacy XML format.
application_manifest load_application_manifest(const std::string& path);

} // namespace ark
