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

#include "base/AFUidGenerator.hpp"
#include "net/interface/AFINet.hpp"
#include "net/include/AFNetSession.hpp"

namespace ark {

class AFCTCPClient final : public AFNoncopyable, public AFINet, public std::enable_shared_from_this<AFCTCPClient>
{
public:
    template<typename BaseType>
    AFCTCPClient(BaseType* pBaseType, void (BaseType::*handleRecv)(const AFNetMsg*),
        void (BaseType::*handleEvent)(const AFNetEvent*))
    {
        net_msg_cb_ = std::bind(handleRecv, pBaseType, std::placeholders::_1);
        net_event_cb_ = std::bind(handleEvent, pBaseType, std::placeholders::_1);

        brynet::net::base::InitSocket();
        tcp_service_ = brynet::net::TcpService::Create();
        // This could let you use common connector when multi-client created
        connector_ = brynet::net::AsyncConnector::Create();

        uid_generator_ = std::make_shared<AFUidGeneratorThreadSafe>();
    }

    ~AFCTCPClient() override;

    void Update() override;

    bool StartClient(
        AFHeadLength head_len, const int dst_busid, const std::string& ip, const int port, bool ip_v6 = false) override;

    bool Shutdown() override final;
    bool SendMsg(AFMsgHead* head, const char* msg_data, const int64_t session_id) override;

    bool CloseSession(const AFGUID& session_id) override;

protected:
    bool SendMsg(const char* msg, const size_t msg_len, const AFGUID& session_id = 0);

    void UpdateNetSession();
    void UpdateNetEvent(AFTCPSessionPtr session);
    void UpdateNetMsg(AFTCPSessionPtr session);

    bool CloseSession();

private:
    std::unique_ptr<AFTCPSession> client_session_ptr_{nullptr};
    int dst_bus_id_{0};

    NET_MSG_FUNCTOR net_msg_cb_;
    NET_EVENT_FUNCTOR net_event_cb_;
    AFCReaderWriterLock rw_lock_;

    brynet::net::TcpService::Ptr tcp_service_{nullptr};
    brynet::net::AsyncConnector::Ptr connector_{nullptr};
    brynet::net::wrapper::ConnectionBuilder connection_builder_;

    //uint64_t trust_session_id_{1};
    std::shared_ptr<AFUidGeneratorThreadSafe> uid_generator_{nullptr};
};

} // namespace ark
