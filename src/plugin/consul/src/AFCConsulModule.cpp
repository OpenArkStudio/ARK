#include "consul/include/AFCConsulModule.hpp"
#include "consul/include/AFConsulPlugin.hpp"

namespace ark {

static const std::string REGISTER_API("/v1/agent/service/register");
static const std::string DEREGISTER_API("/v1/agent/service/deregister/");
static const std::string KV_API("/v1/kv/");
static const std::string HEALTH_CHECK_API("/v1/health/service/");

bool AFCConsulModule::Init()
{
    m_pHttpClientModule = FindModule<AFIHttpClientModule>();
    return LoadConfig();
}

// void AFCConsulModule::SetRegisterCenter(const std::string& center_ip, const uint16_t center_port)
// {
//     consul_ip_ = center_ip;
//     consul_port_ = center_port;
// }

bool AFCConsulModule::LoadConfig()
{
    AFXml xml(AFConsulPlugin::GetPluginConf());
    auto root = xml.GetRootNode();
    ARK_ASSERT_RET_VAL(root.IsValid(), false);

    auto conf_node = root.FindNode("conf");
    ARK_ASSERT_RET_VAL(conf_node.IsValid(), false);

    consul_ip_ = conf_node.GetString("ip");
    consul_port_ = conf_node.GetUint32("port");

    return true;
}

ananas::Future<std::pair<bool, std::string>> AFCConsulModule::RegisterService(const consul::service_data& service)
{
    std::map<std::string, std::string> params;
    std::vector<std::string> cookies;
    return m_pHttpClientModule->AsyncRequest(
        zephyr::http::verb::put, consul_ip_, consul_port_, REGISTER_API, params, cookies, service);
}

ananas::Future<std::pair<bool, std::string>> AFCConsulModule::DeregisterService(const std::string& service_id)
{
    ananas::Promise<std::pair<bool, std::string>> promise;
    if (service_id.empty())
    {
        promise.SetValue(std::make_pair(false, std::string()));
        return promise.GetFuture();
    }

    auto url = DEREGISTER_API + service_id;
    return m_pHttpClientModule->AsyncRequest(zephyr::http::verb::put, consul_ip_, consul_port_, url);
}

ananas::Future<std::pair<bool, std::string>> AFCConsulModule::GetKeyValue(const std::string& key)
{
    ananas::Promise<std::pair<bool, std::string>> promise;
    if (key.empty())
    {
        promise.SetValue(std::make_pair(false, std::string()));
        return promise.GetFuture();
    }

    std::string url = KV_API + key;
    return m_pHttpClientModule->AsyncRequest(zephyr::http::verb::get, consul_ip_, consul_port_, url);
}

ananas::Future<std::pair<bool, std::string>> AFCConsulModule::SetKeyValue(
    const std::string& key, const std::string& value)
{
    ananas::Promise<std::pair<bool, std::string>> promise;
    if (key.empty())
    {
        promise.SetValue(std::make_pair(false, std::string()));
        return promise.GetFuture();
    }

    std::map<std::string, std::string> params;
    std::vector<std::string> cookies;
    return m_pHttpClientModule->AsyncRequest(
        zephyr::http::verb::put, consul_ip_, consul_port_, KV_API, params, cookies, value);
}

ananas::Future<std::pair<bool, std::string>> AFCConsulModule::DelKeyValue(const std::string& key)
{
    ananas::Promise<std::pair<bool, std::string>> promise;
    if (key.empty())
    {
        promise.SetValue(std::make_pair(false, std::string()));
        return promise.GetFuture();
    }

    std::string url = KV_API + key;
    return m_pHttpClientModule->AsyncRequest(zephyr::http::verb::delete_, consul_ip_, consul_port_, url);
}

ananas::Future<std::pair<bool, std::string>> AFCConsulModule::GetHealthServices(
    const std::string& service_name, const std::string& tag_filter)
{
    ananas::Promise<std::pair<bool, std::string>> promise;
    if (service_name.empty())
    {
        promise.SetValue(std::make_pair(false, std::string()));
        return promise.GetFuture();
    }

    std::string url = HEALTH_CHECK_API + service_name;
    std::map<std::string, std::string> params;
    params.insert(std::make_pair("tag", tag_filter));
    params.insert(std::make_pair("passing", "1"));

    std::vector<std::string> cookies;
    //    return m_pHttpClientModule->AsyncRequest(brynet::net::http::HttpRequest::HTTP_METHOD::HTTP_METHOD_GET, consul_ip_,
    //        consul_port_, url, params, cookies, std::string(""));

    return promise.GetFuture();
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
