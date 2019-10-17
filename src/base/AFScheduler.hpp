/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include <ctpl/ctpl_stl.h>

#include "base/AFPlatform.hpp"
#include "base/AFCron.hpp"

// this file is based on https://github.com/Bosma/Scheduler.
// under the MIT license.

namespace ark {

class AFInterruptableSleep final
{
    using Clock = std::chrono::system_clock;

    // InterruptableSleep offers a sleep that can be interrupted by any thread.
    // It can be interrupted multiple times
    // and be interrupted before any sleep is called (the sleep will immediately complete)
    // Has same interface as condition_variables and futures, except with sleep instead of wait.
    // For a given object, sleep can be called on multiple threads safely, but is not recommended as behaviour is undefined.

public:
    AFInterruptableSleep()
        : interrupted(false)
    {
    }

    AFInterruptableSleep(const AFInterruptableSleep&) = delete;
    AFInterruptableSleep(AFInterruptableSleep&&) noexcept = delete;

    ~AFInterruptableSleep() noexcept = default;

    AFInterruptableSleep& operator=(const AFInterruptableSleep&) noexcept = delete;
    AFInterruptableSleep& operator=(AFInterruptableSleep&&) noexcept = delete;

    void sleep_for(Clock::duration duration)
    {
        std::unique_lock<std::mutex> ul(m);
        cv.wait_for(ul, duration, [this] { return interrupted; });
        interrupted = false;
    }

    void sleep_until(Clock::time_point time)
    {
        std::unique_lock<std::mutex> ul(m);
        cv.wait_until(ul, time, [this] { return interrupted; });
        interrupted = false;
    }

    void sleep()
    {
        std::unique_lock<std::mutex> ul(m);
        cv.wait(ul, [this] { return interrupted; });
        interrupted = false;
    }

    void interrupt()
    {
        std::lock_guard<std::mutex> lg(m);
        interrupted = true;
        cv.notify_one();
    }

private:
    bool interrupted;
    std::mutex m;
    std::condition_variable cv;
};

class AFTask
{
public:
    using Clock = std::chrono::system_clock;

    explicit AFTask(std::function<void()>&& f, bool recur = false, bool interval = false)
        : f(std::move(f))
        , recur(recur)
        , interval(interval)
    {
    }

    virtual Clock::time_point get_new_time() const = 0;

    std::function<void()> f;

    bool recur;
    bool interval;
};

class AFInTask final : public AFTask
{
public:
    using Clock = std::chrono::system_clock;

    explicit AFInTask(std::function<void()>&& f)
        : AFTask(std::move(f))
    {
    }

    // dummy time_point because it's not used
    Clock::time_point get_new_time() const override
    {
        return Clock::time_point(Clock::duration(0));
    }
};

class AFEveryTask final : public AFTask
{
public:
    using Clock = std::chrono::system_clock;

    AFEveryTask(Clock::duration time, std::function<void()>&& f, bool interval = false)
        : AFTask(std::move(f), true, interval)
        , time(time)
    {
    }

    Clock::time_point get_new_time() const override
    {
        return Clock::now() + time;
    };

    Clock::duration time;
};

class AFCronTask final : public AFTask
{
public:
    AFCronTask(const std::string& expression, std::function<void()>&& f)
        : AFTask(std::move(f), true)
        , cron(expression)
    {
    }

    Clock::time_point get_new_time() const override
    {
        return cron.cron_to_next();
    };

    AFCron cron;
};

class AFScheduler final
{
public:
    using Clock = std::chrono::system_clock;

    explicit AFScheduler(unsigned int max_n_tasks = 4)
        : done(false)
        , threads(max_n_tasks + 1)
    {
        threads.push([this](int) {
            while (!done)
            {
                if (tasks.empty())
                {
                    sleeper.sleep();
                }
                else
                {
                    auto time_of_first_task = (*tasks.begin()).first;
                    sleeper.sleep_until(time_of_first_task);
                }
                manage_tasks();
            }
        });
    }

    AFScheduler(const AFScheduler&) = delete;
    AFScheduler(AFScheduler&&) noexcept = delete;
    AFScheduler& operator=(const AFScheduler&) = delete;
    AFScheduler& operator=(AFScheduler&&) noexcept = delete;

    ~AFScheduler()
    {
        done = true;
        sleeper.interrupt();
    }

    template<typename _Callable, typename... _Args>
    void in(const Clock::time_point time, _Callable&& f, _Args&&... args)
    {
        std::shared_ptr<AFTask> t =
            std::make_shared<AFInTask>(std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...));
        add_task(time, std::move(t));
    }

    template<typename _Callable, typename... _Args>
    void in(const Clock::duration time, _Callable&& f, _Args&&... args)
    {
        in(Clock::now() + time, std::forward<_Callable>(f), std::forward<_Args>(args)...);
    }

