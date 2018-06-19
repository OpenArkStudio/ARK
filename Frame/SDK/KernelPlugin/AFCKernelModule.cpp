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

#include "SDK/Core/Base/AFDefine.h"
#include "SDK/Core/Base/AFGUID.h"
#include "SDK/Core/AFCEntity.h"
#include "SDK/Core/AFDataNode.h"
#include "SDK/Core/AFDataTable.h"
#include "SDK/Proto/ARKDataDefine.hpp"
#include "AFCKernelModule.h"

AFCKernelModule::AFCKernelModule(AFIPluginManager* p)
{
    pPluginManager = p;

    nLastTime = pPluginManager->GetNowTime();
    InitRandom();

    mInnerProperty.AddElement(ARK::IObject::ConfigID(), ARK_NEW int32_t(0));
    mInnerProperty.AddElement(ARK::IObject::ClassName(), ARK_NEW int32_t(0));
    mInnerProperty.AddElement(ARK::IObject::SceneID(), ARK_NEW int32_t(0));
    mInnerProperty.AddElement(ARK::IObject::GroupID(), ARK_NEW int32_t(0));
}

AFCKernelModule::~AFCKernelModule()
{
    for(size_t i = 0; i < mInnerProperty.GetCount(); ++i)
    {
        delete mInnerProperty[i];
    }

    ClearAll();
}

void AFCKernelModule::InitRandom()
{
    mvRandom.clear();

    int nRandomMax = 100000;
    mnRandomPos = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0f, 1.0f);

    for(int i = 0; i < nRandomMax; i++)
    {
        mvRandom.push_back(dis(gen));
    }
}

bool AFCKernelModule::Init()
{
    mtDeleteSelfList.clear();

    m_pSceneModule = pPluginManager->FindModule<AFISceneModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pGUIDModule = pPluginManager->FindModule<AFIGUIDModule>();

    return true;
}

bool AFCKernelModule::Shut()
{
    return true;
}

bool AFCKernelModule::Update()
{
    mnCurExeEntity = NULL_GUID;

    if(mtDeleteSelfList.size() > 0)
    {
        for(auto it : mtDeleteSelfList)
        {
            DestroyEntity(it);
        }

        mtDeleteSelfList.clear();
    }

    ARK_SHARE_PTR<AFIEntity> pEntity = First();
    while(pEntity)
    {
        mnCurExeEntity = pEntity->Self();
        pEntity->Update();
        mnCurExeEntity = NULL_GUID;

        pEntity = Next();
    }

    return true;
}

bool AFCKernelModule::FindHeartBeat(const AFGUID& self, const std::string& name)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(pEntity != nullptr)
    {
        return pEntity->CheckHeartBeatExist(name);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::RemoveHeartBeat(const AFGUID& self, const std::string& name)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(pEntity != nullptr)
    {
        return pEntity->RemoveHeartBeat(name);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());

    return false;
}

