// -------------------------------------------------------------------------
//    @FileName         :    NFGameServerNet_ServerPlugin.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :    NFGameServerNet_ServerPlugin
//
// -------------------------------------------------------------------------


#include "AFCGameServerNet_ServerModule.h"
#include "AFGameNetServerPlugin.h"

//
//
#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, AFGameNetServerPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFGameNetServerPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFGameNetServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFGameNetServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFGameNetServerPlugin)
}

void AFGameNetServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIGameServerNet_ServerModule, AFCGameServerNet_ServerModule)

}

void AFGameNetServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIGameServerNet_ServerModule, AFCGameServerNet_ServerModule)
}