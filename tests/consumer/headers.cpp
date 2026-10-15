// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/hash/crc.hpp>
#include <ark/kernel/component.hpp>

static_assert(std::is_base_of_v<ark::module, ark::component>);

static_assert(std::is_default_constructible_v<ark::crc>);
