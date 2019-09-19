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

#include "net/include/AFCNetServerService.hpp"
#include "net/include/AFCNetClientService.hpp"
#include "net/include/AFCNetServiceManagerModule.hpp"

namespace ark {

bool AFCNetServiceManagerModule::Init()
{
    m_pBusModule = FindModule<AFIBusModule>();
    m_pLogModule = FindModule<AFILogModule>();
    m_pConsulModule = FindModule<AFIConsulModule>();
    m_pTimerModule = FindModule<AFITimerModule>();

    return true;
}

bool AFCNetServiceManagerModule::PostInit()
{
    // set reg center info
    auto reg_center = m_pBusModule->GetRegCenter();
    m_pConsulModule->SetRegisterCenter(reg_center.ip, reg_center.port);

    // start health check timer
    m_pTimerModule->AddForeverTimer("service_discovery_health_check", 0, 20, this, &AFCNetServiceManagerModule::HealthCheck);

    return true;
}

void AFCNetServiceManagerModule::HealthCheck(const std::string& name, const AFGUID& entity_id)
{
    // check if I need to connect other servers from consul center.
    std::vector<ARK_APP_TYPE> target_list;
    m_pBusModule->GetTargetBusRelations(target_list);

    if (target_list.empty())
    {
        return;
    }

    std::vector<std::string> tag_filter_list;
    for (auto target : target_list)
    {
        auto& name = m_pBusModule->GetAppName(target);
        if (name.empty())
        {
            continue;
        }

        tag_filter_list.emplace_back(name);
    }

    if (tag_filter_list.empty())
    {
        return;
    }

    auto reg_center = m_pBusModule->GetRegCenter();

    //consulpp::ConsulServiceSet service_list;
    consul::service_set service_list;
    auto ret = m_pConsulModule->GetHealthServices(reg_center.service_name, tag_filter_list, service_list);
    if (!ret)
    {
        return;
    }

    // start clients
    //for (auto service : service_list)
    for (int index = 0; index < service_list.services_size(); ++index)
    {
        auto& service = service_list.services(index);

        auto& metas = service.metas();
        auto iter = metas.find("busid");
        if (iter == metas.end())
        {
            ARK_LOG_ERROR("service={} do not have Meta=busid, please check it.", service.name());
            continue;
        }

        AFBusAddr bus_addr;
        bus_addr.FromString(iter->second);

        // Check if already connected target server
        if (CheckConnectedTargetServer(bus_addr))
        {
            continue;
        }

        // TODO: will add a meta that represent the endpoint

        CreateClientService(bus_addr, service.ip(), service.port());
    }
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

        // unregister from consul
        DeregisterFromConsul(iter.first);

        //ARK_DELETE(pServerData);
    }

    for (auto iter : client_services_)
    {
        auto pData = iter.second;
        //ARK_DELETE(pData);
    }

    return true;
}

ananas::Future<std::pair<bool, std::string>> AFCNetServiceManagerModule::CreateServer(const AFHeadLength head_len /* = AFHeadLength::SS_HEAD_LENGTH*/)
{
    ananas::Promise<std::pair<bool, std::string>> promise;

    const AFProcConfig& self_proc = m_pBusModule->GetSelfProc();

    auto pServer = std::make_shared<AFCNetServerService>(GetPluginManager());
    bool ret = pServer->Start(head_len, self_proc.bus_id, self_proc.server_ep, self_proc.thread_num, self_proc.max_connection);
    if (ret)
    {
        ARK_LOG_INFO("Start net server successful, url = {}", self_proc.server_ep.ToString());
    }
    else
    {
        ARK_LOG_ERROR("Cannot start server net, url = {}", self_proc.server_ep.ToString());
        promise.SetValue(std::make_pair(false, std::string()));
        return promise.GetFuture();
    }

    server_services_.insert(m_pBusModule->GetSelfBusID(), pServer);

    // Register self to service discovery center(consul)
    return RegisterToConsul(m_pBusModule->GetSelfBusID());
}

