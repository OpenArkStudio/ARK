#include "consul/include/AFConsulPlugin.hpp"
#include "consul/include/AFCConsulModule.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(AFConsulPlugin)

void AFConsulPlugin::Install()
{
    ARK_REGISTER_MODULE(AFIConsulModule, AFCConsulModule);
}

void AFConsulPlugin::Uninstall()
{
    ARK_UNREGISTER_MODULE(AFIConsulModule, AFCConsulModule);
}

} // namespace ark
