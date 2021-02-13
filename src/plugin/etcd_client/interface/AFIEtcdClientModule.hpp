#pragma once

#include "interface/AFIModule.hpp"
#include "AFIEtcdClient.hpp"

namespace ark {

class AFIEtcdClientModule : public AFIModule {
public:
    virtual std::shared_ptr<AFIEtcdClient> CreateEtcdClient(std::vector<std::string> endpoints, int ioTimeout = 5, int connectTimeout = 2) = 0;
};

} // namespace ark
