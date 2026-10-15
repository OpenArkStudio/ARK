#pragma once

#include "ark/etcd_client/etcd_client_module.hpp"
#include "ark/net/http_client_module.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

class etcd_client_module_impl final : public etcd_client_module
{
    ARK_DECLARE_MODULE_FUNCTIONS

public:
    virtual bool init() override;

    virtual std::shared_ptr<etcd_client> create_etcd_client(
        std::vector<std::string> endpoints, int io_timeout = 5, int connect_timeout = 2) override;

private:
    http_client_module* http_client_module_;
};

} // namespace ark
