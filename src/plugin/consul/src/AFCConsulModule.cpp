#include "consul/include/AFCConsulModule.h"

namespace ark {

void AFCConsulModule::SetRegisterCenter(const std::string& center_ip, const uint16_t center_port)
{
    ctx_ = std::make_shared<consulpp::Consulpp>(center_ip, center_port);
}

bool AFCConsulModule::RegisterService(const consulpp::ConsulService& service)
{
    return ctx_->RegisterService(service);
}

bool AFCConsulModule::DeregisterService(const std::string& service_id)
{
    return ctx_->Deregister(service_id);
}

bool AFCConsulModule::GetHealthServices(
    const std::string& service_name, const std::string& tag_filter, consulpp::ConsulServiceSet& services)
{
    return ctx_->HealthCheck(service_name, tag_filter, services);
}

const std::string AFCConsulModule::GetKeyValue(const std::string& key)
{
    if (key.empty())
    {
        return "";
    }
    else
    {
        return ctx_->GetValue(key);
    }
}

void AFCConsulModule::SetKeyValue(const std::string& key, const std::string& value)
{
    if (key.empty() || value.empty())
    {
        return;
    }

    ctx_->SetValue(key, value);
}

//////////////////////////////////////////////////////////////////////////
// Test
// bool AFCServiceDiscoveryModule::Test()
//{
//	consulpp::ConsulService service;
//
//	service.SetId("test_service_id");
//	service.SetName("client_test_servie");
//	service.SetAddress("127.0.0.1");
//	service.SetPort(22);
//	//multi-tags
//	service.SetTag("client");
//	service.SetTag("test");
//	//multi-metas
//	service.SetMeta("version", "1.2.3");
//	service.SetMeta("test_meta", "test_meta_value");
//
//	consulpp::Check check;
//	check.SetId("client_test_check");
//	check.SetName("Client test check");
//	check.SetTcp("127.0.0.1:22");
//	check.SetInterval("5s");
//	check.SetTimeout("1s");
//	check.SetNote("test note");
//	service.SetCheck(check);

//	return RegisterService(service);
//}

} // namespace ark
