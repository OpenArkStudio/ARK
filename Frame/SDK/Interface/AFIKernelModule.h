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

#include "SDK/Core/AFIEntity.h"
#include "SDK/Core/AFGUID.hpp"
#include "AFIModule.h"

class AFIKernelModule : public AFIModule
{
public:
    template<typename BaseType>
    bool AddHeartBeat(const AFGUID self, const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const int64_t, const int), const int64_t nTime, const int nCount, const bool bForever = false)
    {
        ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(self);

        if (nullptr != pEntity)
        {
            return pEntity->AddHeartBeat(name, pBase, handler, nTime, nCount, bForever);
        }

        return false;
    }

    virtual bool FindHeartBeat(const AFGUID& self, const std::string& name) = 0;
    virtual bool RemoveHeartBeat(const AFGUID& self, const std::string& name) = 0;

    //////////////////////////////////////////////////////////////////////////

    template<typename BaseType>
    bool AddNodeCallBack(const AFGUID& self, const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
    {
        ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(self);

        if (nullptr != pEntity)
        {
            return pEntity->AddNodeCallBack(name, pBase, handler);
        }

        return false;
    }

    template<typename BaseType>
    bool AddTableCallBack(const AFGUID& self, const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
    {
        ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(self);

        if (nullptr != pEntity)
        {
            return pEntity->AddTableCallBack(name, pBase, handler);
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    template<typename BaseType>
    bool AddEventCallBack(const AFGUID& self, const int nEventID, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const int, const AFIDataList&))
    {
        EVENT_PROCESS_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        return AddEventCallBack(self, nEventID, std::make_shared<EVENT_PROCESS_FUNCTOR>(functor));
    }

    template<typename BaseType>
    bool AddClassCallBack(const std::string& name, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const ARK_ENTITY_EVENT, const AFIDataList&))
    {
        CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return AddClassCallBack(name, std::make_shared<CLASS_EVENT_FUNCTOR>(functor));
    }

    virtual bool DoEvent(const AFGUID& self, const std::string& name, ARK_ENTITY_EVENT eEvent, const AFIDataList& valueList) = 0;
    virtual bool DoEvent(const AFGUID& self, const int nEventID, const AFIDataList& valueList) = 0;

    //////////////////////////////////////////////////////////////////////////
    template<typename BaseType>
    bool RegCommonClassEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const ARK_ENTITY_EVENT, const AFIDataList&))
    {
        CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return RegCommonClassEvent(std::make_shared<CLASS_EVENT_FUNCTOR>(functor));
    }

    template<typename BaseType>
    bool RegCommonDataNodeEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
    {
        DATA_NODE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return RegCommonDataNodeEvent(std::make_shared<DATA_NODE_EVENT_FUNCTOR>(functor));
    }

    template<typename BaseType>
    bool RegCommonDataTableEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const DATA_TABLE_EVENT_DATA&, const AFIData&, const AFIData&))
    {
        DATA_TABLE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        return RegCommonDataTableEvent(std::make_shared<DATA_TABLE_EVENT_FUNCTOR>(functor));
    }

    /////////////////////////////////////////////////////////////////
    virtual bool IsContainer(const AFGUID& self) = 0;
    virtual bool ExistContainer(const int nContainerIndex) = 0;

    virtual ARK_SHARE_PTR<AFIEntity> GetEntity(const AFGUID& ident) = 0;
    virtual ARK_SHARE_PTR<AFIEntity> CreateEntity(const AFGUID& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const AFIDataList& arg) = 0;

    virtual bool DestroyEntity(const AFGUID& self) = 0;
    virtual bool DestroyAll() = 0;
    //////////////////////////////////////////////////////////////////////////
    virtual bool FindNode(const AFGUID& self, const std::string& name) = 0;

    virtual bool SetNodeBool(const AFGUID& self, const std::string& name, const bool value) = 0;
    virtual bool SetNodeInt(const AFGUID& self, const std::string& name, const int32_t value) = 0;
    virtual bool SetNodeInt64(const AFGUID& self, const std::string& name, const int64_t value) = 0;
    virtual bool SetNodeFloat(const AFGUID& self, const std::string& name, const float value) = 0;
    virtual bool SetNodeDouble(const AFGUID& self, const std::string& name, const double value) = 0;
    virtual bool SetNodeString(const AFGUID& self, const std::string& name, const std::string& value) = 0;
    virtual bool SetNodeObject(const AFGUID& self, const std::string& name, const AFGUID& value) = 0;

    virtual bool GetNodeBool(const AFGUID& self, const std::string& name) = 0;
    virtual int32_t GetNodeInt(const AFGUID& self, const std::string& name) = 0;
    virtual int64_t GetNodeInt64(const AFGUID& self, const std::string& name) = 0;
    virtual float GetNodeFloat(const AFGUID& self, const std::string& name) = 0;
    virtual double GetNodeDouble(const AFGUID& self, const std::string& name) = 0;
    virtual const char*  GetNodeString(const AFGUID& self, const std::string& name) = 0;
    virtual const AFGUID GetNodeObject(const AFGUID& self, const std::string& name) = 0;
    //////////////////////////////////////////////////////////////////////////
    virtual AFDataTable* FindTable(const AFGUID& self, const std::string& name) = 0;
    virtual bool ClearTable(const AFGUID& self, const std::string& name) = 0;

    virtual bool SetTableBool(const AFGUID& self, const std::string& name, const int row, const int col, const bool value) = 0;
    virtual bool SetTableInt(const AFGUID& self, const std::string& name, const int row, const int col, const int32_t value) = 0;
    virtual bool SetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col, const int64_t value) = 0;
    virtual bool SetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col, const float value) = 0;
    virtual bool SetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col, const double value) = 0;
    virtual bool SetTableString(const AFGUID& self, const std::string& name, const int row, const int col, const std::string& value) = 0;
    virtual bool SetTableObject(const AFGUID& self, const std::string& name, const int row, const int col, const AFGUID& value) = 0;

    virtual bool GetTableBool(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
    virtual int32_t GetTableInt(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
    virtual int64_t GetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
    virtual float GetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
    virtual double GetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
    virtual const char* GetTableString(const AFGUID& self, const std::string& name, const int row, const int col) = 0;
    virtual const AFGUID GetTableObject(const AFGUID& self, const std::string& name, const int row, const int col) = 0;

    //////////////////////////////////////////////////////////////////////////
    virtual bool SwitchScene(const AFGUID& self, const int nTargetSceneID, const int nTargetGroupID, const Point3D& pos, const float fOrient, const AFIDataList& arg) = 0;

    virtual bool CreateScene(const int nSceneID) = 0;
    virtual bool DestroyScene(const int nSceneID) = 0;

    virtual int GetOnLineCount() = 0;
    virtual int GetMaxOnLineCount() = 0;
    virtual int GetSceneOnLineCount(const int nSceneID) = 0;
    virtual int GetSceneOnLineCount(const int nSceneID, const int nGroupID) = 0;
    virtual int GetSceneOnLineList(const int nSceneID, AFIDataList& var) = 0;

    virtual int RequestGroupScene(const int nSceneID) = 0;
    virtual bool ReleaseGroupScene(const int nSceneID, const int nGroupID) = 0;
    virtual bool ExitGroupScene(const int nSceneID, const int nGroupID) = 0;

    virtual bool GetGroupEntityList(const int nSceneID, const int nGroupID, AFIDataList& list) = 0;
    virtual int GetEntityByDataNode(const int nSceneID, const std::string& strPropertyName, const AFIDataList& valueArg, AFIDataList& list) = 0;

    virtual bool LogInfo(const AFGUID& ident) = 0;

protected:
    virtual bool AddEventCallBack(const AFGUID& self, const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb) = 0;
    virtual bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;

    virtual bool RegCommonClassEvent(const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool RegCommonDataNodeEvent(const DATA_NODE_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool RegCommonDataTableEvent(const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) = 0;
};