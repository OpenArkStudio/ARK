// -------------------------------------------------------------------------
//    @FileName			:    NFProxyServerNet_ClientPlugin.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    NFProxyServerNet_ClientPlugin
//
// -------------------------------------------------------------------------

#ifndef NF_PROXYSERVER_NETCLIENTPLUGIN_MODULE_H
#define NF_PROXYSERVER_NETCLIENTPLUGIN_MODULE_H
///
#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

#ifdef NFPROXYSERVERNET_CLIENTPLUGIN_EXPORTS
#define NFPROXYSERVERNET_CLIENTPLUGIN_API __declspec(dllexport)
#else
#define NFPROXYSERVERNET_CLIENTPLUGIN_API __declspec(dllimport)
#endif

class NFProxyServerNet_ClientPlugin : public AFIPlugin
{
public:
    NFProxyServerNet_ClientPlugin(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif