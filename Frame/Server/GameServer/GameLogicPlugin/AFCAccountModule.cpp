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

    //m_pEventProcessModule->AddEventCallBack(0, AFED_ON_DATABASE_SERVER_LOADROE_BEGIN, OnLoadRoleBeginEvent);
    //m_pEventProcessModule->AddEventCallBack(0, AFED_ON_DATABASE_SERVER_LOADROE_FINAL_RESULTS, OnLoadRoleFinalEvent);
    //m_pEventProcessModule->AddEventCallBack(0, AFED_ON_CLIENT_CREATEROLE, OnCreateRoleEvent);
    //m_pEventProcessModule->AddEventCallBack(0, AFED_ON_CLIENT_DELETEROLE, OnDeleteRoleEvent);
    //m_pEventProcessModule->AddEventCallBack(0, AFED_ON_CLIENT_LEAVE_GAME, OnAcountDisConnectEvent);

    return true;
}

bool AFCAccountModule::Shut()
{
    return true;
}

bool AFCAccountModule::Update()
{
    return true;
}

int AFCAccountModule::OnLoadRoleFinalEvent(const AFGUID& object, const int nEventID, const AFIDataList& var)
{
    //if(9 != var.GetCount())
    //{
    //    return -1;
    //}

    ////赋予属性
    //const char* pstrAccount = var.StringVal(0);
    //AFCValueList valueInfo;
    //valueInfo << pstrAccount;

    //int nCount = 0;
    //for(int i = 1; i <= 7; i += 2)
    //{
    //    const char* pstrRoleName = var.StringVal(i);
    //    if(strlen(pstrRoleName) > 0)
    //    {
    //        //看容器中是否已经存在，存在则不创建
    //        AFCValueList varHallObjectList;
    //        AFCValueList varHalvalueInfo;
    //        varHalvalueInfo << pstrRoleName;
    //        int nHallObjectCount = m_pKernelModule->GetObjectByProperty(mnRoleHallContainer, "RoleName", varHalvalueInfo, varHallObjectList);
    //        if(nHallObjectCount > 0)
    //        {

    //            for(int j = 0; j < varHallObjectList.GetCount(); j++)
    //            {
    //                m_pKernelModule->LogErrorObject(varHallObjectList.ObjectVal(j));
    //                m_pKernelModule->DestroyObject(varHallObjectList.ObjectVal(j));
    //            }
    //        }

    //        GTProperty* gtPproperty = m_pElementInfoModule->GetPropertyManager("Scene0")->GetElement("RelivePos");
    //        const char* pstrRelivePos = gtPproperty->QueryString();
    //        AFCValueList valueRelivePos(pstrRelivePos, ",");

    //        char szConfigIindex[MAX_PATH] = { 0 };
    //        sprintf(szConfigIindex, "%d", var.IntVal(i + 1));

    //        AFCValueList arg;
    //        arg << "Account" << pstrAccount;
    //        arg << "RoleName" << pstrRoleName;
    //        arg << "SceneID" << mnRoleHallContainer;
    //        arg << "X" << atof(valueRelivePos.StringVal(0));
    //        arg << "Y" << atof(valueRelivePos.StringVal(1));
    //        arg << "Z" << atof(valueRelivePos.StringVal(2));
    //        m_pKernelModule->CreateObject(0, mnRoleHallContainer, 0, "Player", szConfigIindex, arg);

    //        nCount++;
    //    }
    //}

    //char szInfo[MAX_PATH] = { 0 };
    //sprintf(szInfo, "Load data final, %s: have %d role.", pstrAccount, nCount);
    //m_pKernelModule->LogInfo(szInfo);

    return 0;
}

