// -------------------------------------------------------------------------
//    @FileName			:    NFCProxyLogicModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    NFCProxyLogicModule
//
// -------------------------------------------------------------------------

#ifndef NFC_PROXYLOGIC_MODULE_H
#define NFC_PROXYLOGIC_MODULE_H

#include "SDK/Core/NFMap.h"
#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFIGameLogicModule.h"
#include "SDK/Interface/NFIClassModule.h"
#include "SDK/Interface/NFIProxyLogicModule.h"

class NFCProxyLogicModule
    : public NFIProxyLogicModule
{
public:
    NFCProxyLogicModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();


protected:


protected:
    NFIClassModule* m_pClassModule;
    NFIKernelModule* m_pKernelModule;
private:
};

#endif