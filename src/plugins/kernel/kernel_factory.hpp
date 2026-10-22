// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <memory>

#include "ark/kernel/class_meta_module.hpp"
#include "ark/kernel/config_module.hpp"
#include "ark/kernel/kernel_module.hpp"
#include "ark/kernel/map_module.hpp"

// Plugin-private construction; deliberately absent from public header mappings.
namespace ark::detail {

std::unique_ptr<class_meta_module> make_class_meta_module();
std::unique_ptr<config_module> make_config_module();
std::unique_ptr<kernel_module> make_kernel_module();
std::unique_ptr<map_module> make_map_module();

} // namespace ark::detail
