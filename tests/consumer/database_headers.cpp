// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <type_traits>

#include <ark/redis/redis_module.hpp>

#if defined(ARKNET_ENABLE_SSL)
#include <ark/mysql/mysql_module.hpp>
static_assert(std::is_base_of_v<ark::module, ark::mysql_module>);
#endif

static_assert(std::is_base_of_v<ark::module, ark::redis_module>);

#if __has_include(<ark/mysql/mysql_module_impl.hpp>) || __has_include(<ark/redis/redis_module_impl.hpp>) || \
    __has_include(<ark/mysql/mysql_plugin.hpp>) || __has_include(<ark/redis/redis_plugin.hpp>) || \
    __has_include(<mysql/mysql_plugin.hpp>) || __has_include(<redis/redis_plugin.hpp>)
#error "Database SDK targets must expose interfaces without plugin-private construction headers"
#endif
