#include "base/AFLogger.hpp"
#include "net/include/AFCRDPServer.hpp"

namespace ark {

AFCRDPServer::~AFCRDPServer()
{
    Shutdown();
}

bool AFCRDPServer::StartServer(std::shared_ptr<const AFIMsgHeader> head, bus_id_t bus_id, const std::string& host,
    uint16_t port, uint8_t thread_num, uint32_t max_client, const size_t silent_timeout)
{
    using namespace zephyr;
    using namespace zephyr::detail;

    server_ = std::make_shared<zephyr::udp_server>(silent_timeout, udp_frame_size, ARK_TCP_RECV_BUFFER_SIZE);
    this->bus_id_ = bus_id;

    server_
        ->bind_start([self = shared_from_this()](asio::error_code ec) {
            std::ignore = ec;
            if (zephyr::get_last_error())
            {
                ARK_LOG_INFO(
                    "Server(UDP) start failed, error={}, errno={}", zephyr::last_error_msg(), zephyr::last_error_val());
            }
            else
            {
                ARK_LOG_INFO("Server(UDP) start success, listen_addr={} listen_port={}",
                    self->server_->listen_address(), self->server_->listen_port());
            }
        })
        .bind_stop([](asio::error_code ec) {
            std::ignore = ec;
            ARK_LOG_INFO("Server(UDP) stop, error={}, errno={}", zephyr::last_error_msg(), zephyr::last_error_val());
        })
        .bind_handshake([](std::shared_ptr<zephyr::udp_session>& session_ptr, asio::error_code ec) {
            if (zephyr::get_last_error())
            {
                ARK_LOG_INFO("Client(UDP) handshake failed, remote_addr={} remote_port={} error={} errno={}",
                    session_ptr->remote_address(), session_ptr->remote_port(), zephyr::last_error_msg(),
                    zephyr::last_error_val());
            }
            else
            {
                ARK_LOG_INFO("Client(UDP) handshake success, remote_addr={} remote_port={}",
                    session_ptr->remote_address(), session_ptr->remote_port());
            }
        })
        .bind_connect([self = shared_from_this(), head](std::shared_ptr<zephyr::udp_session>& session_ptr) {
            ARK_LOG_INFO("Client(UDP) connected, remote_addr={} remote_port={} local_addr={} local_port={}",
                session_ptr->remote_address(), session_ptr->remote_port(), session_ptr->local_address(),
                session_ptr->local_port());

            auto cur_session_id = self->trusted_session_id_++;
            session_ptr->user_data(cur_session_id);

            AFNetEvent* net_connect_event = AFNetEvent::AllocEvent();
            net_connect_event->SetId(cur_session_id);
            net_connect_event->SetType(AFNetEventType::CONNECT);
            net_connect_event->SetBusId(self->bus_id_);
            net_connect_event->SetIP(session_ptr->remote_address());
            // TODO: will add port

            // add session scope
            {
                AFScopeWLock guard(self->rw_lock_);
                AFUDPSessionPtr session = std::make_shared<AFUDPSession>(head, cur_session_id, session_ptr);
                if (self->AddNetSession(session))
                {
                    session->AddNetEvent(net_connect_event);
                }
            }
        })
        .bind_disconnect([self = shared_from_this()](std::shared_ptr<zephyr::udp_session>& session_ptr) {
            ARK_LOG_INFO("Client disconnected, remote_addr={} remote_port={} error={}", session_ptr->remote_address(),
                session_ptr->remote_port(), zephyr::last_error_msg());

            auto session_id = session_ptr->user_data<conv_id_t>();

            AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
            net_disconnect_event->SetId(session_id);
            net_disconnect_event->SetType(AFNetEventType::DISCONNECT);
            net_disconnect_event->SetBusId(self->bus_id_);
            net_disconnect_event->SetIP(session_ptr->remote_address());
            // TODO: will add port

            // remove session scope
            {
                AFScopeWLock xGuard(self->rw_lock_);
                AFUDPSessionPtr session = self->GetNetSession(session_id);
                if (session == nullptr)
                {
                    return;
                }

                session->AddNetEvent(net_disconnect_event);
                session->NeedRemove(true);
            }
        })
        .bind_recv([self = shared_from_this()](std::shared_ptr<zephyr::udp_session>& session_ptr, std::string_view s) {
            auto session_id = session_ptr->user_data<conv_id_t>();

            // process msg data scope
            {
                AFScopeRLock guard(self->rw_lock_);
                const AFUDPSessionPtr session = self->GetNetSession(session_id);
                if (session == nullptr)
                {
                    return size_t(0);
                }

                session->AddBuffer(s.data(), s.length());
                session->ParseBufferToMsg();
            }

            return s.length();
        });

    bool ret = server_->start(host, port, zephyr::use_kcp);
    SetWorking(ret);
    return ret;
}

void AFCRDPServer::Update()
{
    UpdateNetSession();
}

void AFCRDPServer::UpdateNetSession()
{
    std::vector<AFUDPSessionPtr> remove_sessions;

    {
        AFScopeRLock guard(rw_lock_);
        for (auto& iter : sessions_)
        {
            auto& session = iter.second;
            ARK_ASSERT_CONTINUE(session != nullptr);

            UpdateNetEvent(session);
            UpdateNetMsg(session);

            if (!session->NeedRemove())
            {
                continue;
            }

            remove_sessions.emplace_back(session);
        }
    }

    for (auto& iter : remove_sessions)
    {
        auto& session = iter;
        if (session == nullptr)
        {
            continue;
        }

        AFScopeWLock guard(rw_lock_);
        CloseSession(session);
    }
}

void AFCRDPServer::UpdateNetEvent(const AFUDPSessionPtr& session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    AFNetEvent* event{nullptr};
    while (session->PopNetEvent(event))
    {
        net_event_cb_(event);
        AFNetEvent::Release(event);
    }
}

void AFCRDPServer::UpdateNetMsg(const AFUDPSessionPtr& session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);

