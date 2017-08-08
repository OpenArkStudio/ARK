// NFLoginNet_ServerPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFLoginNet_ServerPlugin.h"
#include "AFCLoginNet_ServerModule.h"

//
//
#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NFLoginNet_ServerPlugin)

};

NF_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFLoginNet_ServerPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int NFLoginNet_ServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFLoginNet_ServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(NFLoginNet_ServerPlugin)
}

void NFLoginNet_ServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFILoginNet_ServerModule, AFCLoginNet_ServerModule)
}

void NFLoginNet_ServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFILoginNet_ServerModule, AFCLoginNet_ServerModule)
}