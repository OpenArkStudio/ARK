#include "ark/etcd_client/etcd_client_impl.hpp"
#include "ark/etcd_client/etcd_client_module_impl.hpp"

namespace ark {

bool etcd_client_module_impl::init() {
    httpClientModule_ = find_module<http_client_module>();
    return httpClientModule_ != nullptr;
}

std::shared_ptr<etcd_client> etcd_client_module_impl::create_etcd_client(
    std::vector<std::string> endpoints, int ioTimeout, int connectTimeout) {
    return std::make_shared<etcd_client_impl>(httpClientModule_, std::move(endpoints), connectTimeout, ioTimeout);
}

} // namespace ark
