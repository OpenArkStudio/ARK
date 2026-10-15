#include <google/protobuf/util/json_util.h>

#include "ark/consul/consul_module_impl.hpp"
#include "ark/consul/consul_plugin.hpp"

namespace ark {

namespace {

std::future<std::pair<bool, std::string>> FailedRequest() {
    std::promise<std::pair<bool, std::string>> promise;
    promise.set_value({false, {}});
    return promise.get_future();
}

} // namespace

static const std::string REGISTER_API("/v1/agent/service/register");
static const std::string DEREGISTER_API("/v1/agent/service/deregister/");
static const std::string KV_API("/v1/kv/");
static const std::string HEALTH_CHECK_API("/v1/health/service/");

bool consul_module_impl::init() {
    m_pHttpClientModule = find_module<http_client_module>();
    return m_pHttpClientModule != nullptr && LoadConfig();
}

// void AFCConsulModule::SetRegisterCenter(const std::string& center_ip, const uint16_t center_port)
// {
//     consul_ip_ = center_ip;
//     consul_port_ = center_port;
// }

bool consul_module_impl::LoadConfig() {
    xml xml(consul_plugin::get_plugin_conf());
    auto root = xml.GetRootNode();
    ARK_ASSERT_RET_VAL(root.IsValid(), false);

    auto conf_node = root.FindNode("conf");
    ARK_ASSERT_RET_VAL(conf_node.IsValid(), false);

    consul_ip_ = conf_node.GetString("ip");
    consul_port_ = conf_node.GetUint32("port");

    return true;
}

std::future<std::pair<bool, std::string>> consul_module_impl::RegisterService(const consul::service_data& service) {
    std::string body;
    if (!google::protobuf::util::MessageToJsonString(service, &body).ok())
    {
        return FailedRequest();
    }
    const std::string content_type = "application/json";
    return m_pHttpClientModule->AsyncRequest(
        arknet::http::verb::put, consul_ip_, consul_port_, REGISTER_API, nullptr, nullptr, &body, &content_type);
}

std::future<std::pair<bool, std::string>> consul_module_impl::DeregisterService(const std::string& service_id) {
    if (service_id.empty())
    {
        return FailedRequest();
    }

    auto url = DEREGISTER_API + service_id;
    return m_pHttpClientModule->AsyncRequest(arknet::http::verb::put, consul_ip_, consul_port_, url);
}

std::future<std::pair<bool, std::string>> consul_module_impl::GetKeyValue(const std::string& key) {
    if (key.empty())
    {
        return FailedRequest();
    }

    std::string url = KV_API + key;
    return m_pHttpClientModule->AsyncRequest(arknet::http::verb::get, consul_ip_, consul_port_, url);
}

std::future<std::pair<bool, std::string>> consul_module_impl::SetKeyValue(
    const std::string& key, const std::string& value) {
    if (key.empty())
    {
        return FailedRequest();
    }

    return m_pHttpClientModule->AsyncRequest(
        arknet::http::verb::put, consul_ip_, consul_port_, KV_API + key, nullptr, nullptr, &value);
}

std::future<std::pair<bool, std::string>> consul_module_impl::DelKeyValue(const std::string& key) {
    if (key.empty())
    {
        return FailedRequest();
    }

    std::string url = KV_API + key;
    return m_pHttpClientModule->AsyncRequest(arknet::http::verb::delete_, consul_ip_, consul_port_, url);
}

std::future<std::pair<bool, std::string>> consul_module_impl::GetHealthServices(
    const std::string& service_name, const std::string& tag_filter) {
    if (service_name.empty())
    {
        return FailedRequest();
    }

    std::string url = HEALTH_CHECK_API + service_name;
    std::map<std::string, std::string> params;
    params.insert(std::make_pair("tag", tag_filter));
    params.insert(std::make_pair("passing", "1"));

    return m_pHttpClientModule->AsyncRequest(arknet::http::verb::get, consul_ip_, consul_port_, url, &params);
}

//bool AFCConsulModule::GetHealthServices(
//    const std::string& service_name, const std::vector<std::string>& tag_filter_list, consul::service_set& services)
//{
//    for (auto tag : tag_filter_list)
//    {
//        auto check_future = HealthCheck(service_name, tag);
//    }
//
//    // TODO:
//    // when all
//
//    return true;
//}

} // namespace ark