bool AFCAccountModule::PostInit()
{
    //m_pKernelModule->CreateContainer(mnRoleHallContainer, "");

    //m_pKernelModule->LogInfo(" -3 RoleHallContainer ");

    return true;
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
    //if(6 != var.GetCount())
    //{
    //    return 0;
    //}

    ////如果有4个玩家则不让创建
    //const char* pstrAccountName = var.StringVal(0);
    //const char* pstrRoleName = var.StringVal(1);
    //int nRoleSex = var.IntVal(2);
    //int nRoleJob = var.IntVal(3);
    //int nRoleRace = var.IntVal(4);
    //int nRoleCamp = var.IntVal(5);

    //AFCValueList roleLlist;
    //if(m_pNoSqlModule->QueryAccountRoleList(pstrAccountName, roleLlist) >= 4)
    //    //if (m_pDataBaseModule->QueryAccountRoleList(pstrAccountName, roleLlist) >= 4)
    //{
    //    return 0;
    //}
    ////////////////////////////////////////////////////////////////////////////
    ////创建人物直接走了数据库
    ////////////////////////////////////////////////////////////////////////////
    //if(m_pNoSqlModule->CreateRole(pstrAccountName, pstrRoleName) <= 0)
    //    //if (m_pDataBaseModule->CreateRole(pstrAccountName, pstrRoleName) <= 0)
    //{
    //    return 0;
    //}

    //AFCValueList varPropertyKeyList;
    //AFCValueList varPropertyValueList;

    //varPropertyKeyList << "RoleName"
    //                   << "Sex"
    //                   << "Job"
    //                   << "Race"
    //                   << "Camp"
    //                   << "SceneID"
    //                   << "LastContainerID"
    //                   << "ClassName";

    //varPropertyValueList << pstrRoleName
    //                     << nRoleSex
    //                     << nRoleJob
    //                     << nRoleRace
    //                     << nRoleCamp
    //                     << mnRoleHallContainer
    //                     << 1
    //                     << "Player";

    //int nConfigName = nRoleJob + nRoleSex * 10;
    //char szConfigName[MAX_PATH] = { 0 };
    //sprintf(szConfigName, "%d", nConfigName);

    ////弄到所有的属性
    //GTPropertyManager* pConfigPropertyManager = m_pElementInfoModule->GetPropertyManager(szConfigName);
    //if(pConfigPropertyManager)
    //{
    //    GTProperty* pConfigPropertyInfo = pConfigPropertyManager->First();
    //    while(pConfigPropertyInfo)
    //    {
    //        //看属性是否需要保存,不需要保存的就别村
    //        if(!pConfigPropertyInfo->GetSave())
    //        {
    //            pConfigPropertyInfo = pConfigPropertyManager->Next();
    //            continue;
    //        }
    //        //这个属性RoleName是玩家的，因此，这里不能有RoleName
    //        const AFIDataList& valueList = pConfigPropertyInfo->GetValue();


    //        if(0 != strcmp(pConfigPropertyInfo->GetKey(), "RoleName")
    //                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "Sex")
    //                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "Job")
    //                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "Race")
    //                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "Camp")
    //                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "Account")
    //                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "SceneID")
    //                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "LastContainerID")
    //                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "ClassName"))
    //        {
    //            varPropertyKeyList << pConfigPropertyInfo->GetKey();
    //            varPropertyValueList.Append(valueList, 0, 1);
    //        }

    //        pConfigPropertyInfo = pConfigPropertyManager->Next();
    //    }
    //}
    //varPropertyKeyList << "RoleName";
    //varPropertyValueList << pstrRoleName;
    //varPropertyKeyList << "Sex";
    //varPropertyValueList << nRoleSex;
    //varPropertyKeyList << "Job";
    //varPropertyValueList << nRoleJob;
    //varPropertyKeyList << "Race";
    //varPropertyValueList << nRoleRace;
    //varPropertyKeyList << "Camp";
    //varPropertyValueList << nRoleCamp;
    //varPropertyKeyList << "SceneID";
    //varPropertyValueList << mnRoleHallContainer;
    //varPropertyKeyList << "LastContainerID";
    //varPropertyValueList << 1;//1号场景为新手村
    //varPropertyKeyList << "ClassName";
    //varPropertyValueList << "Player";
    //varPropertyKeyList << "Level";
    //varPropertyValueList << 1;

    //m_pNoSqlModule->SetRoleProperty(pstrRoleName, varPropertyKeyList, varPropertyValueList);

    ////////////////////////////////////////////////////////////////////////////
    //GTProperty* gtPproperty = m_pElementInfoModule->GetPropertyManager("Scene0")->GetElement("RelivePos");
    //const char* pstrRelivePos = gtPproperty->QueryString();
    //AFCValueList valueRelivePos(pstrRelivePos, ",");

    //AFCValueList arg;
    //arg << "Account" << pstrAccountName;
    //arg << "RoleName" << pstrRoleName;
    //arg << "SceneID" << mnRoleHallContainer;
    //arg << "X" << atof(valueRelivePos.StringVal(0));
    //arg << "Y" << atof(valueRelivePos.StringVal(1));
    //arg << "Z" << atof(valueRelivePos.StringVal(2));
    //m_pKernelModule->CreateObject(0, mnRoleHallContainer, 0, "Player", szConfigName, arg);
    ////nosql 则不需要这样了
    ////m_pEventProcessModule->DoEvent(0, AFED_ON_DATABASE_SERVER_LOADROE_BEGIN, AFCValueList() << pstrAccountName);

    return 0;
}

