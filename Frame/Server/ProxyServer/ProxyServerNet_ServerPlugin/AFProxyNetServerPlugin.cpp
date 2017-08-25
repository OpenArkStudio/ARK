// -------------------------------------------------------------------------
//    @FileName         :    NFGameServerNet_ServerPlugin.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :    NFGameServerNet_ServerPlugin
//
// -------------------------------------------------------------------------


#include "AFCProxyServerNet_ServerModule.h"
#include "AFProxyNetServerPlugin.h"

//
//
#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, AFProxyNetServerPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFProxyNetServerPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFProxyNetServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFProxyNetServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFProxyNetServerPlugin)
}

void AFProxyNetServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIProxyServerNet_ServerModule, AFCProxyServerNet_ServerModule)

}

void AFProxyNetServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIProxyServerNet_ServerModule, AFCProxyServerNet_ServerModule)
}