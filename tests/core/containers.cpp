// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <memory>
#include <string>

#include "../network/doctest.h"

#include <ark/container/map.hpp>

namespace {

struct counted_object
{
    explicit counted_object(int& destroyed)
        : destroyed_(destroyed) {
    }

    ~counted_object() {
        ++destroyed_;
    }

    int& destroyed_;
};

} // namespace

DOCTEST_TEST_CASE("Object-map constant end iterators delimit all entries") {
    ark::shared_object_map<int, std::string> values;
    values.insert(1, std::make_shared<std::string>("one"));
    values.insert(2, std::make_shared<std::string>("two"));
    const auto& constant_values = values;
    DOCTEST_CHECK(constant_values.cbegin() != constant_values.cend());
    DOCTEST_CHECK(constant_values.cend() == constant_values.end());
    int count = 0;
    for (auto it = constant_values.cbegin(); it != constant_values.cend(); ++it)
    {
        ++count;
    }
    DOCTEST_CHECK(count == 2);
}

DOCTEST_TEST_CASE("Object maps release owned objects exactly once and shared maps retain external owners") {
    int destroyed = 0;
    {
        ark::object_map<int, counted_object> values;
        values.insert(1, new counted_object(destroyed));
        values.insert(2, new counted_object(destroyed));
        DOCTEST_REQUIRE(values.erase(1));
        DOCTEST_REQUIRE(destroyed == 1);
        DOCTEST_REQUIRE_FALSE(values.erase(1));
        values.clear();
        DOCTEST_REQUIRE(values.empty());
        DOCTEST_REQUIRE(destroyed == 2);
    }
    auto owner = std::make_shared<counted_object>(destroyed);
    {
        ark::shared_object_hashmap<int, counted_object> values;
        values.insert(1, owner);
        values.clear();
        DOCTEST_REQUIRE(destroyed == 2);
    }
    owner.reset();
    DOCTEST_REQUIRE(destroyed == 3);
}