ARK_SHARE_PTR<AFIEntity> AFCKernelModule::CreateEntity(const AFGUID& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const AFIDataList& arg)
{
    AFGUID ident = self;

    ARK_SHARE_PTR<AFCSceneInfo> pContainerInfo = m_pSceneModule->GetElement(nSceneID);
    if(!pContainerInfo)
    {
        ARK_LOG_ERROR("There is no scene, scene = {}", nSceneID);
        return nullptr;
    }

    if(!pContainerInfo->GetElement(nGroupID))
    {
        ARK_LOG_ERROR("There is no group, scene = {} group = {}", nSceneID, nGroupID);
        return nullptr;
    }

    if(ident.IsNULL())
    {
        ident = m_pGUIDModule->CreateGUID();
    }

    if(GetElement(ident))
    {
        ARK_LOG_ERROR("The object has Exists, id = {}", ident.ToString().c_str());
        return nullptr;
    }

    ARK_SHARE_PTR<AFIEntity> pEntity;
    pEntity = std::make_shared<AFCEntity>(ident);
    AddElement(ident, pEntity);
    pContainerInfo->AddObjectToGroup(nGroupID, ident, strClassName == ARK::Player::ThisName() ? true : false);

    ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager = pEntity->GetNodeManager();
    ARK_SHARE_PTR<AFIDataTableManager> pTableManager = pEntity->GetTableManager();
    m_pClassModule->InitDataNodeManager(strClassName, pNodeManager);
    m_pClassModule->InitDataTableManager(strClassName, pTableManager);
    pNodeManager->AddCommonCallBack(this, &AFCKernelModule::OnCommonNodeEvent);
    pTableManager->AddTableCommonCallback(this, &AFCKernelModule::OnCommonTableEvent);

    ARK_SHARE_PTR<AFIDataNodeManager> pConfigNodeManager = m_pElementModule->GetNodeManager(strConfigIndex);
    if(pConfigNodeManager != nullptr)
    {
        size_t configNodeCount = pConfigNodeManager->GetNodeCount();
        for(size_t i = 0; i < configNodeCount; ++i)
        {
            AFDataNode* pConfigNode = pConfigNodeManager->GetNodeByIndex(i);
            if(pConfigNode != nullptr || pConfigNode->Changed())
            {
                pNodeManager->SetNode(pConfigNode->GetName().c_str(), pConfigNode->GetValue());
            }
        }
    }

    DoEvent(ident, strClassName, ENTITY_EVT_PRE_LOAD_DATA, arg);

    for(int i = 0; (i + 1) < (arg.GetCount() - 1); i += 2)
    {
        const std::string& strDataNodeName = arg.String(i);
        if(!mInnerProperty.ExistElement(strDataNodeName))
        {
            AFDataNode* pArgNode = pNodeManager->GetNode(strDataNodeName.c_str());
            if(pArgNode != nullptr)
            {
                arg.ToAFIData(i + 1, pArgNode->value);
            }
        }
    }

    pEntity->SetNodeString(ARK::IObject::ConfigID(), strConfigIndex);
    pEntity->SetNodeString(ARK::IObject::ClassName(), strClassName);
    pEntity->SetNodeInt(ARK::IObject::SceneID(), nSceneID);
    pEntity->SetNodeInt(ARK::IObject::GroupID(), nGroupID);

    DoEvent(ident, strClassName, ENTITY_EVT_LOAD_DATA, arg);
    DoEvent(ident, strClassName, ENTITY_EVT_PRE_EFFECT_DATA, arg);
    DoEvent(ident, strClassName, ENTITY_EVT_EFFECT_DATA, arg);
    DoEvent(ident, strClassName, ENTITY_EVT_POST_EFFECT_DATA, arg);
    DoEvent(ident, strClassName, ENTITY_EVT_DATA_FINISHED, arg);

    return pEntity;
}

bool AFCKernelModule::DestroyEntity(const AFGUID& self)
{
    if(self == mnCurExeEntity && !self.IsNULL())
    {
        return DestroySelf(self);
    }

    int32_t nGroupID = GetNodeInt(self, ARK::IObject::GroupID());
    int32_t nSceneID = GetNodeInt(self, ARK::IObject::SceneID());

    ARK_SHARE_PTR<AFCSceneInfo> pContainerInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pContainerInfo)
    {
        const std::string& strClassName = GetNodeString(self, ARK::IObject::ClassName());

        pContainerInfo->RemoveObjectFromGroup(nGroupID, self, strClassName == ARK::Player::ThisName() ? true : false);

        DoEvent(self, strClassName, ENTITY_EVT_PRE_DESTROY, AFCDataList());
        DoEvent(self, strClassName, ENTITY_EVT_DESTROY, AFCDataList());

        return RemoveElement(self);
    }

    ARK_LOG_ERROR("Cannot find this scene, entity_id = {} scene = {} group = {}", self.ToString(), nSceneID, nGroupID);
    return false;
}