    template<typename _Callable, typename... _Args>
    void at(const std::string& time, _Callable&& f, _Args&&... args)
    {
        // get current time as a tm object
        auto time_now = Clock::to_time_t(Clock::now());
        std::tm tm = *std::localtime(&time_now);

        // our final time as a time_point
        Clock::time_point tp;

        if (try_parse(tm, time, "%H:%M:%S"))
        {
            // convert tm back to time_t, then to a time_point and assign to final
            tp = Clock::from_time_t(std::mktime(&tm));

            // if we've already passed this time, the user will mean next day, so add a day.
            if (Clock::now() >= tp)
                tp += std::chrono::hours(24);
        }
        else if (try_parse(tm, time, "%Y-%m-%d %H:%M:%S"))
        {
            tp = Clock::from_time_t(std::mktime(&tm));
        }
        else if (try_parse(tm, time, "%Y/%m/%d %H:%M:%S"))
        {
            tp = Clock::from_time_t(std::mktime(&tm));
        }
        else
        {
            // could not parse time
            throw std::runtime_error("Cannot parse time string: " + time);
        }

        in(tp, std::forward<_Callable>(f), std::forward<_Args>(args)...);
    }

    template<typename _Callable, typename... _Args>
    void every(const Clock::duration time, _Callable&& f, _Args&&... args)
    {
        std::shared_ptr<AFTask> t =
            std::make_shared<AFEveryTask>(time, std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...));
        auto next_time = t->get_new_time();
        add_task(next_time, std::move(t));
    }

    // expression format:
    // from https://en.wikipedia.org/wiki/Cron#Overview
    //    ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ minute (0 - 59)
    //    ©¦ ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ hour (0 - 23)
    //    ©¦ ©¦ ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ day of month (1 - 31)
    //    ©¦ ©¦ ©¦ ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ month (1 - 12)
    //    ©¦ ©¦ ©¦ ©¦ ©°©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤©¤ day of week (0 - 6) (Sunday to Saturday)
    //    ©¦ ©¦ ©¦ ©¦ ©¦
    //    ©¦ ©¦ ©¦ ©¦ ©¦
    //    * * * * *
    template<typename _Callable, typename... _Args>
    void cron(const std::string& expression, _Callable&& f, _Args&&... args)
    {
        std::shared_ptr<AFTask> t = std::make_shared<AFCronTask>(
            expression, std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...));
        auto next_time = t->get_new_time();
        add_task(next_time, std::move(t));
    }

    template<typename _Callable, typename... _Args>
    void interval(const Clock::duration time, _Callable&& f, _Args&&... args)
    {
        std::shared_ptr<AFTask> t = std::make_shared<AFEveryTask>(
            time, std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...), true);
        add_task(Clock::now(), std::move(t));
    }

private:
    std::atomic<bool> done;

    AFInterruptableSleep sleeper;

    std::multimap<Clock::time_point, std::shared_ptr<AFTask>> tasks;
    std::mutex lock;
    ctpl::thread_pool threads;

    void add_task(const Clock::time_point time, std::shared_ptr<AFTask> t)
    {
        std::lock_guard<std::mutex> l(lock);
        tasks.emplace(time, std::move(t));
        sleeper.interrupt();
    }

    void manage_tasks()
    {
        std::lock_guard<std::mutex> l(lock);

        auto end_of_tasks_to_run = tasks.upper_bound(Clock::now());

        // if there are any tasks to be run and removed
        if (end_of_tasks_to_run != tasks.begin())
        {
            // keep track of tasks that will be re-added
            decltype(tasks) recurred_tasks;

            // for all tasks that have been triggered
            for (auto i = tasks.begin(); i != end_of_tasks_to_run; ++i)
            {

                auto& task = (*i).second;

                if (task->interval)
                {
                    // if it's an interval task, only add the task back after f() is completed
                    threads.push([this, task](int) {
                        task->f();
                        // no risk of race-condition,
                        // add_task() will wait for manage_tasks() to release lock
                        add_task(task->get_new_time(), task);
                    });
                }
                else
                {
                    threads.push([task](int) { task->f(); });
                    // calculate time of next run and add the new task to the tasks to be recurred
                    if (task->recur)
                        recurred_tasks.emplace(task->get_new_time(), std::move(task));
                }
            }

            // remove the completed tasks
            tasks.erase(tasks.begin(), end_of_tasks_to_run);

            // re-add the tasks that are recurring
            for (auto& task : recurred_tasks)
                tasks.emplace(task.first, std::move(task.second));
        }
    }

    inline bool try_parse(std::tm& tm, const std::string& expression, const std::string& format)
    {
        std::stringstream ss(expression);
        return !(ss >> std::get_time(&tm, format.c_str())).fail();
    }
};

} // namespace ark
