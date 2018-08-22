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

#include "SDK/Core/AFDateTime.hpp"
#include "SDK/Proto/ARKDataDefine.hpp"
#include "Server/Interface/AFEventDefine.h"
#include "AFCSceneProcessModule.h"

bool AFCSceneProcessModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();
    m_pGameServerNet_ServerModule = pPluginManager->FindModule<AFIGameNetServerModule>();

    m_pKernelModule->AddClassCallBack(ARK::Player::ThisName(), this, &AFCSceneProcessModule::OnObjectClassEvent);
    //////////////////////////////////////////////////////////////////////////

    //Init scene container
    ARK_SHARE_PTR<AFIClass> pLogicClass =  m_pClassModule->GetElement("Scene");

    if (nullptr == pLogicClass)
    {
        return false;
    }

    AFList<std::string>& list = pLogicClass->GetConfigNameList();
    std::string strData;
    for (bool bRet = list.First(strData); bRet; bRet = list.Next(strData))
    {
        int nSceneID = ARK_LEXICAL_CAST<int>(strData);

        if (!LoadSceneResource(nSceneID))
        {
            return false;
        }

        m_pKernelModule->CreateScene(nSceneID);
    }

    return true;
}

bool AFCSceneProcessModule::CreateSceneObject(const int nSceneID, const int nGroupID)
{
    ARK_SHARE_PTR<AFMapEx<std::string, SceneSeedResource>> pSceneResource = mtSceneResourceConfig.GetElement(nSceneID);

    if (nullptr == pSceneResource)
    {
        return false;
    }

    for (ARK_SHARE_PTR<SceneSeedResource> pResource = pSceneResource->First(); nullptr != pResource; pResource = pSceneResource->Next())
    {
        const std::string strClassName(m_pConfigModule->GetNodeString(pResource->strConfigID, ARK::NPC::ClassName()));

        AFCDataList arg;
        arg << ARK::NPC::X() << pResource->fSeedX;
        arg << ARK::NPC::Y() << pResource->fSeedY;
        arg << ARK::NPC::Z() << pResource->fSeedZ;
        arg << ARK::NPC::SeedID() << pResource->strSeedID;

        m_pKernelModule->CreateEntity(AFGUID(nSceneID, nGroupID), nSceneID, nGroupID, strClassName, pResource->strConfigID, arg);
    }

    return true;
}

int AFCSceneProcessModule::CreateCloneScene(const int& nSceneID)
{
    const E_SCENE_TYPE eType = GetCloneSceneType(nSceneID);
    int nTargetGroupID = m_pKernelModule->RequestGroupScene(nSceneID);

    if (nTargetGroupID > 0 && eType == SCENE_TYPE_CLONE_SCENE && !CreateSceneObject(nSceneID, nTargetGroupID))
    {
        return -1;
    }

    return nTargetGroupID;
}

