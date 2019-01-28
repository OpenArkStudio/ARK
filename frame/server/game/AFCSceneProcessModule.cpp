/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include "base/AFEventDefine.hpp"
#include "base/AFDateTime.hpp"
#include "base/AFDataDefine.hpp"
#include "AFCSceneProcessModule.h"

namespace ark
{

    bool AFCSceneProcessModule::Init()
    {
        m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
        m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
        m_pClassModule = pPluginManager->FindModule<AFIMetaClassModule>();
        m_pMapModule = pPluginManager->FindModule<AFIMapModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();
        m_pGameNetModule = pPluginManager->FindModule<AFIGameNetModule>();
        m_pGUIDModule = pPluginManager->FindModule<AFIGUIDModule>();

        m_pKernelModule->AddClassCallBack(ark::Player::ThisName(), this, &AFCSceneProcessModule::OnObjectClassEvent);

        return true;
    }

    bool AFCSceneProcessModule::PostInit()
    {
        //Init scene container
        ARK_SHARE_PTR<AFIMetaClass> pLogicClass = m_pClassModule->GetMetaClass("Scene");
        if (nullptr == pLogicClass)
        {
            return false;
        }

        AFList<std::string>& list = pLogicClass->GetConfigNameList();
        for (auto iter : list)
        {
            int map_id = ARK_LEXICAL_CAST<int>(iter);
            if (!LoadSceneResource(map_id))
            {
                return false;
            }

            m_pMapModule->CreateMap(map_id);
        }

        return true;
    }

    bool AFCSceneProcessModule::CreateMapEntities(const int map_id, const int inst_id)
    {
        auto pMapRes = map_res_.find_value(map_id);
        if (pMapRes == nullptr)
        {
            return false;
        }

        for (auto iter : *pMapRes)
        {
            auto pResource = iter.second;
            const std::string class_name = m_pConfigModule->GetNodeString(pResource->strConfigID, ark::NPC::ClassName());

            AFCDataList arg;
            arg << ark::NPC::X() << pResource->fSeedX;
            arg << ark::NPC::Y() << pResource->fSeedY;
            arg << ark::NPC::Z() << pResource->fSeedZ;
            arg << ark::NPC::SeedID() << pResource->strSeedID;

            AFGUID new_guid = m_pGUIDModule->CreateGUID();
            m_pKernelModule->CreateEntity(new_guid, map_id, inst_id, class_name, pResource->strConfigID, arg);
        }

        return true;
    }

    int AFCSceneProcessModule::CreateCloneScene(const int& nSceneID)
    {
        const E_SCENE_TYPE eType = GetCloneSceneType(nSceneID);
        int nTargetGroupID = m_pMapModule->CreateMapInstance(nSceneID);

        if (nTargetGroupID > 0 && eType == SCENE_TYPE_CLONE_SCENE && !CreateMapEntities(nSceneID, nTargetGroupID))
        {
            return -1;
        }

        return nTargetGroupID;
    }

