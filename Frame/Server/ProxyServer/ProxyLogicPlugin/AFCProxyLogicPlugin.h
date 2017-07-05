// -------------------------------------------------------------------------
//    @FileName			:    AFCProxyPlugin.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    AFCProxyPlugin
//
// -------------------------------------------------------------------------

#ifndef AFC_PROXYSERVERPLUGIN_H
#define AFC_PROXYSERVERPLUGIN_H

#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"


class NFProxyLogicPlugin : public AFIPlugin
{
public:
    NFProxyLogicPlugin(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif