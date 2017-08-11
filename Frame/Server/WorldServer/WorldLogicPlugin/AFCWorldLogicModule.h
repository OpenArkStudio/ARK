// -------------------------------------------------------------------------
//    @FileName			:    AFCWorldLogicModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCWorldLogicModule
//
// -------------------------------------------------------------------------

#ifndef AFC_WORLDLOGIC_MODULE_H
#define AFC_WORLDLOGIC_MODULE_H

#include "SDK/Base/AFMap.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIWorldLogicModule.h"

class AFCWorldLogicModule
    : public AFIWorldLogicModule
{
public:
    AFCWorldLogicModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

protected:

protected:
    AFIKernelModule* m_pKernelModule;
private:
};

#endif