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

#include <zephyr/zephyr.hpp>

#include "net/interface/AFINet.hpp"
#include "net/include/AFNetSession.hpp"

namespace ark {

class AFCWebSocketServer final
    : public AFINet
    , public AFNoncopyable
    , public std::enable_shared_from_this<AFCWebSocketServer>
{
public:
    template<typename BaseType>
    AFCWebSocketServer(BaseType* pBaseType, void (BaseType::*handleReceive)(const AFNetMsg*, conv_id_t),
        void (BaseType::*handleEvent)(const AFNetEvent*))
    {
        net_msg_cb_ = std::bind(handleReceive, pBaseType, std::placeholders::_1, std::placeholders::_2);
        net_event_cb_ = std::bind(handleEvent, pBaseType, std::placeholders::_1);
    }

    ~AFCWebSocketServer() override;

    void Update() override;

    bool StartServer(std::shared_ptr<const AFIMsgHeader> head, bus_id_t bus_id, const std::string& host, uint16_t port, uint8_t thread_num,
        uint32_t max_client, const size_t silent_timeout) override;
    void Shutdown() final;

    bool SendMsg(AFIMsgHeader* head, const char* msg_data, conv_id_t session_id) override;
    void BroadcastMsg(AFIMsgHeader* head, const char* msg_data) override;

    void CloseSession(conv_id_t session_id) override;

protected:
    void SendMsgToAllClient(const char* msg, size_t msg_len);
    bool SendMsg(const char* msg, size_t msg_len, conv_id_t session_id);

    bool AddNetSession(const AFWSSessionPtr& session);
    AFWSSessionPtr GetNetSession(conv_id_t session_id);
    void CloseSession(const AFWSSessionPtr& session);

    void UpdateNetSession();
    void UpdateNetEvent(const AFWSSessionPtr& session);
    void UpdateNetMsg(const AFWSSessionPtr& session);

    void CloseAllSession();

private:
    std::map<conv_id_t, AFWSSessionPtr> sessions_;
    AFReaderWriterLock rw_lock_;
    bus_id_t bus_id_{0};

    NET_MSG_FUNCTOR net_msg_cb_;
    NET_EVENT_FUNCTOR net_event_cb_;

    std::atomic<conv_id_t> trusted_session_id_{ARK_CONV_START_ID};
    std::shared_ptr<zephyr::ws_server> server_;
};

} // namespace ark
