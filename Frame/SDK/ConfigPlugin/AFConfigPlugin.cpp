// AFConfigPlugin.cpp : Defines the exported functions for the DLL application.
//

#include "AFConfigPlugin.h"
#include "AFCClassModule.h"
#include "AFCElementModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, AFConfigPlugin)
};

NF_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFConfigPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////
AFConfigPlugin::AFConfigPlugin(AFIPluginManager* p)
{
    pPluginManager = p;
}

const int AFConfigPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFConfigPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFConfigPlugin)
}

void AFConfigPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIClassModule, AFCClassModule)
    REGISTER_MODULE(pPluginManager, AFIElementModule, AFCElementModule)
}

void AFConfigPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIElementModule, AFCElementModule)
    UNREGISTER_MODULE(pPluginManager, AFIClassModule, AFCClassModule)
}
