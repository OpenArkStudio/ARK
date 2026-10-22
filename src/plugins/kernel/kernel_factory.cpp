// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include "ark/kernel/class_meta_module_impl.hpp"
#include "ark/kernel/config_module_impl.hpp"
#include "ark/kernel/kernel_module_impl.hpp"
#include "ark/kernel/map_module_impl.hpp"
#include "kernel_factory.hpp"

namespace ark::detail {

std::unique_ptr<class_meta_module> make_class_meta_module() {
    return std::make_unique<class_meta_module_impl>();
}

std::unique_ptr<config_module> make_config_module() {
    return std::make_unique<config_module_impl>();
}

std::unique_ptr<kernel_module> make_kernel_module() {
    return std::make_unique<kernel_module_impl>();
}

std::unique_ptr<map_module> make_map_module() {
    return std::make_unique<map_module_impl>();
}

} // namespace ark::detail
