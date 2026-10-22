// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <type_traits>

#include <ark/kernel/class_meta_module_impl.hpp>
#include <ark/kernel/config_module_impl.hpp>
#include <ark/kernel/data_impl.hpp>
#include <ark/kernel/data_list_impl.hpp>
#include <ark/kernel/kernel_plugin.hpp>
#include <ark/kernel/node_impl.hpp>

static_assert(std::is_base_of_v<ark::module_base<ark::class_meta_module>, ark::class_meta_module_impl>);
static_assert(std::is_base_of_v<ark::module_base<ark::config_module>, ark::config_module_impl>);
static_assert(std::is_base_of_v<ark::plugin_base<ark::kernel_plugin>, ark::kernel_plugin>);
static_assert(std::is_same_v<decltype(&ark::class_meta_module_impl::update), decltype(&ark::module::update)>);
static_assert(std::is_same_v<decltype(&ark::config_module_impl::update), decltype(&ark::module::update)>);

#ifdef ARK_HAS_PROTOBUF
#include <ark/kernel/kernel_module_impl.hpp>
#include <ark/kernel/map_module_impl.hpp>

static_assert(std::is_base_of_v<ark::module_base<ark::kernel_module>, ark::kernel_module_impl>);
static_assert(std::is_base_of_v<ark::module_base<ark::map_module>, ark::map_module_impl>);
static_assert(!std::is_same_v<decltype(&ark::kernel_module_impl::update), decltype(&ark::module::update)>);
static_assert(std::is_same_v<decltype(&ark::map_module_impl::update), decltype(&ark::module::update)>);
#endif

static_assert(std::is_same_v<ark::data_impl, ark::data_base<4, ark::core_allocator>>);
static_assert(std::is_same_v<ark::data_list_impl, ark::base_data_list<8, 128>>);

int main() {
    ark::data_impl value(ark::data_type::dt_int32, 42);
    ark::data_list_impl arguments;
    arguments << 42;
    return value.get_int() == 42 && arguments.get_count() == 1 ? 0 : 1;
}
