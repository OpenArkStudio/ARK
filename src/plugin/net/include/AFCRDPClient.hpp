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

#include "base/AFPlatform.hpp"
#include "net/interface/AFINet.hpp"
#include "net/include/AFNetDefine.hpp"
#include "net/include/AFNetSession.hpp"

namespace ark {

class AFCRDPClient final
    : public AFINet
    , public std::enable_shared_from_this<AFCRDPClient>
    , private AFNoncopyable

{
private:
    // data
    bus_id_t dst_bus_id_{0};
    AFReaderWriterLock rw_lock_;

    NET_MSG_FUNCTOR net_msg_cb_;
    NET_EVENT_FUNCTOR net_event_cb_;

    std::atomic<conv_id_t> trust_session_id_{ARK_CONV_START_ID};

    AFRDPClientSessionPtr session_;
    std::shared_ptr<zephyr::udp_client> client_;

public:
    template<typename BaseType>
    AFCRDPClient(BaseType* pBaseType, void (BaseType::*handleRecv)(AFNetMsg const*, conv_id_t),
        void (BaseType::*handleEvent)(AFNetEvent const*))
    {
        net_msg_cb_ = std::bind(handleRecv, pBaseType, std::placeholders::_1, std::placeholders::_2);
        net_event_cb_ = std::bind(handleEvent, pBaseType, std::placeholders::_1);
    }

    ~AFCRDPClient() override;

    void Update() override;
    bool StartClient(std::shared_ptr<const AFIMsgHeader>, bus_id_t dst_busid, const std::string& host, uint16_t port) override;

    void Shutdown() override;

    bool SendMsg(AFIMsgHeader* head, const char* msg_data, conv_id_t session_id) override;
    void CloseSession(conv_id_t session_id) override;

protected:
    void UpdateNetEvent();
    void UpdateNetMsg();
};

} // namespace ark