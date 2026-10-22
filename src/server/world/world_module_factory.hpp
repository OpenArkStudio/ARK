// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <memory>

#include "world_net_module.hpp"

namespace ark::detail {

std::unique_ptr<world_net_module> make_world_net_module();

} // namespace ark::detail
