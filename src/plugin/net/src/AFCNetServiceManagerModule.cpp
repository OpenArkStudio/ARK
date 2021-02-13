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

#include <json/json.hpp>

#include "net/include/AFCNetServerService.hpp"
#include "net/include/AFCNetClientService.hpp"
#include "net/include/AFCNetServiceManagerModule.hpp"

namespace ark {

bool AFCNetServiceManagerModule::Init()
{
    m_pBusModule = FindModule<AFIBusModule>();
    m_pTimerModule = FindModule<AFITimerModule>();

    return true;
}

bool AFCNetServiceManagerModule::PostInit()
{
    // start health check timer
    m_pTimerModule->AddForeverTimer(0, std::chrono::seconds(20), this, &AFCNetServiceManagerModule::HealthCheck);

    return true;
}

void AFCNetServiceManagerModule::HealthCheck(uint64_t timer_id, const guid_t& entity_id)
{
    // TODO: health checking, now using etcd just need to watch.

    //    // check if I need to connect other servers from consul center.
    //    std::vector<ARK_APP_TYPE> target_list;
    //    m_pBusModule->GetTargetBusRelations(target_list);
    //
    //    if (target_list.empty())
    //    {
    //        return;
    //    }
    //
    //    auto reg_center = m_pBusModule->GetRegCenter();
    //    for (auto target : target_list)
    //    {
    //        const auto& name = m_pBusModule->GetAppName(target);
    //        if (name.empty())
    //        {
    //            continue;
    //        }
    //
    //        auto ret = m_pConsulModule->GetHealthServices(reg_center.service_name, name);
    //        ret.Then([=](const std::pair<bool, std::string>& response) {
    //            if (!response.first)
    //            {
    //                return;
    //            }
    //
    //            using json = nlohmann::json;
    //
    //            json resp_json = json::parse(response.second);
    //            if (!resp_json.is_array())
    //            {
    //                return;
    //            }
    //
    //            for (json::iterator iter = resp_json.begin(); iter != resp_json.end(); ++iter)
    //            {
    //                auto& health_check_json = iter.value();
    //                auto& service_json = health_check_json["Service"];
    //                if (!service_json.is_object())
    //                {
    //                    continue;
    //                }
    //
    //                consul::service_data service_msg;
    //                google::protobuf::util::JsonParseOptions option;
    //                option.ignore_unknown_fields = true;
    //                auto status = google::protobuf::util::JsonStringToMessage(service_json.dump(), &service_msg, option);
    //                if (!status.ok())
    //                {
    //                    continue;
    //                }
    //
    //                auto find_iter = service_msg.meta().find("busid");
    //                if (find_iter == service_msg.meta().end())
    //                {
    //                    ARK_LOG_ERROR("service={} do not have Meta=busid, please check it.", service_msg.id());
    //                    return;
    //                }
    //
    //                AFBusAddr bus_addr;
    //                bus_addr.FromString(find_iter->second);
    //                // Check if already connected target server
    //                if (CheckConnectedTargetServer(bus_addr))
    //                {
    //                    return;
    //                }
    //
    //                // TODO: NickYang
    //                // Will add a meta that represent the endpoint
    //                CreateClientService(bus_addr, proto_type::tcp, service_msg.address(), service_msg.port());
    //            }
    //        });
    //    }
}

bool AFCNetServiceManagerModule::Update()
{
    for (auto iter : server_services_)
    {
        auto pServerData = iter.second;
        if (pServerData != nullptr)
        {
            pServerData->Update();
        }
    }

    for (auto iter : intra_services_)
    {
        auto pServerData = iter.second;
        if (pServerData != nullptr)
        {
            pServerData->Update();
        }
    }

    for (auto iter : client_services_)
    {
        auto pData = iter.second;
        if (pData != nullptr)
        {
            pData->Update();
        }
    }

    return true;
}

bool AFCNetServiceManagerModule::Shut()
{
    for (auto iter : server_services_)
    {
        auto pServerData = iter.second;

        //        // deregister from consul
        //        DeregisterFromConsul(iter.first);
    }

    for (auto iter : client_services_)
    {
        auto pData = iter.second;
    }

    return true;
}

bool AFCNetServiceManagerModule::CreateServer(const size_t silent_timeout, const AFIMsgHeader& csReqHeader, const AFIMsgHeader& ssReqHeader)
{
    //    ananas::Promise<std::pair<bool, std::string>> promise;

    shared_ptr<const AFIMsgHeader> cs_req_head(csReqHeader.New());
    shared_ptr<const AFIMsgHeader> ss_req_head(ssReqHeader.New());

    const AFProcConfig& self_proc = m_pBusModule->GetSelfProc();
    auto func = [this, &self_proc, silent_timeout](
                    const AFEndpoint& ep, shared_ptr<const AFIMsgHeader> header, ServerService& server_service) {
        auto pServer = std::make_shared<AFCNetServerService>(GetPluginManager());
        bool ret = pServer->Start(header, self_proc.bus_id, ep, self_proc.thread_num, self_proc.max_connection, silent_timeout);
        if (ret)
        {
            auto pIServer = static_pointer_cast<AFINetServerService>(pServer);
            pIServer->RegRegServerCallBack(this, &AFCNetServiceManagerModule::OnRegServerCallBack);
            server_service.insert(m_pBusModule->GetSelfBusID(), pIServer);

            ARK_LOG_INFO("Start net server successful, url = {}", ep.ToString());
            return true;
        }
        else
        {
            ARK_LOG_ERROR("Cannot start server net, url = {}", ep.ToString());
            return false;
        }
    };

    if (!self_proc.server_ep.GetIP().empty())
    {
        // create inter server
        auto result = func(self_proc.server_ep, cs_req_head, server_services_);
        if (!result)
        {
            ARK_LOG_ERROR("Cannot start inter server net, url = {}", self_proc.server_ep.ToString());
            //        promise.SetValue(std::make_pair(false, std::string()));
            //        return promise.GetFuture();
            return false;
        }
    }

    auto& intra_ep = self_proc.intranet_ep;
    if (intra_ep.GetPort() == self_proc.server_ep.GetPort())
    {
        ARK_LOG_ERROR(
            "Cannot start intra server net, has the same port with server, url = {}", intra_ep.ToString());
        //            promise.SetValue(std::make_pair(false, std::string()));
        //            return promise.GetFuture();
        return false;
    }
    else
    {
	    if (!intra_ep.GetIP().empty())
	    {
            // create intra server
            auto result = func(intra_ep, ss_req_head, intra_services_);
            if (!result)
            {
                ARK_LOG_ERROR("Cannot start intra server net, url = {}", intra_ep.ToString());
                //            promise.SetValue(std::make_pair(false, std::string()));
                //            return promise.GetFuture();
                return false;
            }
	    }
    }

    // Register self to service discovery center(consul)
    //return RegisterToConsul(m_pBusModule->GetSelfBusID());
    return true;
}

std::shared_ptr<AFINetServerService> AFCNetServiceManagerModule::GetInterNetServer()
{
    return server_services_.find_value(m_pBusModule->GetSelfBusID());
}

std::shared_ptr<AFINetServerService> AFCNetServiceManagerModule::GetIntraNetServer()
{
    return intra_services_.find_value(m_pBusModule->GetSelfBusID());
}

bool AFCNetServiceManagerModule::AddNetConnectionBus(bus_id_t target_bus_id, std::shared_ptr<AFINet> net_ptr)
{
    if (/*target_bus_id <= 0 || */net_ptr == nullptr)
    {
        return false;
    }

    int self_bus_id = m_pBusModule->GetSelfBusID();
    return net_bus_relations_.insert(std::make_pair(self_bus_id, target_bus_id), net_ptr).second;
}

bool AFCNetServiceManagerModule::RemoveNetConnectionBus(bus_id_t target_bus_id)
{
    //if (target_bus_id <= 0)
    //{
    //    return false;
    //}

    int self_bus_id = m_pBusModule->GetSelfBusID();
    net_bus_relations_.erase(std::make_pair(self_bus_id, target_bus_id));
    return true;
}

std::shared_ptr<AFINet> AFCNetServiceManagerModule::GetNetConnectionBus(bus_id_t src_bus, bus_id_t target_bus)
{
    return net_bus_relations_.find_value(std::make_pair(src_bus, target_bus));
}

std::shared_ptr<AFINetClientService> AFCNetServiceManagerModule::GetClientService(const ARK_APP_TYPE app_type)
{
    return client_services_.find_value(app_type);
}

//ananas::Future<std::pair<bool, std::string>> AFCNetServiceManagerModule::RegisterToConsul(bus_id_t bus_id)
//{
//    auto reg_center = m_pBusModule->GetRegCenter();
//    AFBusAddr bus(bus_id);
//    auto bus_name = bus.ToString();
//
//    const std::string app_whole_name = m_pBusModule->GetAppWholeName(bus.bus_id);
//    const std::string app_name = m_pBusModule->GetAppName(static_cast<ARK_APP_TYPE>(bus.app_type));
//
//    const AFProcConfig& self_proc = m_pBusModule->GetSelfProc();
//
//    consul::service_data service;
//    service.set_id(app_whole_name);
//    service.set_name(reg_center.service_name);
//    service.set_address(self_proc.intranet_ep.GetIP());
//    service.set_port(self_proc.intranet_ep.GetPort());
//
//    // multi-tag
//    service.add_tags(app_name);
//    service.add_tags(bus_name);
//
//    // multi-meta
//    service.mutable_meta()->insert({"busid", bus_name});
//
//    // health check
//    auto check = service.mutable_check();
//    check->set_id(app_whole_name + "_check");
//    check->set_name(app_whole_name);
//    check->set_tcp(self_proc.intranet_ep.GetIP() + ":" + ARK_TO_STRING(self_proc.intranet_ep.GetPort()));
//    check->set_interval(reg_center.check_interval);
//    check->set_timeout(reg_center.check_timeout);
//
//    return m_pConsulModule->RegisterService(service);
//}
//
//int AFCNetServiceManagerModule::DeregisterFromConsul(bus_id_t bus_id)
//{
//    auto ret = m_pConsulModule->DeregisterService(m_pBusModule->GetAppWholeName(bus_id));
//    ret.Then([=](const std::pair<bool, std::string>& response) {
//        if (!response.first)
//        {
//            ARK_LOG_ERROR(
//                "DeregisterFromConsul failed, busid={}, error={}", AFBusAddr(bus_id).ToString(), response.second);
//        }
//    });
//
//    return 0;
//}

bool AFCNetServiceManagerModule::CreateClientService(const AFBusAddr& bus_addr, proto_type proto, const std::string& ip,
    uint16_t port, std::shared_ptr<const AFIMsgHeader> head)
{
    ARK_APP_TYPE app_type = ARK_APP_TYPE(bus_addr.app_type);
    auto pClient = client_services_.find_value(app_type);
    if (pClient == nullptr)
    {
        pClient = std::make_shared<AFCNetClientService>(GetPluginManager());
        client_services_.insert(app_type, pClient);
    }

    AFEndpoint target_ep;
    target_ep.SetProtocol(proto);
    target_ep.SetIP(ip);
    target_ep.SetPort(port);

#ifdef ARK_DOCKER_ALPINE
    // TODO: need remove, now zephyr support domain.
    auto GetIP = [](std::string strHostName) -> std::string {
        if (strHostName.empty())
        {
            return NULL_STR;
        }

        hostent* pHost = gethostbyname(strHostName.c_str());
        if (!pHost)
        {
            return NULL_STR;
        }

        if (pHost->h_addr_list[0])
        {
            char strIp[32] = {0};
            inet_ntop(pHost->h_addrtype, pHost->h_addr_list[0], strIp, sizeof(strIp));
            return std::string(strIp);
        }

        return NULL_STR;
    };
    target_ep.SetIP(GetIP(ip));
#endif

    bool ret = pClient->StartClient(head, bus_addr.bus_id, target_ep);
    if (!ret)
    {
        ARK_LOG_ERROR("start net client failed, self_bus_id={} target_url={}", m_pBusModule->GetSelfBusName(),
            target_ep.ToString());
        return ret;
    }

    ARK_LOG_INFO("start net client successful, self_bus_id={} target_url={}", m_pBusModule->GetSelfBusName(),
        target_ep.ToString());
    return true;
}

conv_id_t AFCNetServiceManagerModule::GetSessionID(bus_id_t target_bus_id)
{
    auto iter = bus_session_list_.find(target_bus_id);
    if (iter == bus_session_list_.end())
    {
        return 0;
    }

    return iter->second;
}

bool AFCNetServiceManagerModule::RemoveSessionID(conv_id_t session_id)
{
    for (auto& iter : bus_session_list_)
    {
        if (iter.second == session_id)
        {
            bus_session_list_.erase(iter.first);
            break;
        }
    }

    return true;
}

bool AFCNetServiceManagerModule::CheckConnectedTargetServer(const AFBusAddr& bus_addr)
{
    return (GetTargetClientConnection(bus_addr) != nullptr);
}

std::shared_ptr<AFConnectionData> AFCNetServiceManagerModule::GetTargetClientConnection(const AFBusAddr& bus_addr)
{
    auto client_service = client_services_.find_value(static_cast<ARK_APP_TYPE>(bus_addr.app_type));
    if (client_service == nullptr)
    {
        return nullptr;
    }

    return client_service->GetConnectionInfo(bus_addr.bus_id);
}

void AFCNetServiceManagerModule::OnRegServerCallBack(const AFNetMsg* msg, conv_id_t session_id)
{
    // msg->GetSrcBus() means client msg bus id
    auto iter = bus_session_list_.find(msg->GetSrcBus());
    if (iter == bus_session_list_.end())
    {
        bus_session_list_.insert(std::make_pair(msg->GetSrcBus(), session_id));
    }
    else
    {
        iter->second = session_id;
    }
}

} // namespace ark
