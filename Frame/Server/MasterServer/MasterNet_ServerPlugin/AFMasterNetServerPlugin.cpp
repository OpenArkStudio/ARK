// AFLoginNetServerPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFMasterNetServerPlugin.h"
#include "AFCMasterNet_ServerModule.h"

//
//
#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, AFMasterNetServerPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFMasterNetServerPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int AFMasterNetServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFMasterNetServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFMasterNetServerPlugin)
}

void AFMasterNetServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIMasterNet_ServerModule, AFCMasterNet_ServerModule)
}

void AFMasterNetServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIMasterNet_ServerModule, AFCMasterNet_ServerModule)
}