std::shared_ptr<AFINetServerService> AFCNetServiceManagerModule::GetSelfNetServer()
{
    return server_services_.find_value(m_pBusModule->GetSelfBusID());
}

//int AFCNetServiceManagerModule::CreateClusterClients(const AFHeadLength head_len /* = AFHeadLength::SS_HEAD_LENGTH*/)
//{
//    //std::vector<AFServerConfig> target_list;
//    //if (!m_pBusModule->GetDirectBusRelations(target_list))
//    //{
//    //    return -1;
//    //}
//
//    //if (target_list.empty())
//    //{
//    //    return 0;
//    //}
//
//    //for (auto& target : target_list)
//    //{
//    //    ARK_APP_TYPE app_type = ARK_APP_TYPE(AFBusAddr(target.self_id).app_type);
//    //    AFINetClientService* pClient = net_clients_.find_value(app_type);
//    //    if (pClient == nullptr)
//    //    {
//    //        pClient = ARK_NEW AFCNetClientService(GetPluginManager());
//    //        net_clients_.insert(app_type, pClient);
//    //    }
//    //    bool ret = pClient->StartClient(head_len, target.self_id, target.intranet_ep_);
//    //    if (!ret)
//    //    {
//    //        ARK_LOG_ERROR("start net client failed, self_bus_id={} target_url={}", m_pBusModule->GetSelfBusName(), target.intranet_ep_.ToString());
//    //        return ret;
//    //    }
//    //}
//
//    return 0;
//}

//int AFCNetServiceManagerModule::CreateClusterClient(const AFHeadLength head_len, const int bus_id, const std::string& url)
//{
//    //ArkConnectionType connection_type = m_pBusModule->GetConnectionType(bus_id);
//    //if (connection_type != ArkConnectionType::BRT_DIRECT)
//    //{
//    //    return 0;
//    //}
//
//    //ARK_APP_TYPE app_type = ARK_APP_TYPE(AFBusAddr(bus_id).app_type);
//    //AFINetClientService* pClient = net_clients_.find_value(app_type);
//    //if (pClient == nullptr)
//    //{
//    //    pClient = ARK_NEW AFCNetClientService(GetPluginManager());
//    //    net_clients_.insert(app_type, pClient);
//    //}
//
//    //std::error_code ec;
//    //AFEndpoint target_ep = AFEndpoint::FromString(url, ec);
//    //bool ret = pClient->StartClient(head_len, bus_id, target_ep);
//    //if (!ret)
//    //{
//    //    ARK_LOG_ERROR("start net client failed, self_bus_id={} target_url={}", m_pBusModule->GetSelfBusName(), url);
//    //}
//
//    //return ret;
//
//    return 0;
//}

//AFINetClientService* AFCNetServiceManagerModule::GetNetClientService(const ARK_APP_TYPE& app_type)
//{
//    return net_clients_.find_value(app_type);
//}
//
//AFINetClientService* AFCNetServiceManagerModule::GetNetClientServiceByBusID(const int bus_id)
//{
//    AFBusAddr addr(bus_id);
//    return GetNetClientService(ARK_APP_TYPE(addr.app_type));
//}

bool AFCNetServiceManagerModule::AddNetConnectionBus(int client_bus_id, AFINet* net_ptr)
{
    if (client_bus_id <= 0 || net_ptr == nullptr)
    {
        return false;
    }

    int self_bus_id = m_pBusModule->GetSelfBusID();
    return net_bus_relations_.insert(std::make_pair(self_bus_id, client_bus_id), net_ptr).second;
}

bool AFCNetServiceManagerModule::RemoveNetConnectionBus(int client_bus_id)
{
    if (client_bus_id <= 0)
    {
        return false;
    }

    int self_bus_id = m_pBusModule->GetSelfBusID();
    net_bus_relations_.erase(std::make_pair(self_bus_id, client_bus_id));
    return true;
}

