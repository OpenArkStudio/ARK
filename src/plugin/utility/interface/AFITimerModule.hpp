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

class AFITimerModule : public AFIModule
{
public:
    template<typename BaseType>
    uint64_t AddSingleTimer(const AFGUID& entity_id, const std::chrono::milliseconds interval, const uint32_t count,
        BaseType* pBase, void (BaseType::*handler)(uint64_t, const AFGUID&))
    {
        TIMER_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2);
        return AddSingleTimer(entity_id, interval, count, std::move(functor));
    }

    template<typename BaseType>
    uint64_t AddForeverTimer(const AFGUID& entity_id, const std::chrono::milliseconds interval, BaseType* pBase,
        void (BaseType::*handler)(uint64_t, const AFGUID&))
    {
        TIMER_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2);
        return AddForeverTimer(entity_id, interval, std::move(functor));
    }

    virtual bool RemoveTimer(const uint64_t timer_id) = 0;
    //virtual bool RemoveTimer(const std::string& name, const AFGUID& entity_id) = 0;

protected:
    virtual uint64_t AddSingleTimer(const AFGUID& entity_id, const std::chrono::milliseconds interval,
        const uint32_t count, TIMER_FUNCTOR&& cb) = 0;
    virtual uint64_t AddForeverTimer(
        const AFGUID& entity_id, const std::chrono::milliseconds interval, TIMER_FUNCTOR&& cb) = 0;
};

} // namespace ark
