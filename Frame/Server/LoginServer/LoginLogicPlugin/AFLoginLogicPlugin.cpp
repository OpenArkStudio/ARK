// -------------------------------------------------------------------------
//    @FileName         :    AFLoginLogicPlugin.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :    LoginLogicPluginModule
//
// -------------------------------------------------------------------------


#include "AFLoginLogicPlugin.h"
#include "AFCLoginLogicModule.h"

//
#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTitle("NFLoginServer");
#endif
    CREATE_PLUGIN(pm, AFLoginLogicPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFLoginLogicPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFLoginLogicPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFLoginLogicPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFLoginLogicPlugin);
}

void AFLoginLogicPlugin::Install()
{

    REGISTER_MODULE(pPluginManager, AFILoginLogicModule, AFCLoginLogicModule)

}

void AFLoginLogicPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFILoginLogicModule, AFCLoginLogicModule)
}