bool AFCKernelModule::FindNode(const AFGUID& self, const std::string& name)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->CheckNodeExist(name);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetNodeBool(const AFGUID& self, const std::string& name, const bool value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->SetNodeBool(name, value);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetNodeInt(const AFGUID& self, const std::string& name, const int32_t value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->SetNodeInt(name, value);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetNodeInt64(const AFGUID& self, const std::string& name, const int64_t value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->SetNodeInt64(name, value);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetNodeFloat(const AFGUID& self, const std::string& name, const float value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->SetNodeFloat(name, value);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetNodeDouble(const AFGUID& self, const std::string& name, const double value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->SetNodeDouble(name, value);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetNodeString(const AFGUID& self, const std::string& name, const std::string& value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->SetNodeString(name, value);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetNodeObject(const AFGUID& self, const std::string& name, const AFGUID& value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->SetNodeObject(name, value);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::GetNodeBool(const AFGUID& self, const std::string& name)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetNodeBool(name);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return NULL_BOOLEAN;
}

int32_t AFCKernelModule::GetNodeInt(const AFGUID& self, const std::string& name)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetNodeInt(name);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return NULL_INT;
}

int64_t AFCKernelModule::GetNodeInt64(const AFGUID& self, const std::string& name)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetNodeInt64(name);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return NULL_INT64;
}

float AFCKernelModule::GetNodeFloat(const AFGUID& self, const std::string& name)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetNodeFloat(name);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return NULL_FLOAT;
}

double AFCKernelModule::GetNodeDouble(const AFGUID& self, const std::string& name)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetNodeDouble(name);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return NULL_DOUBLE;
}

const char* AFCKernelModule::GetNodeString(const AFGUID& self, const std::string& name)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetNodeString(name);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return nullptr;
}

const AFGUID AFCKernelModule::GetNodeObject(const AFGUID& self, const std::string& name)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetNodeObject(name);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return NULL_GUID;
}

AFDataTable* AFCKernelModule::FindTable(const AFGUID& self, const std::string& name)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetTableManager()->GetTable(name.c_str());
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return NULL;
}

bool AFCKernelModule::ClearTable(const AFGUID& self, const std::string& name)
{
    AFDataTable* pTable = FindTable(self, name);
    if(pTable != nullptr)
    {
        pTable->Clear();
        return true;
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetTableBool(const AFGUID& self, const std::string& name, const int row, const int col, const bool value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        if(!pEntity->SetTableBool(name, row, col, value))
        {
            ARK_LOG_ERROR("error for row or col, id = {}", self.ToString().c_str());
            return false;
        }

        return true;
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetTableInt(const AFGUID& self, const std::string& name, const int row, const int col, const int32_t value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        if(!pEntity->SetTableInt(name, row, col, value))
        {
            ARK_LOG_ERROR("error for row or col, id = {}", self.ToString().c_str());
            return false;
        }

        return true;
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col, const int64_t value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        if(!pEntity->SetTableInt64(name, row, col, value))
        {
            ARK_LOG_ERROR("error for row or col, id = {}", self.ToString().c_str());
            return false;
        }

        return true;
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col, const float value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        if(!pEntity->SetTableFloat(name, row, col, value))
        {
            ARK_LOG_ERROR("error for row or col, id = {}", self.ToString().c_str());
            return false;
        }

        return true;
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col, const double value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        if(!pEntity->SetTableDouble(name, row, col, value))
        {
            ARK_LOG_ERROR("error for row or col, id = {}", self.ToString().c_str());
            return false;
        }

        return true;
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetTableString(const AFGUID& self, const std::string& name, const int row, const int col, const std::string& value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        if(!pEntity->SetTableString(name, row, col, value))
        {
            ARK_LOG_ERROR("error for row or col, id = {}", self.ToString().c_str());
            return false;
        }

        return true;
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::SetTableObject(const AFGUID& self, const std::string& name, const int row, const int col, const AFGUID& value)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        if(!pEntity->SetTableObject(name, row, col, value))
        {
            ARK_LOG_ERROR("error for row or col, id = {}", self.ToString().c_str());
            return false;
        }

        return true;
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return false;
}

bool AFCKernelModule::GetTableBool(const AFGUID& self, const std::string& name, const int row, const int col)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetTableBool(name, row, col);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return 0;
}

int32_t AFCKernelModule::GetTableInt(const AFGUID& self, const std::string& name, const int row, const int col)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetTableInt(name, row, col);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return 0;
}

int64_t AFCKernelModule::GetTableInt64(const AFGUID& self, const std::string& name, const int row, const int col)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetTableInt64(name, row, col);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return 0;
}

float AFCKernelModule::GetTableFloat(const AFGUID& self, const std::string& name, const int row, const int col)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetTableFloat(name, row, col);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return 0;
}

