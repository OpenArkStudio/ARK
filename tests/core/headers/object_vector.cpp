// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/container/vector.hpp>

// Keep the second inclusion separate to retain the include-guard check.
#include <ark/container/vector.hpp>

template class ark::object_vector_base<int, false>;
template class ark::object_vector_base<int, true>;
