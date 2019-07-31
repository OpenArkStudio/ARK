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

#include <brynet/net/http/HttpService.h>
#include <brynet/net/http/HttpFormat.h>
#include "net/interface/AFINet.hpp"
#include "net/include/AFNetSession.hpp"

namespace ark {

class AFCWebSocktServer : public AFINet
{
public:
    AFCWebSocktServer();

    template<typename BaseType>
    AFCWebSocktServer(BaseType* pBaseType, void (BaseType::*handleRecieve)(const AFNetMsg*, const int64_t),
        void (BaseType::*handleEvent)(const AFNetEvent*))
    {
        net_msg_cb_ = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2);
        net_event_cb_ = std::bind(handleEvent, pBaseType, std::placeholders::_1);

        brynet::net::base::InitSocket();
    }

    ~AFCWebSocktServer() override;

    void Update() override;

    bool StartServer(AFHeadLength head_len, const int busid, const std::string& ip, const int port,
        const int thread_num, const unsigned int max_client, bool ip_v6 = false) override;
    bool Shutdown() override final;

    bool SendMsg(AFMsgHead* head, const char* msg_data, const int64_t session_id) override;
    bool BroadcastMsg(AFMsgHead* head, const char* msg_data) override;

    bool CloseSession(const AFGUID& session_id) override;

protected:
    bool SendMsgToAllClient(const char* msg, const size_t msg_len);
    bool SendMsg(const char* msg, const size_t msg_len, const AFGUID& session_id);

    bool AddNetSession(AFHttpSessionPtr session);
    AFHttpSessionPtr GetNetSession(const AFGUID& session_id);
    bool CloseSession(const AFHttpSessionPtr session);

    void UpdateNetSession();
    void UpdateNetEvent(AFHttpSessionPtr session);
    void UpdateNetMsg(AFHttpSessionPtr session);

    bool CloseAllSession();

private:
    std::map<int64_t, AFHttpSessionPtr> sessions_;
    AFCReaderWriterLock rw_lock_;
    int max_connection_{0};
    int thread_num_{0};
    int bus_id_{0};

    NET_MSG_FUNCTOR net_msg_cb_;
    NET_EVENT_FUNCTOR net_event_cb_;

    brynet::net::TcpService::Ptr tcp_service_ptr_{nullptr};
    brynet::net::wrapper::HttpListenerBuilder listen_builder_;
    std::atomic<std::uint64_t> trusted_session_id_{1};
};

} // namespace ark
