#include "consul/include/AFConsulPlugin.h"
#include "consul/include/AFCConsulModule.h"

namespace ark {

// Dll entrance and exit function
ARK_PLUGIN_DECLARE(AFConsulPlugin)

// For now, we don't check version number
int AFConsulPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFConsulPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFConsulPlugin);
}

void AFConsulPlugin::Install()
{
    RegisterModule<AFIConsulModule, AFCConsulModule>();
}

void AFConsulPlugin::Uninstall()
{
    DeregisterModule<AFIConsulModule, AFCConsulModule>();
}

} // namespace ark
