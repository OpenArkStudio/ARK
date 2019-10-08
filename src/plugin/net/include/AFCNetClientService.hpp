/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "base/AFMap.hpp"
#include "base/AFCConsistentHash.hpp"
#include "base/AFPluginManager.hpp"
#include "net/interface/AFINetClientService.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "bus/interface/AFIMsgModule.hpp"
#include "log/interface/AFILogModule.hpp"

namespace ark {

class AFCNetClientService final : public AFINetClientService
{
public:
    enum EConstDefine
    {
        EConstDefine_DefaultWeight = 255,
    };

    AFCNetClientService(AFPluginManager* p);

    bool StartClient(const AFHeadLength head_len, const int& target_bus_id, const AFEndpoint& endpoint) override;
    void Update() override;
    void Shutdown() override;

    bool RegMsgCallback(const int msg_id, const NET_MSG_FUNCTOR&& cb) override;
    bool RegForwardMsgCallback(const NET_MSG_FUNCTOR&& cb) override;
    bool RegNetEventCallback(const NET_EVENT_FUNCTOR&& cb) override;

    ARK_SHARE_PTR<AFConnectionData> GetConnectionInfo(const int bus_id) override;
    //AFMapEx<int, AFConnectionData>& GetServerList() override;

protected:
    void ProcessUpdate();
    void ProcessAddConnection();

    AFINet* CreateNet(const proto_type proto);

    //void RegisterToServer(const AFGUID& session_id, const int bus_id);
    int OnConnect(const AFNetEvent* event);
    int OnDisconnect(const AFNetEvent* event);

    void OnNetMsg(const AFNetMsg* msg, const int64_t session_id);
    void OnNetEvent(const AFNetEvent* event);

    void KeepReport(ARK_SHARE_PTR<AFConnectionData>& connection_data)
    {
        // TODO
    }
    void LogServerInfo(const std::string& strServerInfo)
    {
        // TODO
    }

    void LogServerInfo();
    void KeepAlive(ARK_SHARE_PTR<AFConnectionData>& pServerData);

    bool GetServerMachineData(const std::string& strServerID, AFCMachineNode& xMachineData);
    void AddServerWeightData(ARK_SHARE_PTR<AFConnectionData>& xInfo);

    void RemoveServerWeightData(ARK_SHARE_PTR<AFConnectionData>& xInfo);

private:
    AFPluginManager* m_pPluginManager;
    AFINetServiceManagerModule* m_pNetServiceManagerModule;
    AFIBusModule* m_pBusModule;
    AFIMsgModule* m_pMsgModule;
    AFILogModule* m_pLogModule;

    // Connected connections(may the ConnectState is different)
    AFSmartPtrMap<int, AFConnectionData> real_connections_;
    //AFMap<int, AFINet> bus_to_net_; // TODO: check if delete

    // TODO: change to AFConsistentHashmap
    AFCConsistentHash consistent_hashmap_;

    std::list<AFConnectionData> tmp_connections_;

    std::map<int, NET_MSG_FUNCTOR> net_msg_callbacks_;
    std::list<NET_EVENT_FUNCTOR> net_event_callbacks_;

    // [NOT USE now] - forward to other processes
    std::list<NET_MSG_FUNCTOR> net_msg_forward_callbacks_;
};

} // namespace ark
