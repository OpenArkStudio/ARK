// -------------------------------------------------------------------------
//    @FileName         :    AFCCreateRoleModule.cpp
//    @Author               :    Ark Game Tech
//    @Date                 :    2013-03-29
//    @Module               :    AFCCreateRoleModule
//    @Desc                 :
// -------------------------------------------------------------------------

////#include "stdafx.h"
#include "AFCCreateRoleModule.h"

//

int AFCCreateRoleModule::mnConnectContainer = -1;
int AFCCreateRoleModule::mnRoleHallContainer = -3;

AFIGameLogicModule* AFCCreateRoleModule::m_pGameLogicModule = NULL;
AFIKernelModule* AFCCreateRoleModule::m_pKernelModule = NULL;
//AFIDataBaseModule* AFCCreateRoleModule::m_pDataBaseModule = NULL;
AFIDataNoSqlModule* AFCCreateRoleModule::m_pNoSqlModule = NULL;
AFIEventProcessModule* AFCCreateRoleModule::m_pEventProcessModule = NULL;
AFIElementInfoModule* AFCCreateRoleModule::m_pElementInfoModule = NULL;
AFCCreateRoleModule* AFCCreateRoleModule::m_pThis = NULL;

bool AFCCreateRoleModule::Init()
{
    m_pEventProcessModule = dynamic_cast<AFIEventProcessModule*>(pPluginManager->FindModule("AFCEventProcessModule"));
    m_pNoSqlModule = dynamic_cast<AFIDataNoSqlModule*>(pPluginManager->FindModule("AFCDataNoSqlModule"));
    m_pKernelModule = dynamic_cast<AFIKernelModule*>(pPluginManager->FindModule("AFCKernelModule"));
    m_pElementInfoModule = dynamic_cast<AFIElementInfoModule*>(pPluginManager->FindModule("AFCElementInfoModule"));
    m_pGameLogicModule = dynamic_cast<AFIGameLogicModule*>(pPluginManager->FindModule("AFCGameLogicModule"));

    assert(NULL != m_pElementInfoModule);
    assert(NULL != m_pEventProcessModule);
    assert(NULL != m_pNoSqlModule);
    assert(NULL != m_pKernelModule);

    m_pEventProcessModule->AddEventCallBack(0, NFED_ON_DATABASE_SERVER_LOADROE_BEGIN, OnLoadRoleBeginEvent);
    m_pEventProcessModule->AddEventCallBack(0, NFED_ON_DATABASE_SERVER_LOADROE_FINAL_RESULTS, OnLoadRoleFinalEvent);
    m_pEventProcessModule->AddEventCallBack(0, NFED_ON_CLIENT_CREATEROLE, OnCreateRoleEvent);
    m_pEventProcessModule->AddEventCallBack(0, NFED_ON_CLIENT_DELETEROLE, OnDeleteRoleEvent);
    m_pEventProcessModule->AddEventCallBack(0, NFED_ON_CLIENT_LEAVE_GAME, OnAcountDisConnectEvent);

    return true;
}

bool AFCCreateRoleModule::Shut()
{
    return true;
}

bool AFCCreateRoleModule::Execute(const float fLasFrametime, const float fStartedTime)
{
    return true;
}

