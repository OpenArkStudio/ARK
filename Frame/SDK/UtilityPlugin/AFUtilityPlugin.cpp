// NFLogPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFUtilityPlugin.h"
#include "AFCGUIDModule.h"
#include "AFCLogModule.h"

#ifdef NF_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, UtilityPlugin)
};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, UtilityPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int UtilityPlugin::GetPluginVersion()
{
    return 0;
}

const std::string UtilityPlugin::GetPluginName()
{
    return GET_CLASS_NAME(UtilityPlugin)
}

void UtilityPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFILogModule, AFCLogModule)
    REGISTER_MODULE(pPluginManager, AFIGUIDModule, AFCGUIDModule)
}

void UtilityPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIGUIDModule, AFCGUIDModule)
    UNREGISTER_MODULE(pPluginManager, AFILogModule, AFCLogModule)
}