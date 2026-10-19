// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/container/map.hpp>

// Keep the second inclusion separate to retain the include-guard check.
#include <ark/container/map.hpp>

template class ark::object_map_base<int, int*>;
template class ark::object_map_base<int, std::shared_ptr<int>>;
template class ark::object_map_base<int, int*, true>;
template class ark::object_map_base<int, std::shared_ptr<int>, true>;
