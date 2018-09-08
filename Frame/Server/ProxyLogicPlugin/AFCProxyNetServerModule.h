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

#include "Common/AFProtoCPP.hpp"
#include "SDK/Core/AFCConsistentHash.hpp"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIConfigModule.h"
#include "SDK/Interface/AFIGUIDModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFINetServerManagerModule.h"
#include "Server/Interface/AFIProxyNetServerModule.h"
#include "Server/Interface/AFIProxyServerToWorldModule.h"
#include "Server/Interface/AFIProxyServerToGameModule.h"

class AFCProxyNetServerModule : public AFIProxyNetServerModule
{
public:
    explicit AFCProxyNetServerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool PostInit();
    virtual bool Update();

    virtual int Transpond(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen);
    virtual int SendToPlayerClient(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nClientID, const AFGUID& nPlayer);

    //进入游戏成功
    virtual int EnterGameSuccessEvent(const AFGUID xClientID, const AFGUID xPlayerID);

protected:
    int StartServer();
    void OnSocketClientEvent(const NetEventType eEvent, const AFGUID& xClientID, const int nSeverID);

    //连接丢失,删2层(连接对象，帐号对象)
    void OnClientDisconnect(const AFGUID& xClientID);
    //有连接
    void OnClientConnected(const AFGUID& xClientID);

    void OnConnectKeyProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnReqServerListProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnSelectServerProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnReqRoleListProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnReqCreateRoleProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnReqDelRoleProcess(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
    void OnReqEnterGameServer(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    //客户端的连接60秒删掉
    int HB_OnConnectCheckTime(const AFGUID& self, const std::string& strHeartBeat, const float fTime, const int nCount, const AFIDataList& var);
    //////////////////////////////////////////////////////////////////////////

    void OnOtherMessage(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

    template<class TypeName> void CheckSessionTransMsg(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
    {
        //在没有正式进入游戏之前，nPlayerID都是FD
        ARK_MSG_PROCESS_NO_OBJECT(xHead, msg, nLen, TypeName);

        if (CheckSessionState(xMsg.game_id(), xClientID, xMsg.account()))
        {
            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(xMsg.game_id(), nMsgID, msg, nLen, xClientID);
        }
    }

    bool CheckSessionState(const int64_t nGameID, const AFGUID& xClientID, const std::string& strAccount);

private:
    AFMapEx<AFGUID, SessionData> mmSessionData; //Player Client <--> SessionData
    AFCConsistentHash mxConsistentHash;

    AFIProxyServerToWorldModule* m_pProxyToWorldModule;
    AFIProxyServerToGameModule* m_pProxyServerToGameModule;
    AFIKernelModule* m_pKernelModule;
    AFILogModule* m_pLogModule;
    AFIBusModule* m_pBusModule;
    AFINetServerManagerModule* m_pNetServerManagerModule;

    AFINetServer* m_pNetServer{ nullptr };
};