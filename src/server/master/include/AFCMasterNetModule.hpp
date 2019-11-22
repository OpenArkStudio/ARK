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
#include "base/AFPluginManager.hpp"
#include "proto/AFProtoCPP.hpp"
#include "bus/interface/AFIMsgModule.hpp"
#include "bus/interface/AFIBusModule.hpp"
#include "bus/interface/AFIMsgModule.hpp"
#include "net/interface/AFINetServiceManagerModule.hpp"
#include "log/interface/AFILogModule.hpp"
#include "utility/interface/AFITimerModule.hpp"
#include "master/interface/AFIMasterNetModule.hpp"

namespace ark {

class AFCMasterNetModule final : public AFIMasterNetModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool PostInit() override;

protected:
    int StartServer();
    // void OnSocketEvent(const NetEventType event, const AFGUID& conn_id, const std::string& ip, const int bus_id);

    // void OnClientConnected(const AFGUID& xClientID);
    // void OnClientDisconnect(int bus_id, const AFGUID& xClientID);

    //////////////////////////////////////////////////////////////////////////
    // void OnTimerLogServer(const std::string& name, const AFGUID& id);

    // void OnHeartBeat(const ARK_PKG_BASE_HEAD& head, const int msg_id, const char* msg, const uint32_t msg_len, const
    // AFGUID& conn_id);

private:
    AFILogModule* m_pLogModule;
    AFIBusModule* m_pBusModule;
    AFINetServiceManagerModule* m_pNetServiceManagerModule;
    AFITimerModule* m_pTimerModule;
    AFIMsgModule* m_pMsgModule;

    std::shared_ptr<AFINetServerService> m_pNetServer{nullptr};
};

} // namespace ark
