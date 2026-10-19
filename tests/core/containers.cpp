// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "../network/doctest.h"

#include <ark/container/list.hpp>
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

DOCTEST_TEST_CASE("List removal keeps the void API and removes all matching values under C++20") {
    ark::list<int> values;
    values.push_back(1);
    values.push_back(2);
    values.push_back(1);
    values.push_back(3);
    values.push_back(1);
    static_assert(std::is_void_v<decltype(values.remove(1))>);
    values.remove(1);
    DOCTEST_CHECK(values.size() == 2);
    const std::vector<int> expected{2, 3};
    DOCTEST_CHECK(std::vector<int>(values.begin(), values.end()) == expected);
    values.remove(99);
    DOCTEST_CHECK(values.size() == 2);
    values.remove(2);
    values.remove(3);
    DOCTEST_CHECK(values.empty());
    values.remove(1);
    DOCTEST_CHECK(values.empty());
}

DOCTEST_TEST_CASE("List reverse iterators delimit every value and the empty range") {
    ark::list<int> values;
    DOCTEST_CHECK(values.rbegin() == values.rend());
    values.push_back(1);
    values.push_back(2);
    values.push_back(3);
    const std::vector<int> expected{3, 2, 1};
    DOCTEST_CHECK(std::vector<int>(values.rbegin(), values.rend()) == expected);
    values.clear();
    DOCTEST_CHECK(values.rbegin() == values.rend());
}

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
