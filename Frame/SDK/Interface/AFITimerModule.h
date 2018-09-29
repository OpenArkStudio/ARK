/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
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

#include "SDK/Interface/AFIModule.h"

namespace ark
{

    class AFITimerModule : public AFIModule
    {
    public:
        template<typename BaseType>
        bool AddSingleTimer(const std::string& name, const AFGUID& entity_id, const uint32_t interval_time, const uint32_t count, BaseType* pBase, void (BaseType::*handler)(const std::string&, const AFGUID&))
        {
            TIMER_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2);
            return AddSingleTimer(name, entity_id, interval_time, count, std::make_shared<TIMER_FUNCTOR>(functor));
        }

        template<typename BaseType>
        bool AddForeverTimer(const std::string& name, const AFGUID& entity_id, const uint32_t interval_time, BaseType* pBase, void (BaseType::*handler)(const std::string&, const AFGUID&))
        {
            TIMER_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2);
            return AddForeverTimer(name, entity_id, interval_time, std::make_shared<TIMER_FUNCTOR>(functor));
        }

        virtual bool RemoveTimer(const std::string& name) = 0;
        virtual bool RemoveTimer(const std::string& name, const AFGUID& entity_id) = 0;

    protected:
        virtual bool AddSingleTimer(const std::string& name, const AFGUID& entity_id, const uint32_t interval_time, const uint32_t count, TIMER_FUNCTOR_PTR cb) = 0;
        virtual bool AddForeverTimer(const std::string& name, const AFGUID& entity_id, const uint32_t interval_time, TIMER_FUNCTOR_PTR cb) = 0;
    };

}