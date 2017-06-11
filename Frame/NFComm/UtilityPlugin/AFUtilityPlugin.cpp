// NFLogPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFUtilityPlugin.h"
#include "AFCGUIDModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, AFUtilityPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
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
    REGISTER_MODULE(pPluginManager, AFIGUIDModule, AFCGUIDModule)
}

void AFUtilityPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIGUIDModule, AFCGUIDModule)
}