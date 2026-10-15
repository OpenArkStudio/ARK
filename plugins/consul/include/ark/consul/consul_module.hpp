#pragma once

#include <future>
#include <string>
#include <utility>

#include "ark/module.hpp"
#include "ark/proto/AFConsul.pb.h"

namespace ark {

class consul_module : public module
{
public:
    // Set register center url(You should call this function first)
    // virtual void SetRegisterCenter(const std::string& center_ip, const uint16_t center_port) = 0;

    // Register a service to register center
    virtual std::future<std::pair<bool, std::string>> RegisterService(const consul::service_data& service) = 0;

    // Unregister service form register center
    virtual std::future<std::pair<bool, std::string>> DeregisterService(const std::string& service_id) = 0;

    // Get all health service with tag filter
    virtual std::future<std::pair<bool, std::string>> GetHealthServices(
        const std::string& service_name, const std::string& tag_filter) = 0;

    //virtual bool GetHealthServices(const std::string& service_name, const std::vector<std::string>& tag_filter_list,
    //    consul::service_set& services) = 0;

    // Get/Set value
    virtual std::future<std::pair<bool, std::string>> GetKeyValue(const std::string& key) = 0;
    virtual std::future<std::pair<bool, std::string>> SetKeyValue(const std::string& key, const std::string& value) = 0;
    virtual std::future<std::pair<bool, std::string>> DelKeyValue(const std::string& key) = 0;
};

} // namespace ark
