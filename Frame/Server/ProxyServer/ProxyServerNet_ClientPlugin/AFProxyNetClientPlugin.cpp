// -------------------------------------------------------------------------
//    @FileName         :    AFProxyNetClientPlugin.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    AFProxyNetClientPlugin
//
// -------------------------------------------------------------------------


#include "AFCProxyServerToWorldModule.h"
#include "AFProxyNetClientPlugin.h"
#include "AFCProxyServerToGameModule.h"

//
//
#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, AFProxyNetClientPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFProxyNetClientPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFProxyNetClientPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFProxyNetClientPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFProxyNetClientPlugin)
}

void AFProxyNetClientPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIProxyServerToWorldModule, AFCProxyServerToWorldModule)
    REGISTER_MODULE(pPluginManager, AFIProxyServerToGameModule, AFCProxyServerToGameModule)
}

void AFProxyNetClientPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIProxyServerToGameModule, AFCProxyServerToGameModule)
    UNREGISTER_MODULE(pPluginManager, AFIProxyServerToWorldModule, AFCProxyServerToWorldModule)
}