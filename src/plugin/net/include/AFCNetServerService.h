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
#include "log/interface/AFILogModule.h"
#include "bus/interface/AFIMsgModule.h"
#include "net/interface/AFINetServiceManagerModule.h"
#include "net/interface/AFINetServerService.h"

namespace ark {

class AFCNetServerService : public AFINetServerService
{
public:
    explicit AFCNetServerService(AFIPluginManager *p);
    ~AFCNetServerService() override;

    bool Start(
        const AFHeadLength len, const int bus_id, const AFEndpoint &ep, const uint8_t thread_count, const uint32_t max_connection) override;
    bool Update() override;

    AFINet *GetNet() override;

    bool RegMsgCallback(const int msg_id, const NET_MSG_FUNCTOR_PTR &cb) override;
    bool RegForwardMsgCallback(const NET_MSG_FUNCTOR_PTR &cb) override;
    bool RegNetEventCallback(const NET_EVENT_FUNCTOR_PTR &cb) override;

protected:
    void OnNetMsg(const AFNetMsg *msg, const int64_t session_id);
    void OnNetEvent(const AFNetEvent *event);

    void OnClientRegister(const AFNetMsg *msg, const int64_t session_id);
    void SyncToAllClient(const int bus_id, const AFGUID &session_id);

private:
    AFIPluginManager *m_pPluginManager;
    AFINetServiceManagerModule *m_pNetServiceManagerModule;
    AFILogModule *m_pLogModule;
    AFIMsgModule *m_pMsgModule;

    AFINet *m_pNet{nullptr};

    std::map<int, NET_MSG_FUNCTOR_PTR> net_msg_callbacks_;
    std::list<NET_MSG_FUNCTOR_PTR> net_forward_msg_callbacks_;
    std::list<NET_EVENT_FUNCTOR_PTR> net_event_callbacks_;

    AFMapEx<int, AFServerData> reg_clients_;
};

} // namespace ark