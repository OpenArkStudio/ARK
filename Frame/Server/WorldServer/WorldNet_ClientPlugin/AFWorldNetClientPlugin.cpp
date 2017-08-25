// AFWorldNetClientPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFWorldNetClientPlugin.h"
#include "AFCWorldToMasterModule.h"

//
//
#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, AFWorldNetClientPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFWorldNetClientPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFWorldNetClientPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFWorldNetClientPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFWorldNetClientPlugin)
}

void AFWorldNetClientPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIWorldToMasterModule, AFCWorldToMasterModule)
}

void AFWorldNetClientPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIWorldToMasterModule, AFCWorldToMasterModule)
}