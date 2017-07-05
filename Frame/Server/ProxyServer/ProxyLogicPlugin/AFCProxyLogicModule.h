// -------------------------------------------------------------------------
//    @FileName			:    AFCProxyLogicModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    AFCProxyLogicModule
//
// -------------------------------------------------------------------------

#ifndef AFC_PROXYLOGIC_MODULE_H
#define AFC_PROXYLOGIC_MODULE_H

#include "SDK/Core/AFMap.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIGameLogicModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIProxyLogicModule.h"

class AFCProxyLogicModule
    : public AFIProxyLogicModule
{
public:
    AFCProxyLogicModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();


protected:


protected:
    AFIClassModule* m_pClassModule;
    AFIKernelModule* m_pKernelModule;
private:
};

#endif