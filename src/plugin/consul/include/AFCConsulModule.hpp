#pragma once

#include "base/AFPluginManager.hpp"
#include "net/interface/AFIHttpClientModule.hpp"
#include "consul/interface/AFIConsulModule.hpp"

namespace ark {

class AFCConsulModule final : public AFIConsulModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;

    void SetRegisterCenter(const std::string& center_ip, const uint16_t center_port) override;

    ananas::Future<std::pair<bool, std::string>> RegisterService(const consul::service_data& service) override;

    ananas::Future<std::pair<bool, std::string>> DeregisterService(const std::string& service_id) override;

    ananas::Future<std::pair<bool, std::string>> GetHealthServices(
        const std::string& service_name, const std::string& tag_filter) override;

    //bool GetHealthServices(const std::string& service_name, const std::vector<std::string>& tag_filter_list,
    //    consul::service_set& services) override;

    ananas::Future<std::pair<bool, std::string>> GetKeyValue(const std::string& key) override;
    ananas::Future<std::pair<bool, std::string>> SetKeyValue(const std::string& key, const std::string& value) override;
    ananas::Future<std::pair<bool, std::string>> DelKeyValue(const std::string& key) override;

private:
    std::string consul_ip_{};
    uint16_t consul_port_{8500};

    AFIHttpClientModule* m_pHttpClientModule;
};

} // namespace ark
