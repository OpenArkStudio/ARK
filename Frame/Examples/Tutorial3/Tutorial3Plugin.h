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
#include "SDK/Interface/NFIPlugin.h"
#include "SDK/Interface/NFIPluginManager.h"

class Tutorial3Plugin : public NFIPlugin
{
public:
    Tutorial3Plugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif