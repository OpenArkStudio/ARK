// -------------------------------------------------------------------------
//    @FileName			:    NFTutorial3.h
//    @Author           :    Ark Game Tech
//    @Date             :    2014-05-14 08:51
//    @Module           :   NFTutorial3
//
// -------------------------------------------------------------------------

#ifndef NF_TUTORIAL3_H
#define NF_TUTORIAL3_H

///
#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

class Tutorial3Plugin : public AFIPlugin
{
public:
    Tutorial3Plugin(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif