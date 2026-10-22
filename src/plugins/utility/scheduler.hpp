/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once

#include <iomanip>

#include "ark/utility/ctpl_stl.h"

#include "ark/cron.hpp"
#include "ark/platform.hpp"

namespace ark {

// this file is based on https://github.com/Bosma/Scheduler.
// under the MIT license.
class interruptable_sleep final
{
    using clock = std::chrono::system_clock;

    // InterruptableSleep offers a sleep that can be interrupted by any thread.
    // It can be interrupted multiple times
    // and be interrupted before any sleep is called (the sleep will immediately complete)
    // Has same interface as condition_variables and futures, except with sleep instead of wait.
    // For a given object, sleep can be called on multiple threads safely, but is not recommended as behaviour is undefined.

public:
    interruptable_sleep()
        : interrupted_(false) {
    }

    interruptable_sleep(const interruptable_sleep&) = delete;
    interruptable_sleep(interruptable_sleep&&) noexcept = delete;

    ~interruptable_sleep() noexcept = default;

    interruptable_sleep& operator=(const interruptable_sleep&) noexcept = delete;
    interruptable_sleep& operator=(interruptable_sleep&&) noexcept = delete;

    void sleep_for(clock::duration duration) {
        std::unique_lock<std::mutex> ul(mutex_);
        condition_.wait_for(ul, duration, [this] { return interrupted_; });
        interrupted_ = false;
    }

    void sleep_until(clock::time_point time) {
        std::unique_lock<std::mutex> ul(mutex_);
        condition_.wait_until(ul, time, [this] { return interrupted_; });
        interrupted_ = false;
    }

    void sleep() {
        std::unique_lock<std::mutex> ul(mutex_);
        condition_.wait(ul, [this] { return interrupted_; });
        interrupted_ = false;
    }

    void interrupt() {
        std::lock_guard<std::mutex> lg(mutex_);
        interrupted_ = true;
        condition_.notify_one();
    }

private:
    bool interrupted_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

class scheduler_task
{
public:
    using clock = std::chrono::system_clock;

    explicit scheduler_task(std::function<void()>&& f, bool recur = false, bool interval = false)
        : function(std::move(f))
        , recurring(recur)
        , interval(interval) {
    }

    virtual clock::time_point get_new_time() const = 0;

    std::function<void()> function;

    bool recurring;
    bool interval;
};

class in_task final : public scheduler_task
{
public:
    using clock = std::chrono::system_clock;

    explicit in_task(std::function<void()>&& f)
        : scheduler_task(std::move(f)) {
    }

    // dummy time_point because it's not used
    clock::time_point get_new_time() const override {
        return clock::time_point(clock::duration(0));
    }
};

class every_task final : public scheduler_task
{
public:
    using clock = std::chrono::system_clock;

    every_task(clock::duration time, std::function<void()>&& f, bool interval = false)
        : scheduler_task(std::move(f), true, interval)
        , interval_duration(time) {
    }

    clock::time_point get_new_time() const override {
        return clock::now() + interval_duration;
    };

    clock::duration interval_duration;
};

class cron_task final : public scheduler_task
{
public:
    cron_task(const std::string& expression, std::function<void()>&& f)
        : scheduler_task(std::move(f), true)
        , cron_expression(expression) {
    }

    clock::time_point get_new_time() const override {
        return cron_expression.cron_to_next();
    };

    ark::cron cron_expression;
};

class scheduler final
{
public:
    using clock = std::chrono::system_clock;

    explicit scheduler(unsigned int max_n_tasks = 4)
        : stopping_(false)
        , threads_(max_n_tasks + 1) {
        threads_.push([this](int) {
            while (!stopping_)
            {
                if (tasks_.empty())
                {
                    sleeper_.sleep();
                }
                else
                {
                    auto time_of_first_task = (*tasks_.begin()).first;
                    sleeper_.sleep_until(time_of_first_task);
                }
                manage_tasks();
            }
        });
    }

    scheduler(const scheduler&) = delete;
    scheduler(scheduler&&) noexcept = delete;
    scheduler& operator=(const scheduler&) = delete;
    scheduler& operator=(scheduler&&) noexcept = delete;

    ~scheduler() {
        stopping_ = true;
        sleeper_.interrupt();
    }

    template<typename callable_type, typename... args_type>
    void in(const clock::time_point time, callable_type&& f, args_type&&... args) {
        std::shared_ptr<scheduler_task> t =
            std::make_shared<in_task>(std::bind(std::forward<callable_type>(f), std::forward<args_type>(args)...));
        add_task(time, std::move(t));
    }

    template<typename callable_type, typename... args_type>
    void in(const clock::duration time, callable_type&& f, args_type&&... args) {
        in(clock::now() + time, std::forward<callable_type>(f), std::forward<args_type>(args)...);
    }

    template<typename callable_type, typename... args_type>
    void at(const std::string& time, callable_type&& f, args_type&&... args) {
        // get current time as a tm object
        auto time_now = clock::to_time_t(clock::now());
        std::tm tm = *std::localtime(&time_now);

        // our final time as a time_point
        clock::time_point tp;

        if (try_parse(tm, time, "%H:%M:%S"))
        {
            // convert tm back to time_t, then to a time_point and assign to final
            tp = clock::from_time_t(std::mktime(&tm));

            // if we've already passed this time, the user will mean next day, so add a day.
            if (clock::now() >= tp)
            {
                tp += std::chrono::hours(24);
            }
        }
        else if (try_parse(tm, time, "%Y-%m-%d %H:%M:%S"))
        {
            tp = clock::from_time_t(std::mktime(&tm));
        }
        else if (try_parse(tm, time, "%Y/%m/%d %H:%M:%S"))
        {
            tp = clock::from_time_t(std::mktime(&tm));
        }
        else
        {
            // could not parse time
            throw std::runtime_error("Cannot parse time string: " + time);
        }

        in(tp, std::forward<callable_type>(f), std::forward<args_type>(args)...);
    }

    template<typename callable_type, typename... args_type>
    void every(const clock::duration time, callable_type&& f, args_type&&... args) {
        std::shared_ptr<scheduler_task> t = std::make_shared<every_task>(
            time, std::bind(std::forward<callable_type>(f), std::forward<args_type>(args)...));
        auto next_time = t->get_new_time();
        add_task(next_time, std::move(t));
    }

    // expression format:
    // from https://en.wikipedia.org/wiki/Cron#Overview
    //    ┌───────────── minute (0 - 59)
    //    │ ┌───────────── hour (0 - 23)
    //    │ │ ┌───────────── day of month (1 - 31)
    //    │ │ │ ┌───────────── month (1 - 12)
    //    │ │ │ │ ┌───────────── day of week (0 - 6) (Sunday to Saturday)
    //    │ │ │ │ │
    //    │ │ │ │ │
    //    * * * * *
    template<typename callable_type, typename... args_type>
    void cron(const std::string& expression, callable_type&& f, args_type&&... args) {
        std::shared_ptr<scheduler_task> t = std::make_shared<cron_task>(
            expression, std::bind(std::forward<callable_type>(f), std::forward<args_type>(args)...));
        auto next_time = t->get_new_time();
        add_task(next_time, std::move(t));
    }

    template<typename callable_type, typename... args_type>
    void interval(const clock::duration time, callable_type&& f, args_type&&... args) {
        std::shared_ptr<scheduler_task> t = std::make_shared<every_task>(
            time, std::bind(std::forward<callable_type>(f), std::forward<args_type>(args)...), true);
        add_task(clock::now(), std::move(t));
    }

private:
    std::atomic<bool> stopping_;

    interruptable_sleep sleeper_;

    std::multimap<clock::time_point, std::shared_ptr<scheduler_task>> tasks_;
    std::mutex mutex_;
    ctpl::thread_pool threads_;

    void add_task(const clock::time_point time, std::shared_ptr<scheduler_task> t) {
        std::lock_guard<std::mutex> l(mutex_);
        tasks_.emplace(time, std::move(t));
        sleeper_.interrupt();
    }

    void manage_tasks() {
        std::lock_guard<std::mutex> l(mutex_);

        auto end_of_tasks_to_run = tasks_.upper_bound(clock::now());

        // if there are any tasks to be run and removed
        if (end_of_tasks_to_run != tasks_.begin())
        {
            // keep track of tasks that will be re-added
            decltype(tasks_) recurred_tasks;

            // for all tasks that have been triggered
            for (auto i = tasks_.begin(); i != end_of_tasks_to_run; ++i)
            {

                auto& task = (*i).second;

                if (task->interval)
                {
                    // if it's an interval task, only add the task back after f() is completed
                    threads_.push([this, task](int) {
                        task->function();
                        // no risk of race-condition,
                        // add_task() will wait for manage_tasks() to release lock
                        add_task(task->get_new_time(), task);
                    });
                }
                else
                {
                    threads_.push([task](int) { task->function(); });
                    // calculate time of next run and add the new task to the tasks to be recurred
                    if (task->recurring)
                    {
                        recurred_tasks.emplace(task->get_new_time(), std::move(task));
                    }
                }
            }

            // remove the completed tasks
            tasks_.erase(tasks_.begin(), end_of_tasks_to_run);

            // re-add the tasks that are recurring
            for (auto& task : recurred_tasks)
            {
                tasks_.emplace(task.first, std::move(task.second));
            }
        }
    }

    inline bool try_parse(std::tm& tm, const std::string& expression, const std::string& format) {
        std::stringstream ss(expression);
        return !(ss >> std::get_time(&tm, format.c_str())).fail();
    }
};

} // namespace ark
