// AFLoginNetServerPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFLoginNetServerPlugin.h"
#include "AFCLoginNet_ServerModule.h"

//
//
#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, AFLoginNetServerPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFLoginNetServerPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int AFLoginNetServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFLoginNetServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFLoginNetServerPlugin)
}

void AFLoginNetServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFILoginNet_ServerModule, AFCLoginNet_ServerModule)
}

void AFLoginNetServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFILoginNet_ServerModule, AFCLoginNet_ServerModule)
}