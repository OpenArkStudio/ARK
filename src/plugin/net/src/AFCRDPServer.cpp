#include <chrono>
#include <vector>

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
    if (!head || !max_client)
    {
        return false;
    }
    Shutdown();
    server_ = std::make_shared<arknet::rudp_server>(65536, ARK_TCP_RECV_BUFFER_SIZE, thread_num == 0 ? 1 : thread_num);
    this->bus_id_ = bus_id;
    arknet::arq::options transport_options;
    transport_options.mtu = 1400;
    transport_options.congestion_control = false;
    server_->set_rudp_options(transport_options).set_max_sessions(max_client);
    auto weak = weak_from_this();

    server_
        ->bind_start([weak] {
            const auto ec = arknet::get_last_error();
            auto self = weak.lock();
            if (!self)
            {
                return;
            }
            if (ec)
            {
                ARK_LOG_INFO("Server(RUDP) start failed, error={}, errno={}", ec.message(), ec.value());
            }
            else
            {
                ARK_LOG_INFO("Server(RUDP) start success, listen_addr={} listen_port={}",
                    self->server_->listen_address(), self->server_->listen_port());
            }
        })
        .bind_stop([] {
            const auto ec = arknet::get_last_error();
            ARK_LOG_INFO("Server(RUDP) stop, error={}, errno={}", ec.message(), ec.value());
        })
        .bind_handshake([](std::shared_ptr<arknet::rudp_session>& session_ptr, arknet::error_code ec) {
            if (ec)
            {
                ARK_LOG_INFO("Client(RUDP) handshake failed, remote_addr={} remote_port={} error={} errno={}",
                    session_ptr->remote_address(), session_ptr->remote_port(), ec.message(), ec.value());
            }
            else
            {
                ARK_LOG_INFO("Client(RUDP) handshake success, remote_addr={} remote_port={}",
                    session_ptr->remote_address(), session_ptr->remote_port());
            }
        })
        .bind_connect([weak, head, silent_timeout](std::shared_ptr<arknet::rudp_session>& session_ptr) {
            auto self = weak.lock();
            if (!self)
            {
                session_ptr->stop();
                return;
            }
            session_ptr->set_silence_timeout(std::chrono::milliseconds(silent_timeout));
            ARK_LOG_INFO("Client(RUDP) connected, remote_addr={} remote_port={} local_addr={} local_port={}",
                session_ptr->remote_address(), session_ptr->remote_port(), session_ptr->local_address(),
                session_ptr->local_port());

            auto cur_session_id = self->trusted_session_id_++;
            session_ptr->set_user_data(cur_session_id);

            AFNetEvent* net_connect_event = AFNetEvent::AllocEvent();
            net_connect_event->SetId(cur_session_id);
            net_connect_event->SetType(AFNetEventType::CONNECT);
            net_connect_event->SetBusId(self->bus_id_);
            net_connect_event->SetIP(session_ptr->remote_address());
            AFUDPSessionPtr session = std::make_shared<AFUDPSession>(head, cur_session_id, session_ptr);
            if (self->AddNetSession(session))
            {
                if (!session->AddNetEvent(net_connect_event))
                {
                    AFNetEvent::Release(net_connect_event);
                }
            }
            else
            {
                AFNetEvent::Release(net_connect_event);
                session_ptr->stop();
            }
        })
        .bind_disconnect([weak](std::shared_ptr<arknet::rudp_session>& session_ptr) {
            const auto ec = arknet::get_last_error();
            auto self = weak.lock();
            if (!self)
            {
                return;
            }
            ARK_LOG_INFO("Client(RUDP) disconnected, remote_addr={} remote_port={} error={}",
                session_ptr->remote_address(), session_ptr->remote_port(), ec.message());

            auto session_id = session_ptr->get_user_data<conv_id_t>();
            auto session = self->GetNetSession(session_id);
            if (!session || session->NeedRemove())
            {
                return;
            }

            AFNetEvent* net_disconnect_event = AFNetEvent::AllocEvent();
            net_disconnect_event->SetId(session_id);
            net_disconnect_event->SetType(AFNetEventType::DISCONNECT);
            net_disconnect_event->SetBusId(self->bus_id_);
            net_disconnect_event->SetIP(session_ptr->remote_address());
            if (!session->AddNetEvent(net_disconnect_event))
            {
                AFNetEvent::Release(net_disconnect_event);
            }
            session->NeedRemove(true);
        })
        .bind_recv([weak](std::shared_ptr<arknet::rudp_session>& session_ptr, std::string_view s) {
            auto self = weak.lock();
            if (!self)
            {
                return;
            }
            auto session = self->GetNetSession(session_ptr->get_user_data<conv_id_t>());
            if (session && !session->NeedRemove())
            {
                session->AddBuffer(s.data(), s.length());
                if (!session->ParseBufferToMsg())
                {
                    session_ptr->stop();
                }
            }
        });

    bool ret = server_->start(host, port);
    SetWorking(ret);
    return ret;
}

