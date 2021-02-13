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

#include "utility/include/AFCTimerModule.hpp"

namespace ark {

bool AFCTimerModule::Init()
{
    timer_manager_ptr = std::make_shared<AFTimeWheelManager>();
    timer_manager_ptr->Init(GetPluginManager()->GetNowTime());

    return true;
}

bool AFCTimerModule::PreShut()
{
    timer_manager_ptr->Shut();
    return true;
}

bool AFCTimerModule::Update()
{
    timer_manager_ptr->Update(GetPluginManager()->GetNowTime());
    return true;
}

bool AFCTimerModule::RemoveTimer(const uint64_t timer_id)
{
    return timer_manager_ptr->RemoveTimer(timer_id);
}

uint64_t AFCTimerModule::AddSingleTimer(
    const guid_t& entity_id, const std::chrono::milliseconds interval, const uint32_t count, TIMER_FUNCTOR&& cb)
{
    return timer_manager_ptr->AddTimer(entity_id, count, interval, std::move(cb));
}

uint64_t AFCTimerModule::AddForeverTimer(
    const guid_t& entity_id, const std::chrono::milliseconds interval, TIMER_FUNCTOR&& cb)
{
    return timer_manager_ptr->AddTimer(entity_id, 0, interval, std::move(cb));
}

} // namespace ark
