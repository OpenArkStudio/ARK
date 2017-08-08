// -------------------------------------------------------------------------
//    @FileName         :    NFWorldLogicPlugin.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :    NFWorldLogicPlugin
//
// -------------------------------------------------------------------------


#include "AFCWorldLogicModule.h"
#include "AFWorldLogicPlugin.h"

//
//

NF_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
#if NF_PLATFORM == NF_PLATFORM_WIN
    SetConsoleTitle("NFWorldServer");
#endif // NF_PLATFORM
    CREATE_PLUGIN(pm, NFWorldLogicPlugin)
};

NF_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFWorldLogicPlugin)
};

//////////////////////////////////////////////////////////////////////////

const int NFWorldLogicPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFWorldLogicPlugin::GetPluginName()
{
    return GET_CLASS_NAME(NFWorldLogicPlugin);
}

void NFWorldLogicPlugin::Install()
{

    REGISTER_MODULE(pPluginManager, AFIWorldLogicModule, AFCWorldLogicModule)
}

void NFWorldLogicPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIWorldLogicModule, AFCWorldLogicModule)
}
