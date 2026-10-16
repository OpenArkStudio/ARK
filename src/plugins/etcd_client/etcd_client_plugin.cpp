#include "ark/etcd_client/etcd_client_module_impl.hpp"
#include "ark/etcd_client/etcd_client_plugin.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(etcd_client_plugin)

void etcd_client_plugin::install() {
    ARK_REGISTER_MODULE(etcd_client_module, etcd_client_module_impl);
}

void etcd_client_plugin::uninstall() {
    ARK_UNREGISTER_MODULE(etcd_client_module, etcd_client_module_impl);
}

} // namespace ark
