// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <exception>
#include <memory>
#include <stdexcept>
#include <thread>

#include "../network/doctest.h"

#include "detail/module_task_group.hpp"

namespace {

using ark::detail::module_task;
using ark::detail::module_task_group;

module_task<> finish_task(unsigned& started) {
    ++started;
    co_return;
}

module_task<> throw_task() {
    throw std::runtime_error("task body failure");
    co_return;
}

module_task<> wait_task(asio::steady_timer& timer, bool& cancelled) {
    try
    { co_await timer.async_wait(asio::use_awaitable); }
    catch (const asio::system_error& error)
    { cancelled = error.code() == asio::error::operation_aborted; }
}

} // namespace

TEST_CASE("Task completion and exceptions run on the module owner") {
    auto runtime = std::make_shared<arknet::manual_runtime>();
    module_task_group group(runtime, 2);
    unsigned started{};
    unsigned completed{};
    const auto owner = std::this_thread::get_id();
    auto first = group.start(finish_task(started), [&](std::exception_ptr error) {
        CHECK(std::this_thread::get_id() == owner);
        CHECK_FALSE(error);
        CHECK(group.pending() == 1);
        ++completed;
    });
    REQUIRE(first != 0);
    REQUIRE(group.start(throw_task(), [&](std::exception_ptr error) {
        CHECK(std::this_thread::get_id() == owner);
        REQUIRE(error);
        try
        { std::rethrow_exception(error); }
        catch (const std::runtime_error& failure)
        { CHECK(std::string(failure.what()) == "task body failure"); }
        ++completed;
    }));
    CHECK(group.start(finish_task(started)) == 0);
    CHECK(started == 0);
    for (unsigned step = 0; step < 16 && group.pending(); ++step)
    {
        group.poll(1);
    }
    CHECK(completed == 2);
    CHECK(started == 1);
    CHECK(group.pending() == 0);
    CHECK_FALSE(group.cancel(first));
    group.close();
    CHECK(group.closed());
    CHECK(group.start(finish_task(started)) == 0);
}

TEST_CASE("Close requests cancellation and requires explicit owner polling to drain") {
    auto runtime = std::make_shared<arknet::manual_runtime>();
    module_task_group group(runtime, 1);
    asio::steady_timer timer(runtime->get_executor(), std::chrono::hours(1));
    bool cancelled{};
    unsigned completed{};
    const auto id = group.start(wait_task(timer, cancelled), [&](std::exception_ptr error) {
        CHECK_FALSE(error);
        ++completed;
    });
    REQUIRE(id != 0);
    group.poll();
    REQUIRE(group.pending() == 1);
    // Discarding an observation ID does not destroy the Asio-owned frame.
    group.close();
    CHECK_FALSE(group.closed());
    CHECK(completed == 0);
    for (unsigned step = 0; step < 16 && !group.closed(); ++step)
    {
        group.poll(1);
    }
    CHECK(group.closed());
    CHECK(cancelled);
    CHECK(completed == 1);
    CHECK_FALSE(group.cancel(id));
}

TEST_CASE("The final completion may destroy its task group facade") {
    auto runtime = std::make_shared<arknet::manual_runtime>();
    std::weak_ptr<arknet::manual_runtime> observed_runtime = runtime;
    auto group = std::make_unique<module_task_group>(runtime);
    unsigned started{};
    unsigned completed{};
    group->start(finish_task(started), [&](std::exception_ptr error) {
        CHECK_FALSE(error);
        CHECK(group->pending() == 0);
        group.reset();
        ++completed;
    });
    runtime.reset();
    REQUIRE_FALSE(observed_runtime.expired());
    group->poll();
    CHECK_FALSE(group);
    CHECK(completed == 1);
    CHECK(started == 1);
    CHECK(observed_runtime.expired());
}

TEST_CASE("Task group rejects cross-thread state access") {
    auto runtime = std::make_shared<arknet::manual_runtime>();
    module_task_group group(runtime);
    unsigned rejected{};
    std::thread worker([&] {
        try
        { group.pending(); }
        catch (const std::logic_error&)
        { ++rejected; }
        try
        { group.close(); }
        catch (const std::logic_error&)
        { ++rejected; }
    });
    worker.join();
    CHECK(rejected == 2);
    group.close();
}

TEST_CASE("Close before the first poll still drains the accepted task completion") {
    auto runtime = std::make_shared<arknet::manual_runtime>();
    module_task_group group(runtime);
    unsigned started{};
    unsigned completed{};
    group.start(finish_task(started), [&](std::exception_ptr error) {
        if (error)
        {
            try
            { std::rethrow_exception(error); }
            catch (const asio::system_error& failure)
            { CHECK(failure.code() == asio::error::operation_aborted); }
        }
        ++completed;
    });
    group.close();
    CHECK_FALSE(group.closed());
    for (unsigned step = 0; step < 16 && !group.closed(); ++step)
    {
        group.poll(1);
    }
    CHECK(group.closed());
    CHECK(completed == 1);
    CHECK(started <= 1);
}

TEST_CASE("Dropping an unstarted awaitable performs no work") {
    unsigned started{};
    { auto unstarted = finish_task(started); }
    CHECK(started == 0);
}
