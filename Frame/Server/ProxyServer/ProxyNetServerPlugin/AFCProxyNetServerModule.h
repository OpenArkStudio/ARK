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

#include "SDK/Core/Base/AFCConsistentHash.hpp"
#include "SDK/Proto/AFProtoCPP.hpp"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIGUIDModule.h"
#include "Server/Interface/AFINetServerModule.h"
#include "Server/Interface/AFIProxyNetServerModule.h"
#include "Server/Interface/AFIProxyServerToWorldModule.h"
#include "Server/Interface/AFIProxyServerToGameModule.h"
#include "Server/Interface/AFINetClientModule.hpp"

class AFCProxyNetServerModule : public AFIProxyNetServerModule
{
public:
    explicit AFCProxyNetServerModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Update();

    virtual bool PostInit();

    virtual int Transpond(const AFIMsgHead& xHead, const int nMsgID, const char* msg, const uint32_t nLen);
    virtual int SendToPlayerClient(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nClientID, const AFGUID& nPlayer);

    //进入游戏成功
    virtual int EnterGameSuccessEvent(const AFGUID xClientID, const AFGUID xPlayerID);

protected:

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

private:
    AFMapEx<AFGUID, SessionData> mmSessionData; //Player Client <--> SessionData
    AFCConsistentHash mxConsistentHash;

    AFIProxyServerToWorldModule* m_pProxyToWorldModule;
    AFIProxyServerToGameModule* m_pProxyServerToGameModule;
    AFIKernelModule* m_pKernelModule;
    AFILogModule* m_pLogModule;
    AFIElementModule* m_pElementModule;
    AFIClassModule* m_pClassModule;
    AFIGUIDModule* m_pUUIDModule;
    AFINetServerModule* m_pNetModule;

};