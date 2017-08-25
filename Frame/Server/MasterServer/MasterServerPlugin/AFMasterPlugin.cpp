// -------------------------------------------------------------------------
//    @FileName         :    AFMasterPlugin.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :    CentreLogicModule
//
// -------------------------------------------------------------------------


#include "AFCMasterModule.h"
#include "AFMasterPlugin.h"

//
//
#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
#if ARK_PLATFORM == PLATFORM_WIN
    SetConsoleTitle("NFMasterServer");
#endif

    CREATE_PLUGIN(pm, AFMasterPlugin)

    //std::cout << "DllStartPlugin::thread id=" << GetCurrentThreadId() << std::endl;
};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFMasterPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFMasterPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFMasterPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFMasterPlugin)
}

void AFMasterPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIMasterModule, AFCMasterModule)


}

void AFMasterPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIMasterModule, AFCMasterModule)
}
