#pragma once

#include "ark/consul/consul_module.hpp"
#include "ark/net/http_client_module.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

class consul_module_impl final : public consul_module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool init() override;

    // void SetRegisterCenter(const std::string& center_ip, const uint16_t center_port) override;

    std::future<std::pair<bool, std::string>> register_service(const consul::service_data& service) override;

    std::future<std::pair<bool, std::string>> deregister_service(const std::string& service_id) override;

    std::future<std::pair<bool, std::string>> get_health_services(
        const std::string& service_name, const std::string& tag_filter) override;

    //bool GetHealthServices(const std::string& service_name, const std::vector<std::string>& tag_filter_list,
    //    consul::service_set& services) override;

    std::future<std::pair<bool, std::string>> get_key_value(const std::string& key) override;
    std::future<std::pair<bool, std::string>> set_key_value(const std::string& key, const std::string& value) override;
    std::future<std::pair<bool, std::string>> del_key_value(const std::string& key) override;

protected:
    bool load_config();

private:
    std::string consul_ip_{};
    uint16_t consul_port_{8500};

    http_client_module* http_client_module_;
};

} // namespace ark
