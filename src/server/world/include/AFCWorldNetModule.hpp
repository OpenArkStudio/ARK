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
#include "base/AFPluginManager.hpp"
#include "kernel/interface/AFIKernelModule.hpp"
#include "utility/interface/AFITimerModule.hpp"
#include "bus/interface/AFIMsgModule.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "bus/interface/AFIMsgModule.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "world/interface/AFIWorldNetModule.hpp"

namespace ark {

class AFCWorldNetModule final : public AFIWorldNetModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool PostInit() override;
    //bool PreUpdate() override;

    bool SendMsgToGame(const int nGameID, const AFMsg::EGameMsgID eMsgID, google::protobuf::Message& xData,
        const guid_t nPlayer = 0) override;
    bool SendMsgToGame(const AFIDataList& argObjectVar, const AFIDataList& argGameID, const AFMsg::EGameMsgID eMsgID,
        google::protobuf::Message& xData) override;

    // std::shared_ptr<AFServerData> GetSuitProxyForEnter();
    std::shared_ptr<AFConnectionData> GetSuitProxyForEnter() override;
    std::shared_ptr<AFINetServerService> GetNetServer() override;

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

    AFIKernelModule* m_pKernelModule;
    AFIBusModule* m_pBusModule;
    AFIMsgModule* m_pMsgModule;
    AFINetServiceManagerModule* m_pNetServiceManagerModule;

    std::shared_ptr<AFINetServerService> m_pNetServer;
};

} // namespace ark
