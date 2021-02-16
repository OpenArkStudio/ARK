#include "etcd_client/include/AFEtcdClientPlugin.hpp"
#include "etcd_client/include/AFCEtcdClientModule.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(AFEtcdClientPlugin)

void AFEtcdClientPlugin::Install()
{
    ARK_REGISTER_MODULE(AFIEtcdClientModule, AFCEtcdClientModule);
}

void AFEtcdClientPlugin::Uninstall()
{
    ARK_UNREGISTER_MODULE(AFIEtcdClientModule, AFCEtcdClientModule);
}

} // namespace ark
