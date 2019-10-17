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

#include "utility/include/AFCScheduleModule.hpp"

namespace ark {

bool AFCScheduleModule::Init()
{
    scheduler = std::make_shared<AFScheduler>(16);
    return true;
}

void AFCScheduleModule::AddCronSchedule(const std::string& cron_expression, SCHEDULER_FUNCTOR&& cb)
{
    scheduler->cron(cron_expression, std::forward<SCHEDULER_FUNCTOR>(cb));
}

void AFCScheduleModule::AddIntervalSchedule(const std::chrono::system_clock::duration interval, SCHEDULER_FUNCTOR&& cb)
{
    scheduler->interval(interval, std::forward<SCHEDULER_FUNCTOR>(cb));
}

void AFCScheduleModule::AddAtSchedule(const std::string& target_time, SCHEDULER_FUNCTOR&& cb)
{
    scheduler->at(target_time, std::forward<SCHEDULER_FUNCTOR>(cb));
}

} // namespace ark
