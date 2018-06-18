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

#include "SDK/Core/AFIEntity.h"
#include "SDK/Core/AFIDataTableManager.h"
#include "SDK/Core/AFIHeartBeatManager.h"
#include "SDK/Core/AFIDataNodeManager.h"

class AFCEntity : public AFIEntity
{
public:
    AFCEntity() = delete;

    explicit AFCEntity(const AFGUID& self);
    virtual ~AFCEntity();

    virtual bool Init();
    virtual bool Shut();
    virtual void Update();
    ///////////////////////////////////////////////////////////////////////
    virtual const AFGUID& Self();

    virtual bool CheckHeartBeatExist(const std::string& name);
    virtual bool RemoveHeartBeat(const std::string& name);
    /////////////////////////////////////////////////////////////////
    virtual bool CheckNodeExist(const std::string& name);

    virtual bool SetNodeBool(const std::string& name, const bool value);
    virtual bool SetNodeInt(const std::string& name, const int32_t value);
    virtual bool SetNodeInt64(const std::string& name, const int64_t value);
    virtual bool SetNodeFloat(const std::string& name, const float value);
    virtual bool SetNodeDouble(const std::string& name, const double value);
    virtual bool SetNodeString(const std::string& name, const std::string& value);
    virtual bool SetNodeObject(const std::string& name, const AFGUID& value);

    virtual bool GetNodeBool(const std::string& name);
    virtual int32_t GetNodeInt(const std::string& name);
    virtual int64_t GetNodeInt64(const std::string& name);
    virtual float GetNodeFloat(const std::string& name);
    virtual double GetNodeDouble(const std::string& name);
    virtual const char*  GetNodeString(const std::string& name);
    virtual const AFGUID GetNodeObject(const std::string& name);

    virtual bool CheckTableExist(const std::string& name);

    virtual bool SetTableBool(const std::string& name, const int row, const int col, const bool value);
    virtual bool SetTableInt(const std::string& name, const int row, const int col, const int32_t value);
    virtual bool SetTableInt64(const std::string& name, const int row, const int col, const int64_t value);
    virtual bool SetTableFloat(const std::string& name, const int row, const int col, const float value);
    virtual bool SetTableDouble(const std::string& name, const int row, const int col, const double value);
    virtual bool SetTableString(const std::string& name, const int row, const int col, const std::string& value);
    virtual bool SetTableObject(const std::string& name, const int row, const int col, const AFGUID& value);

    virtual bool GetTableBool(const std::string& name, const int row, const int col);
    virtual int32_t GetTableInt(const std::string& name, const int row, const int col);
    virtual int64_t GetTableInt64(const std::string& name, const int row, const int col);
    virtual float GetTableFloat(const std::string& name, const int row, const int col);
    virtual double GetTableDouble(const std::string& name, const int row, const int col);
    virtual const char* GetTableString(const std::string& name, const int row, const int col);
    virtual const AFGUID GetTableObject(const std::string& name, const int row, const int col);

    //////////////////////////////////////////////////////////////////////////
    virtual ARK_SHARE_PTR<AFIDataNodeManager> GetNodeManager();
    virtual ARK_SHARE_PTR<AFIDataTableManager> GetTableManager();
    virtual ARK_SHARE_PTR<AFIHeartBeatManager> GetHeartBeatManager();
    virtual ARK_SHARE_PTR<AFIEventManager> GetEventManager();

protected:
    virtual bool AddNodeCallBack(const std::string& col, const DATA_NODE_EVENT_FUNCTOR_PTR& cb);
    virtual bool AddTableCallBack(const std::string& col, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb);
    virtual bool AddHeartBeat(const std::string& col, const HEART_BEAT_FUNCTOR_PTR& cb, const int64_t nTime, const int nCount, const bool bForever);

private:
    AFGUID mSelf;

    ARK_SHARE_PTR<AFIDataTableManager> m_pTableManager;
    ARK_SHARE_PTR<AFIHeartBeatManager> m_pHeartBeatManager;
    ARK_SHARE_PTR<AFIDataNodeManager> m_pNodeManager;
    ARK_SHARE_PTR<AFIEventManager> m_pEventManager;
};