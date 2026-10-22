// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <memory>

#include "simulate_module.hpp"
#include "simulate_net_module.hpp"

namespace ark::detail {

std::unique_ptr<simulate_net_module> make_simulate_net_module();
std::unique_ptr<simulate_module> make_simulate_module();

} // namespace ark::detail
