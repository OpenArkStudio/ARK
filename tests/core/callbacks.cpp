// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <chrono>
#include <string>
#include <utility>

#include "../network/doctest.h"

#include <ark/utility/schedule_module.hpp>
#include <ark/utility/timer_module.hpp>

namespace {

struct receiver
{
    void on_timer(std::uint64_t timer_id, const ark::guid_t& entity_id) {
        timer_id_ = timer_id;
        entity_id_ = entity_id;
    }

    void on_schedule() {
        ++calls_;
    }

    std::uint64_t timer_id_{};
    ark::guid_t entity_id_{};
    int calls_{};
};

class capturing_timer final : public ark::timer_module
{
public:
    using ark::timer_module::add_forever_timer;
    using ark::timer_module::add_single_timer;

    bool remove_timer(std::uint64_t) override {
        return true;
    }

    ark::timer_functor callback_;

private:
    std::uint64_t add_single_timer(
        const ark::guid_t&, std::chrono::milliseconds, std::uint32_t, ark::timer_functor&& callback) override {
        callback_ = std::move(callback);
        return 91;
    }

    std::uint64_t add_forever_timer(
        const ark::guid_t&, std::chrono::milliseconds, ark::timer_functor&& callback) override {
        callback_ = std::move(callback);
        return 92;
    }
};

class capturing_schedule final : public ark::schedule_module
{
public:
    using ark::schedule_module::add_at_schedule;
    using ark::schedule_module::add_interval_schedule;

    std::string expression_;
    ark::scheduler_functor callback_;

private:
    void add_interval_schedule(std::chrono::system_clock::duration, ark::scheduler_functor&& callback) override {
        callback_ = std::move(callback);
    }

    void add_at_schedule(const std::string& target, ark::scheduler_functor&& callback) override {
        expression_ = target;
        callback_ = std::move(callback);
    }

    void add_cron_schedule(const std::string& expression, ark::scheduler_functor&& callback) override {
        expression_ = expression;
        callback_ = std::move(callback);
    }
};

} // namespace

DOCTEST_TEST_CASE("Member timer callbacks preserve receiver and invocation arguments") {
    receiver target;
    capturing_timer timers;
    const ark::guid_t entity_id = 77;
    DOCTEST_REQUIRE(
        timers.add_single_timer(entity_id, std::chrono::milliseconds(10), 2, &target, &receiver::on_timer) == 91);
    timers.callback_(123, entity_id);
    DOCTEST_REQUIRE(target.timer_id_ == 123);
    DOCTEST_REQUIRE(target.entity_id_ == entity_id);
    DOCTEST_REQUIRE(
        timers.add_forever_timer(entity_id, std::chrono::milliseconds(10), &target, &receiver::on_timer) == 92);
    timers.callback_(456, entity_id);
    DOCTEST_REQUIRE(target.timer_id_ == 456);
}

DOCTEST_TEST_CASE("Member schedule callbacks preserve receiver and scheduling expressions") {
    receiver target;
    capturing_schedule schedules;
    schedules.add_schedule("1 2 * * *", &target, &receiver::on_schedule);
    DOCTEST_REQUIRE(schedules.expression_ == "1 2 * * *");
    schedules.callback_();
    schedules.add_daily_schedule(&target, &receiver::on_schedule);
    DOCTEST_REQUIRE(schedules.expression_ == "0 0 * * * ");
    schedules.callback_();
    schedules.add_weekly_schedule(&target, &receiver::on_schedule);
    DOCTEST_REQUIRE(schedules.expression_ == "0 0 * * 1");
    schedules.callback_();
    schedules.add_monthly_schedule(&target, &receiver::on_schedule);
    DOCTEST_REQUIRE(schedules.expression_ == "0 0 1 * * ");
    schedules.callback_();
    schedules.add_interval_schedule(std::chrono::seconds(1), &target, &receiver::on_schedule);
    schedules.callback_();
    schedules.add_at_schedule("12:00", &target, &receiver::on_schedule);
    DOCTEST_REQUIRE(schedules.expression_ == "12:00");
    schedules.callback_();
    DOCTEST_REQUIRE(target.calls_ == 6);
}