double AFCKernelModule::GetTableDouble(const AFGUID& self, const std::string& name, const int row, const int col)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetTableDouble(name, row, col);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return 0.0;
}

const char* AFCKernelModule::GetTableString(const AFGUID& self, const std::string& name, const int row, const int col)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetTableString(name, row, col);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return NULL_STR.c_str();
}

const AFGUID AFCKernelModule::GetTableObject(const AFGUID& self, const std::string& name, const int row, const int col)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetTableObject(name, row, col);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
    return NULL_GUID;
}

bool AFCKernelModule::SwitchScene(const AFGUID& self, const int nTargetSceneID, const int nTargetGroupID, const Point3D& pos, const float fOrient, const AFIDataList& arg)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr == pEntity)
    {
        ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString().c_str());
        return false;
    }

    int32_t nOldSceneID = pEntity->GetNodeInt("SceneID");
    int32_t nOldGroupID = pEntity->GetNodeInt("GroupID");

    ARK_SHARE_PTR<AFCSceneInfo> pOldSceneInfo = m_pSceneModule->GetElement(nOldSceneID);
    ARK_SHARE_PTR<AFCSceneInfo> pNewSceneInfo = m_pSceneModule->GetElement(nTargetSceneID);
    if(nullptr == pOldSceneInfo)
    {
        ARK_LOG_ERROR("no this container, id = {}", nOldSceneID);
        return false;
    }

    if(nullptr == pNewSceneInfo)
    {
        ARK_LOG_ERROR("no this container, id = {}", nTargetSceneID);
        return false;
    }

    if(nullptr == pNewSceneInfo->GetElement(nTargetGroupID))
    {
        ARK_LOG_ERROR("no this container, id = {}", nTargetGroupID);
        return false;
    }

    pOldSceneInfo->RemoveObjectFromGroup(nOldGroupID, self, true);

    if(nTargetSceneID != nOldSceneID)
    {
        pEntity->SetNodeInt("GroupID", 0);

        pEntity->SetNodeInt("SceneID", nTargetSceneID);
    }

    pEntity->SetNodeInt("GroupID", nTargetGroupID);

    pEntity->SetNodeDouble("X", pos.x);
    pEntity->SetNodeDouble("Y", pos.y);
    pEntity->SetNodeDouble("Z", pos.z);

    pNewSceneInfo->AddObjectToGroup(nTargetGroupID, self, true);

    return true;
}

bool AFCKernelModule::CreateScene(const int nSceneID)
{
    ARK_SHARE_PTR<AFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pSceneInfo)
    {
        return false;
    }

    pSceneInfo = std::make_shared<AFCSceneInfo>(nSceneID);
    if(nullptr == pSceneInfo)
    {
        return false;
    }

    m_pSceneModule->AddElement(nSceneID, pSceneInfo);

    //create group 0
    ARK_SHARE_PTR<AFCSceneGroupInfo> pGroupInfo = std::make_shared<AFCSceneGroupInfo>(0);
    if(nullptr == pGroupInfo)
    {
        return false;
    }

    pSceneInfo->AddElement(0, pGroupInfo);
    ARK_LOG_INFO("Create scene success, scene id:{} groupId:0", nSceneID);
    return true;
}

bool AFCKernelModule::DestroyScene(const int nSceneID)
{
    return m_pSceneModule->RemoveElement(nSceneID);
}

