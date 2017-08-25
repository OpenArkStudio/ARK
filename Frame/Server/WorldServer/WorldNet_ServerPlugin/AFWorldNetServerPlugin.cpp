// AFWorldNetServerPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFWorldNetServerPlugin.h"
#include "AFCWorldNet_ServerModule.h"

//
//
#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, AFWorldNetServerPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFWorldNetServerPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFWorldNetServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFWorldNetServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFWorldNetServerPlugin)
}

void AFWorldNetServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIWorldNet_ServerModule, AFCWorldNet_ServerModule)
}

void AFWorldNetServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIWorldNet_ServerModule, AFCWorldNet_ServerModule)
}