void AFCRDPServer::Update()
{
    UpdateNetSession();
}

void AFCRDPServer::UpdateNetSession()
{
    std::vector<AFUDPSessionPtr> sessions;
    {
        AFScopeRLock guard(rw_lock_);
        for (auto& iter : sessions_)
        {
            sessions.push_back(iter.second);
        }
    }
    for (auto& session : sessions)
    {
        const auto disconnected = session->NeedRemove();
        UpdateNetEvent(session);
        const auto drained = UpdateNetMsg(session);
        if (disconnected && drained)
        {
            AFScopeWLock guard(rw_lock_);
            CloseSession(session);
        }
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

bool AFCRDPServer::UpdateNetMsg(const AFUDPSessionPtr& session)
{
    AFNetMsg* msg{nullptr};
    int msg_count = 0;
    while (session->PopNetMsg(msg))
    {
        net_msg_cb_(msg, session->GetSessionId());
        AFNetMsg::Release(msg);

        ++msg_count;
        if (msg_count >= ARK_PROCESS_NET_MSG_COUNT_ONCE)
        {
            return false;
        }
    }
    return true;
}

void AFCRDPServer::Shutdown()
{
    if (server_)
    {
        server_->stop();
    }
    SetWorking(false);
}

bool AFCRDPServer::AddNetSession(const AFUDPSessionPtr& session)
{
    AFScopeWLock guard(rw_lock_);
    return sessions_.insert(std::make_pair(session->GetSessionId(), session)).second;
}

AFUDPSessionPtr AFCRDPServer::GetNetSession(conv_id_t session_id)
{
    AFScopeRLock guard(rw_lock_);
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

    session->GetSession()->stop();
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

bool AFCRDPServer::SendMsg(AFIMsgHeader* head, const char* msg_data, conv_id_t session_id)
{
    if (!head || !msg_data || head->MessageLength() > ARK_MSG_MAX_LENGTH)
    {
        return false;
    }

    std::string buffer;
    head->SerializeToString(buffer);
    if (buffer.empty())
    {
        return false;
    }

    buffer.append(msg_data, head->MessageLength());
    return SendMsg(buffer.data(), buffer.length(), session_id);
}

bool AFCRDPServer::SendMsg(const char* msg, size_t msg_len, conv_id_t session_id)
{
    auto session = GetNetSession(session_id);
    if (session == nullptr || session->NeedRemove())
    {
        return false;
    }

    auto transport = session->GetSession();
    return transport->async_send(msg, msg_len,
        [weak = std::weak_ptr<arknet::rudp_session>(transport), session_id](arknet::error_code ec, size_t) {
            if (!ec)
            {
                return;
            }
            ARK_LOG_ERROR(
                "Server(RUDP) send failed, session={} error={} errno={}", session_id, ec.message(), ec.value());
            if (auto current = weak.lock())
            {
                current->stop();
            }
        });
}

void AFCRDPServer::BroadcastMsg(AFIMsgHeader* head, const char* msg_data)
{
    if (!head || !msg_data || head->MessageLength() > ARK_MSG_MAX_LENGTH)
    {
        return;
    }

    std::string buffer;
    head->SerializeToString(buffer);
    if (buffer.empty())
    {
        return;
    }

    buffer.append(msg_data, head->MessageLength());
    SendMsgToAll(buffer.data(), buffer.length());
}

void AFCRDPServer::SendMsgToAll(const char* msg, size_t msg_len)
{
    std::vector<conv_id_t> session_ids;
    {
        AFScopeRLock guard(rw_lock_);
        for (const auto& [id, session] : sessions_)
        {
            if (!session->NeedRemove())
            {
                session_ids.push_back(id);
            }
        }
    }
    for (auto id : session_ids)
    {
        SendMsg(msg, msg_len, id);
    }
}

} // namespace ark
