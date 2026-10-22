// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <memory>

#include "game_net_module.hpp"

namespace ark::detail {

std::unique_ptr<game_net_module> make_game_net_module();

} // namespace ark::detail
