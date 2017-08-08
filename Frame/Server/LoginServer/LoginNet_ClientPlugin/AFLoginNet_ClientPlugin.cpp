// NFLoginNet_ClientPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFLoginNet_ClientPlugin.h"
#include "AFCLoginToMasterModule.h"

//
//
#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, NFLoginNet_ClientPlugin)

};

NF_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFLoginNet_ClientPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int NFLoginNet_ClientPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFLoginNet_ClientPlugin::GetPluginName()
{
    return GET_CLASS_NAME(NFLoginNet_ClientPlugin)
}

void NFLoginNet_ClientPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFILoginToMasterModule, AFCLoginToMasterModule)
}

void NFLoginNet_ClientPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFILoginToMasterModule, AFCLoginToMasterModule)
}