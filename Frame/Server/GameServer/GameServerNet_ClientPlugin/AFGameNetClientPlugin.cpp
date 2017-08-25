// -------------------------------------------------------------------------
//    @FileName         :    AFGameNetClientPlugin.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :    AFGameNetClientPlugin
//
// -------------------------------------------------------------------------


#include "AFCGameServerToWorldModule.h"
#include "AFGameNetClientPlugin.h"

#ifdef ARK_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, AFGameNetClientPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, AFGameNetClientPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int AFGameNetClientPlugin::GetPluginVersion()
{
    return 0;
}

const std::string AFGameNetClientPlugin::GetPluginName()
{
    return GET_CLASS_NAME(AFGameNetClientPlugin)
}

void AFGameNetClientPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIGameServerToWorldModule, AFCGameServerToWorldModule)

}

void AFGameNetClientPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIGameServerToWorldModule, AFCGameServerToWorldModule)
}