// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <string>
#include <type_traits>

#include "../network/doctest.h"

#include <ark/lexical_cast.hpp>
#include <ark/plugin_manager.hpp>

namespace {

template<typename type>
concept has_module_lookup = requires(ark::plugin_manager& manager) { manager.find_module<type>(); };

static_assert(has_module_lookup<ark::module>);
static_assert(!has_module_lookup<int>);

} // namespace

DOCTEST_TEST_CASE("Core conversion templates preserve supported values and errors") {
    DOCTEST_REQUIRE(lexical_cast<int>(std::string("42")) == 42);
    DOCTEST_REQUIRE(lexical_cast<int>(7) == 7);
    DOCTEST_REQUIRE(lexical_cast<std::string>(9) == "9");
    DOCTEST_REQUIRE(lexical_cast<std::string>(std::string("same")) == "same");
    DOCTEST_REQUIRE(lexical_cast<bool>(-1));
    DOCTEST_REQUIRE_FALSE(lexical_cast<bool>(0));
    DOCTEST_REQUIRE(lexical_cast<bool>("true"));
    DOCTEST_REQUIRE_FALSE(lexical_cast<bool>(std::string("false")));
    DOCTEST_REQUIRE_THROWS_AS(lexical_cast<bool>("nope"), std::invalid_argument);
    DOCTEST_REQUIRE_FALSE(lexical_cast<bool>("invalid"));
}

DOCTEST_TEST_CASE("Plugin-manager typed lookup exposes the requested module type") {
    static_assert(std::is_same_v<decltype(ark::plugin_manager::instance()->find_module<ark::module>()), ark::module*>);
    DOCTEST_REQUIRE(ark::plugin_manager::instance() != nullptr);
}
