// -------------------------------------------------------------------------
//    @FileName         :    NFProxyServerNet_ClientPlugin.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    NFProxyServerNet_ClientPlugin
//
// -------------------------------------------------------------------------


#include "AFCProxyServerToWorldModule.h"
#include "AFProxyServerNet_ClientPlugin.h"
#include "AFCProxyServerToGameModule.h"

//
//
#ifdef NF_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NFProxyServerNet_ClientPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFProxyServerNet_ClientPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int NFProxyServerNet_ClientPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFProxyServerNet_ClientPlugin::GetPluginName()
{
    return GET_CLASS_NAME(NFProxyServerNet_ClientPlugin)
}

void NFProxyServerNet_ClientPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIProxyServerToWorldModule, AFCProxyServerToWorldModule)
    REGISTER_MODULE(pPluginManager, AFIProxyServerToGameModule, AFCProxyServerToGameModule)
}

void NFProxyServerNet_ClientPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIProxyServerToGameModule, AFCProxyServerToGameModule)
    UNREGISTER_MODULE(pPluginManager, AFIProxyServerToWorldModule, AFCProxyServerToWorldModule)
}