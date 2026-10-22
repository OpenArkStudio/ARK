// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <memory>

#include "router_net_module.hpp"

namespace ark::detail {

std::unique_ptr<router_net_module> make_router_net_module();

} // namespace ark::detail
