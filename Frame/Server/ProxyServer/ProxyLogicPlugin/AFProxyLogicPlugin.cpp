// -------------------------------------------------------------------------
//    @FileName         :    AFCProxyPlugin.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    AFCProxyPlugin
//
// -------------------------------------------------------------------------


#include "AFProxyLogicPlugin.h"
#include "AFCProxyLogicModule.h"

//
//
#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTitle("NFProxyServer");
#endif
    CREATE_PLUGIN(pm, AFProxyLogicPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFProxyLogicPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFProxyLogicPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFProxyLogicPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFProxyLogicPlugin)
}

void AFProxyLogicPlugin::Install()
{

    REGISTER_MODULE(pPluginManager, AFIProxyLogicModule, AFCProxyLogicModule)

}

void AFProxyLogicPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIProxyLogicModule, AFCProxyLogicModule)
}
