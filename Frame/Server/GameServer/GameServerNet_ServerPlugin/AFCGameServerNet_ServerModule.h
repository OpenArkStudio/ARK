/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @FileName  AFCGameServerNet_ServerModule.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFCGameServerNet_ServerModule                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFCGameServerNet_ServerModule                                                  *
*****************************************************************************/
#ifndef AFC_GAMESERVER_SERVER_MODULE_H
#define AFC_GAMESERVER_SERVER_MODULE_H

//#include "GameServerPCH.h"
//#include "NW_Helper.h"
//  the cause of sock'libariy, thenfore "AFCNet.h" much be included first.
#include <memory>
#include "SDK/Proto/AFMsgDefine.h"
#include "SDK/Interface/AFINetServerModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFISceneProcessModule.h"
#include "SDK/Interface/AFIGameLogicModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFIGUIDModule.h"
#include "SDK/Interface/AFIGameServerToWorldModule.h"
#include "SDK/Interface/AFIGameServerNet_ServerModule.h"
#include "SDK/Interface/AFIGameServerNet_ServerModule.h"
////////////////////////////////////////////////////////////////////////////



class AFCGameServerNet_ServerModule
    : public AFIGameServerNet_ServerModule
{
public:
    AFCGameServerNet_ServerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

    virtual void LogReceive(const char* str) {}
    virtual void LogSend(const char* str) {}
    virtual void SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const AFGUID& self);
    virtual void SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const AFGUID& self);
    virtual AFINetServerModule* GetNetModule();

    virtual bool AddPlayerGateInfo(const AFGUID& nRoleID, const AFGUID& nClientID, const int nGateID);
    virtual bool RemovePlayerGateInfo(const AFGUID& nRoleID);
    virtual ARK_SHARE_PTR<GateBaseInfo> GetPlayerGateInfo(const AFGUID& nRoleID);

    virtual ARK_SHARE_PTR<GateServerInfo> GetGateServerInfo(const int nGateID);
    virtual ARK_SHARE_PTR<GateServerInfo> GetGateServerInfoByClientID(const AFGUID& nClientID);

    virtual int OnPropertyEnter(const AFIDataList& argVar, const AFGUID& self);
    virtual int OnRecordEnter(const AFIDataList& argVar, const AFGUID& self);

    virtual int OnObjectListEnter(const AFIDataList& self, const AFIDataList& argVar);
    virtual int OnObjectListLeave(const AFIDataList& self, const AFIDataList& argVar);

protected:
    void OnSocketPSEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nServerID);
    void OnClientDisconnect(const AFGUID& xClientID);
    void OnClientConnected(const AFGUID& xClientID);

protected:
    void OnProxyServerRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnProxyServerUnRegisteredProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnRefreshProxyServerInfoProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

protected:
    void OnReqiureRoleListProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnCreateRoleGameProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnDeleteRoleGameProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnClienEnterGameProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnClienLeaveGameProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnClienSwapSceneProcess(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);


    ///////////WORLD_START///////////////////////////////////////////////////////////////
    void OnTransWorld(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

protected:
    //网络同步
    int OnPropertyCommonEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar);
    int OnRecordCommonEvent(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar);
    int OnClassCommonEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& var);

    int OnGroupEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar);
    int OnContainerEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar);

    int OnObjectClassEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& var);
    int OnSwapSceneResultEvent(const AFGUID& self, const int nEventID, const AFIDataList& var);

    int GetBroadCastObject(const AFGUID& self, const std::string& strPropertyName, const bool bTable, AFIDataList& valueObject);
    int GetBroadCastObject(const int nObjectContainerID, const int nGroupID, AFIDataList& valueObject);

private:
    //<角色id,角色网关基础信息>//其实可以在object系统中被代替
    AFMapEx<AFGUID, GateBaseInfo> mRoleBaseData;
    //gateid,data
    AFMapEx<int, GateServerInfo> mProxyMap;

    //////////////////////////////////////////////////////////////////////////
    AFIGUIDModule* m_pUUIDModule;
    AFIKernelModule* m_pKernelModule;
    AFIClassModule* m_pClassModule;
    AFILogModule* m_pLogModule;
    AFISceneProcessModule* m_pSceneProcessModule;
    AFIElementModule* m_pElementModule;
    AFINetServerModule* m_pNetModule;
    //////////////////////////////////////////////////////////////////////////
    AFIGameServerToWorldModule* m_pGameServerToWorldModule;
};
#endif

