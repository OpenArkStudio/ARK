// -------------------------------------------------------------------------
//    @FileName         :    AFGamePlugin.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :    AFGamePlugin
//
// -------------------------------------------------------------------------


#include "AFGamePlugin.h"
#include "AFCGameServerModule.h"
#include "AFCSceneProcessModule.h"
#include "AFCPropertyModule.h"
#include "AFCLevelModule.h"
#include "AFCPropertyConfigModule.h"

#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTitle("NFGameServer");
#endif // ARK_PLATFORM
    CREATE_PLUGIN(pm, AFGamePlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFGamePlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFGamePlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFGamePlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFGamePlugin)
}

void AFGamePlugin::Install()
{

    REGISTER_MODULE(pPluginManager, AFIGameServerModule, AFCGameServerModule)
    REGISTER_MODULE(pPluginManager, AFISceneProcessModule, AFCSceneProcessModule)

    REGISTER_MODULE(pPluginManager, AFIPropertyModule, AFCPropertyModule)
    REGISTER_MODULE(pPluginManager, AFILevelModule, AFCLevelModule)
    REGISTER_MODULE(pPluginManager, AFIPropertyConfigModule, AFCPropertyConfigModule)
}

void AFGamePlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIPropertyConfigModule, AFCPropertyConfigModule)
    UNREGISTER_MODULE(pPluginManager, AFILevelModule, AFCLevelModule)
    UNREGISTER_MODULE(pPluginManager, AFIPropertyModule, AFCPropertyModule)

    UNREGISTER_MODULE(pPluginManager, AFISceneProcessModule, AFCSceneProcessModule)
    UNREGISTER_MODULE(pPluginManager, AFIGameServerModule, AFCGameServerModule)
}
