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

#include "SDK/Core/Base/AFPlatform.hpp"
#include "SDK/Core/Base/AFCDataList.h"
#include "AFDataTable.h"
#include "AFIDataTableManager.h"
#include "AFIHeartBeatManager.h"
#include "AFIDataNodeManager.h"
#include "AFIEventManager.h"
#include "SDK/Interface/AFIPluginManager.h"

class AFIEntity
{
public:
    explicit AFIEntity(const AFGUID& self) {}

    virtual ~AFIEntity() {}

    virtual void Update() = 0;
    virtual const AFGUID& Self() = 0;

    template<typename BaseType>
    bool AddNodeCallBack(const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
    {
        DATA_NODE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddNodeCallBack(name, std::make_shared<DATA_NODE_EVENT_FUNCTOR>(functor));
    }

    template<typename BaseType>
    bool AddTableCallBack(const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
    {
        DATA_TABLE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddTableCallBack(name, std::make_shared<DATA_TABLE_EVENT_FUNCTOR>(functor));
    }

    template<typename BaseType>
    bool AddHeartBeat(const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const int64_t, const int), const int64_t nTime, const int nCount, const bool bForever)
    {
        HEART_BEAT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddHeartBeat(name, std::make_shared<HEART_BEAT_FUNCTOR>(functor), nTime, nCount,  bForever);
    }

    virtual bool CheckHeartBeatExist(const std::string& name) = 0;
    virtual bool RemoveHeartBeat(const std::string& name) = 0;

    virtual bool CheckNodeExist(const std::string& name) = 0;

    virtual bool SetNodeBool(const std::string& name, const bool value) = 0;
    virtual bool SetNodeInt(const std::string& name, const int32_t value) = 0;
    virtual bool SetNodeInt64(const std::string& name, const int64_t value) = 0;
    virtual bool SetNodeFloat(const std::string& name, const float value) = 0;
    virtual bool SetNodeDouble(const std::string& name, const double value) = 0;
    virtual bool SetNodeString(const std::string& name, const std::string& value) = 0;
    virtual bool SetNodeObject(const std::string& name, const AFGUID& value) = 0;

    virtual bool GetNodeBool(const std::string& name) = 0;
    virtual int32_t GetNodeInt(const std::string& name) = 0;
    virtual int64_t GetNodeInt64(const std::string& name) = 0;
    virtual float GetNodeFloat(const std::string& name) = 0;
    virtual double GetNodeDouble(const std::string& name) = 0;
    virtual const char* GetNodeString(const std::string& name) = 0;
    virtual const AFGUID GetNodeObject(const std::string& name) = 0;

    virtual bool CheckTableExist(const std::string& name) = 0;

    virtual bool SetTableBool(const std::string& name, const int row, const int col, const bool value) = 0;
    virtual bool SetTableInt(const std::string& name, const int row, const int col, const int32_t value) = 0;
    virtual bool SetTableInt64(const std::string& name, const int row, const int col, const int64_t value) = 0;
    virtual bool SetTableFloat(const std::string& name, const int row, const int col, const float value) = 0;
    virtual bool SetTableDouble(const std::string& name, const int row, const int col, const double value) = 0;
    virtual bool SetTableString(const std::string& name, const int row, const int col, const std::string& value) = 0;
    virtual bool SetTableObject(const std::string& name, const int row, const int col, const AFGUID& value) = 0;

    virtual bool GetTableBool(const std::string& name, const int row, const int col) = 0;
    virtual int32_t GetTableInt(const std::string& name, const int row, const int col) = 0;
    virtual int64_t GetTableInt64(const std::string& name, const int row, const int col) = 0;
    virtual float GetTableFloat(const std::string& name, const int row, const int col) = 0;
    virtual double GetTableDouble(const std::string& name, const int row, const int col) = 0;
    virtual const char* GetTableString(const std::string& name, const int row, const int col) = 0;
    virtual const AFGUID GetTableObject(const std::string& name, const int row, const int col) = 0;

    virtual ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager() = 0;
    virtual ARK_SHARE_PTR<AFIDataTableManager> GetTableManager() = 0;
    virtual ARK_SHARE_PTR<AFIHeartBeatManager> GetHeartBeatManager() = 0;
    virtual ARK_SHARE_PTR<AFIEventManager> GetEventManager() = 0;

protected:
    virtual bool AddNodeCallBack(const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool AddTableCallBack(const std::string& name, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool AddHeartBeat(const std::string& name, const HEART_BEAT_FUNCTOR_PTR& cb, const int64_t nTime, const int nCount, const bool bForever) = 0;
};