int AFCKernelModule::GetOnLineCount()
{
    int nCount = 0;
    for(ARK_SHARE_PTR<AFCSceneInfo> pSceneInfo = m_pSceneModule->First(); pSceneInfo != nullptr; pSceneInfo = m_pSceneModule->Next())
    {
        for(ARK_SHARE_PTR<AFCSceneGroupInfo> pGroupInfo = pSceneInfo->First(); pGroupInfo != nullptr; pGroupInfo = pSceneInfo->Next())
        {
            nCount += pGroupInfo->mxPlayerList.GetCount();
        }
    }

    return nCount;
}

int AFCKernelModule::GetMaxOnLineCount()
{
    // test count 5000
    // and it should be define in a config file
    return 10000;
}

int AFCKernelModule::GetSceneOnLineCount(const int nSceneID)
{
    int nCount = 0;

    ARK_SHARE_PTR<AFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr == pSceneInfo)
    {
        return nCount;
    }

    for(ARK_SHARE_PTR<AFCSceneGroupInfo> pGroupInfo = pSceneInfo->First(); pGroupInfo != nullptr; pGroupInfo = pSceneInfo->Next())
    {
        nCount += pGroupInfo->mxPlayerList.GetCount();
    }

    return nCount;
}

int AFCKernelModule::GetSceneOnLineCount(const int nSceneID, const int nGroupID)
{
    int nCount = 0;

    ARK_SHARE_PTR<AFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr == pSceneInfo)
    {
        return nCount;
    }

    ARK_SHARE_PTR<AFCSceneGroupInfo> pGroupInfo = pSceneInfo->GetElement(nGroupID);
    if(nullptr != pGroupInfo)
    {
        nCount = pGroupInfo->mxPlayerList.GetCount();
    }

    return nCount;
}

int AFCKernelModule::GetSceneOnLineList(const int nSceneID, AFIDataList& var)
{
    ARK_SHARE_PTR<AFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr == pSceneInfo)
    {
        return 0;
    }

    for(ARK_SHARE_PTR<AFCSceneGroupInfo> pGroupInfo = pSceneInfo->First(); pGroupInfo != nullptr; pGroupInfo = pSceneInfo->Next())
    {
        AFGUID ident;
        for(ARK_SHARE_PTR<int> pRet = pGroupInfo->mxPlayerList.First(ident); !ident.IsNULL(); pRet = pGroupInfo->mxPlayerList.Next(ident))
        {
            var.AddObject(ident);
            ident = NULL_GUID;
        }
    }

    return var.GetCount();
}

int AFCKernelModule::RequestGroupScene(const int nSceneID)
{
    ARK_SHARE_PTR<AFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr == pSceneInfo)
    {
        return -1;
    }

    int nNewGroupID = pSceneInfo->NewGroupID();
    if(nullptr != pSceneInfo->GetElement(nNewGroupID))
    {
        return -1;
    }

    ARK_SHARE_PTR<AFCSceneGroupInfo> pGroupInfo = std::make_shared<AFCSceneGroupInfo>(nNewGroupID);
    if(nullptr == pGroupInfo)
    {
        return -1;
    }

    pSceneInfo->AddElement(nNewGroupID, pGroupInfo);
    return nNewGroupID;
}

bool AFCKernelModule::ReleaseGroupScene(const int nSceneID, const int nGroupID)
{
    ARK_SHARE_PTR<AFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr == pSceneInfo)
    {
        return false;
    }

    if(nullptr == pSceneInfo->GetElement(nGroupID))
    {
        return false;
    }

    AFCDataList listObject;
    if(GetGroupEntityList(nSceneID, nGroupID, listObject))
    {
        for(int i = 0; i < listObject.GetCount(); ++i)
        {
            AFGUID ident = listObject.Object(i);
            if(!DestroyEntity(ident))
            {
                ARK_LOG_ERROR("Destory entity failed, id  = {}, pls check", ident.ToString().c_str());
            }
        }
    }

    pSceneInfo->RemoveElement(nGroupID);

    return true;
}

bool AFCKernelModule::ExitGroupScene(const int nSceneID, const int nGroupID)
{
    ARK_SHARE_PTR<AFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pSceneInfo)
    {
        ARK_SHARE_PTR<AFCSceneGroupInfo> pGroupInfo = pSceneInfo->GetElement(nGroupID);
        return (nullptr != pGroupInfo);
    }

    return false;
}

