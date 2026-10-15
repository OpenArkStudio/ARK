// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace ark::detail {

inline std::string resource_file(std::string_view name) {
    // Leading separators in existing metadata refer to the resource directory.
    return (std::filesystem::path{"resource"} / std::filesystem::path{name}.relative_path()).string();
}

} // namespace ark::detail
