// AFLoginNetClientPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFLoginNetClientPlugin.h"
#include "AFCLoginToMasterModule.h"

//
//
#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, AFLoginNetClientPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFLoginNetClientPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFLoginNetClientPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFLoginNetClientPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFLoginNetClientPlugin)
}

void AFLoginNetClientPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFILoginToMasterModule, AFCLoginToMasterModule)
}

void AFLoginNetClientPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFILoginToMasterModule, AFCLoginToMasterModule)
}