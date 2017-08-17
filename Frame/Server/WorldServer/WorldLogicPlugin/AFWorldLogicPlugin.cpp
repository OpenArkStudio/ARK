#include "AFCWorldLogicModule.h"
#include "AFWorldLogicPlugin.h"

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTitle("ArkWorldServer");
#endif // ARK_PLATFORM
    CREATE_PLUGIN(pm, AFWorldLogicPlugin)
};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFWorldLogicPlugin)
};

//////////////////////////////////////////////////////////////////////////

const int AFWorldLogicPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFWorldLogicPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFWorldLogicPlugin);
}

void AFWorldLogicPlugin::Install()
{

    REGISTER_MODULE(pPluginManager, AFIWorldLogicModule, AFCWorldLogicModule)
}

void AFWorldLogicPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIWorldLogicModule, AFCWorldLogicModule)
}
