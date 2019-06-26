/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2019 ArkNX authors.
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

#include "base/AFProtoCPP.hpp"
#include "base/AFCConsistentHash.hpp"
#include "interface/AFILogModule.h"
#include "interface/AFIMsgModule.h"
#include "interface/AFIBusModule.h"
#include "interface/AFINetServiceManagerModule.h"
#include "interface/AFIProxyNetModule.h"

namespace ark
{

    class AFCProxyNetModule : public AFIProxyNetModule
    {
    public:
        explicit AFCProxyNetModule() = default;

        bool Init() override;
        bool PostInit() override;
        bool PreUpdate() override;

        int Transpond(const AFNetMsg* msg) override;
        int SendToPlayerClient(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nClientID, const AFGUID& nPlayer) override;

        bool VerifyConnectData(const std::string& strAccount, const std::string& strKey) override;

        //进入游戏成功
        int EnterGameSuccessEvent(const AFGUID xClientID, const AFGUID xPlayerID) override;

    protected:
        int StartServer();

        int StartClient();

        void OnSelectServerResultProcess(const AFNetMsg* msg, const int64_t session_id);
        void OnServerInfoProcess(const AFNetMsg* msg, const int64_t session_id);

        void OnOtherMessage(const AFNetMsg* msg, const int64_t session_id);
        void OnBrocastmsg(const AFNetMsg* msg, const int64_t session_id);
        void OnAckEnterGame(const AFNetMsg* msg, const int64_t session_id);

        void OnSocketEvent(const AFNetEvent* event);

        //连接丢失,删2层(连接对象，帐号对象)
        void OnClientDisconnect(const AFGUID& xClientID);
        //有连接
        void OnClientConnected(const AFGUID& xClientID);

        void OnConnectKeyProcess(const AFNetMsg* msg, const int64_t session_id);
        void OnReqServerListProcess(const AFNetMsg* msg, const int64_t session_id);
        void OnSelectServerProcess(const AFNetMsg* msg, const int64_t session_id);
        void OnReqRoleListProcess(const AFNetMsg* msg, const int64_t session_id);
        void OnReqCreateRoleProcess(const AFNetMsg* msg, const int64_t session_id);
        void OnReqDelRoleProcess(const AFNetMsg* msg, const int64_t session_id);
        void OnReqEnterGameServer(const AFNetMsg* msg, const int64_t session_id);

        //////////////////////////////////////////////////////////////////////////

        //void OnTransMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        template<class TypeName>
        void CheckSessionTransMsg(const AFNetMsg* msg)
        {
            ////在没有正式进入游戏之前，nPlayerID都是FD
            //ARK_PROCESS_MSG(xHead, msg, nLen, TypeName);

            //if (CheckSessionState(xMsg.game_id(), xClientID, xMsg.account()))
            //{
            //    m_pMsgModule->SendSSMsg(xMsg.game_id(), nMsgID, msg, nLen, xClientID);
            //}
        }

        bool CheckSessionState(const int nGameID, const AFGUID& xClientID, const std::string& strAccount);

        class ClientConnectData
        {
        public:
            ClientConnectData()
            {
            }

            std::string strAccount{};
            std::string strConnectKey{};
        };

        AFMapEx<std::string, ClientConnectData> mxWantToConnectMap;

    private:
        AFMapEx<AFGUID, AFClientConnectionData> client_connections_; //net_conn_id <--> SessionData
        AFCConsistentHash mxConsistentHash;

        AFILogModule* m_pLogModule;
        AFIBusModule* m_pBusModule;
        AFINetServiceManagerModule* m_pNetServiceManagerModule;
        AFIMsgModule* m_pMsgModule;

        AFINetServerService* m_pNetServer{ nullptr };
    };

}