int AFCSceneProcessModule::OnEnterSceneEvent(const AFGUID& self, const int nEventID, const AFIDataList& var)
{
    if (var.GetCount() != 4
            || !var.TypeEx(AF_DATA_TYPE::DT_OBJECT, AF_DATA_TYPE::DT_INT,
                           AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_UNKNOWN))
    {
        return 0;
    }

    const AFGUID ident = var.Object(0);
    const int nTargetScene = var.Int(2);
    const int nTargetGroupID = var.Int(3);
    const int nNowSceneID = m_pKernelModule->GetNodeInt(self, ARK::Player::SceneID());
    const int nNowGroupID = m_pKernelModule->GetNodeInt(self, ARK::Player::GroupID());

    if (self != ident)
    {
        ARK_LOG_ERROR("you are not you self, but you want to entry this scene, id = {} scene_id = {}", ident.ToString(), nTargetScene);
        return 1;
    }

    if (nNowSceneID == nTargetScene && nTargetGroupID == nNowGroupID)
    {
        ARK_LOG_ERROR("In same scene and group but it not a clone scene, id = {} scene_id = {}", ident.ToString(), nTargetScene);
        return 1;
    }

    //每个玩家，一个副本
    int64_t nNewGroupID = 0;

    if (nTargetGroupID <= 0)
    {
        nNewGroupID = CreateCloneScene(nTargetScene);
    }
    else
    {
        nNewGroupID = nTargetGroupID;
    }

    if (nNewGroupID <= 0)
    {
        ARK_LOG_ERROR("CreateCloneScene failed, id = {} scene_id  = {}=", ident.ToString(), nTargetScene);
        return 0;
    }

    //得到坐标
    Point3D xRelivePos;
    const std::string strSceneID = ARK_LEXICAL_CAST<std::string>(nTargetScene);
    const std::string& strRelivePosList = m_pConfigModule->GetNodeString(strSceneID, ARK::Scene::RelivePos());

    AFCDataList valueRelivePosList(strRelivePosList.c_str(), strRelivePosList.length(), ';');

    if (valueRelivePosList.GetCount() >= 1)
    {
        xRelivePos.FromString(valueRelivePosList.String(0));
    }

    AFCDataList xSceneResult(var);
    xSceneResult.AddFloat(xRelivePos.x);
    xSceneResult.AddFloat(xRelivePos.y);
    xSceneResult.AddFloat(xRelivePos.z);

    m_pKernelModule->DoEvent(self, AFED_ON_OBJECT_ENTER_SCENE_BEFORE, xSceneResult);

    if (!m_pKernelModule->SwitchScene(self, nTargetScene, nNewGroupID, xRelivePos, 0.0F, var))
    {
        ARK_LOG_ERROR("SwitchScene failed, id = {} scene_id = {}", ident.ToString(), nTargetScene);
        return 0;
    }

    xSceneResult.AddInt(nNewGroupID);
    m_pKernelModule->DoEvent(self, AFED_ON_OBJECT_ENTER_SCENE_RESULT, xSceneResult);

    return 0;
}

int AFCSceneProcessModule::OnLeaveSceneEvent(const AFGUID& object, const int nEventID, const AFIDataList& var)
{
    if (1 != var.GetCount()
            || !var.TypeEx(AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_UNKNOWN))
    {
        return -1;
    }

    int32_t nOldGroupID = var.Int(0);

    if (nOldGroupID > 0)
    {
        int nSceneID = m_pKernelModule->GetNodeInt(object, ARK::Player::SceneID());

        if (GetCloneSceneType(nSceneID) == SCENE_TYPE_CLONE_SCENE)
        {
            m_pKernelModule->ReleaseGroupScene(nSceneID, nOldGroupID);
            ARK_LOG_ERROR("DestroyCloneSceneGroup, id = {} scene_id = {} group_id = {}", object.ToString(), nSceneID, nOldGroupID);
        }
    }

    return 0;
}

int AFCSceneProcessModule::OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var)
{
    if (strClassName == ARK::Player::ThisName())
    {
        if (ARK_ENTITY_EVENT::ENTITY_EVT_DESTROY == eClassEvent)
        {
            //如果在副本中,则删除他的那个副本
            int nSceneID = m_pKernelModule->GetNodeInt(self, ARK::Player::SceneID());

            if (GetCloneSceneType(nSceneID) == SCENE_TYPE_CLONE_SCENE)
            {
                int nGroupID = m_pKernelModule->GetNodeInt(self, ARK::Player::GroupID());
                m_pKernelModule->ReleaseGroupScene(nSceneID, nGroupID);
                ARK_LOG_INFO("DestroyCloneSceneGroup, id  = {} scene_id  = {} group_id = {}", self.ToString(), nSceneID, nGroupID);
            }
        }
        else if (ARK_ENTITY_EVENT::ENTITY_EVT_DATA_FINISHED == eClassEvent)
        {
            m_pKernelModule->AddEventCallBack(self, AFED_ON_CLIENT_ENTER_SCENE, this, &AFCSceneProcessModule::OnEnterSceneEvent);
            m_pKernelModule->AddEventCallBack(self, AFED_ON_CLIENT_LEAVE_SCENE, this, &AFCSceneProcessModule::OnLeaveSceneEvent);
        }
    }

    return 0;
}

