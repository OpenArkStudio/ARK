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

#include "ark/define.hpp"
#include "ark/kernel/data_list.hpp"
#include "ark/platform.hpp"

namespace ark {

class event_manager
{
public:
    virtual void update() = 0;

    template<typename BaseType>
    bool add_event_call_back(
        const int nEventID, BaseType* pBase, int (BaseType::*handler)(const guid_t&, const int, const data_list&)) {
        event_process_functor functor =
            std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        return add_event_call_back(nEventID, std::move(functor));
    }

    virtual bool remove_event_call_back(const int nEventID) = 0;

    virtual bool do_event(const int nEventID, const data_list& valueList) = 0;

    virtual bool add_event_call_back(const int nEventID, event_process_functor&& cb) = 0;

    virtual bool clear() = 0;

protected:
    virtual bool has_event_call_back(const int nEventID) = 0;
};
} // namespace ark