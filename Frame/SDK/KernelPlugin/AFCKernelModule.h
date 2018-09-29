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

#include "SDK/Core/AFMap.hpp"
#include "SDK/Core/AFArrayMap.hpp"
#include "SDK/Core/AFIEntity.h"
#include "SDK/Core/AFCDataList.hpp"
#include "SDK/Core/AFDataTable.h"
#include "SDK/Core/AFGUID.hpp"
#include "SDK/Interface/AFIGUIDModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIConfigModule.h"
#include "SDK/Interface/AFISceneModule.h"

namespace ark
{

    class AFCKernelModule : public AFIKernelModule, public AFMapEx<AFGUID, AFIEntity> //TODO:put data as a separate data
    {
    public:
        explicit AFCKernelModule();
        ~AFCKernelModule() override;

        bool Init() override;
        bool Shut() override;

        bool PreShut() override;
        bool Update() override;

        ///////////////////////////////////////////////////////////////////////

        bool FindHeartBeat(const AFGUID& self, const std::string& name) override;
        bool RemoveHeartBeat(const AFGUID& self, const std::string& name) override;

        bool IsContainer(const AFGUID& self) override;
        bool ExistContainer(const int nSceneID) override;

        ARK_SHARE_PTR<AFIEntity> GetEntity(const AFGUID& ident) override;
        ARK_SHARE_PTR<AFIEntity> CreateEntity(const AFGUID& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const AFIDataList& arg) override;

        bool DestroyAll() override;
        bool DestroyEntity(const AFGUID& self) override;

        //////////////////////////////////////////////////////////////////////////
        bool FindNode(const AFGUID& self, const std::string& name) override;

        bool SetNodeBool(const AFGUID& self, const std::string& name, const bool value) override;
        bool SetNodeInt(const AFGUID& self, const std::string& name, const int32_t value) override;
        bool SetNodeInt64(const AFGUID& self, const std::string& name, const int64_t value) override;
        bool SetNodeFloat(const AFGUID& self, const std::string& name, const float value) override;
        bool SetNodeDouble(const AFGUID& self, const std::string& name, const double value) override;
        bool SetNodeString(const AFGUID& self, const std::string& name, const std::string& value) override;
        bool SetNodeObject(const AFGUID& self, const std::string& name, const AFGUID& value) override;

        bool GetNodeBool(const AFGUID& self, const std::string& name) override;
        int32_t GetNodeInt(const AFGUID& self, const std::string& name) override;
        int64_t GetNodeInt64(const AFGUID& self, const std::string& name) override;
        float GetNodeFloat(const AFGUID& self, const std::string& name) override;
        double GetNodeDouble(const AFGUID& self, const std::string& name) override;
        const char* GetNodeString(const AFGUID& self, const std::string& name) override;
        const AFGUID GetNodeObject(const AFGUID& self, const std::string& name) override;
        //////////////////////////////////////////////////////////////////////////
        AFDataTable* FindTable(const AFGUID& self, const std::string& name) override;
        bool ClearTable(const AFGUID& self, const std::string& name) override;

        bool SetTableBool(const AFGUID& self, const std::string& name, const int row, const int col, const bool value) override;
        bool SetTableInt(const AFGUID& self, const std::string& name, const int row, const int col, const int32_t value) override;
        bool SetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col, const int64_t value) override;
        bool SetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col, const float value) override;
        bool SetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col, const double value) override;
        bool SetTableString(const AFGUID& self, const std::string& name, const int row, const int col, const std::string& value) override;
        bool SetTableObject(const AFGUID& self, const std::string& name, const int row, const int col, const AFGUID& value) override;

        bool GetTableBool(const AFGUID& self, const std::string& name, const int row, const int col) override;
        int32_t GetTableInt(const AFGUID& self, const std::string& name, const int row, const int col) override;
        int64_t GetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col) override;
        float GetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col) override;
        double GetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col) override;
        const char* GetTableString(const AFGUID& self, const std::string& name, const int row, const int col) override;
        const AFGUID GetTableObject(const AFGUID& self, const std::string& name, const int row, const int col) override;

        //////////////////////////////////////////////////////////////////////////
        bool SwitchScene(const AFGUID& self, const int nTargetSceneID, const int nTargetGroupID, const Point3D& pos, const float fOrient, const AFIDataList& arg) override;

        bool CreateScene(const int nSceneID) override;
        bool DestroyScene(const int nSceneID) override;

        int GetOnLineCount() override;
        int GetMaxOnLineCount() override;
        int GetSceneOnLineCount(const int nSceneID) override;
        int GetSceneOnLineCount(const int nSceneID, const int nGroupID) override;

        int GetSceneOnLineList(const int nSceneID, AFIDataList& var) override;

        int RequestGroupScene(const int nSceneID) override;
        bool ReleaseGroupScene(const int nSceneID, const int nGroupID) override;
        bool ExitGroupScene(const int nSceneID, const int nGroupID) override;

        bool GetGroupEntityList(const int nSceneID, const int nGroupID, AFIDataList& list) override;
        int GetEntityByDataNode(const int nSceneID, const std::string& strNodeName, const AFIDataList& valueArgArg, AFIDataList& list) override;
        //////////////////////////////////////////////////////////////////////////
        bool LogStack();
        bool LogInfo(const AFGUID& ident) override;
        bool LogSelfInfo(const AFGUID& ident);

        //////////////////////////////////////////////////////////////////////////

        bool DoEvent(const AFGUID& self, const std::string& strClassName, ARK_ENTITY_EVENT eEvent, const AFIDataList& valueList) override;
        bool DoEvent(const AFGUID& self, const int nEventID, const AFIDataList& valueList) override;

    protected:
        bool DestroySelf(const AFGUID& self);

        bool RegCommonClassEvent(const CLASS_EVENT_FUNCTOR_PTR& cb) override;
        bool RegCommonDataNodeEvent(const DATA_NODE_EVENT_FUNCTOR_PTR& cb) override;
        bool RegCommonDataTableEvent(const DATA_TABLE_EVENT_FUNCTOR_PTR& cb) override;

        bool AddEventCallBack(const AFGUID& self, const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb) override;
        bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb) override;

    private:
        std::list<AFGUID> mtDeleteSelfList;
        //////////////////////////////////////////////////////////////////////////
        std::list<CLASS_EVENT_FUNCTOR_PTR> mxCommonClassCBList;
        std::list<DATA_NODE_EVENT_FUNCTOR_PTR> mxCommonNodeCBList;
        std::list<DATA_TABLE_EVENT_FUNCTOR_PTR> mxCommonTableCBList;

        AFGUID mnCurExeEntity;

        AFISceneModule* m_pSceneModule = nullptr;
        AFILogModule* m_pLogModule = nullptr;
        AFIClassModule* m_pClassModule = nullptr;
        AFIConfigModule* m_pConfigModule = nullptr;
        AFIGUIDModule* m_pGUIDModule = nullptr;

        AFArrayMap<std::string, int32_t> mInnerProperty;
    };

}