    AFNetMsg* msg{nullptr};
    int msg_count = 0;
    while (session->PopNetMsg(msg))
    {
        net_msg_cb_(msg, session->GetSessionId());
        AFNetMsg::Release(msg);

        ++msg_count;
        if (msg_count > ARK_PROCESS_NET_MSG_COUNT_ONCE)
        {
            break;
        }
    }
}

void AFCRDPServer::Shutdown()
{
    CloseAllSession();
    server_->stop();
    SetWorking(false);
}

bool AFCRDPServer::AddNetSession(const AFUDPSessionPtr& session)
{
    return sessions_.insert(std::make_pair(session->GetSessionId(), session)).second;
}

AFUDPSessionPtr AFCRDPServer::GetNetSession(conv_id_t session_id)
{
    auto iter = sessions_.find(session_id);
    return (iter != sessions_.end() ? iter->second : nullptr);
}

void AFCRDPServer::CloseSession(conv_id_t session_id)
{
    auto session = GetNetSession(session_id);
    if (session == nullptr)
    {
        return;
    }

    session->NeedRemove(true);
}

std::string AFCRDPServer::GetIP(const conv_id_t session_id)
{
    auto session = GetNetSession(session_id);
    if (session == nullptr)
    {
        return NULL_STR;
    }

    return session->GetSession()->remote_address();
}

void AFCRDPServer::CloseSession(AFUDPSessionPtr& session)
{
    ARK_ASSERT_RET_NONE(session != nullptr);
    sessions_.erase(session->GetSessionId());
}

void AFCRDPServer::CloseAllSession()
{
    sessions_.clear();
}

bool AFCRDPServer::SendMsg(AFIMsgHeader* head, const char* msg_data, conv_id_t session_id)
{
    ARK_ASSERT_RET_VAL(head != nullptr && msg_data != nullptr, false);
    auto session = GetNetSession(session_id);
    ARK_ASSERT_RET_VAL(session != nullptr, false);

    std::string buffer;
    head->SerializeToString(buffer);
    if (buffer.empty())
    {
        return false;
    }

    buffer.append(msg_data, head->MessageLength());
    return session->GetSession()->send(buffer.c_str(), buffer.length());
}

bool AFCRDPServer::SendMsg(const char* msg, size_t msg_len, conv_id_t session_id)
{
    AFScopeRLock xGuard(rw_lock_);
    auto session = GetNetSession(session_id);
    if (session == nullptr)
    {
        return false;
    }

    return session->GetSession()->send(msg, msg_len);
}

void AFCRDPServer::BroadcastMsg(AFIMsgHeader* head, const char* msg_data)
{
    ARK_ASSERT_RET_NONE(head != nullptr && msg_data != nullptr);

    if (sessions_.empty())
    {
        return;
    }

    auto it = sessions_.begin();
    auto first_session = it->second;
    if (first_session == nullptr)
    {
        return;
    }

    string buffer;
    head->SerializeToString(buffer);
    if (buffer.empty())
    {
        return;
    }

    for (const auto& iter : sessions_)
    {
        auto& session = iter.second;
        if (session != nullptr && !session->NeedRemove())
        {
            session->GetSession()->send(buffer.c_str(), buffer.length());
        }
    }
}

void AFCRDPServer::SendMsgToAll(const char* msg, size_t msg_len)
{
    AFScopeRLock guard(rw_lock_);
    for (const auto& it : sessions_)
    {
        auto& session = it.second;
        if (session != nullptr && !session->NeedRemove())
        {
            session->GetSession()->send(msg, msg_len);
        }
    }
}

} // namespace ark