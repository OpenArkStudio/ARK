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

#include "AFCAccountModule.h"
#include "Server/Interface/AFINetServerModule.h"

bool AFCAccountModule::Init()
{
    mnRoleHallContainer = -3;
    mnConnectContainer = -1;

    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pElementInfoModule = pPluginManager->FindModule<AFIElementModule>();
    m_pUUIDModule = pPluginManager->FindModule<AFIGUIDModule>();
    return true;
}

int AFCAccountModule::OnLoadRoleFinalEvent(const AFGUID& object, const int nEventID, const AFIDataList& var)
{
    return 0;
}

bool AFCAccountModule::GetRoleList(const std::string& strAccount, AFMsg::AckRoleLiteInfoList& xAckRoleLiteInfoList)
{

    return true;
}

bool AFCAccountModule::CreateRole(const std::string& strAccount, AFMsg::AckRoleLiteInfoList& xAckRoleLiteInfoList, const AFIDataList& varList)
{
    AFMsg::RoleLiteInfo* pData = xAckRoleLiteInfoList.add_char_data();
    pData->mutable_id()->CopyFrom(AFINetModule::GUIDToPB(m_pUUIDModule->CreateGUID()));

    int nCareer = varList.Int(0);
    int sex = varList.Int(1);
    int race = varList.Int(2);
    std::string noob_name = varList.String(3);
    int game_id = varList.Int(4);

    pData->set_career(nCareer);
    pData->set_sex(sex);
    pData->set_race(race);
    pData->set_noob_name(noob_name);
    pData->set_game_id(game_id);
    pData->set_role_level(1);
    pData->set_delete_time(0);
    pData->set_reg_time(0);
    pData->set_last_offline_time(0);
    pData->set_last_offline_ip(0);
    pData->set_view_record("");

    return true;
}

bool AFCAccountModule::DeleteRole(const std::string& strAccount, AFMsg::AckRoleLiteInfoList& xAckRoleLiteInfoList)
{
    return true;
}

int AFCAccountModule::OnCreateRoleEvent(const AFGUID& object, const int nEventID, const AFIDataList& var)
{
    return 0;
}

int AFCAccountModule::OnDeleteRoleEvent(const AFGUID& object, const int nEventID, const AFIDataList& var)
{
    return 0;
}

int AFCAccountModule::OnAcountDisConnectEvent(const AFGUID& object, const int nEventID, const AFIDataList& var)
{
    return 0;
}

int AFCAccountModule::OnLoadRoleBeginEvent(const AFGUID& object, const int nEventID, const AFIDataList& var)
{
    return 0;
}

