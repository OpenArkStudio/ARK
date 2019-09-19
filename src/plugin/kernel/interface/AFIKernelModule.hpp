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
#include "kernel/interface/AFIEntity.hpp"
#include "proto/AFProtoCPP.hpp"
#include "AFIStaticEntity.hpp"

namespace ark {

class AFIKernelModule : public AFIModule
{
public:
    template<typename BaseType>
    bool AddCommonClassEvent(BaseType* pBase,
        int (BaseType::*handler)(const AFGUID&, const std::string&, const ArkEntityEvent, const AFIDataList&))
    {
        auto functor = std::bind(
            handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddCommonClassEvent(std::move(functor));
    }

    template<typename BaseType>
    bool AddCommonNodeEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&,
                                                 const uint32_t index, const AFIData&, const AFIData&))
    {
        auto functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
            std::placeholders::_4, std::placeholders::_5);
        return AddCommonNodeEvent(std::move(functor));
    }

    template<typename BaseType>
    bool AddCommonTableEvent(BaseType* pBase,
        int (BaseType::*handler)(const AFGUID&, const TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
    {
        auto functor = std::bind(
            handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddCommonTableEvent(std::move(functor));
    }
    /////////////////////////////////////////////////////////////////
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
        int (BaseType::*handler)(const AFGUID&, const std::string&, const uint32_t, const AFIData&, const AFIData&))
    {
        return AddDataCallBack(class_name, name,
            std::move(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5)));
    }

    template<typename BaseType>
    bool AddTableCallBack(const std::string& class_name, const std::string& name, BaseType* pBase,
        int (BaseType::*handler)(const AFGUID&, const TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
    {
        return AddTableCallBack(class_name, name,
            std::move(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4)));
    }

    // call back by index
    template<typename BaseType>
    bool AddDataCallBack(const std::string& class_name, const uint32_t index, BaseType* pBase,
        int (BaseType::*handler)(const AFGUID&, const std::string&, const uint32_t, const AFIData&, const AFIData&))
    {
        return AddDataCallBack(class_name, index,
            std::move(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5)));
    }

    template<typename BaseType>
    bool AddTableCallBack(const std::string& class_name, const uint32_t index, BaseType* pBase,
        int (BaseType::*handler)(const AFGUID&, const TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
    {
        return AddTableCallBack(class_name, index,
            std::move(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4)));
    }

    // container call back
    template<typename BaseType>
    bool AddContainerCallBack(const std::string& class_name, const uint32_t index, BaseType* pBase,
        int (BaseType::*handler)(
            const AFGUID&, const uint32_t, const ArkContainerOpType, const uint32_t, const uint32_t))
    {
        return AddContainerCallBack(class_name, index,
            std::move(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5)));
    }

    // only for player
    template<typename BaseType>
    bool AddCommonContainerCallBack(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const uint32_t,
                                                         const ArkContainerOpType, const uint32_t, const uint32_t))
    {
        return AddCommonContainerCallBack(std::move(std::bind(handler, pBase, std::placeholders::_1,
            std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));
    }

    //////////////////////////////////////////////////////////////////////////

    virtual bool DoEvent(
        const AFGUID& self, const std::string& name, ArkEntityEvent eEvent, const AFIDataList& valueList) = 0;
    virtual bool DoEvent(const AFGUID& self, const int nEventID, const AFIDataList& valueList) = 0;

    /////////////////////////////////////////////////////////////////
    virtual ARK_SHARE_PTR<AFIEntity> CreateEntity(const AFGUID& self, const int map_id, const int map_instance_id,
        const std::string& class_name, const ID_TYPE config_id, const AFIDataList& args) = 0;

    virtual ARK_SHARE_PTR<AFIEntity> CreateContainerEntity(
        const AFGUID& self, const uint32_t container_index, const std::string& class_name, const ID_TYPE config_id) = 0;

    virtual ARK_SHARE_PTR<AFIEntity> GetEntity(const AFGUID& self) = 0;
    virtual ARK_SHARE_PTR<AFIStaticEntity> GetStaticEntity(const ID_TYPE config_id) = 0;

    virtual bool DestroyEntity(const AFGUID& self) = 0;
    virtual bool DestroyAll() = 0;

    virtual bool Exist(const AFGUID& self) = 0;

    // entity to db data for save
    virtual bool EntityToDBData(const AFGUID& self, AFMsg::pb_db_entity& pb_data) = 0;
    // create entity by db data
    virtual ARK_SHARE_PTR<AFIEntity> CreateEntity(const AFMsg::pb_db_entity& pb_data) = 0;
    // send message
    virtual bool SendCustomMessage(const AFGUID& target, const uint32_t msg_id, const AFIDataList& args) = 0;

    virtual bool LogInfo(const AFGUID& ident) = 0;

protected:
    virtual bool AddEventCallBack(const AFGUID& self, const int nEventID, EVENT_PROCESS_FUNCTOR&& cb) = 0;
    virtual bool AddClassCallBack(const std::string& strClassName, CLASS_EVENT_FUNCTOR&& cb) = 0;

    virtual bool AddDataCallBack(const std::string& class_name, const std::string& name, DATA_EVENT_FUNCTOR&& cb) = 0;
    virtual bool AddTableCallBack(const std::string& class_name, const std::string& name, TABLE_EVENT_FUNCTOR&& cb) = 0;

    virtual bool AddDataCallBack(const std::string& class_name, const uint32_t index, DATA_EVENT_FUNCTOR&& cb) = 0;
    virtual bool AddTableCallBack(const std::string& class_name, const uint32_t index, TABLE_EVENT_FUNCTOR&& cb) = 0;

    virtual bool AddContainerCallBack(
        const std::string& class_name, const uint32_t index, CONTAINER_EVENT_FUNCTOR&& cb) = 0;
    virtual bool AddCommonContainerCallBack(CONTAINER_EVENT_FUNCTOR&& cb) = 0;

    virtual bool AddCommonClassEvent(CLASS_EVENT_FUNCTOR&& cb) = 0;
    virtual bool AddCommonNodeEvent(DATA_EVENT_FUNCTOR&& cb) = 0;
    virtual bool AddCommonTableEvent(TABLE_EVENT_FUNCTOR&& cb) = 0;
};

} // namespace ark
