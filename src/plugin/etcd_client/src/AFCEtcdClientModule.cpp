#include "etcd_client/include/AFCEtcdClient.hpp"
#include "etcd_client/include/AFCEtcdClientModule.hpp"

namespace ark {

bool AFCEtcdClientModule::Init()
{
    httpClientModule_ = FindModule<AFIHttpClientModule>();
    return httpClientModule_ != nullptr;
}

std::shared_ptr<AFIEtcdClient> AFCEtcdClientModule::CreateEtcdClient(std::vector<std::string> endpoints, int ioTimeout, int connectTimeout)
{
    return std::make_shared<AFCEtcdClient>(httpClientModule_, std::move(endpoints), connectTimeout, ioTimeout);
}

} // namespace ark
