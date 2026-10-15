#include "ark/consul/consul_module_impl.hpp"
#include "ark/consul/consul_plugin.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(consul_plugin)

void consul_plugin::install() {
    ARK_REGISTER_MODULE(consul_module, consul_module_impl);
}

void consul_plugin::uninstall() {
    ARK_UNREGISTER_MODULE(consul_module, consul_module_impl);
}

} // namespace ark
