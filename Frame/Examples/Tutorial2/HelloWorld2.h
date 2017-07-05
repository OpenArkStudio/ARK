// -------------------------------------------------------------------------
//    @FileName			:    HelloWorld2.h
//    @Author           :    Ark Game Tech
//    @Date             :    2014-05-14 08:51
//    @Module           :   HelloWorld2
//
// -------------------------------------------------------------------------

#ifndef AFC_HELLO_WORLD2_H
#define AFC_HELLO_WORLD2_H

#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

class HelloWorld2
    : public AFIModule
{
public:
    HelloWorld2(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool AfterInit();

    virtual bool Execute();

    virtual bool BeforeShut();
    virtual bool Shut();

protected:
    int OnPropertyCallBackEvent(const AFGUID& self, const std::string& strProperty, const AFIDataList::TData& oldVarList, const AFIDataList::TData& newVarList);



};

#endif