int AFCCreateRoleModule::OnLoadRoleFinalEvent(const AFIDENTID& object, const int nEventID, const AFIValueList& var)
{
    if (9 != var.GetCount())
    {
        return -1;
    }

    //赋予属性
    const char* pstrAccount = var.StringVal(0);
    AFCValueList valueInfo;
    valueInfo << pstrAccount;

    int nCount = 0;
    for (int i = 1; i <= 7; i += 2)
    {
        const char* pstrRoleName = var.StringVal(i);
        if (strlen(pstrRoleName) > 0)
        {
            //看容器中是否已经存在，存在则不创建
            AFCValueList varHallObjectList;
            AFCValueList varHalvalueInfo;
            varHalvalueInfo << pstrRoleName;
            int nHallObjectCount = m_pKernelModule->GetObjectByProperty(mnRoleHallContainer, "RoleName", varHalvalueInfo, varHallObjectList);
            if (nHallObjectCount > 0)
            {

                for (int j = 0; j < varHallObjectList.GetCount(); j++)
                {
                    m_pKernelModule->LogErrorObject(varHallObjectList.ObjectVal(j));
                    m_pKernelModule->DestroyObject(varHallObjectList.ObjectVal(j));
                }
            }

            GTProperty* gtPproperty = m_pElementInfoModule->GetPropertyManager("Scene0")->GetElement("RelivePos");
            const char* pstrRelivePos = gtPproperty->QueryString();
            AFCValueList valueRelivePos(pstrRelivePos, ",");

            char szConfigIindex[MAX_PATH] = { 0 };
            sprintf(szConfigIindex, "%d", var.IntVal(i + 1));

            AFCValueList arg;
            arg << "Account" << pstrAccount;
            arg << "RoleName" << pstrRoleName;
            arg << "SceneID" << mnRoleHallContainer;
            arg << "X" << atof(valueRelivePos.StringVal(0));
            arg << "Y" << atof(valueRelivePos.StringVal(1));
            arg << "Z" << atof(valueRelivePos.StringVal(2));
            m_pKernelModule->CreateObject(0, mnRoleHallContainer, 0, "Player", szConfigIindex, arg);

            nCount++;
        }
    }

    char szInfo[MAX_PATH] = { 0 };
    sprintf(szInfo, "Load data final, %s: have %d role.", pstrAccount, nCount);
    m_pKernelModule->LogInfo(szInfo);

    return 0;
}

bool AFCCreateRoleModule::AfterInit()
{
    m_pKernelModule->CreateContainer(mnRoleHallContainer, "");

    m_pKernelModule->LogInfo(" -3 RoleHallContainer ");

    return true;
}

int AFCCreateRoleModule::OnCreateRoleEvent(const AFIDENTID& object, const int nEventID, const AFIValueList& var)
{
    if (6 != var.GetCount())
    {
        return 0;
    }

    //如果有4个玩家则不让创建
    const char* pstrAccountName = var.StringVal(0);
    const char* pstrRoleName = var.StringVal(1);
    int nRoleSex = var.IntVal(2);
    int nRoleJob = var.IntVal(3);
    int nRoleRace = var.IntVal(4);
    int nRoleCamp = var.IntVal(5);

    AFCValueList roleLlist;
    if (m_pNoSqlModule->QueryAccountRoleList(pstrAccountName, roleLlist) >= 4)
        //if (m_pDataBaseModule->QueryAccountRoleList(pstrAccountName, roleLlist) >= 4)
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    //创建人物直接走了数据库
    //////////////////////////////////////////////////////////////////////////
    if (m_pNoSqlModule->CreateRole(pstrAccountName, pstrRoleName) <= 0)
        //if (m_pDataBaseModule->CreateRole(pstrAccountName, pstrRoleName) <= 0)
    {
        return 0;
    }

    AFCValueList varPropertyKeyList;
    AFCValueList varPropertyValueList;

    varPropertyKeyList << "RoleName"
                       << "Sex"
                       << "Job"
                       << "Race"
                       << "Camp"
                       << "SceneID"
                       << "LastContainerID"
                       << "ClassName";

    varPropertyValueList << pstrRoleName
                         << nRoleSex
                         << nRoleJob
                         << nRoleRace
                         << nRoleCamp
                         << mnRoleHallContainer
                         << 1
                         << "Player";

    int nConfigName = nRoleJob + nRoleSex * 10;
    char szConfigName[MAX_PATH] = { 0 };
    sprintf(szConfigName, "%d", nConfigName);

    //弄到所有的属性
    GTPropertyManager* pConfigPropertyManager = m_pElementInfoModule->GetPropertyManager(szConfigName);
    if (pConfigPropertyManager)
    {
        GTProperty* pConfigPropertyInfo = pConfigPropertyManager->First();
        while (pConfigPropertyInfo)
        {
            //看属性是否需要保存,不需要保存的就别村
            if (!pConfigPropertyInfo->GetSave())
            {
                pConfigPropertyInfo = pConfigPropertyManager->Next();
                continue;
            }
            //这个属性RoleName是玩家的，因此，这里不能有RoleName
            const AFIValueList& valueList = pConfigPropertyInfo->GetValue();


            if (0 != strcmp(pConfigPropertyInfo->GetKey(), "RoleName")
                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "Sex")
                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "Job")
                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "Race")
                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "Camp")
                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "Account")
                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "SceneID")
                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "LastContainerID")
                && 0 != strcmp(pConfigPropertyInfo->GetKey(), "ClassName"))
            {
                varPropertyKeyList << pConfigPropertyInfo->GetKey();
                varPropertyValueList.Append(valueList, 0, 1);
            }

            pConfigPropertyInfo = pConfigPropertyManager->Next();
        }
    }
    varPropertyKeyList << "RoleName";
    varPropertyValueList << pstrRoleName;
    varPropertyKeyList << "Sex";
    varPropertyValueList << nRoleSex;
    varPropertyKeyList << "Job";
    varPropertyValueList << nRoleJob;
    varPropertyKeyList << "Race";
    varPropertyValueList << nRoleRace;
    varPropertyKeyList << "Camp";
    varPropertyValueList << nRoleCamp;
    varPropertyKeyList << "SceneID";
    varPropertyValueList << mnRoleHallContainer;
    varPropertyKeyList << "LastContainerID";
    varPropertyValueList << 1;//1号场景为新手村
    varPropertyKeyList << "ClassName";
    varPropertyValueList << "Player";
    varPropertyKeyList << "Level";
    varPropertyValueList << 1;

    m_pNoSqlModule->SetRoleProperty(pstrRoleName, varPropertyKeyList, varPropertyValueList);

    //////////////////////////////////////////////////////////////////////////
    GTProperty* gtPproperty = m_pElementInfoModule->GetPropertyManager("Scene0")->GetElement("RelivePos");
    const char* pstrRelivePos = gtPproperty->QueryString();
    AFCValueList valueRelivePos(pstrRelivePos, ",");

    AFCValueList arg;
    arg << "Account" << pstrAccountName;
    arg << "RoleName" << pstrRoleName;
    arg << "SceneID" << mnRoleHallContainer;
    arg << "X" << atof(valueRelivePos.StringVal(0));
    arg << "Y" << atof(valueRelivePos.StringVal(1));
    arg << "Z" << atof(valueRelivePos.StringVal(2));
    m_pKernelModule->CreateObject(0, mnRoleHallContainer, 0, "Player", szConfigName, arg);
    //nosql 则不需要这样了
    //m_pEventProcessModule->DoEvent(0, NFED_ON_DATABASE_SERVER_LOADROE_BEGIN, AFCValueList() << pstrAccountName);

    return 0;
}

int AFCCreateRoleModule::OnDeleteRoleEvent(const AFIDENTID& object, const int nEventID, const AFIValueList& var)
{
    if (2 != var.GetCount())
    {
        return 0;
    }

    const char* pstrAccountName = var.StringVal(0);
    const char* pstrRoleName = var.StringVal(1);

    AFCValueList valObjctList;
    m_pKernelModule->GetObjectByProperty(mnRoleHallContainer, "RoleName", AFCValueList() << pstrRoleName, valObjctList);
    if (valObjctList.GetCount() == 1)
    {
        m_pNoSqlModule->DeleteRole(pstrAccountName, pstrRoleName);
        //m_pDataBaseModule->DeleteRole(pstrAccountName, pstrRoleName);
        m_pKernelModule->DestroyObject(valObjctList.ObjectVal(0));
        //m_pEventProcessModule->DoEvent( 0, NFED_ON_CLIENT_DELETEROLE_RESULTS, AFCValueList() << pstrAccountName << pstrRoleName );
    }

    return 0;
}

