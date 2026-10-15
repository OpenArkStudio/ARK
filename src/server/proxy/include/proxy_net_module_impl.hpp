/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "ark/bus/bus_module.hpp"
#include "ark/bus/msg_module.hpp"
#include "ark/container/consistent_hash.hpp"
#include "ark/net/net_service_manager_module.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/proto.hpp"
#include "proxy/interface/proxy_net_module.hpp"

namespace ark {

class proxy_net_module_impl final : public proxy_net_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;
    bool post_init() override;
    //bool PreUpdate() override;

    bool VerifyConnectData(const std::string& strAccount, const std::string& strKey) override;

protected:
    int StartServer();

    void OnSelectServerResultProcess(const net_msg* msg, conv_id_t session_id);
    void OnServerInfoProcess(const net_msg* msg, conv_id_t session_id);

    void OnOtherMessage(const net_msg* msg, conv_id_t session_id);
    void OnBrocastmsg(const net_msg* msg, conv_id_t session_id);

    void OnSocketEvent(const net_event* event);

    void OnClientDisconnect(const guid_t& xClientID);
    void OnClientConnected(const guid_t& xClientID);

    // void OnTransMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const
    // guid_t& xClientID);

    template<class TypeName>
    void CheckSessionTransMsg(const net_msg* msg) {
        // ARK_PROCESS_MSG(xHead, msg, nLen, TypeName);

        // if (CheckSessionState(xMsg.game_id(), xClientID, xMsg.account()))
        //{
        //    m_pMsgModule->SendSSMsg(xMsg.game_id(), nMsgID, msg, nLen, xClientID);
        //}
    }

    bool CheckSessionState(const int nGameID, const guid_t& xClientID, const std::string& strAccount);

    class client_connect_data
    {
    public:
        client_connect_data() {}

        std::string strAccount{};
        std::string strConnectKey{};
    };

    shared_object_map<std::string, client_connect_data> mxWantToConnectMap;

private:
    // AFSmartPtrMap<guid_t, AFClientConnectionData> client_connections_; // net_conn_id <--> SessionData

    bus_module* m_pBusModule;
    net_service_manager_module* m_pNetServiceManagerModule;
    msg_module* m_pMsgModule;

    std::shared_ptr<net_server_service> m_pNetServer{nullptr};
};

} // namespace ark
