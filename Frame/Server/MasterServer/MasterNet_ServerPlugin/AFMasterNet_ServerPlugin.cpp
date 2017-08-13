// NFLoginNet_ServerPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFMasterNet_ServerPlugin.h"
#include "AFCMasterNet_ServerModule.h"

//
//
#ifdef NF_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, NFMasterNet_ServerPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFMasterNet_ServerPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int NFMasterNet_ServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFMasterNet_ServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(NFMasterNet_ServerPlugin)
}

void NFMasterNet_ServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIMasterNet_ServerModule, AFCMasterNet_ServerModule)
}

void NFMasterNet_ServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIMasterNet_ServerModule, AFCMasterNet_ServerModule)
}