// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <type_traits>

#include <ark/kernel/data_impl.hpp>
#include <ark/kernel/data_list_impl.hpp>
#include <ark/kernel/node_impl.hpp>

static_assert(std::is_same_v<ark::data_impl, ark::data_base<4, ark::core_allocator>>);
static_assert(std::is_same_v<ark::data_list_impl, ark::base_data_list<8, 128>>);

int main() {
    ark::data_impl value(ark::data_type::dt_int32, 42);
    ark::data_list_impl arguments;
    arguments << 42;
    return value.get_int() == 42 && arguments.get_count() == 1 ? 0 : 1;
}
