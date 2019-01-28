/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include "AFDefine.hpp"
#include "AFCDataList.hpp"
#include "AFList.hpp"
#include "AFMap.hpp"
#include "interface/AFIEventManager.h"

namespace ark
{

    class AFCEventManager : public AFIEventManager
    {
    public:
        AFCEventManager() = delete;

        explicit AFCEventManager(AFGUID self) :
            self_(self)
        {
        }

        ~AFCEventManager() override
        {
            Shut();
        }

        void Update() override
        {
            int event_id = 0;
            for (auto iter : need_remove_events_)
            {
                event_callbacks_.erase(iter);
            }

            need_remove_events_.clear();
        }

        bool AddEventCallBack(const int event_id, const EVENT_PROCESS_FUNCTOR_PTR& cb) override
        {
            auto pEventInfo = event_callbacks_.find_value(event_id);
            if (pEventInfo == nullptr)
            {
                auto pEventInfo = std::make_shared<AFList<EVENT_PROCESS_FUNCTOR_PTR>>();
                return event_callbacks_.insert(event_id, pEventInfo).second;
            }
            else
            {
                pEventInfo->emplace_back(cb);
                return true;
            }
        }

        bool RemoveEventCallBack(const int event_id) override
        {
            need_remove_events_.emplace_back(event_id);
            return true;
        }

        bool DoEvent(const int event_id, const AFIDataList& args) override
        {
            auto event_info = event_callbacks_.find_value(event_id);
            if (event_info == nullptr)
            {
                return false;
            }

            for (auto iter : *event_info)
            {
                (*iter)(self_, event_id, args);
            }

            return true;
        }

    protected:
        bool HasEventCallBack(const int event_id) override
        {
            auto event_info = event_callbacks_.find_value(event_id);
            return (event_info != nullptr);
        }

        bool Shut()
        {
            need_remove_events_.clear();
            event_callbacks_.clear();

            return true;
        }

    private:
        AFGUID self_;

        AFList<int> need_remove_events_;
        AFMapEx<int, AFList<EVENT_PROCESS_FUNCTOR_PTR>> event_callbacks_;
    };

}