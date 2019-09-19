#include "consul/include/AFCConsulModule.hpp"

namespace ark {

//void AFCConsulModule::SetRegisterCenter(const std::string& center_ip, const uint16_t center_port)
//{
//    ctx_ = std::make_shared<consulpp::Consulpp>(center_ip, center_port);
//}
//
//bool AFCConsulModule::RegisterService(const consulpp::ConsulService& service)
//{
//    return ctx_->RegisterService(service);
//}
//
//bool AFCConsulModule::DeregisterService(const std::string& service_id)
//{
//    return ctx_->Deregister(service_id);
//}
//
//bool AFCConsulModule::GetHealthServices(const std::string& service_name, const std::string& tag_filter, consulpp::ConsulServiceSet& services)
//{
//    return ctx_->HealthCheck(service_name, tag_filter, services);
//}
//
//bool AFCConsulModule::GetHealthServices(const std::string& service_name, const std::vector<std::string>& tag_filter_list, consulpp::ConsulServiceSet& services)
//{
//    return ctx_->MultiHealthCheck(service_name, tag_filter_list, services);
//}
//
//const std::string AFCConsulModule::GetKeyValue(const std::string& key)
//{
//    if (key.empty())
//    {
//        const static std::string& null_str = {};
//        return null_str;
//    }
//    else
//    {
//        return ctx_->GetValue(key);
//    }
//}
//
//bool AFCConsulModule::SetKeyValue(const std::string& key, const std::string& value)
//{
//    if (key.empty() || value.empty())
//    {
//        return false;
//    }
//
//    return ctx_->SetValue(key, value);
//}
//
//bool AFCConsulModule::DelKeyValue(const std::string& key)
//{
//    if (key.empty())
//    {
//        return true;
//    }
//
//    return ctx_->DeleteValue(key);
//}
//
//////////////////////////////////////////////////////////////////////////
//// Test
//bool AFCServiceDiscoveryModule::Test()
//{
//    consulpp::ConsulService service;
//
//    service.SetId("test_service_id");
//    service.SetName("client_test_servie");
//    service.SetAddress("127.0.0.1");
//    service.SetPort(22);
//    //multi-tags
//    service.SetTag("client");
//    service.SetTag("test");
//    //multi-metas
//    service.SetMeta("version", "1.2.3");
//    service.SetMeta("test_meta", "test_meta_value");
//
//    consulpp::Check check;
//    check.SetId("client_test_check");
//    check.SetName("Client test check");
//    check.SetTcp("127.0.0.1:22");
//    check.SetInterval("5s");
//    check.SetTimeout("1s");
//    check.SetNote("test note");
//    service.SetCheck(check);
//
//    return RegisterService(service);
//}

void AFCConsulModule::SetRegisterCenter(const std::string& center_ip, const uint16_t center_port)
{
    ctx_ = std::make_shared<AFConsulClient>(center_ip, center_port);
}

ananas::Future<std::pair<bool, std::string>> AFCConsulModule::RegisterService(const consul::service_data& service)
{
    return ctx_->RegisterService(service);
}

ananas::Future<std::pair<bool, std::string>> AFCConsulModule::DeregisterService(const std::string& service_id)
{
    return ctx_->DeregisterService(service_id);
}

bool AFCConsulModule::GetHealthServices(const std::string& service_name, const std::string& tag_filter, consul::service_set& services)
{
    return ctx_->HealthCheck(service_name, tag_filter, services);
}

bool AFCConsulModule::GetHealthServices(const std::string& service_name, const std::vector<std::string>& tag_filter_list, consul::service_set& services)
{
    return ctx_->MultiHealthCheck(service_name, tag_filter_list, services);
}

const std::string AFCConsulModule::GetKeyValue(const std::string& key)
{
    if (key.empty())
    {
        const static std::string& null_str = {};
        return null_str;
    }
    else
    {
        return ctx_->GetValue(key);
    }
}

bool AFCConsulModule::SetKeyValue(const std::string& key, const std::string& value)
{
    if (key.empty() || value.empty())
    {
        return false;
    }

    ctx_->SetKeyValue(key, value).Then([=](const std::pair<bool, std::string>& resp) { return resp.first; });
}

bool AFCConsulModule::DelKeyValue(const std::string& key)
{
    if (key.empty())
    {
        return true;
    }

    return ctx_->DeleteValue(key);
}

////////////////////////////////////////////////////////////////////////
//// Test
//bool AFCServiceDiscoveryModule::Test()
//{
//    consulpp::ConsulService service;
//
//    service.SetId("test_service_id");
//    service.SetName("client_test_servie");
//    service.SetAddress("127.0.0.1");
//    service.SetPort(22);
//    //multi-tags
//    service.SetTag("client");
//    service.SetTag("test");
//    //multi-metas
//    service.SetMeta("version", "1.2.3");
//    service.SetMeta("test_meta", "test_meta_value");
//
//    consulpp::Check check;
//    check.SetId("client_test_check");
//    check.SetName("Client test check");
//    check.SetTcp("127.0.0.1:22");
//    check.SetInterval("5s");
//    check.SetTimeout("1s");
//    check.SetNote("test note");
//    service.SetCheck(check);
//
//    return RegisterService(service);
//}

} // namespace ark
