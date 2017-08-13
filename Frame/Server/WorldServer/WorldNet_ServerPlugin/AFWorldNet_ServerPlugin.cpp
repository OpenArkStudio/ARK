// NFWorldNet_ServerPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFWorldNet_ServerPlugin.h"
#include "AFCWorldNet_ServerModule.h"

//
//
#ifdef NF_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NFWorldNet_ServerPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFWorldNet_ServerPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int NFWorldNet_ServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFWorldNet_ServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(NFWorldNet_ServerPlugin)
}

void NFWorldNet_ServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIWorldNet_ServerModule, AFCWorldNet_ServerModule)
}

void NFWorldNet_ServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIWorldNet_ServerModule, AFCWorldNet_ServerModule)
}