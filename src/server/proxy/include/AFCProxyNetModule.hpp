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

#include "proto/AFProtoCPP.hpp"
#include "base/container/AFCConsistentHash.hpp"
#include "base/AFPluginManager.hpp"
#include "bus/interface/AFIMsgModule.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "proxy/interface/AFIProxyNetModule.hpp"

namespace ark {

class AFCProxyNetModule final : public AFIProxyNetModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool PostInit() override;
    //bool PreUpdate() override;

    bool VerifyConnectData(const std::string& strAccount, const std::string& strKey) override;

protected:
    int StartServer();

    void OnSelectServerResultProcess(const AFNetMsg* msg, conv_id_t session_id);
    void OnServerInfoProcess(const AFNetMsg* msg, conv_id_t session_id);

    void OnOtherMessage(const AFNetMsg* msg, conv_id_t session_id);
    void OnBrocastmsg(const AFNetMsg* msg, conv_id_t session_id);

    void OnSocketEvent(const AFNetEvent* event);

    void OnClientDisconnect(const guid_t& xClientID);
    void OnClientConnected(const guid_t& xClientID);

    // void OnTransMessage(const ARK_PKG_BASE_HEAD& xHead, const int nMsgID, const char* msg, const uint32_t nLen, const
    // guid_t& xClientID);

    template<class TypeName>
    void CheckSessionTransMsg(const AFNetMsg* msg)
    {
        // ARK_PROCESS_MSG(xHead, msg, nLen, TypeName);

        // if (CheckSessionState(xMsg.game_id(), xClientID, xMsg.account()))
        //{
        //    m_pMsgModule->SendSSMsg(xMsg.game_id(), nMsgID, msg, nLen, xClientID);
        //}
    }

    bool CheckSessionState(const int nGameID, const guid_t& xClientID, const std::string& strAccount);

    class ClientConnectData
    {
    public:
        ClientConnectData() {}

        std::string strAccount{};
        std::string strConnectKey{};
    };

    AFSmartPtrMap<std::string, ClientConnectData> mxWantToConnectMap;

private:
    // AFSmartPtrMap<guid_t, AFClientConnectionData> client_connections_; // net_conn_id <--> SessionData

    AFIBusModule* m_pBusModule;
    AFINetServiceManagerModule* m_pNetServiceManagerModule;
    AFIMsgModule* m_pMsgModule;

    std::shared_ptr<AFINetServerService> m_pNetServer{nullptr};
};

} // namespace ark
