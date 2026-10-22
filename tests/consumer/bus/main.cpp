// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <type_traits>

#include <ark/bus.hpp>
#include <ark/bus/bus_module.hpp>
#include <ark/bus/msg_module.hpp>

static_assert(std::is_base_of_v<ark::module, ark::bus_module>);
static_assert(std::is_base_of_v<ark::module, ark::msg_module>);

#if __has_include(<ark/bus/bus_module_impl.hpp>) || __has_include(<ark/bus/msg_module_impl.hpp>) || \
    __has_include(<ark/bus/bus_plugin.hpp>) || __has_include(<bus/bus_plugin.hpp>) || \
    __has_include(<ark/bus/bus_configuration.hpp>)
#error "Bus SDK mapping must exclude implementation and private construction/configuration headers"
#endif

int main() {
    const ark::bus_addr address(1, 1, 31, 1);
    return address.bus_id == 0;
}