int AFCAccountModule::OnDeleteRoleEvent(const AFGUID& object, const int nEventID, const AFIDataList& var)
{
    //if(2 != var.GetCount())
    //{
    //    return 0;
    //}

    //const char* pstrAccountName = var.StringVal(0);
    //const char* pstrRoleName = var.StringVal(1);

    //AFCValueList valObjctList;
    //m_pKernelModule->GetObjectByProperty(mnRoleHallContainer, "RoleName", AFCValueList() << pstrRoleName, valObjctList);
    //if(valObjctList.GetCount() == 1)
    //{
    //    m_pNoSqlModule->DeleteRole(pstrAccountName, pstrRoleName);
    //    //m_pDataBaseModule->DeleteRole(pstrAccountName, pstrRoleName);
    //    m_pKernelModule->DestroyObject(valObjctList.ObjectVal(0));
    //    //m_pEventProcessModule->DoEvent( 0, AFED_ON_CLIENT_DELETEROLE_RESULTS, AFCValueList() << pstrAccountName << pstrRoleName );
    //}

    return 0;
}

int AFCAccountModule::OnAcountDisConnectEvent(const AFGUID& object, const int nEventID, const AFIDataList& var)
{
    /*   if(var.GetCount() == 3)
       {
           const char* pstrAccount = var.StringVal(0);
           const char* pstrRoleName = var.StringVal(1);
           AFGUID ident = var.ObjectVal(2);

           if(strlen(pstrAccount) > 0)
           {
               if(m_pGameLogicModule->GetGameID() == 1)
               {

                   if(!ident.IsNull())
                   {
                       m_pKernelModule->DestroyObject(ident);
                   }
                   else
                   {
                       AFCValueList varHallObjectList;
                       AFCValueList varHalvalueInfo;
                       varHalvalueInfo << pstrAccount;

                       int nHallObjectCount = m_pKernelModule->GetObjectByProperty(mnRoleHallContainer, "Account", varHalvalueInfo, varHallObjectList);
                       for(int i  = 0; i < varHallObjectList.GetCount(); i++)
                       {
                           AFGUID identHall = varHallObjectList.ObjectVal(i);
                           if(ident.nSerial == m_pGameLogicModule->GetGameID()
                                   && m_pKernelModule->GetObject(identHall))
                           {
                               m_pKernelModule->DestroyObject(identHall);
                           }
                       }
                   }
               }
           }
       }
    */
    return 0;
}

int AFCAccountModule::OnLoadRoleBeginEvent(const AFGUID& object, const int nEventID, const AFIDataList& var)
{
    ////直接从NOSQL数据库拉
    //const char* pstrAccount = var.StringVal(0);
    //AFCValueList roleLlist;
    //if(m_pNoSqlModule->QueryAccountRoleList(pstrAccount, roleLlist) > 0)
    //{
    //    for(int i = 0; i < roleLlist.GetCount(); i++)
    //    {
    //        const char* pstrRoleName = roleLlist.StringVal(i);
    //        if(strlen(pstrRoleName) > 0)
    //        {
    //            //看容器中是否已经存在，存在则不创建
    //            AFCValueList varHallObjectList;
    //            AFCValueList varHalvalueInfo;
    //            varHalvalueInfo << pstrRoleName;
    //            int nHallObjectCount = m_pKernelModule->GetObjectByProperty(mnRoleHallContainer, "RoleName", varHalvalueInfo, varHallObjectList);
    //            if(nHallObjectCount > 0)
    //            {
    //                for(int j = 0; j < varHallObjectList.GetCount(); j++)
    //                {
    //                    m_pKernelModule->LogErrorObject(varHallObjectList.ObjectVal(j));
    //                    m_pKernelModule->DestroyObject(varHallObjectList.ObjectVal(j));
    //                }
    //            }

    //            GTProperty* gtPproperty = m_pElementInfoModule->GetPropertyManager("Scene0")->GetElement("RelivePos");
    //            const char* pstrRelivePos = gtPproperty->QueryString();
    //            AFCValueList valueRelivePos(pstrRelivePos, ",");

    //            char szConfigIindex[MAX_PATH] = { 0 };
    //            sprintf(szConfigIindex, "%d", var.IntVal(i + 1));

    //            AFCValueList arg;
    //            arg << "Account" << pstrAccount;
    //            arg << "RoleName" << pstrRoleName;
    //            arg << "SceneID" << mnRoleHallContainer;
    //            arg << "X" << atof(valueRelivePos.StringVal(0));
    //            arg << "Y" << atof(valueRelivePos.StringVal(1));
    //            arg << "Z" << atof(valueRelivePos.StringVal(2));
    //            m_pKernelModule->CreateObject(0, mnRoleHallContainer, 0, "Player", szConfigIindex, arg);
    //        }
    //    }
    //}

    return 0;
}

