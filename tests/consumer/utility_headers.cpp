// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <type_traits>

#include <ark/utility/compress_module.hpp>
#include <ark/utility/guid_module.hpp>
#include <ark/utility/schedule_module.hpp>
#include <ark/utility/scheduler.hpp>
#include <ark/utility/timer_module.hpp>

static_assert(std::is_base_of_v<ark::module, ark::compress_module>);
static_assert(std::is_base_of_v<ark::module, ark::guid_module>);
static_assert(std::is_base_of_v<ark::module, ark::schedule_module>);
static_assert(std::is_base_of_v<ark::module, ark::timer_module>);

#if !__has_include(<ark/utility/ctpl.h>) || !__has_include(<ark/utility/ctpl_stl.h>)
#error "Utility SDK mapping must retain the existing scheduler helper headers"
#endif

#if __has_include(<ark/utility/compress_module_impl.hpp>) || __has_include(<ark/utility/guid_module_impl.hpp>) || \
    __has_include(<ark/utility/schedule_module_impl.hpp>) || __has_include(<ark/utility/timer_module_impl.hpp>) || \
    __has_include(<ark/utility/utility_plugin.hpp>) || __has_include(<utility/utility_plugin.hpp>)
#error "Utility SDK targets must exclude implementation types and private construction headers"
#endif
