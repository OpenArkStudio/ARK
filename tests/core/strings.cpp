// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <string>
#include <unordered_map>

#include "../network/doctest.h"

#include <ark/string.hpp>

DOCTEST_TEST_CASE("Small strings preserve content through growth, copies and inline/heap swaps") {
    ark::small_string<char, 8> short_text("abc");
    ark::small_string<char, 8> long_text("a longer string");
    short_text.swap(long_text);
    DOCTEST_REQUIRE(std::string(short_text.c_str()) == "a longer string");
    DOCTEST_REQUIRE(std::string(long_text.c_str()) == "abc");
    auto copy = short_text;
    short_text.clear();
    DOCTEST_REQUIRE(copy.size() == 15);
    DOCTEST_REQUIRE(std::string(copy.c_str()) == "a longer string");
    DOCTEST_REQUIRE(short_text.empty());
    short_text.append("12345678");
    short_text.append('9');
    DOCTEST_REQUIRE(short_text.size() == 9);
    DOCTEST_REQUIRE(std::string(short_text.c_str()) == "123456789");
    DOCTEST_REQUIRE(short_text.find("456") == 3);
    DOCTEST_REQUIRE(std::string(short_text.substr(2, 3).c_str()) == "345");
    long_text = copy;
    DOCTEST_REQUIRE(std::string(long_text.c_str()) == "a longer string");
}

DOCTEST_TEST_CASE("Small-string equality is consistent for both operand orders and inequalities") {
    const ark::small_string<char, 8> value("hello");
    const ark::small_string<char, 8> same("hello");
    const ark::small_string<char, 8> other("world");
    DOCTEST_CHECK(value == same);
    DOCTEST_CHECK_FALSE(value == other);
    DOCTEST_CHECK(value != other);
    DOCTEST_CHECK_FALSE(value != same);
    DOCTEST_CHECK(value == "hello");
    DOCTEST_CHECK("hello" == value);
    DOCTEST_CHECK_FALSE(value != "hello");
    DOCTEST_CHECK_FALSE("hello" != value);
    DOCTEST_CHECK(value != "world");
    DOCTEST_CHECK("world" != value);
}

DOCTEST_TEST_CASE("Data-node names work as hash-map keys with equal content") {
    std::unordered_map<ark::data_node_name, int, ark::hash<ark::data_node_name>> values;
    values.emplace(ark::data_node_name("health"), 42);
    values.emplace(ark::data_node_name("health"), 99);
    DOCTEST_REQUIRE(values.size() == 1);
    DOCTEST_REQUIRE(values.at(ark::data_node_name("health")) == 42);
    DOCTEST_REQUIRE_FALSE(values.contains(ark::data_node_name("energy")));
}
