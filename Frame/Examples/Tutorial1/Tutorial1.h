// -------------------------------------------------------------------------
//    @FileName			:    Tutorial1.h
//    @Author           :    Ark Game Tech
//    @Date             :    2014-05-14 08:51
//    @Module           :   Tutorial1
//
// -------------------------------------------------------------------------

#ifndef NF_TUTORIAL1_H
#define NF_TUTORIAL1_H

#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

class Tutorial1 : public AFIPlugin
{
public:
    Tutorial1(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif