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

        bool StartClient(const int& target_bus_id, const AFEndpoint& endpoint) override;
        void Update() override;
        void Shutdown() override;

        bool AddNetRecvCallback(const int nMsgID, const NET_PKG_RECV_FUNCTOR_PTR& cb) override;
        //bool AddRecvCallback(const NET_PKG_RECV_FUNCTOR_PTR& cb) override;
        bool AddNetEventCallBack(const NET_EVENT_FUNCTOR_PTR& cb) override;

        ////////////////////////////////////////////////////////////////////////////////
        //void SendByServerID(const int nServerID, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID);

        ////裸数据,发时组包
        //void SendByServerID(const int nServerID, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID);
        ////裸数据,发时组包
        //void SendToAllServer(const int nMsgID, const std::string& strData, const AFGUID& nPlayerID);
        //void SendToServerByPB(const int nServerID, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID) override;
        //void SendToAllServerByPB(const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID);
        //void SendBySuit(const std::string& strHashKey, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID);
        //void SendBySuit(const std::string& strHashKey, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID);
        //void SendBySuit(const int& nHashKey, const int nMsgID, const std::string& strData, const AFGUID& nPlayerID);
        //void SendBySuit(const int& nHashKey, const int nMsgID, const char* msg, const uint32_t nLen, const AFGUID& nPlayerID);
        //void SendSuitByPB(const std::string& strHashKey, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID);
        //void SendSuitByPB(const int& nHashKey, const uint16_t nMsgID, google::protobuf::Message& xData, const AFGUID& nPlayerID);

        const ARK_SHARE_PTR<AFConnectionData>& GetServerNetInfo(const int nServerID) override;
        AFMapEx<int, AFConnectionData>& GetServerList() override;

    protected:
        void ProcessUpdate();
        void ProcessAddNewNetClient();

        AFINet* CreateNet(const proto_type proto);

        void RegisterToServer(const int bus_id);
        int OnConnect(const NetEventType event, const AFGUID& conn_id, const std::string& ip, int bus_id);
        int OnDisconnect(const NetEventType event, const AFGUID& conn_id, const std::string& ip, int bus_id);

        void OnRecvNetPack(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const size_t msg_len, const AFGUID& conn_id);
        void OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, int bus_id);

        void KeepReport(ARK_SHARE_PTR<AFConnectionData>& connection_data) {/*Will add*/ }
        void LogServerInfo(const std::string& strServerInfo) {/*Will add*/ }

        void LogServerInfo();
        void KeepAlive(ARK_SHARE_PTR<AFConnectionData>& pServerData);

        bool GetServerMachineData(const std::string& strServerID, AFCMachineNode& xMachineData);
        void AddServerWeightData(ARK_SHARE_PTR<AFConnectionData>& xInfo);

        void RemoveServerWeightData(ARK_SHARE_PTR<AFConnectionData>& xInfo);

        //recv other server infos
        void OnServerNotify(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len, const AFGUID& conn_id);

    private:
        AFIPluginManager* m_pPluginManager;
        AFINetServiceManagerModule* m_pNetServiceManagerModule;
        AFIBusModule* m_pBusModule;
        AFIMsgModule* m_pMsgModule;
        AFILogModule* m_pLogModule;

        AFMapEx<int, AFConnectionData> mxTargetServerMap;
        AFCConsistentHash mxConsistentHash;

        std::list<AFConnectionData> _tmp_nets;

        std::map<int, NET_PKG_RECV_FUNCTOR_PTR> mxRecvCallBack;
        std::list<NET_EVENT_FUNCTOR_PTR> net_event_callbacks_;
        //std::list<NET_PKG_RECV_FUNCTOR_PTR> mxCallBackList;

        std::map<int, std::map<int, AFMsg::msg_ss_server_report>> reg_servers_;
    };

}