// -------------------------------------------------------------------------
//    @FileName			:    HelloWorld1.h
//    @Author           :    Ark Game Tech
//    @Date             :    2014-05-14 08:51
//    @Module           :   HelloWorld1
//
// -------------------------------------------------------------------------

#ifndef AFC_HELLO_WORLD1_H
#define AFC_HELLO_WORLD1_H

#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

class HelloWorld1
    : public AFIModule
{
public:
    HelloWorld1(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool AfterInit();

    virtual bool Execute();

    virtual bool BeforeShut();
    virtual bool Shut();

protected:

};

#endif
