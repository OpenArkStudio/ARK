// -------------------------------------------------------------------------
//    @FileName			:    NFTutorial2.h
//    @Author           :    Ark Game Tech
//    @Date             :    2014-05-14 08:51
//    @Module           :   NFTutorial2
//
// -------------------------------------------------------------------------

#ifndef NF_TUTORIAL2_H
#define NF_TUTORIAL2_H

///
#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

class Tutorial2 : public AFIPlugin
{
public:
    Tutorial2(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif