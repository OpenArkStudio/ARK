#pragma once

#include "base/AFPluginManager.hpp"
#include "consul/interface/AFIConsulModule.hpp"

namespace ark {

class AFCConsulModule : public AFIConsulModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    explicit AFCConsulModule() = default;

    void SetRegisterCenter(const std::string& center_ip, const uint16_t center_port) override;

    bool RegisterService(const consulpp::ConsulService& service) override;

    bool DeregisterService(const std::string& service_id) override;

    bool GetHealthServices(
        const std::string& service_name, const std::string& tag_filter, consulpp::ConsulServiceSet& services) override;

    bool GetHealthServices(const std::string& service_name, const std::vector<std::string>& tag_filter_list,
        consulpp::ConsulServiceSet& services) override;

    const std::string GetKeyValue(const std::string& key) override;
    bool SetKeyValue(const std::string& key, const std::string& value) override;
    bool DelKeyValue(const std::string& key) override;

protected:
    // bool Test();

private:
    std::shared_ptr<consulpp::Consulpp> ctx_{nullptr};
};

} // namespace ark
