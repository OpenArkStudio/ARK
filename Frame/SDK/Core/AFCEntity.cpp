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

#include "AFCEntity.h"
#include "AFCDataTableManager.h"
#include "AFCHeartBeatManager.h"
#include "AFCDataNodeManager.h"
#include "AFCEventManager.h"

AFCEntity::AFCEntity(const AFGUID& self) :
    AFIEntity(self),
    mSelf(self)
{
    m_pNodeManager = std::make_shared<AFCDataNodeManager>(mSelf);
    m_pTableManager = std::make_shared<AFCDataTableManager>(mSelf);
    m_pHeartBeatManager = std::make_shared<AFCHeartBeatManager>(mSelf);
    m_pEventManager = std::make_shared<AFCEventManager>(mSelf);
}

AFCEntity::~AFCEntity()
{

}

bool AFCEntity::Init()
{
    return true;
}

bool AFCEntity::Shut()
{
    return true;
}

void AFCEntity::Update()
{
    GetHeartBeatManager()->Update();
    GetEventManager()->Update();
}

bool AFCEntity::AddHeartBeat(const std::string& name, const HEART_BEAT_FUNCTOR_PTR& cb, const int64_t nTime, const int nCount, const bool bForever)
{
    return GetHeartBeatManager()->AddHeartBeat(mSelf, name, cb, nTime, nCount, bForever);
}

bool AFCEntity::CheckHeartBeatExist(const std::string& name)
{
    return GetHeartBeatManager()->Exist(name);
}

bool AFCEntity::RemoveHeartBeat(const std::string& name)
{
    return GetHeartBeatManager()->RemoveHeartBeat(name);
}

bool AFCEntity::AddTableCallBack(const std::string& name, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb)
{
    ARK_SHARE_PTR<AFIDataTableManager> pTableManager = GetTableManager();

    if (nullptr == pTableManager)
    {
        return false;
    }

    return pTableManager->AddTableCallback(name.c_str(), cb);
}

bool AFCEntity::AddNodeCallBack(const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR& cb)
{
    return GetNodeManager()->RegisterCallback(name, cb);
}

bool AFCEntity::CheckNodeExist(const std::string& name)
{
    return (GetNodeManager()->GetNode(name.c_str()) != nullptr);
}

bool AFCEntity::SetNodeBool(const std::string& name, const bool value)
{
    return GetNodeManager()->SetNodeBool(name.c_str(), value);
}

bool AFCEntity::SetNodeInt(const std::string& name, const int32_t value)
{
    return GetNodeManager()->SetNodeInt(name.c_str(), value);
}

bool AFCEntity::SetNodeInt64(const std::string& name, const int64_t value)
{
    return GetNodeManager()->SetNodeInt64(name.c_str(), value);
}

bool AFCEntity::SetNodeFloat(const std::string& name, const float value)
{
    return GetNodeManager()->SetNodeFloat(name.c_str(), value);
}

bool AFCEntity::SetNodeDouble(const std::string& name, const double value)
{
    return GetNodeManager()->SetNodeDouble(name.c_str(), value);
}

bool AFCEntity::SetNodeString(const std::string& name, const std::string& value)
{
    return GetNodeManager()->SetNodeString(name.c_str(), value);
}

bool AFCEntity::SetNodeObject(const std::string& name, const AFGUID& value)
{
    return GetNodeManager()->SetNodeObject(name.c_str(), value);
}

bool AFCEntity::GetNodeBool(const std::string& name)
{
    return GetNodeManager()->GetNodeBool(name.c_str());
}

int32_t AFCEntity::GetNodeInt(const std::string& name)
{
    return GetNodeManager()->GetNodeInt(name.c_str());
}

int64_t AFCEntity::GetNodeInt64(const std::string& name)
{
    return GetNodeManager()->GetNodeInt64(name.c_str());
}

float AFCEntity::GetNodeFloat(const std::string& name)
{
    return GetNodeManager()->GetNodeFloat(name.c_str());
}

double AFCEntity::GetNodeDouble(const std::string& name)
{
    return GetNodeManager()->GetNodeDouble(name.c_str());
}

const char* AFCEntity::GetNodeString(const std::string& name)
{
    return GetNodeManager()->GetNodeString(name.c_str());
}

const AFGUID AFCEntity::GetNodeObject(const std::string& name)
{
    return GetNodeManager()->GetNodeObject(name.c_str());
}

bool AFCEntity::CheckTableExist(const std::string& name)
{
    AFDataTable* pTable = GetTableManager()->GetTable(name.c_str());
    return (nullptr != pTable);
}

bool AFCEntity::SetTableBool(const std::string& name, const int row, const int col, const bool value)
{
    return GetTableManager()->SetTableBool(name.c_str(), row, col, value);
}

bool AFCEntity::SetTableInt(const std::string& name, const int row, const int col, const int32_t value)
{
    return GetTableManager()->SetTableInt(name.c_str(), row, col, value);
}

bool AFCEntity::SetTableInt64(const std::string& name, const int row, const int col, const int64_t value)
{
    return GetTableManager()->SetTableInt64(name.c_str(), row, col, value);
}

bool AFCEntity::SetTableFloat(const std::string& name, const int row, const int col, const float value)
{
    return GetTableManager()->SetTableFloat(name.c_str(), row, col, value);
}

bool AFCEntity::SetTableDouble(const std::string& name, const int row, const int col, const double value)
{
    return GetTableManager()->SetTableDouble(name.c_str(), row, col, value);
}

bool AFCEntity::SetTableString(const std::string& name, const int row, const int col, const std::string& value)
{
    return GetTableManager()->SetTableString(name.c_str(), row, col, value.c_str());
}

bool AFCEntity::SetTableObject(const std::string& name, const int row, const int col, const AFGUID& value)
{
    return GetTableManager()->SetTableObject(name.c_str(), row, col, value);
}

bool AFCEntity::GetTableBool(const std::string& name, const int row, const int col)
{
    return GetTableManager()->GetTableBool(name.c_str(), row, col);
}

int32_t AFCEntity::GetTableInt(const std::string& name, const int row, const int col)
{
    return GetTableManager()->GetTableInt(name.c_str(), row, col);
}

int64_t AFCEntity::GetTableInt64(const std::string& name, const int row, const int col)
{
    return GetTableManager()->GetTableInt64(name.c_str(), row, col);
}

float AFCEntity::GetTableFloat(const std::string& name, const int row, const int col)
{
    return GetTableManager()->GetTableFloat(name.c_str(), row, col);
}

double AFCEntity::GetTableDouble(const std::string& name, const int row, const int col)
{
    return GetTableManager()->GetTableDouble(name.c_str(), row, col);
}

const char* AFCEntity::GetTableString(const std::string& name, const int row, const int col)
{
    return GetTableManager()->GetTableString(name.c_str(), row, col);
}

const AFGUID AFCEntity::GetTableObject(const std::string& name, const int row, const int col)
{
    return GetTableManager()->GetTableObject(name.c_str(), row, col);
}

ARK_SHARE_PTR<AFIDataNodeManager> AFCEntity::GetNodeManager()
{
    return m_pNodeManager;
}

ARK_SHARE_PTR<AFIDataTableManager> AFCEntity::GetTableManager()
{
    return m_pTableManager;
}

ARK_SHARE_PTR<AFIHeartBeatManager> AFCEntity::GetHeartBeatManager()
{
    return m_pHeartBeatManager;
}

ARK_SHARE_PTR<AFIEventManager> AFCEntity::GetEventManager()
{
    return m_pEventManager;
}

const AFGUID& AFCEntity::Self()
{
    return mSelf;
}
