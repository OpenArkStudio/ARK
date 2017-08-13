// NFWorldNet_ClientPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFWorldNet_ClientPlugin.h"
#include "AFCWorldToMasterModule.h"

//
//
#ifdef NF_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, NFWorldNet_ClientPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFWorldNet_ClientPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int NFWorldNet_ClientPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFWorldNet_ClientPlugin::GetPluginName()
{
    return GET_CLASS_NAME(NFWorldNet_ClientPlugin)
}

void NFWorldNet_ClientPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIWorldToMasterModule, AFCWorldToMasterModule)
}

void NFWorldNet_ClientPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIWorldToMasterModule, AFCWorldToMasterModule)
}