    int AFCSceneProcessModule::OnEnterSceneEvent(const AFGUID& self, const int nEventID, const AFIDataList& var)
    {
        if (var.GetCount() != 4
                || !var.TypeEx(AF_DATA_TYPE::DT_INT64, AF_DATA_TYPE::DT_INT,
                               AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_UNKNOWN))
        {
            return 0;
        }

        const AFGUID ident = var.Int64(0);
        const int target_map = var.Int(2);
        const int target_inst = var.Int(3);
        const int cur_map = m_pKernelModule->GetNodeInt(self, ark::Player::MapID());
        const int cur_inst = m_pKernelModule->GetNodeInt(self, ark::Player::InstanceID());

        if (self != ident)
        {
            ARK_LOG_ERROR("you are not you self, but you want to entry this scene, id = {} scene_id = {}", ident, target_map);
            return 1;
        }

        if (cur_map == target_map && target_inst == cur_inst)
        {
            ARK_LOG_ERROR("In same scene and group but it not a clone scene, id = {} scene_id = {}", ident, target_map);
            return 1;
        }

        //每个玩家，一个副本
        int new_inst = 0;

        if (target_inst <= 0)
        {
            new_inst = CreateCloneScene(target_map);
        }
        else
        {
            new_inst = target_inst;
        }

        if (new_inst <= 0)
        {
            ARK_LOG_ERROR("CreateCloneScene failed, id = {} scene_id  = {}=", ident, target_map);
            return 0;
        }

        //得到坐标
        Point3D xRelivePos;
        const std::string strSceneID = ARK_LEXICAL_CAST<std::string>(target_map);
        const std::string& strRelivePosList = m_pConfigModule->GetNodeString(strSceneID, ark::Scene::RelivePos());

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

        if (!m_pMapModule->SwitchMap(self, target_map, new_inst, xRelivePos, 0.0f, var))
        {
            ARK_LOG_ERROR("SwitchScene failed, id = {} scene_id = {}", ident, target_map);
            return 0;
        }

        xSceneResult.AddInt(new_inst);
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
            int nSceneID = m_pKernelModule->GetNodeInt(object, ark::Player::MapID());

            if (GetCloneSceneType(nSceneID) == SCENE_TYPE_CLONE_SCENE)
            {
                m_pMapModule->ReleaseMapInstance(nSceneID, nOldGroupID);
                ARK_LOG_ERROR("DestroyCloneSceneGroup, id = {} scene_id = {} group_id = {}", object, nSceneID, nOldGroupID);
            }
        }

        return 0;
    }

    int AFCSceneProcessModule::OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& var)
    {
        if (strClassName == ark::Player::ThisName())
        {
            if (ARK_ENTITY_EVENT::ENTITY_EVT_DESTROY == eClassEvent)
            {
                //如果在副本中,则删除他的那个副本
                int nSceneID = m_pKernelModule->GetNodeInt(self, ark::Player::MapID());

                if (GetCloneSceneType(nSceneID) == SCENE_TYPE_CLONE_SCENE)
                {
                    int nGroupID = m_pKernelModule->GetNodeInt(self, ark::Player::InstanceID());
                    m_pMapModule->ReleaseMapInstance(nSceneID, nGroupID);
                    ARK_LOG_INFO("DestroyCloneSceneGroup, id  = {} scene_id  = {} group_id = {}", self, nSceneID, nGroupID);
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
            return (E_SCENE_TYPE)m_pConfigModule->GetNodeInt(scene_id, ark::Scene::CanClone());
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
        return m_pMapModule->ExitMapInstance(nSceneID, nGroupID);
    }

    bool AFCSceneProcessModule::LoadSceneResource(const int nSceneID)
    {
        std::string sceneIDName = ARK_TO_STRING(nSceneID);
        const std::string strSceneFilePath(m_pConfigModule->GetNodeString(sceneIDName, ark::Scene::FilePath()));

        //场景对应资源
        ARK_SHARE_PTR<AFMapEx<std::string, SceneSeedResource>> pSceneResourceMap = map_res_.find_value(nSceneID);

        if (pSceneResourceMap == nullptr)
        {
            pSceneResourceMap = std::make_shared<AFMapEx<std::string, SceneSeedResource>>();
            map_res_.insert(nSceneID, pSceneResourceMap);
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

            ARK_SHARE_PTR<SceneSeedResource> pSeedResource = pSceneResourceMap->find_value(strSeedID);

            if (nullptr == pSeedResource)
            {
                pSeedResource = std::make_shared<SceneSeedResource>();
                pSceneResourceMap->insert(strSeedID, pSeedResource);
            }

            pSeedResource->strSeedID = strSeedID;
            pSeedResource->strConfigID = strConfigID;
            pSeedResource->fSeedX = fSeedX;
            pSeedResource->fSeedY = fSeedY;
            pSeedResource->fSeedZ = fSeedZ;

        }

        return true;
    }

    void AFCSceneProcessModule::OnClientSwapSceneProcess(const AFNetMsg* msg)
    {
        //Will add by yourself
    }

}