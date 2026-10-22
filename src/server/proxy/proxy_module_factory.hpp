// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <memory>

#include "proxy_net_module.hpp"

namespace ark::detail {

std::unique_ptr<proxy_net_module> make_proxy_net_module();

} // namespace ark::detail
