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

#include "interface/AFIModule.hpp"
#include "kernel/interface/AFIObject.hpp"

namespace ark {

class AFIKernelNewModule : public AFIModule
{
public:
    template<typename BaseType>
    bool AddEventCallBack(const AFGUID& self, const int nEventID, BaseType* pBase,
        int (BaseType::*handler)(const AFGUID&, const int, const AFIDataList&))
    {
        auto functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        return AddEventCallBack(self, nEventID, std::move(functor));
    }

    template<typename BaseType>
    bool AddClassCallBack(const std::string& name, BaseType* pBase,
        int (BaseType::*handler)(const AFGUID&, const std::string&, const ArkEntityEvent, const AFIDataList&))
    {
        auto functor = std::bind(
            handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddClassCallBack(name, std::move(functor));
    }

    template<typename BaseType>
    bool AddDataCallBack(const std::string& class_name, const std::string& name, BaseType* pBase,
        int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIDataNewType&, const AFIDataNewType&))
    {
        return AddDataCallBack(class_name, name,
            std::move(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4)));
    }

    template<typename BaseType>
    bool AddRecordCallBack(const std::string& class_name, const std::string& name, BaseType* pBase,
        int (BaseType::*handler)(
            const AFGUID&, const DATA_RECORD_EVENT_DATA&, const AFIDataNewType&, const AFIDataNewType&))
    {
        return AddRecordCallBack(class_name, name,
            std::move(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4)));
    }

    virtual bool AddDataCallBack(const std::string& class_name, const std::string& name, DATA_EVENT_FUNCTOR&& cb) = 0;

    virtual bool AddRecordCallBack(
        const std::string& class_name, const std::string& name, RECORD_EVENT_FUNCTOR&& cb) = 0;

    virtual bool DoEvent(
        const AFGUID& self, const std::string& name, ArkEntityEvent eEvent, const AFIDataList& valueList) = 0;
    virtual bool DoEvent(const AFGUID& self, const int nEventID, const AFIDataList& valueList) = 0;

    /////////////////////////////////////////////////////////////////
    virtual ARK_SHARE_PTR<AFIObject> CreateObject(const AFGUID& self, const int map_id, const int map_instance_id,
        const std::string& class_name, const uint64_t config_id, const AFIDataList& args) = 0;

    virtual ARK_SHARE_PTR<AFIObject> CreateContainerObject(const AFGUID& self, const std::string& container_name,
        const std::string& class_name, const uint64_t config_id) = 0;

    virtual ARK_SHARE_PTR<AFIObject> FindObject(const AFGUID& self) = 0;

    virtual bool DestroyEntity(const AFGUID& self) = 0;
    virtual bool DestroyAll() = 0;

    virtual bool LogInfo(const AFGUID& ident) = 0;

protected:
    virtual bool AddEventCallBack(const AFGUID& self, const int nEventID, EVENT_PROCESS_FUNCTOR&& cb) = 0;
    virtual bool AddClassCallBack(const std::string& strClassName, CLASS_EVENT_FUNCTOR&& cb) = 0;
};

} // namespace ark
