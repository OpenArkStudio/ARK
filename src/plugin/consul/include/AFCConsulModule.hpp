#pragma once

#include "base/AFPluginManager.hpp"
#include "consul/interface/AFIConsulModule.hpp"
#include "consul/include/AFConsulClient.hpp"

namespace ark {

class AFCConsulModule : public AFIConsulModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit AFCConsulModule() = default;
    bool Update() override;

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
    std::shared_ptr<AFConsulClient> ctx_{nullptr};
};

} // namespace ark
