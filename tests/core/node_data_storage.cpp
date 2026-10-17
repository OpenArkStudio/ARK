// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <cstddef>
#include <cstring>
#include <memory>
#include <new>
#include <string>
#include <unordered_map>

#include "../network/doctest.h"

#include <ark/kernel/data_impl.hpp>
#include <ark/kernel/node_impl.hpp>

namespace {

class tracked_allocator
{
public:
    static inline std::unordered_map<void*, std::size_t> outstanding;
    static inline bool fail_next{false};

    void* alloc(std::size_t size) {
        if (fail_next)
        {
            fail_next = false;
            throw std::bad_alloc{};
        }

        auto* ptr = new char[size];
        outstanding.emplace(ptr, size);
        return ptr;
    }

    void free(void* ptr, std::size_t size) {
        if (ptr == nullptr)
        {
            return;
        }

        const auto entry = outstanding.find(ptr);
        DOCTEST_REQUIRE(entry != outstanding.end());
        DOCTEST_CHECK(entry->second == size);
        outstanding.erase(entry);
        delete[] static_cast<char*>(ptr);
    }

    void swap(tracked_allocator&) {
    }
};

class allocation_scope
{
public:
    allocation_scope() {
        DOCTEST_REQUIRE(tracked_allocator::outstanding.empty());
    }

    ~allocation_scope() {
        // Keep a failing ownership assertion from leaking test allocations.
        for (const auto& [ptr, size] : tracked_allocator::outstanding)
        {
            delete[] static_cast<char*>(ptr);
        }

        tracked_allocator::outstanding.clear();
        tracked_allocator::fail_next = false;
    }
};

std::shared_ptr<ark::node_meta> metadata(ark::data_type type) {
    auto result = std::make_shared<ark::node_meta>("value", 0);
    result->set_type(type);
    return result;
}

using tracked_data = ark::data_base<8, tracked_allocator>;

} // namespace

DOCTEST_TEST_CASE("Node conversion releases an old allocated string before retaining a scalar") {
    allocation_scope allocations;
    ark::node_int32 source(metadata(ark::data_type::dt_int32));
    source.set_int32(42);
    {
        tracked_data value(ark::data_type::dt_string, "previous allocated storage");
        DOCTEST_REQUIRE(tracked_allocator::outstanding.size() == 1);
        DOCTEST_REQUIRE(value.from(&source));
        DOCTEST_CHECK(value.get_int() == 42);
        DOCTEST_CHECK(source.get_int32() == 42);
        DOCTEST_CHECK(tracked_allocator::outstanding.empty());
    }

    DOCTEST_CHECK(tracked_allocator::outstanding.empty());
}

DOCTEST_TEST_CASE("Node conversion releases replaced string and user data allocations") {
    for (const auto& text : {std::string("short"), std::string(80, 'x')})
    {
        ark::node_string source(metadata(ark::data_type::dt_string));
        source.set_string(text);
        const auto expected = text.size() + 1 > 8 ? 1u : 0u;
        {
            allocation_scope allocations;
            {
                tracked_data value(ark::data_type::dt_string, "previous allocated storage");
                for (int repeat = 0; repeat < 5; ++repeat)
                {
                    DOCTEST_REQUIRE(value.from(&source));
                    DOCTEST_CHECK(std::string(value.get_string()) == text);
                    DOCTEST_CHECK(tracked_allocator::outstanding.size() == expected);
                    DOCTEST_CHECK(source.get_string() == text);
                }
            }
            DOCTEST_CHECK(tracked_allocator::outstanding.empty());
        }
        {
            allocation_scope allocations;
            {
                const char bytes[] = "owned payload";
                tracked_data value(ark::data_type::dt_userdata, bytes, sizeof(bytes));
                DOCTEST_REQUIRE(tracked_allocator::outstanding.size() == 1);
                DOCTEST_REQUIRE(value.from(&source));
                DOCTEST_CHECK(std::string(value.get_string()) == text);
                DOCTEST_CHECK(tracked_allocator::outstanding.size() == expected);
            }
            DOCTEST_CHECK(tracked_allocator::outstanding.empty());
        }
    }
}

DOCTEST_TEST_CASE("Null node conversion preserves the existing allocation and value") {
    allocation_scope allocations;
    {
        tracked_data value(ark::data_type::dt_string, "previous allocated storage");
        const auto* storage = value.get_string();
        DOCTEST_CHECK_FALSE(value.from(nullptr));
        DOCTEST_CHECK(value.get_type() == ark::data_type::dt_string);
        DOCTEST_CHECK(value.get_string() == storage);
        DOCTEST_CHECK(std::string(value.get_string()) == "previous allocated storage");
        DOCTEST_CHECK(tracked_allocator::outstanding.size() == 1);
    }

    DOCTEST_CHECK(tracked_allocator::outstanding.empty());
}

DOCTEST_TEST_CASE("Failed node replacement allocation preserves the previous value and ownership") {
    for (int kind = 0; kind < 3; ++kind)
    {
        allocation_scope allocations;
        ark::node_string source(metadata(ark::data_type::dt_string));
        source.set_string(std::string(80, 'x'));
        {
            const char bytes[] = "owned payload";
            tracked_data value;
            if (kind == 0)
            {
                value.set_int(42);
            }
            else if (kind == 1)
            {
                value.set_string("previous allocated storage");
            }
            else
            {
                value.set_user_data(bytes, sizeof(bytes));
            }

            const auto previous_type = value.get_type();
            const auto previous_allocations = tracked_allocator::outstanding;
            tracked_allocator::fail_next = true;
            DOCTEST_REQUIRE_THROWS_AS(value.from(&source), std::bad_alloc);
            DOCTEST_CHECK_FALSE(tracked_allocator::fail_next);
            DOCTEST_CHECK(value.get_type() == previous_type);
            DOCTEST_CHECK(tracked_allocator::outstanding == previous_allocations);
            DOCTEST_CHECK(source.get_string() == std::string(80, 'x'));
            if (kind == 0)
            {
                DOCTEST_CHECK(value.get_int() == 42);
            }
            else if (kind == 1)
            {
                DOCTEST_CHECK(std::string(value.get_string()) == "previous allocated storage");
            }
            else
            {
                std::size_t size = 0;
                const auto* result = value.get_user_data(size);
                DOCTEST_CHECK(size == sizeof(bytes));
                DOCTEST_CHECK(std::memcmp(result, bytes, sizeof(bytes)) == 0);
            }
        }

        DOCTEST_CHECK(tracked_allocator::outstanding.empty());
    }
}
