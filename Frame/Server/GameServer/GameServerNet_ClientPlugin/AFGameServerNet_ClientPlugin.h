// -------------------------------------------------------------------------
//    @FileName			:    NFGameServerNet_ClientPlugin.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :   NFGameServerNet_ClientPlugin
//
// -------------------------------------------------------------------------

#ifndef NF_GAMESERVER_NETCLIENTPLUGIN_MODULE_H
#define NF_GAMESERVER_NETCLIENTPLUGIN_MODULE_H
///
#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

#ifdef NFGAMESERVERNET_CLIENTPLUGIN_EXPORTS
#define NFGAMESERVERNET_CLIENTPLUGIN_API __declspec(dllexport)
#else
#define NFGAMESERVERNET_CLIENTPLUGIN_API __declspec(dllimport)
#endif

class NFGameServerNet_ClientPlugin : public AFIPlugin
{
public:
    NFGameServerNet_ClientPlugin(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif