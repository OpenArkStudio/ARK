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
    bool AddSchedule(
        const int user_arg, const std::string& cron_expression, BaseType* pBase, bool (BaseType::*handler)(const int))
    {
        SCHEDULER_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1);
        return AddSchedule(cron_expression, std::move(functor), user_arg);
    }

    // every day
    template<typename BaseType>
    bool AddDailySchedule(const int user_arg, BaseType* pBase, bool (BaseType::*handler)(const int, const int))
    {
        SCHEDULER_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1);
        return AddSchedule("0 0 * * * ", std::move(functor), user_arg);
    }

    // every Monday
    template<typename BaseType>
    bool AddWeeklySchedule(const int user_arg, BaseType* pBase, bool (BaseType::*handler)(const int, const int))
    {
        SCHEDULER_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1);
        return AddSchedule("0 0 * * 1", std::move(functor), user_arg);
    }

    // every month
    template<typename BaseType>
    bool AddMonthlySchedule(const int user_arg, BaseType* pBase, bool (BaseType::*handler)(const int, const int))
    {
        SCHEDULER_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1);
        return AddSchedule("0 0 1 * * ", std::move(functor), user_arg);
    }

protected:
    virtual bool AddCronSchedule(const std::string& cron_expression, SCHEDULER_FUNCTOR&& cb, const int user_arg) = 0;
};

} // namespace ark
