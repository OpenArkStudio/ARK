// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <type_traits>

#include <ark/http/http_server_module.hpp>

static_assert(std::is_base_of_v<ark::module, ark::http_server_module>);
static_assert(std::is_same_v<decltype(std::declval<ark::http_server_module&>().get_router()), arknet::http_router&>);

#if __has_include(<ark/http/http_server_module_impl.hpp>) || __has_include(<ark/http/http_plugin.hpp>) || \
    __has_include(<http/http_plugin.hpp>) || __has_include(<detail/plugin_configuration.hpp>)
#error "HTTP public header mapping must exclude implementation, assembly and parser headers"
#endif

int main() {
    return 0;
}
