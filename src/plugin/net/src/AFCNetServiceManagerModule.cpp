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

    consulpp::ConsulServiceSet service_list;
    auto ret = m_pConsulModule->GetHealthServices(reg_center.service_name, tag_filter_list, service_list);
    if (!ret)
    {
        return;
    }

    // start clients
    for (auto service : service_list)
    {
        auto& metas = service.GetMeta();
        auto iter = metas.find("busid");
        if (iter == metas.end())
        {
            ARK_LOG_ERROR("service={} do not have Meta=busid, please check it.", service.GetName());
            continue;
        }

        AFBusAddr bus_addr;
        bus_addr.FromString(iter->second);

        // TODO: check if connected

        CreateClientService(bus_addr, service.GetAddress(), service.GetPort());
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

    for (auto iter : net_clients_)
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

        ARK_DELETE(pServerData);
    }

    for (auto iter : net_clients_)
    {
        auto pData = iter.second;
        ARK_DELETE(pData);
    }

    return true;
}

int AFCNetServiceManagerModule::CreateServer(const AFHeadLength head_len /* = AFHeadLength::SS_HEAD_LENGTH*/)
{
    const AFProcConfig& self_proc = m_pBusModule->GetSelfProc();
    //TODO: change to shared_ptr
    AFINetServerService* pServer = ARK_NEW AFCNetServerService(GetPluginManager());
    bool ret = pServer->Start(head_len, self_proc.bus_id, self_proc.server_ep, self_proc.thread_num, self_proc.max_connection);
    if (ret)
    {
        ARK_LOG_INFO("Start net server successful, url = {}", self_proc.server_ep.ToString());
    }
    else
    {
        ARK_LOG_ERROR("Cannot start server net, url = {}", self_proc.server_ep.ToString());
        ARK_ASSERT_NO_EFFECT(0);
        return -2;
    }

    server_services_.insert(m_pBusModule->GetSelfBusID(), pServer);

    // Register self to service discovery center(consul)
    return RegisterToConsul(m_pBusModule->GetSelfBusID());
}

AFINetServerService* AFCNetServiceManagerModule::GetSelfNetServer()
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

AFINetClientService* AFCNetServiceManagerModule::GetNetClientService(const ARK_APP_TYPE& app_type)
{
    return net_clients_.find_value(app_type);
}

AFINetClientService* AFCNetServiceManagerModule::GetNetClientServiceByBusID(const int bus_id)
{
    AFBusAddr addr(bus_id);
    return GetNetClientService(ARK_APP_TYPE(addr.app_type));
}

bool AFCNetServiceManagerModule::AddNetConnectionBus(int client_bus_id, AFINet* net_server_ptr)
{
    if (client_bus_id <= 0 || net_server_ptr == nullptr)
    {
        return false;
    }

    int self_bus_id = m_pBusModule->GetSelfBusID();
    return net_bus_relations_.insert(std::make_pair(self_bus_id, client_bus_id), net_server_ptr).second;
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

int AFCNetServiceManagerModule::RegisterToConsul(const int bus_id)
{
    auto reg_center = m_pBusModule->GetRegCenter();
    AFBusAddr bus(bus_id);
    auto bus_name = bus.ToString();

    const std::string app_whole_name = m_pBusModule->GetAppWholeName(bus.bus_id);
    const std::string app_name = m_pBusModule->GetAppName(static_cast<ARK_APP_TYPE>(bus.app_type));

    const AFProcConfig& self_proc = m_pBusModule->GetSelfProc();

    consulpp::ConsulService service;
    service.SetId(app_whole_name);
    service.SetName(reg_center.service_name);
    service.SetAddress(self_proc.intranet_ep.GetIP());
    service.SetPort(self_proc.intranet_ep.GetPort());
    // multi-tag
    service.SetTag(app_name);
    service.SetTag(bus_name);
    // multi-meta
    service.SetMeta("busid", bus_name);

    consulpp::ConsulCheck check;
    check.SetId(app_whole_name + "_check");
    check.SetName(app_whole_name);
    check.SetTcp(self_proc.intranet_ep.GetIP() + ":" + ARK_TO_STRING(self_proc.intranet_ep.GetPort()));
    check.SetInterval(reg_center.check_interval);
    check.SetTimeout(reg_center.check_timeout);
    service.SetCheck(check);

    return m_pConsulModule->RegisterService(service) ? 0 : -2;
}

int AFCNetServiceManagerModule::DeregisterFromConsul(const int bus_id)
{
    return m_pConsulModule->DeregisterService(m_pBusModule->GetAppWholeName(bus_id));
}

bool AFCNetServiceManagerModule::CreateClientService(const AFBusAddr& bus_addr, const std::string& ip, uint16_t port)
{
    //    ARK_APP_TYPE app_type = ARK_APP_TYPE(AFBusAddr(target.self_id).app_type);
    //    AFINetClientService* pClient = net_clients_.find_value(app_type);
    //    if (pClient == nullptr)
    //    {
    //        pClient = ARK_NEW AFCNetClientService(GetPluginManager());
    //        net_clients_.insert(app_type, pClient);
    //    }
    //    bool ret = pClient->StartClient(head_len, target.self_id, target.intranet_ep_);
    //    if (!ret)
    //    {
    //        ARK_LOG_ERROR("start net client failed, self_bus_id={} target_url={}", m_pBusModule->GetSelfBusName(), target.intranet_ep_.ToString());
    //        return ret;
    //    }
    return true;
}

} // namespace ark
