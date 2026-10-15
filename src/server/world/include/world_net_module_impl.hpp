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
#include "ark/kernel/kernel_module.hpp"
#include "ark/net/net_service_manager_module.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/proto.hpp"
#include "ark/utility/timer_module.hpp"
#include "world/interface/world_net_module.hpp"

namespace ark {

class world_net_module_impl final : public world_net_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;
    bool post_init() override;
    //bool PreUpdate() override;

    bool SendMsgToGame(const int nGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData,
        const guid_t nPlayer = 0) override;
    bool SendMsgToGame(const data_list& argObjectVar, const data_list& argGameID, const AFMsg::EGameMsgID eMsgID,
        google::protobuf::Message& xData) override;

    // std::shared_ptr<AFServerData> GetSuitProxyForEnter();
    std::shared_ptr<connection_data> GetSuitProxyForEnter() override;
    std::shared_ptr<net_server_service> GetNetServer() override;

protected:
    int StartServer();
    //int StartClient();

    bool SendMsgToPlayer(const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData, const guid_t nPlayer);

    int GetPlayerGameID(const guid_t& self);

private:
    // AFSmartPtrMap<int, AFServerData> reg_servers_;

    int64_t mnLastCheckTime;

    // server id, data
    // AFSmartPtrMap<int, AFServerData> mGameMap;
    // AFSmartPtrMap<int, AFServerData> mProxyMap;

    kernel_module* m_pKernelModule;
    bus_module* m_pBusModule;
    msg_module* m_pMsgModule;
    net_service_manager_module* m_pNetServiceManagerModule;

    std::shared_ptr<net_server_service> m_pNetServer;
};

} // namespace ark
