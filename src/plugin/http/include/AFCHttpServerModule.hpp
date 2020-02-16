#pragma once

#include "base/AFPluginManager.hpp"
#include "http/interface/AFIHttpServerModule.hpp"

namespace ark {

class AFCHttpServerModule final : public AFIHttpServerModule
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    bool Init() override;
    bool PreShut() override;

    bool CreateServer(std::string const& address, uint16_t port = 8080, uint8_t thread_num = 1) override;
    bool AddRoute(std::string const& url, OB::Belle::Method method, OB::Belle::fn_on_http&& http_function) override;
    bool StartWork() override;

private:
    std::shared_ptr<OB::Belle::Server> http_server_;
};

} // namespace ark