E_SCENE_TYPE AFCSceneProcessModule::GetCloneSceneType(const int nSceneID)
{
    std::string scene_id = ARK_TO_STRING(nSceneID);
    if (m_pConfigModule->ExistConfig(scene_id))
    {
        return (E_SCENE_TYPE)m_pConfigModule->GetNodeInt(scene_id, ARK::Scene::CanClone());
    }

    return SCENE_TYPE_ERROR;
}

bool AFCSceneProcessModule::IsCloneScene(const int nSceneID)
{
    return GetCloneSceneType(nSceneID) == SCENE_TYPE_CLONE_SCENE;
}

bool AFCSceneProcessModule::ApplyCloneGroup(const int nSceneID, int& nGroupID)
{
    nGroupID = CreateCloneScene(nSceneID);

    return true;
}

bool AFCSceneProcessModule::ExitCloneGroup(const int nSceneID, const int& nGroupID)
{
    return m_pKernelModule->ExitGroupScene(nSceneID, nGroupID);
}

bool AFCSceneProcessModule::LoadSceneResource(const int nSceneID)
{
    std::string sceneIDName = ARK_TO_STRING(nSceneID);
    const std::string strSceneFilePath(m_pConfigModule->GetNodeString(sceneIDName, ARK::Scene::FilePath()));

    //场景对应资源
    ARK_SHARE_PTR<AFMapEx<std::string, SceneSeedResource>> pSceneResourceMap = mtSceneResourceConfig.GetElement(nSceneID);

    if (nullptr == pSceneResourceMap)
    {
        pSceneResourceMap = std::make_shared<AFMapEx<std::string, SceneSeedResource>>();
        mtSceneResourceConfig.AddElement(nSceneID, pSceneResourceMap);
    }

    if (strSceneFilePath.empty())
    {
        return false;
    }

    rapidxml::file<> xFileSource(strSceneFilePath.c_str());
    rapidxml::xml_document<> xFileDoc;
    xFileDoc.parse<0>(xFileSource.data());

    //资源文件列表
    rapidxml::xml_node<>* pSeedFileRoot = xFileDoc.first_node();

    for (rapidxml::xml_node<>* pSeedFileNode = pSeedFileRoot->first_node(); pSeedFileNode; pSeedFileNode = pSeedFileNode->next_sibling())
    {
        //种子具体信息
        std::string strSeedID = pSeedFileNode->first_attribute("ID")->value();
        std::string strConfigID = pSeedFileNode->first_attribute("NPCConfigID")->value();
        float fSeedX = ARK_LEXICAL_CAST<float>(pSeedFileNode->first_attribute("SeedX")->value());
        float fSeedY = ARK_LEXICAL_CAST<float>(pSeedFileNode->first_attribute("SeedY")->value());
        float fSeedZ = ARK_LEXICAL_CAST<float>(pSeedFileNode->first_attribute("SeedZ")->value());

        if (!m_pConfigModule->ExistConfig(strConfigID))
        {
            ARK_ASSERT_NO_EFFECT(0);
        }

        ARK_SHARE_PTR<SceneSeedResource> pSeedResource = pSceneResourceMap->GetElement(strSeedID);

        if (nullptr == pSeedResource)
        {
            pSeedResource = std::make_shared<SceneSeedResource>();
            pSceneResourceMap->AddElement(strSeedID, pSeedResource);
        }

        pSeedResource->strSeedID = strSeedID;
        pSeedResource->strConfigID = strConfigID;
        pSeedResource->fSeedX = fSeedX;
        pSeedResource->fSeedY = fSeedY;
        pSeedResource->fSeedZ = fSeedZ;

    }

    return true;
}

void AFCSceneProcessModule::OnClientSwapSceneProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
{
    //Will add by yourself
}