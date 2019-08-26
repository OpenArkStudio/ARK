/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "base/AFMacros.hpp"
#include "base/AFMap.hpp"
#include "base/AFList.hpp"
#include "base/AFDefine.hpp"

namespace ark {

class AFClassCallBackManager final
{
public:
    explicit AFClassCallBackManager() = default;

    virtual ~AFClassCallBackManager()
    {
        class_events_.clear();

        data_call_backs_list_.removeall();
        record_call_backs_list_.removeall();
    }

    bool AddClassCallBack(CLASS_EVENT_FUNCTOR&& cb)
    {
        class_events_.emplace_back(std::forward<CLASS_EVENT_FUNCTOR>(cb));
        return true;
    }

    bool AddDataCallBack(const std::string& name, DATA_EVENT_FUNCTOR&& cb)
    {
        auto iter = data_call_backs_list_.find(name);
        if (iter == data_call_backs_list_.end())
        {
            DataCallBacks* pDataCBs = ARK_NEW DataCallBacks;
            ARK_ASSERT_RET_VAL(pDataCBs != nullptr, false);

            pDataCBs->push_back(std::forward<DATA_EVENT_FUNCTOR>(cb));
            data_call_backs_list_.insert(name, pDataCBs);
        }
        else
        {
            iter->second->push_back(cb);
        }

        return true;
    }

    bool AddRecordCallBack(const std::string& name, RECORD_EVENT_FUNCTOR&& cb)
    {
        auto iter = record_call_backs_list_.find(name);
        if (iter == record_call_backs_list_.end())
        {
            RecordCallBacks* pRecordCBs = ARK_NEW RecordCallBacks;
            ARK_ASSERT_RET_VAL(pRecordCBs != nullptr, false);

            pRecordCBs->push_back(std::forward<RECORD_EVENT_FUNCTOR>(cb));
            record_call_backs_list_.insert(name, pRecordCBs);
        }
        else
        {
            iter->second->push_back(cb);
        }

        return true;
    }

    bool OnDataCallBack(
        const AFGUID& self, const std::string& name, const AFIDataNewType& old_data, const AFIDataNewType& new_data)
    {
        auto data_call_backs = data_call_backs_list_.find_value(name);
        if (data_call_backs == nullptr)
        {
            return false;
        }

        for (auto& cb : *data_call_backs)
        {
            cb(self, name, old_data, new_data);
        }

        return true;
    }

    bool DoEvent(
        const AFGUID& id, const std::string& class_name, const ArkEntityEvent eClassEvent, const AFIDataList& valueList)
    {
        for (auto iter : class_events_)
        {
            iter(id, class_name, eClassEvent, valueList);
        }

        return true;
    }

    bool OnRecordCallBack(const AFGUID& id, const DATA_RECORD_EVENT_DATA& event_data, const AFIDataNewType& old_data,
        const AFIDataNewType& new_data)
    {
        auto record_call_backs = record_call_backs_list_.find_value(event_data.record_name_);
        if (record_call_backs == nullptr)
        {
            return false;
        }

        for (auto& cb : *record_call_backs)
        {
            cb(id, event_data, old_data, new_data);
        }

        return true;
    }

private:
    // class event list
    AFList<CLASS_EVENT_FUNCTOR> class_events_;

    // data call backs list
    using DataCallBacks = std::vector<DATA_EVENT_FUNCTOR>;
    AFNewHashmap<std::string, DataCallBacks> data_call_backs_list_;

    // record call backs list
    using RecordCallBacks = std::vector<RECORD_EVENT_FUNCTOR>;
    AFNewHashmap<std::string, RecordCallBacks> record_call_backs_list_;
};

} // namespace ark
