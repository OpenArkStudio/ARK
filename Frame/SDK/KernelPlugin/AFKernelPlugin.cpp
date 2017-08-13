// AFKernelPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFKernelPlugin.h"
#include "AFCKernelModule.h"
#include "AFCSceneModule.h"


#ifdef NF_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, AFKernelPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFKernelPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int AFKernelPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFKernelPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFKernelPlugin)
}

void AFKernelPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFISceneModule, AFCSceneModule)
    REGISTER_MODULE(pPluginManager, AFIKernelModule, AFCKernelModule)
}

void AFKernelPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIKernelModule, AFCKernelModule)
    UNREGISTER_MODULE(pPluginManager, AFISceneModule, AFCSceneModule)
}