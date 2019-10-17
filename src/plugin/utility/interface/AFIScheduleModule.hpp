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

#include "base/AFDefine.hpp"
#include "interface/AFIModule.hpp"

namespace ark {

class AFIScheduleModule : public AFIModule
{
public:
    template<typename BaseType>
    void AddSchedule(const std::string& cron_expression, BaseType* pBase, void (BaseType::*handler)())
    {
        SCHEDULER_FUNCTOR functor = std::bind(handler, pBase);
        return AddSchedule(cron_expression, std::move(functor));
    }

    // every day
    template<typename BaseType>
    void AddDailySchedule(BaseType* pBase, void (BaseType::*handler)())
    {
        SCHEDULER_FUNCTOR functor = std::bind(handler, pBase);
        return AddSchedule("0 0 * * * ", std::move(functor));
    }

    // every Monday
    template<typename BaseType>
    void AddWeeklySchedule(BaseType* pBase, void (BaseType::*handler)())
    {
        SCHEDULER_FUNCTOR functor = std::bind(handler, pBase);
        return AddSchedule("0 0 * * 1", std::move(functor));
    }

    // every month
    template<typename BaseType>
    void AddMonthlySchedule(BaseType* pBase, void (BaseType::*handler)())
    {
        SCHEDULER_FUNCTOR functor = std::bind(handler, pBase);
        AddSchedule("0 0 1 * * ", std::move(functor));
    }

    template<typename BaseType>
    void AddIntervalSchedule(
        const std::chrono::system_clock::duration interval, BaseType* pBase, void (BaseType::*handler)())
    {
        SCHEDULER_FUNCTOR functor = std::bind(handler, pBase);
        AddIntervalSchedule(interval, std::move(functor));
    }

    template<typename BaseType>
    void AddAtSchedule(const std::string& target_time, BaseType* pBase, void (BaseType::*handler)())
    {
        SCHEDULER_FUNCTOR functor = std::bind(handler, pBase);
        AddAtSchedule(target_time, std::move(functor));
    }

protected:
    virtual void AddIntervalSchedule(const std::chrono::system_clock::duration interval, SCHEDULER_FUNCTOR&& cb) = 0;
    virtual void AddAtSchedule(const std::string& target_time, SCHEDULER_FUNCTOR&& cb) = 0;
    virtual void AddCronSchedule(const std::string& cron_expression, SCHEDULER_FUNCTOR&& cb) = 0;
};

} // namespace ark
