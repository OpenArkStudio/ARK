// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <ark/container/array_map.hpp>

// Keep the second inclusion separate to retain the include-guard check.
#include <ark/container/array_map.hpp>

template class ark::array_map<std::string, int>;

#if defined(ARK_CPP11_OR_GREATER) || defined(ARK_CPP14_OR_GREATER) || defined(ARK_CPP17_OR_GREATER) ||                 \
    defined(ARK_NOEXCEPT) || defined(ARK_CONSTEXPR) || defined(ARK_FINAL) || defined(MAKE_BUILDIN_TYPE)
#error "Use C++20 keywords and explicit type traits instead of removed compatibility macros"
#endif
