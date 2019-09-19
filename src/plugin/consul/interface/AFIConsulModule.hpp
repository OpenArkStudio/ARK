#pragma once

//#include "consulpp/consulpp.hpp"
//#include "interface/AFIModule.hpp"
//
//namespace ark {
//
//class AFIConsulModule : public AFIModule
//{
//public:
//    // Set register center url(You should call this function first)
//    virtual void SetRegisterCenter(const std::string& center_ip, const uint16_t center_port) = 0;
//
//    // Register a service to register center
//    virtual bool RegisterService(const consulpp::ConsulService& service) = 0;
//
//    // Deregister service form register center
//    virtual bool DeregisterService(const std::string& service_id) = 0;
//
//    // Get all health service with tag filter
//    virtual bool GetHealthServices(const std::string& service_name, const std::string& tag_filter, consulpp::ConsulServiceSet& services) = 0;
//
//    virtual bool GetHealthServices(const std::string& service_name, const std::vector<std::string>& tag_filter_list, consulpp::ConsulServiceSet& services) = 0;
//
//    // Get/Set value
//    virtual const std::string GetKeyValue(const std::string& key) = 0;
//    virtual bool SetKeyValue(const std::string& key, const std::string& value) = 0;
//    virtual bool DelKeyValue(const std::string& key) = 0;
//};
//
//} // namespace ark

#include "interface/AFIModule.hpp"
#include "proto/cpp/AFConsul.pb.h"
#include "ananas/future/Future.h"

namespace ark {

class AFIConsulModule : public AFIModule
{
public:
    // Set register center url(You should call this function first)
    virtual void SetRegisterCenter(const std::string& center_ip, const uint16_t center_port) = 0;

    // Register a service to register center
    virtual ananas::Future<std::pair<bool, std::string>> RegisterService(const consul::service_data& service) = 0;

    // Deregister service form register center
    virtual ananas::Future<std::pair<bool, std::string>> DeregisterService(const std::string& service_id) = 0;

    // Get all health service with tag filter
    virtual bool GetHealthServices(const std::string& service_name, const std::string& tag_filter, consul::service_set& services) = 0;

    virtual bool GetHealthServices(const std::string& service_name, const std::vector<std::string>& tag_filter_list, consul::service_set& services) = 0;

    // Get/Set value
    virtual const std::string GetKeyValue(const std::string& key) = 0;
    virtual bool SetKeyValue(const std::string& key, const std::string& value) = 0;
    virtual bool DelKeyValue(const std::string& key) = 0;
};

} // namespace ark
