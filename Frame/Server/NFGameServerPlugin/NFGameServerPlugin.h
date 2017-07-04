// -------------------------------------------------------------------------
//    @FileName			:    NFZoneLogicPlugin.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :   NFZoneLogicPlugin
//
// -------------------------------------------------------------------------

#ifndef NFC_GAMESERVERPLUGIN_H
#define NFC_GAMESERVERPLUGIN_H

#include "SDK/Interface/NFIPlugin.h"
#include "SDK/Interface/NFIPluginManager.h"

class NFGameServerPlugin : public NFIPlugin
{
public:
    NFGameServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif
