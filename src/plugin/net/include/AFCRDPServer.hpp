#pragma once

#include <zephyr/zephyr.hpp>

#include "base/AFUidGenerator.hpp"
#include "net/interface/AFINet.hpp"
#include "net/include/AFNetSession.hpp"

namespace ark {

class AFCRDPServer final
    : public AFINet
    , public std::enable_shared_from_this<AFCRDPServer>
    , private AFNoncopyable
{
    NET_EVENT_FUNCTOR net_event_cb_;
    NET_MSG_SESSION_FUNCTOR net_msg_cb_;

    AFReaderWriterLock rw_lock_;
    bus_id_t bus_id_{0};
    std::atomic<conv_id_t> trusted_session_id_{ARK_CONV_START_ID};
    std::map<conv_id_t, AFUDPSessionPtr> sessions_;

    std::shared_ptr<zephyr::udp_server> server_;

public:
    template<typename BaseType>
    AFCRDPServer(BaseType* pBaseType, void (BaseType::*handleRecv)(AFNetMsg const*, conv_id_t),
        void (BaseType::*handleEvent)(AFNetEvent const*))
    {
        net_msg_cb_ = std::bind(handleRecv, pBaseType, std::placeholders::_1, std::placeholders::_2);
        net_event_cb_ = std::bind(handleEvent, pBaseType, std::placeholders::_1);
    }

    ~AFCRDPServer() override;

    void Update() override;

    bool StartServer(std::shared_ptr<const AFIMsgHeader> head, bus_id_t bus_id, const std::string& ip, uint16_t port,
                     uint8_t thread_num, uint32_t max_client, const size_t silent_timeout) override;

    void Shutdown() final;
    bool SendMsg(AFIMsgHeader* head, const char* msg_data, conv_id_t session_id) override;
    void BroadcastMsg(AFIMsgHeader* header, const char* msg_data) override;

    void CloseSession(conv_id_t session_id) override;

    std::string GetIP(const conv_id_t session_id) override;

protected:
    void SendMsgToAll(const char* msg, size_t msg_len);
    bool SendMsg(const char* msg, size_t msg_len, conv_id_t session_id);

    bool AddNetSession(const AFUDPSessionPtr& session);
    void CloseSession(AFUDPSessionPtr& session);
    AFUDPSessionPtr GetNetSession(conv_id_t session_id);

    void UpdateNetSession();
    void UpdateNetEvent(const AFUDPSessionPtr& session);
    void UpdateNetMsg(const AFUDPSessionPtr& session);

    void CloseAllSession();
};

} // namespace ark