bool AFCKernelModule::GetGroupEntityList(const int nSceneID, const int nGroupID, AFIDataList& list)
{
    ARK_SHARE_PTR<AFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr == pSceneInfo)
    {
        return false;
    }

    ARK_SHARE_PTR<AFCSceneGroupInfo> pGroupInfo = pSceneInfo->GetElement(nGroupID);
    if(nullptr == pGroupInfo)
    {
        return false;
    }

    AFGUID ident = NULL_GUID;
    pGroupInfo->mxPlayerList.First(ident);
    while(!ident.IsNULL())
    {
        list.AddObject(ident);

        ident = NULL_GUID;
        pGroupInfo->mxPlayerList.Next(ident);
    }

    ident = NULL_GUID;
    pGroupInfo->mxOtherList.First(ident);
    while(!ident.IsNULL())
    {
        list.AddObject(ident);

        ident = NULL_GUID;
        pGroupInfo->mxOtherList.Next(ident);
    }

    return true;
}

bool AFCKernelModule::LogStack()
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
#endif

#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
#endif // ARK_PLATFORM

    return true;
}

bool AFCKernelModule::LogInfo(const AFGUID& ident)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(ident);
    if(nullptr == pEntity)
    {
        ARK_LOG_ERROR("Cannot find entity, id = {}", ident.ToString().c_str());
        return false;
    }

    if(IsContainer(ident))
    {
        int nSceneID = GetNodeInt(ident, "SceneID");

        ARK_LOG_INFO("----------child object list-------- , id = {} SceneID = {}", ident.ToString().c_str(), nSceneID);
        AFCDataList valObjectList;
        int nCount = GetSceneOnLineList(nSceneID, valObjectList);
        for(int i  = 0; i < nCount; i++)
        {
            AFGUID targetIdent = valObjectList.Object(i);
            ARK_LOG_INFO("id  = {} SceneID = {}", targetIdent.ToString().c_str(), nSceneID);
        }
    }
    else
    {
        ARK_LOG_INFO("---------printf object start--------, id = {}", ident.ToString().c_str());
        ARK_LOG_INFO("---------printf object end--------", ident.ToString().c_str());
    }

    return true;
}

int AFCKernelModule::OnCommonNodeEvent(const AFGUID& self, const std::string& name, const AFIData& oldVar, const AFIData& newVar)
{
    if(IsContainer(self))
    {
        return 1;
    }

    for(auto it : mxCommonNodeCBList)
    {
        (*it)(self, name, oldVar, newVar);
    }

    return 0;
}

ARK_SHARE_PTR<AFIEntity> AFCKernelModule::GetEntity(const AFGUID& ident)
{
    return GetElement(ident);
}

bool AFCKernelModule::IsContainer(const AFGUID& self)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(self);
    if(nullptr != pEntity)
    {
        return (pEntity->GetNodeInt("GroupID") < 0);
    }

    ARK_LOG_ERROR("Cannot find entity, id = {}", self.ToString());
    return false;
}

int AFCKernelModule::GetEntityByDataNode(const int nSceneID, const std::string& name, const AFIDataList& valueArg, AFIDataList& list)
{
    AFCDataList varObjectList;
    GetSceneOnLineList(nSceneID, varObjectList);
    int nWorldCount = varObjectList.GetCount();

    for(int i = 0; i < nWorldCount; i++)
    {
        AFGUID ident = varObjectList.Object(i);

        ARK_SHARE_PTR<AFIEntity> pEntity = GetEntity(ident);
        if(nullptr == pEntity)
        {
            continue;
        }

        ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager = pEntity->GetNodeManager();
        if(pNodeManager)
        {
            continue;
        }

        AFDataNode* pDataNode = pNodeManager->GetNode(name.c_str());
        if(nullptr != pDataNode && list.Equal(0, pDataNode->value))
        {
            list.AddObject(ident);
        }
    }

    return list.GetCount();
}

