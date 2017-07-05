// -------------------------------------------------------------------------
//    @FileName			:    NFZoneLogicPlugin.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :   NFZoneLogicPlugin
//
// -------------------------------------------------------------------------

#ifndef AFC_GAMESERVERPLUGIN_H
#define AFC_GAMESERVERPLUGIN_H

#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

class NFGameServerPlugin : public AFIPlugin
{
public:
    NFGameServerPlugin(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif
