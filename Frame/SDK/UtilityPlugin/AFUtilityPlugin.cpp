// NFLogPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFUtilityPlugin.h"
#include "AFCGUIDModule.h"
#include "AFCLogModule.h"

#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, AFUtilityPlugin)
};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFUtilityPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int AFUtilityPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFUtilityPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFUtilityPlugin)
}

void AFUtilityPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFILogModule, AFCLogModule)
    REGISTER_MODULE(pPluginManager, AFIGUIDModule, AFCGUIDModule)
}

void AFUtilityPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIGUIDModule, AFCGUIDModule)
    UNREGISTER_MODULE(pPluginManager, AFILogModule, AFCLogModule)
}