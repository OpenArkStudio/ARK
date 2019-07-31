/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "base/AFEventDefine.hpp"
#include "base/AFDateTime.hpp"
#include "interface/AFIPluginManager.hpp"
#include "game/include/AFCSceneProcessModule.hpp"

namespace ark {

bool AFCSceneProcessModule::Init()
{
    m_pKernelModule = FindModule<AFIKernelModule>();
    m_pConfigModule = FindModule<AFIConfigModule>();
    m_pClassModule = FindModule<AFIMetaClassModule>();
    m_pMapModule = FindModule<AFIMapModule>();
    m_pLogModule = FindModule<AFILogModule>();
    m_pGameNetModule = FindModule<AFIGameNetModule>();
    m_pGUIDModule = FindModule<AFIGUIDModule>();

    m_pKernelModule->AddClassCallBack(
        AFEntityMetaPlayer::self_name(), this, &AFCSceneProcessModule::OnObjectClassEvent);

    return true;
}

bool AFCSceneProcessModule::PostInit()
{
    // TOOD:Will use the new design

    ////Init scene container
    // ARK_SHARE_PTR<AFIMetaClass> pLogicClass = m_pClassModule->GetMetaClass("Scene");
    // if (nullptr == pLogicClass)
    //{
    //    return false;
    //}

    // AFList<std::string>& list = pLogicClass->GetConfigNameList();
    // for (auto iter : list)
    //{
    //    int map_id = ARK_LEXICAL_CAST<int>(iter);
    //    if (!LoadMapResource(map_id))
    //    {
    //        return false;
    //    }

    //    m_pMapModule->CreateMap(map_id);
    //}

    return true;
}

bool AFCSceneProcessModule::CreateMapEntities(const int map_id, const int inst_id)
{
    // auto pMapRes = map_res_.find_value(map_id);
    // if (pMapRes == nullptr)
    //{
    //    return false;
    //}

    // for (auto iter : *pMapRes)
    //{
    //    auto pResource = iter.second;
    //    const std::string class_name = m_pConfigModule->GetNodeString(pResource->strConfigID,
    //    AFConfigMetaNpc::self_name());

    //    AFCDataList arg;
    //    arg << ark::NPC::X() << pResource->fSeedX;
    //    arg << ark::NPC::Y() << pResource->fSeedY;
    //    arg << ark::NPC::Z() << pResource->fSeedZ;
    //    arg << ark::NPC::SeedID() << pResource->strSeedID;

    //    AFGUID new_guid = m_pGUIDModule->CreateGUID();
    //    m_pKernelModule->CreateEntity(new_guid, map_id, inst_id, class_name, pResource->strConfigID, arg);
    //}

    return true;
}

int AFCSceneProcessModule::CreateMapInstance(const int& map_id)
{
    const E_SCENE_TYPE eType = GetMapInstanceType(map_id);
    int target_inst_id = m_pMapModule->CreateMapInstance(map_id);

    if (target_inst_id > 0 && eType == SCENE_TYPE_CLONE_SCENE && !CreateMapEntities(map_id, target_inst_id))
    {
        return -1;
    }

    return target_inst_id;
}

int AFCSceneProcessModule::OnEnterSceneEvent(const AFGUID& self, const int nEventID, const AFIDataList& var)
{
    if (var.GetCount() != 4 || !var.TypeEx(AF_DATA_TYPE::DT_INT64, AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_INT,
                                   AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_UNKNOWN))
    {
        return 0;
    }

    const AFGUID ident = var.Int64(0);
    const int target_map = var.Int(2);
    const int target_inst = var.Int(3);
    const int cur_map = m_pKernelModule->GetNodeInt(self, AFEntityMetaPlayer::map_id());
    const int cur_inst = m_pKernelModule->GetNodeInt(self, AFEntityMetaPlayer::map_inst_id());

    if (self != ident)
    {
        ARK_LOG_ERROR(
            "you are not you self, but you want to entry this scene, id = {} scene_id = {}", ident, target_map);
        return 1;
    }

    if (cur_map == target_map && target_inst == cur_inst)
    {
        ARK_LOG_ERROR(
            "In same map and map_instance but it not a clone scene, id = {} scene_id = {}", ident, target_map);
        return 1;
    }

    //每个玩家，一个副本
    int new_inst = 0;

    if (target_inst <= 0)
    {
        new_inst = CreateMapInstance(target_map);
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

    // get the position.
    AFVector3D xRelivePos;
    const std::string strSceneID = ARK_LEXICAL_CAST<std::string>(target_map);
    const std::string& strRelivePosList = m_pConfigModule->GetNodeString(strSceneID, AFConfigMetaMap::RelivePos());

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
    if (1 != var.GetCount() || !var.TypeEx(AF_DATA_TYPE::DT_INT, AF_DATA_TYPE::DT_UNKNOWN))
    {
        return -1;
    }

    int32_t nOldGroupID = var.Int(0);

    if (nOldGroupID > 0)
    {
        int nSceneID = m_pKernelModule->GetNodeInt(object, AFEntityMetaPlayer::map_id());

        if (GetMapInstanceType(nSceneID) == SCENE_TYPE_CLONE_SCENE)
        {
            m_pMapModule->ReleaseMapInstance(nSceneID, nOldGroupID);
            ARK_LOG_ERROR("DestroyCloneSceneGroup, id = {} scene_id = {} group_id = {}", object, nSceneID, nOldGroupID);
        }
    }

    return 0;
}

int AFCSceneProcessModule::OnObjectClassEvent(
    const AFGUID& self, const std::string& strClassName, const ArkEntityEvent eClassEvent, const AFIDataList& var)
{
    if (ArkEntityEvent::ENTITY_EVT_DESTROY == eClassEvent)
    {
        //如果在副本中,则删除他的那个副本
        int map_id = m_pKernelModule->GetNodeInt(self, AFEntityMetaPlayer::map_id());

        if (GetMapInstanceType(map_id) == SCENE_TYPE_CLONE_SCENE)
        {
            int nGroupID = m_pKernelModule->GetNodeInt(self, AFEntityMetaPlayer::map_inst_id());
            m_pMapModule->ReleaseMapInstance(map_id, nGroupID);
            ARK_LOG_INFO("DestroyCloneSceneGroup, id  = {} scene_id  = {} group_id = {}", self, map_id, nGroupID);
        }
    }
    else if (ArkEntityEvent::ENTITY_EVT_DATA_FINISHED == eClassEvent)
    {
        m_pKernelModule->AddEventCallBack(
            self, AFED_ON_CLIENT_ENTER_SCENE, this, &AFCSceneProcessModule::OnEnterSceneEvent);
        m_pKernelModule->AddEventCallBack(
            self, AFED_ON_CLIENT_LEAVE_SCENE, this, &AFCSceneProcessModule::OnLeaveSceneEvent);
    }

    return 0;
}

E_SCENE_TYPE AFCSceneProcessModule::GetMapInstanceType(const int nSceneID)
{
    std::string scene_id = ARK_TO_STRING(nSceneID);
    if (m_pConfigModule->ExistConfig(scene_id))
    {
        return (E_SCENE_TYPE)m_pConfigModule->GetNodeInt(scene_id, AFConfigMetaMap::instance_type());
    }

    return SCENE_TYPE_ERROR;
}

bool AFCSceneProcessModule::IsInstanceMap(const int map_id)
{
    return GetMapInstanceType(map_id) == SCENE_TYPE_CLONE_SCENE;
}

bool AFCSceneProcessModule::ApplyMapInstance(const int map_id, int& map_inst_id)
{
    map_inst_id = CreateMapInstance(map_id);
    return true;
}

bool AFCSceneProcessModule::ExitMapInstance(const int map_id, const int& map_inst_id)
{
    return m_pMapModule->ExitMapInstance(map_id, map_inst_id);
}

bool AFCSceneProcessModule::LoadMapResource(const int map_id)
{
    // const std::string strSceneFilePath(m_pConfigModule->GetNodeString(ARK_LEXICAL_CAST<std::string>(map_id),
    // AFConfigMetaMap::map_info()));

    ////场景对应资源
    // ARK_SHARE_PTR<AFMapEx<std::string, MapSeedResource>> pSceneResourceMap = map_res_.find_value(map_id);

    // if (pSceneResourceMap == nullptr)
    //{
    //    pSceneResourceMap = std::make_shared<AFMapEx<std::string, MapSeedResource>>();
    //    map_res_.insert(map_id, pSceneResourceMap);
    //}

    // if (strSceneFilePath.empty())
    //{
    //    return false;
    //}

    // rapidxml::file<> xFileSource(strSceneFilePath.c_str());
    // rapidxml::xml_document<> xFileDoc;
    // xFileDoc.parse<0>(xFileSource.data());

    ////资源文件列表
    // rapidxml::xml_node<>* pSeedFileRoot = xFileDoc.first_node();

    // for (rapidxml::xml_node<>* pSeedFileNode = pSeedFileRoot->first_node(); pSeedFileNode; pSeedFileNode =
    // pSeedFileNode->next_sibling())
    //{
    //    //种子具体信息
    //    std::string strSeedID = pSeedFileNode->first_attribute("ID")->value();
    //    std::string strConfigID = pSeedFileNode->first_attribute("NPCConfigID")->value();
    //    float fSeedX = ARK_LEXICAL_CAST<float>(pSeedFileNode->first_attribute("SeedX")->value());
    //    float fSeedY = ARK_LEXICAL_CAST<float>(pSeedFileNode->first_attribute("SeedY")->value());
    //    float fSeedZ = ARK_LEXICAL_CAST<float>(pSeedFileNode->first_attribute("SeedZ")->value());

    //    if (!m_pConfigModule->ExistConfig(strConfigID))
    //    {
    //        ARK_ASSERT_NO_EFFECT(0);
    //    }

    //    ARK_SHARE_PTR<MapSeedResource> pSeedResource = pSceneResourceMap->find_value(strSeedID);

    //    if (nullptr == pSeedResource)
    //    {
    //        pSeedResource = std::make_shared<MapSeedResource>();
    //        pSceneResourceMap->insert(strSeedID, pSeedResource);
    //    }

    //    pSeedResource->strSeedID = strSeedID;
    //    pSeedResource->strConfigID = strConfigID;
    //    pSeedResource->fSeedX = fSeedX;
    //    pSeedResource->fSeedY = fSeedY;
    //    pSeedResource->fSeedZ = fSeedZ;
    //}

    return true;
}

void AFCSceneProcessModule::OnClientSwapMapProcess(const AFNetMsg* msg)
{
    // Will add by yourself
}

} // namespace ark
