// -------------------------------------------------------------------------
//    @FileName			:    NFWorldLogicPlugin.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :   NFWorldLogicPluginModule
//
// -------------------------------------------------------------------------

#ifndef AFC_WORLDLOGICPLUGIN_MODULE_H
#define AFC_WORLDLOGICPLUGIN_MODULE_H

#include "SDK/Base/AFPlatform.hpp"
#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

#ifdef NFWORLDLOGICPLUGIN_EXPORTS
#define NFWORLDLOGICPLUGIN_API __declspec(dllexport)
#else
#define NFWORLDLOGICPLUGIN_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
class NFWorldLogicPlugin : public AFIPlugin
{
public:
    NFWorldLogicPlugin(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif