#pragma once

#include "interface/AFIModule.hpp"
#include "common/belle.hh"

namespace ark {

class AFIHttpServerModule : public AFIModule
{
public:
    virtual bool CreateServer(std::string const& address, uint16_t port = 8080, uint8_t thread_num = 1) = 0;
    virtual bool AddRoute(std::string const& url, OB::Belle::Method method, OB::Belle::fn_on_http&& http_function) = 0;

    virtual bool StartWork() = 0;
};

} // namespace ark