int AFCCreateRoleModule::OnAcountDisConnectEvent(const AFIDENTID& object, const int nEventID, const AFIValueList& var)
{
    if (var.GetCount() == 3)
    {
        const char* pstrAccount = var.StringVal(0);
        const char* pstrRoleName = var.StringVal(1);
        AFIDENTID ident = var.ObjectVal(2);

        if (strlen(pstrAccount) > 0)
        {
            if (m_pGameLogicModule->GetGameID() == 1)
            {

                if (!ident.IsNull())
                {
                    m_pKernelModule->DestroyObject(ident);
                }
                else
                {
                    AFCValueList varHallObjectList;
                    AFCValueList varHalvalueInfo;
                    varHalvalueInfo << pstrAccount;

                    int nHallObjectCount = m_pKernelModule->GetObjectByProperty(mnRoleHallContainer, "Account", varHalvalueInfo, varHallObjectList);
                    for (int i  = 0; i < varHallObjectList.GetCount(); i++)
                    {
                        AFIDENTID identHall = varHallObjectList.ObjectVal(i);
                        if (ident.nSerial == m_pGameLogicModule->GetGameID()
                            && m_pKernelModule->GetObject(identHall))
                        {
                            m_pKernelModule->DestroyObject(identHall);
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int AFCCreateRoleModule::OnLoadRoleBeginEvent(const AFIDENTID& object, const int nEventID, const AFIValueList& var)
{
    //直接从NOSQL数据库拉
    const char* pstrAccount = var.StringVal(0);
    AFCValueList roleLlist;
    if (m_pNoSqlModule->QueryAccountRoleList(pstrAccount, roleLlist) > 0)
    {
        for (int i = 0; i < roleLlist.GetCount(); i++)
        {
            const char* pstrRoleName = roleLlist.StringVal(i);
            if (strlen(pstrRoleName) > 0)
            {
                //看容器中是否已经存在，存在则不创建
                AFCValueList varHallObjectList;
                AFCValueList varHalvalueInfo;
                varHalvalueInfo << pstrRoleName;
                int nHallObjectCount = m_pKernelModule->GetObjectByProperty(mnRoleHallContainer, "RoleName", varHalvalueInfo, varHallObjectList);
                if (nHallObjectCount > 0)
                {
                    for (int j = 0; j < varHallObjectList.GetCount(); j++)
                    {
                        m_pKernelModule->LogErrorObject(varHallObjectList.ObjectVal(j));
                        m_pKernelModule->DestroyObject(varHallObjectList.ObjectVal(j));
                    }
                }

                GTProperty* gtPproperty = m_pElementInfoModule->GetPropertyManager("Scene0")->GetElement("RelivePos");
                const char* pstrRelivePos = gtPproperty->QueryString();
                AFCValueList valueRelivePos(pstrRelivePos, ",");

                char szConfigIindex[MAX_PATH] = { 0 };
                sprintf(szConfigIindex, "%d", var.IntVal(i + 1));

                AFCValueList arg;
                arg << "Account" << pstrAccount;
                arg << "RoleName" << pstrRoleName;
                arg << "SceneID" << mnRoleHallContainer;
                arg << "X" << atof(valueRelivePos.StringVal(0));
                arg << "Y" << atof(valueRelivePos.StringVal(1));
                arg << "Z" << atof(valueRelivePos.StringVal(2));
                m_pKernelModule->CreateObject(0, mnRoleHallContainer, 0, "Player", szConfigIindex, arg);
            }
        }
    }

    return 0;
}
