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

#include "SDK/Core/AFMap.hpp"
#include "SDK/Core/AFCConsistentHash.hpp"
#include "SDK/Interface/AFINetClientService.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFINetServiceManagerModule.h"
#include "SDK/Interface/AFIBusModule.h"
#include "SDK/Interface/AFIMsgModule.h"
#include "SDK/Interface/AFILogModule.h"

namespace ark
{

    class AFCNetClientService : public AFINetClientService
    {
    public:
        enum EConstDefine
        {
            EConstDefine_DefaultWeight = 255,
        };

        AFCNetClientService(AFIPluginManager* p);
        ~AFCNetClientService() override;

        bool StartClient(const AFHeadLength head_len, const int& target_bus_id, const AFEndpoint& endpoint) override;
        void Update() override;
        void Shutdown() override;

        bool RegMsgCallback(const int msg_id, const NET_MSG_FUNCTOR_PTR& cb) override;
        bool RegForwardMsgCallback(const NET_MSG_FUNCTOR_PTR& cb) override;
        bool RegNetEventCallback(const NET_EVENT_FUNCTOR_PTR& cb) override;

        const ARK_SHARE_PTR<AFConnectionData>& GetServerNetInfo(const int nServerID) override;
        AFMapEx<int, AFConnectionData>& GetServerList() override;

    protected:
        void ProcessUpdate();
        void ProcessAddNewNetClient();

        AFINet* CreateNet(const proto_type proto);

        void RegisterToServer(const AFGUID& session_id, const int bus_id);
        int OnConnect(const AFNetEvent* event);
        int OnDisconnect(const AFNetEvent* event);

        void OnNetMsg(const AFNetMsg* msg, const int64_t session_id);
        void OnNetEvent(const AFNetEvent* event);

        void KeepReport(ARK_SHARE_PTR<AFConnectionData>& connection_data) {/*Will add*/ }
        void LogServerInfo(const std::string& strServerInfo) {/*Will add*/ }

        void LogServerInfo();
        void KeepAlive(ARK_SHARE_PTR<AFConnectionData>& pServerData);

        bool GetServerMachineData(const std::string& strServerID, AFCMachineNode& xMachineData);
        void AddServerWeightData(ARK_SHARE_PTR<AFConnectionData>& xInfo);

        void RemoveServerWeightData(ARK_SHARE_PTR<AFConnectionData>& xInfo);

        //recv other server infos
        void OnServerNotify(const AFNetMsg* msg, const int64_t session_id);

    private:
        AFIPluginManager* m_pPluginManager;
        AFINetServiceManagerModule* m_pNetServiceManagerModule;
        AFIBusModule* m_pBusModule;
        AFIMsgModule* m_pMsgModule;
        AFILogModule* m_pLogModule;

        AFMapEx<int, AFConnectionData> target_servers_;
        AFCConsistentHash mxConsistentHash;

        std::list<AFConnectionData> _tmp_nets;

        std::map<int, NET_MSG_FUNCTOR_PTR> net_msg_callbacks_;
        std::list<NET_EVENT_FUNCTOR_PTR> net_event_callbacks_;

        //forward to other processes
        std::list<NET_MSG_FUNCTOR_PTR> net_msg_forward_callbacks_;

        std::map<int, std::map<int, AFMsg::msg_ss_server_report>> reg_servers_;
    };

}