AFINet* AFCNetServiceManagerModule::GetNetConnectionBus(int src_bus, int target_bus)
{
    return net_bus_relations_.find_value(std::make_pair(src_bus, target_bus));
}

ananas::Future<std::pair<bool, std::string>> AFCNetServiceManagerModule::RegisterToConsul(const int bus_id)
{
    auto reg_center = m_pBusModule->GetRegCenter();
    AFBusAddr bus(bus_id);
    auto bus_name = bus.ToString();

    const std::string app_whole_name = m_pBusModule->GetAppWholeName(bus.bus_id);
    const std::string app_name = m_pBusModule->GetAppName(static_cast<ARK_APP_TYPE>(bus.app_type));

    const AFProcConfig& self_proc = m_pBusModule->GetSelfProc();

    //consulpp::ConsulService service;
    //service.SetId(app_whole_name);
    //service.SetName(reg_center.service_name);
    //service.SetAddress(self_proc.intranet_ep.GetIP());
    //service.SetPort(self_proc.intranet_ep.GetPort());
    //// multi-tag
    //service.SetTag(app_name);
    //service.SetTag(bus_name);
    //// multi-meta
    //service.SetMeta("busid", bus_name);

    //consulpp::ConsulCheck check;
    //check.SetId(app_whole_name + "_check");
    //check.SetName(app_whole_name);
    //check.SetTcp(self_proc.intranet_ep.GetIP() + ":" + ARK_TO_STRING(self_proc.intranet_ep.GetPort()));
    //check.SetInterval(reg_center.check_interval);
    //check.SetTimeout(reg_center.check_timeout);
    //service.SetCheck(check);

    consul::service_data service;
    service.set_id(app_whole_name);
    service.set_name(reg_center.service_name);
    service.set_ip(self_proc.intranet_ep.GetIP());
    service.set_port(self_proc.intranet_ep.GetPort());

    // multi-tag
    service.add_tags(app_name);
    service.add_tags(bus_name);

    // multi-meta
    service.mutable_metas()->insert({"busid", bus_name});

    // health check
    auto check = service.add_checks();
    check->set_id(app_whole_name + "_check");
    check->set_name(app_whole_name);
    check->set_tcp(self_proc.intranet_ep.GetIP() + ":" + ARK_TO_STRING(self_proc.intranet_ep.GetPort()));
    check->set_interval(reg_center.check_interval);
    check->set_timeout(reg_center.check_timeout);

    return m_pConsulModule->RegisterService(service);
}

int AFCNetServiceManagerModule::DeregisterFromConsul(const int bus_id)
{
    auto ret = m_pConsulModule->DeregisterService(m_pBusModule->GetAppWholeName(bus_id));
    ret.Then([=](const std::pair<bool, std::string>& resp) {
        if (!resp.first)
        {
            ARK_LOG_ERROR("DeregisterFromConsul failed, busid={}", AFBusAddr(bus_id).ToString());
        }
    });

    return 0;
}

bool AFCNetServiceManagerModule::CreateClientService(const AFBusAddr& bus_addr, const std::string& ip, uint16_t port)
{
    ARK_APP_TYPE app_type = ARK_APP_TYPE(bus_addr.app_type);
    auto pClient = client_services_.find_value(app_type);
    if (pClient == nullptr)
    {
        pClient = std::make_shared<AFCNetClientService>(GetPluginManager());
        client_services_.insert(app_type, pClient);
    }

    AFEndpoint target_ep;
    target_ep.SetIsV6(false);
    target_ep.SetProtocol(proto_type::tcp);
    target_ep.SetIP(ip);
    target_ep.SetPort(port);
    bool ret = pClient->StartClient(AFHeadLength::SS_HEAD_LENGTH, bus_addr.bus_id, target_ep);
    if (!ret)
    {
        ARK_LOG_ERROR("start net client failed, self_bus_id={} target_url={}", m_pBusModule->GetSelfBusName(), target_ep.ToString());
        return ret;
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

} // namespace ark
