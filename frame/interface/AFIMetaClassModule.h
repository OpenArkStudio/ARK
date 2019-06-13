/*
* This source file is part of ARK
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

#include "base/AFPlatform.hpp"
#include "base/AFMacros.hpp"
#include "base/AFMap.hpp"
#include "base/AFList.hpp"
#include "AFIDataNodeManager.h"
#include "AFIDataTableManager.h"
#include "AFIModule.h"

namespace ark
{

    class AFIMetaClass
    {
    public:
        virtual ~AFIMetaClass() {}

        virtual ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager() = 0;
        virtual ARK_SHARE_PTR<AFIDataTableManager> GetTableManager() = 0;

        virtual void SetParent(ARK_SHARE_PTR<AFIMetaClass> pClass) = 0;
        virtual ARK_SHARE_PTR<AFIMetaClass> GetParent() = 0;

        virtual void SetTypeName(const char* type) = 0;
        virtual const std::string& GetTypeName() = 0;
        virtual const std::string& GetClassName() = 0;

        virtual bool AddConfigName(std::string& config_name) = 0;
        virtual AFList<std::string>& GetConfigNameList() = 0;

        virtual void SetResPath(const std::string& path) = 0;
        virtual const std::string& GetResPath() = 0;

        virtual bool AddClassCallBack(const CLASS_EVENT_FUNCTOR_PTR cb) = 0;
        virtual bool DoEvent(const AFGUID& id, const ArkEntityEvent class_event, const AFIDataList& args) = 0;

        template<typename BaseType>
        bool AddNodeCallBack(const std::string& class_name, const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
        {
            DATA_NODE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
            return AddNodeCallBack(class_name, std::make_shared<DATA_NODE_EVENT_FUNCTOR>(functor));
        }

        virtual bool AddNodeCallBack(const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR cb) = 0;
        virtual bool AddCommonNodeCallback(const DATA_NODE_EVENT_FUNCTOR_PTR cb) = 0;

        //template<typename BaseType>
        //bool AddTableCommonCallback(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
        //{
        //    DATA_TABLE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        //    return AddTableCommonCallback(std::make_shared<DATA_TABLE_EVENT_FUNCTOR>(functor));
        //}

        virtual bool AddTableCallBack(const std::string& name, const DATA_TABLE_EVENT_FUNCTOR_PTR cb) = 0;
        virtual bool AddCommonTableCallback(const DATA_TABLE_EVENT_FUNCTOR_PTR cb) = 0;

        virtual bool InitDataNodeManager(ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager) = 0;
        virtual bool InitDataTableManager(ARK_SHARE_PTR<AFIDataTableManager> pTableManager) = 0;

        virtual bool ExistInclude(const std::string& include_file) = 0;
        virtual bool AddInclude(const std::string& include_file) = 0;
        virtual AFList<std::string>& GetIncludeFiles() = 0;
    };

    class AFIMetaClassModule : public AFIModule
    {
    public:
        virtual ~AFIMetaClassModule() {}

        virtual bool Load() = 0;

        template<typename BaseType>
        bool AddClassCallBack(const std::string& class_name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const ArkEntityEvent, const AFIDataList&))
        {
            return AddClassCallBack(class_name, std::make_shared<CLASS_EVENT_FUNCTOR>(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
        }

        template<typename BaseType>
        bool AddNodeCallBack(const std::string& class_name, const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
        {
            return AddNodeCallBack(class_name, name, std::make_shared<DATA_NODE_EVENT_FUNCTOR>(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
        }

        template<typename BaseType>
        bool AddTableCallBack(const std::string& class_name, const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
        {
            return AddTableCallBack(class_name, name, std::make_shared<DATA_TABLE_EVENT_FUNCTOR>(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
        }

        template<typename BaseType>
        bool AddCommonNodeCallback(const std::string& class_name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
        {
            return AddCommonNodeCallback(class_name, std::make_shared<DATA_NODE_EVENT_FUNCTOR>(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
        }

        template<typename BaseType>
        bool AddCommonTableCallback(const std::string& class_name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
        {
            return AddCommonTableCallback(class_name, std::make_shared<DATA_TABLE_EVENT_FUNCTOR>(std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
        }

        virtual bool DoEvent(const AFGUID& entity_id, const std::string& class_name, const ArkEntityEvent class_event, const AFIDataList& args) = 0;
        virtual bool AddNodeCallBack(const std::string& class_name, const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR cb) = 0;
        virtual bool AddTableCallBack(const std::string& class_name, const std::string& name, const DATA_TABLE_EVENT_FUNCTOR_PTR cb) = 0;
        virtual bool AddCommonNodeCallback(const std::string& class_name, const DATA_NODE_EVENT_FUNCTOR_PTR cb) = 0;
        virtual bool AddCommonTableCallback(const std::string& class_name, const DATA_TABLE_EVENT_FUNCTOR_PTR cb) = 0;

        virtual bool AddClassCallBack(const std::string& class_name, const CLASS_EVENT_FUNCTOR_PTR cb) = 0;
        virtual ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager(const std::string& class_name) = 0;
        virtual ARK_SHARE_PTR<AFIDataTableManager> GetTableManager(const std::string& class_name) = 0;
        virtual bool InitDataNodeManager(const std::string& class_name, ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager) = 0;
        virtual bool InitDataTableManager(const std::string& class_name, ARK_SHARE_PTR<AFIDataTableManager> pTableManager) = 0;

        virtual std::shared_ptr<AFIMetaClass> GetMetaClass(const std::string& class_name) = 0;
        virtual AFMapEx<std::string, AFIMetaClass>& GetAllMetaClass() = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    //class AFIMetaConfigInfo
    //{
    //public:
    //    virtual const std::string& GetMetaName() = 0;
    //    virtual void SetMetaName(const std::string& name) = 0;

    //    virtual void SetResPath(const std::string& path) = 0;
    //    virtual const std::string& GetResPath() = 0;

    //    virtual void SetMetaPath(const std::string& path) = 0;
    //    virtual const std::string& GetMetaPath() = 0;

    //    virtual void AddConfigID(uint32_t config_id) = 0;
    //    virtual AFList<uint32_t>& GetAllConfigID() = 0;
    //};

    //class AFINewMetaConfigModule : public AFIModule
    //{
    //    virtual bool Load() = 0;

    //    virtual bool AddMetaConfigInfo(const std::string& class_name, const std::string& parent_name) = 0;
    //    virtual std::shared_ptr<AFIMetaConfigInfo> GetMetaClass(const std::string& class_name) = 0;
    //    virtual AFNewSmartPtrMap<std::string, AFIMetaConfigInfo>& GetAllMetaConfigInfo() = 0;
    //};

}