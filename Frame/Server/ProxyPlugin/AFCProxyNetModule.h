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

#pragma once

#include "Common/AFProtoCPP.hpp"
#include "SDK/Core/AFCConsistentHash.hpp"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFINetServiceManagerModule.h"
#include "Server/Interface/AFIProxyNetModule.h"

namespace ark
{

    class AFCProxyNetModule : public AFIProxyNetModule
    {
    public:
        explicit AFCProxyNetModule() = default;

        bool Init() override;
        bool PostInit() override;
        bool PreUpdate() override;

        virtual int Transpond(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen);
        virtual int SendToPlayerClient(const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nClientID, const AFGUID& nPlayer);

        bool VerifyConnectData(const std::string& strAccount, const std::string& strKey) override;

        //进入游戏成功
        virtual int EnterGameSuccessEvent(const AFGUID xClientID, const AFGUID xPlayerID);

    protected:
        int StartServer();

        int StartClient();

        void OnSelectServerResultProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        void OnServerInfoProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        void OnOtherMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        void OnBrocastmsg(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        void OnAckEnterGame(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        void OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id);

        //连接丢失,删2层(连接对象，帐号对象)
        void OnClientDisconnect(const AFGUID& xClientID);
        //有连接
        void OnClientConnected(const AFGUID& xClientID);

        void OnConnectKeyProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        void OnReqServerListProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        void OnSelectServerProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        void OnReqRoleListProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        void OnReqCreateRoleProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        void OnReqDelRoleProcess(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);
        void OnReqEnterGameServer(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        //客户端的连接60秒删掉
        int HB_OnConnectCheckTime(const AFGUID& self, const std::string& strHeartBeat, const float fTime, const int nCount, const AFIDataList& var);
        //////////////////////////////////////////////////////////////////////////

        //void OnTransMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID);

        template<class TypeName>
        void CheckSessionTransMsg(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& xClientID)
        {
            ////在没有正式进入游戏之前，nPlayerID都是FD
            //ARK_PROCESS_MSG(xHead, msg, nLen, TypeName);

            //if (CheckSessionState(xMsg.game_id(), xClientID, xMsg.account()))
            //{
            //    m_pMsgModule->SendSSMsg(xMsg.game_id(), nMsgID, msg, nLen, xClientID);
            //}
        }

        bool CheckSessionState(const int64_t nGameID, const AFGUID& xClientID, const std::string& strAccount);

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