bool AFCKernelModule::ExistContainer(const int nSceneID)
{
    ARK_SHARE_PTR<AFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    return (nullptr != pSceneInfo);
}

bool AFCKernelModule::DestroySelf(const AFGUID& self)
{
    mtDeleteSelfList.push_back(self);
    return true;
}

int AFCKernelModule::OnCommonTableEvent(const AFGUID& self, const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar)
{
    if(IsContainer(self))
    {
        return 1;
    }

    std::list<DATA_TABLE_EVENT_FUNCTOR_PTR>::iterator it = mxCommonTableCBList.begin();
    for(auto it : mxCommonTableCBList)
    {
        (*it)(self, xEventData, oldVar, newVar);
    }

    return 0;
}

int AFCKernelModule::OnCommonClassEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var)
{
    if(IsContainer(self))
    {
        return 1;
    }

    for(auto it : mxCommonClassCBList)
    {
        (*it)(self, strClassName, eClassEvent, var);
    }

    return 0;
}

bool AFCKernelModule::RegCommonClassEvent(const CLASS_EVENT_FUNCTOR_PTR& cb)
{
    mxCommonClassCBList.push_back(cb);
    return true;
}

bool AFCKernelModule::RegCommonDataNodeEvent(const DATA_NODE_EVENT_FUNCTOR_PTR& cb)
{
    mxCommonNodeCBList.push_back(cb);
    return true;
}

bool AFCKernelModule::RegCommonDataTableEvent(const DATA_TABLE_EVENT_FUNCTOR_PTR& cb)
{
    mxCommonTableCBList.push_back(cb);
    return true;
}

bool AFCKernelModule::LogSelfInfo(const AFGUID& ident)
{
    return false;
}

bool AFCKernelModule::PostInit()
{
    ARK_SHARE_PTR<AFIClass> pClass = m_pClassModule->First();
    while(nullptr != pClass)
    {
        AFIKernelModule::AddClassCallBack(pClass->GetClassName(), this, &AFCKernelModule::OnCommonClassEvent);
        pClass = m_pClassModule->Next();
    }

    return true;
}

bool AFCKernelModule::DestroyAll()
{
    ARK_SHARE_PTR<AFIEntity> pEntity = First();
    while(nullptr != pEntity)
    {
        mtDeleteSelfList.push_back(pEntity->Self());

        pEntity = Next();
    }

    //run another frame
    Update();

    m_pSceneModule->ClearAll();

    mvRandom.clear();
    mxCommonClassCBList.clear();
    mxCommonNodeCBList.clear();
    mxCommonTableCBList.clear();

    return true;
}

bool AFCKernelModule::PreShut()
{
    return DestroyAll();
}

void AFCKernelModule::Random(int nStart, int nEnd, int nCount, AFIDataList& valueList)
{
    if(mnRandomPos + nCount >= mvRandom.size())
    {
        mnRandomPos = 0;
    }

    for(int i = mnRandomPos; i < mnRandomPos + nCount; i++)
    {
        float fRanValue = mvRandom.at(i);
        int32_t nValue = int32_t((nEnd - nStart) * fRanValue + nStart);
        valueList.AddInt(nValue);
    }

    mnRandomPos += nCount;
}

bool AFCKernelModule::AddEventCallBack(const AFGUID& self, const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetEventManager()->AddEventCallBack(nEventID, cb);
    }

    return false;
}

bool AFCKernelModule::AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb)
{
    return m_pClassModule->AddClassCallBack(strClassName, cb);
}

bool AFCKernelModule::DoEvent(const AFGUID& self, const std::string& strClassName, ARK_ENTITY_EVENT eEvent, const AFIDataList& valueList)
{
    return m_pClassModule->DoEvent(self, strClassName, eEvent, valueList);
}

bool AFCKernelModule::DoEvent(const AFGUID& self, const int nEventID, const AFIDataList& valueList)
{
    ARK_SHARE_PTR<AFIEntity> pEntity = GetElement(self);
    if(nullptr != pEntity)
    {
        return pEntity->GetEventManager()->DoEvent(nEventID, valueList);
    }

    return false;
}