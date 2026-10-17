// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <chrono>
#include <memory>
#include <optional>
#include <thread>
#include <type_traits>

#include "../network/doctest.h"

#include "mysql/detail/mysql_transaction.hpp"

namespace {

using namespace std::chrono_literals;
using namespace ark::detail;

static_assert(!std::is_copy_constructible_v<mysql_transaction>);
static_assert(std::is_nothrow_move_constructible_v<mysql_transaction>);
static_assert(std::is_nothrow_move_assignable_v<mysql_transaction>);
static_assert(std::is_nothrow_move_constructible_v<mysql_transaction_reply>);

} // namespace

TEST_CASE("Private MySQL transactions require a supplied pool and explicit nonzero limits") {
    CHECK_THROWS_AS(mysql_transaction_service(nullptr, {1, 1024}), std::invalid_argument);
    auto pool = std::make_shared<arknet::mysql::pool>(arknet::mysql::pool_options{});
    CHECK_THROWS_AS(mysql_transaction_service(pool, {0, 1024}), std::invalid_argument);
    CHECK_THROWS_AS(mysql_transaction_service(pool, {1, 0}), std::invalid_argument);
    CHECK(pool->close_and_wait());
}

TEST_CASE("Invalid transaction deadlines reject inline before acquiring a connection") {
    auto pool = std::make_shared<arknet::mysql::pool>(arknet::mysql::pool_options{});
    {
        mysql_transaction_service service(pool, {1, 1024});
        unsigned calls{};
        for (const auto options : {mysql_transaction_options{0ms, 1s}, mysql_transaction_options{1s, 0ms},
                 mysql_transaction_options{-1ms, 1s}, mysql_transaction_options{std::chrono::milliseconds::max(), 1s}})
        {
            CHECK(service.start(options, [&](auto result) {
                ++calls;
                CHECK(result.result.status.code == arknet::mysql::connection_errc::invalid_options);
                CHECK_FALSE(result.transaction.has_value());
                CHECK_FALSE(result.result.status.outcome_unknown);
            }) == 0);
        }
        CHECK(calls == 4);
        CHECK(service.active() == 0);
        CHECK(service.retained_input_bytes() == 0);
        CHECK(service.update() == 0);
        bool wrong_thread_rejected{};
        std::thread worker([&] {
            try
            { service.update(); }
            catch (const std::logic_error&)
            { wrong_thread_rejected = true; }
        });
        worker.join();
        CHECK(wrong_thread_rejected);
    }
    CHECK(pool->close_and_wait());
}

TEST_CASE("An empty transaction handle reports unavailable without sending a command") {
    mysql_transaction transaction;
    unsigned calls{};
    CHECK(transaction.start_query("SELECT 1", [&](auto result) {
        ++calls;
        CHECK(result.result.status.code == arknet::mysql::connection_errc::not_connected);
        CHECK_FALSE(result.result.status.outcome_unknown);
    }) == 0);
    CHECK(calls == 1);
    CHECK_FALSE(transaction.cancel(1));
    transaction.abort();
    CHECK(transaction.snapshot().outcome == mysql_transaction_outcome::not_started);
}
