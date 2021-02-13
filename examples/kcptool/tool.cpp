#include <stdio.h>
#include <kcp/ikcp.h>
#include <boost/asio.hpp>
#include "base/AFPlatform.hpp"
#include "base/container/AFLockFreeQueue.hpp"
#include "net/include/kcp_client.hpp"
#include "net/include/AFNetMsg.hpp"
#include "net/include/AFCKcpClient.hpp"

std::mutex mut;

struct Message
{
    int protocol;
    char* data;
};

class KcpClient
{
    static int base_id_;
    static std::unordered_map<int, KcpClient*> kcp_clients_map_;

public:
    using IdMsgPair = std::pair<int, std::string>;
    using MessageList = std::list<IdMsgPair>;
    MessageList msg_list;
    MessageList::iterator current_iter;

private:
    int client_id_;
    std::shared_ptr<ark::AFCKcpClient> kcp_client_ptr_;

public:
    KcpClient()
    {
        mut.lock();
        client_id_ = ++base_id_;
        kcp_clients_map_.insert(std::make_pair(client_id_, this));
        mut.unlock();

        kcp_client_ptr_ = std::make_shared<ark::AFCKcpClient>(this, &KcpClient::OnNetMsg, &KcpClient::OnNetEvent);
        current_iter = msg_list.begin();
    }

    void OnNetMsg(const ark::AFNetMsg* msg, ark::conv_id_t session_id)
    {
        msg_list.push_back(IdMsgPair(msg->GetMsgId(), std::string(msg->GetMsgData(), msg->GetMsgLength())));
    }

    void OnNetEvent(const ark::AFNetEvent* event) {}

    int ClientID()
    {
        return client_id_;
    }

    bool StartClient(const std::string& ip, const uint16_t port, int time_out)
    {
        if (!kcp_client_ptr_->StartClient(ark::AFHeaderLen::CS_HEAD_LENGTH, 0, ip, port, false))
        {
            return false;
        }

        for (int i = 0; i < time_out; ++i)
        {
            kcp_client_ptr_->Update();
            if (kcp_client_ptr_->Connected())
            {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        if (kcp_client_ptr_->Connected())
        {
            return true;
        }
        return false;
    };

    bool SendMsg(uint16_t msg_id, uint32_t length, const char* msg)
    {
        ark::AFMsgHeader header;
        header.id_ = msg_id;
        header.length_ = length;
        return kcp_client_ptr_->SendMsg(&header, msg, 0);
    }

    static KcpClient* GetClient(int client_id)
    {
        mut.lock();
        auto iter = kcp_clients_map_.find(client_id);
        if (iter == kcp_clients_map_.end())
        {
            mut.unlock();
            return nullptr;
        }

        mut.unlock();
        return iter->second;
    }

    bool Update(int time_out)
    {
        if (kcp_client_ptr_ == nullptr)
        {
            std::cout << "Please call StartClient first";
            return false;
        }
        if (current_iter != msg_list.end())
        {
            msg_list.erase(current_iter);
        }
        for (int i = 0; i < time_out; ++i)
        {
            kcp_client_ptr_->Update();
            if (!msg_list.empty())
            {
                return true;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        return false;
    }
};

int KcpClient::base_id_ = 0;

std::unordered_map<int, KcpClient*> KcpClient::kcp_clients_map_;

extern "C" {

int StartClient(char* ip, uint16_t port = 9001, int time_out = 10)
{

    KcpClient* client = new KcpClient;

    if (client->StartClient(ip, port, time_out))
    {
        return client->ClientID();
    }

    return -1;
}

bool SendMsg(int client_id, uint16_t msg_id, uint32_t length, const char* msg)
{
    auto client = KcpClient::GetClient(client_id);
    if (client == nullptr)
    {
        return false;
    }

    return client->SendMsg(msg_id, length, msg);
}

Message GetMessage(int client_id, int time_out = 50)
{
    Message msg;
    msg.protocol = 0;

    auto client = KcpClient::GetClient(client_id);
    if (client == nullptr)
    {
        return msg;
    }

    auto ret = client->Update(time_out);
    if (!ret)
    {
        std::cout << "Update fail" << std::endl;
        return msg;
    }

    if (client->msg_list.begin() == client->msg_list.end())
    {
        return msg;
    }

    client->current_iter = client->msg_list.begin();
    client->current_iter = client->msg_list.begin();
    msg.data = const_cast<char*>(client->current_iter->second.data());
    msg.protocol = client->current_iter->first;
    return msg;
}
}