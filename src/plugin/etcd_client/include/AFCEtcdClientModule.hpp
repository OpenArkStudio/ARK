#pragma once

#include "base/AFPluginManager.hpp"
#include "net/interface/AFIHttpClientModule.hpp"
#include "etcd_client/interface/AFIEtcdClientModule.hpp"

namespace ark {

class AFCEtcdClientModule final : public AFIEtcdClientModule {
    ARK_DECLARE_MODULE_FUNCTIONS

public:
    virtual bool Init() override;

    virtual std::shared_ptr<AFIEtcdClient> CreateEtcdClient(std::vector<std::string> endpoints, int ioTimeout = 5, int connectTimeout = 2) override;

private:
    AFIHttpClientModule* httpClientModule_;
};

} // namespace ark
