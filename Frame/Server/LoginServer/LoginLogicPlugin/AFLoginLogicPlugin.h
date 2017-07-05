// -------------------------------------------------------------------------
//    @FileName			:    NFLoginLogicPlugin.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :    LoginLogicPluginModule
//
// -------------------------------------------------------------------------

#ifndef AFC_LOGINLOGICPLUGIN_MODULE_H
#define AFC_LOGINLOGICPLUGIN_MODULE_H

#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

#ifdef NFLOGINLOGICPLUGIN_EXPORTS
#define NFLOGINLOGICPLUGIN_API __declspec(dllexport)
#else
#define NFLOGINLOGICPLUGIN_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
class NFLoginLogicPlugin : public AFIPlugin
{
public:
    NFLoginLogicPlugin(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif