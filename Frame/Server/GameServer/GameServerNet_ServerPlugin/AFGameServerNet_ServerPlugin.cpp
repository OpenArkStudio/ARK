// -------------------------------------------------------------------------
//    @FileName         :    NFGameServerNet_ServerPlugin.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :    NFGameServerNet_ServerPlugin
//
// -------------------------------------------------------------------------


#include "AFCGameServerNet_ServerModule.h"
#include "AFGameServerNet_ServerPlugin.h"

//
//
#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, AFGameServerNet_ServerPlugin)

};

NF_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFGameServerNet_ServerPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFGameServerNet_ServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFGameServerNet_ServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFGameServerNet_ServerPlugin)
}

void AFGameServerNet_ServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIGameServerNet_ServerModule, AFCGameServerNet_ServerModule)

}

void AFGameServerNet_ServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIGameServerNet_ServerModule, AFCGameServerNet_ServerModule)
}