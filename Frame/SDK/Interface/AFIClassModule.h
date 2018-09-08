/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "SDK/Core/AFMacros.hpp"
#include "SDK/Core/AFMap.hpp"
#include "SDK/Core/AFIDataNodeManager.h"
#include "SDK/Core/AFIDataTableManager.h"
#include "AFIModule.h"

class AFIClass : public AFList<std::string>
{
public:
    virtual ~AFIClass() {}

    virtual ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager() = 0;
    virtual ARK_SHARE_PTR<AFIDataTableManager> GetTableManager() = 0;

    virtual void SetParent(ARK_SHARE_PTR<AFIClass> pClass) = 0;
    virtual ARK_SHARE_PTR<AFIClass> GetParent() = 0;

    virtual void SetTypeName(const char* strType) = 0;
    virtual const std::string& GetTypeName() = 0;
    virtual const std::string& GetClassName() = 0;

    virtual bool AddConfigName(std::string& strConfigName) = 0;
    virtual AFList<std::string>& GetConfigNameList() = 0;

    virtual void SetResPath(const std::string& strPath) = 0;
    virtual const std::string& GetResPath() = 0;

    virtual bool AddClassCallBack(const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool DoEvent(const AFGUID& objectID, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& valueList) = 0;

    template<typename BaseType>
    bool AddNodeCallBack(const std::string& strClassName, const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
    {
        DATA_NODE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddNodeCallBack(strClassName, std::make_shared<DATA_NODE_EVENT_FUNCTOR>(functor));
    }

    virtual bool AddNodeCallBack(const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool AddCommonNodeCallback(const DATA_NODE_EVENT_FUNCTOR_PTR& cb) = 0;

    template<typename BaseType>
    bool AddTableCommonCallback(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
    {
        DATA_TABLE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddTableCommonCallback(std::make_shared<DATA_TABLE_EVENT_FUNCTOR>(functor));
    }

    virtual bool AddTableCallBack(const std::string& name, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool AddCommonTableCallback(const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) = 0;

    virtual bool InitDataNodeManager(ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager) = 0;
    virtual bool InitDataTableManager(ARK_SHARE_PTR<AFIDataTableManager> pTableManager) = 0;
};

class AFIClassModule
    : public AFIModule,
      public AFMapEx<std::string, AFIClass>
{
public:
    virtual ~AFIClassModule() {}

    virtual bool Load() = 0;
    virtual bool Clear() = 0;

    template<typename BaseType>
    bool AddClassCallBack(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const ARK_ENTITY_EVENT, const AFIDataList&))
    {
        CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        CLASS_EVENT_FUNCTOR_PTR functorPtr(new CLASS_EVENT_FUNCTOR(functor));
        return AddClassCallBack(strClassName, functorPtr);
    }

    template<typename BaseType>
    bool AddNodeCallBack(const std::string& strClassName, const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
    {
        DATA_NODE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddNodeCallBack(strClassName, name, std::make_shared<DATA_NODE_EVENT_FUNCTOR>(functor));
    }

    template<typename BaseType>
    bool AddTableCallBack(const std::string& strClassName, const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
    {
        DATA_TABLE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddTableCallBack(strClassName, name, std::make_shared<DATA_TABLE_EVENT_FUNCTOR>(functor));
    }

    template<typename BaseType>
    bool AddCommonNodeCallback(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
    {
        DATA_NODE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddCommonNodeCallback(strClassName, std::make_shared<DATA_NODE_EVENT_FUNCTOR>(functor));
    }

    template<typename BaseType>
    bool AddCommonTableCallback(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
    {
        DATA_TABLE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddCommonTableCallback(strClassName, std::make_shared<DATA_TABLE_EVENT_FUNCTOR>(functor));
    }

    virtual bool DoEvent(const AFGUID& objectID, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& valueList) = 0;
    virtual bool AddNodeCallBack(const std::string& strClassName, const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool AddTableCallBack(const std::string& strClassName, const std::string& name, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool AddCommonNodeCallback(const std::string& strClassName, const DATA_NODE_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool AddCommonTableCallback(const std::string& strClassName, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) = 0;

    virtual bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager(const std::string& strClassName) = 0;
    virtual ARK_SHARE_PTR<AFIDataTableManager> GetTableManager(const std::string& strClassName) = 0;
    virtual bool InitDataNodeManager(const std::string& strClassName, ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager) = 0;
    virtual bool InitDataTableManager(const std::string& strClassName, ARK_SHARE_PTR<AFIDataTableManager> pTableManager) = 0;
};
