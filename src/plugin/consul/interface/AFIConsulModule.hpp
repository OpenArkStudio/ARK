#pragma once

#include "consulpp/consulpp.hpp"
#include "interface/AFIModule.h"

namespace ark {

class AFIConsulModule : public AFIModule
{
public:
    // Set register center url(You should call this function first)
    virtual void SetRegisterCenter(const std::string& center_ip, const uint16_t center_port) = 0;

    // Register a service to register center
    virtual bool RegisterService(const consulpp::ConsulService& service) = 0;

    // Deregister service form register center
    virtual bool DeregisterService(const std::string& service_id) = 0;

    // Get all health service with tag filter
    virtual bool GetHealthServices(
        const std::string& service_name, const std::string& tag_filter, consulpp::ConsulServiceSet& services) = 0;

    // Get/Set value
    virtual const std::string GetKeyValue(const std::string& key) = 0;
    virtual void SetKeyValue(const std::string& key, const std::string& value) = 0;
};

} // namespace ark
