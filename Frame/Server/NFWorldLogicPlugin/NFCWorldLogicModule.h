// -------------------------------------------------------------------------
//    @FileName			:    NFCWorldLogicModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    NFCWorldLogicModule
//
// -------------------------------------------------------------------------

#ifndef NFC_WORLDLOGIC_MODULE_H
#define NFC_WORLDLOGIC_MODULE_H

#include "SDK/Core/NFMap.h"
#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFIWorldLogicModule.h"

class NFCWorldLogicModule
    : public NFIWorldLogicModule
{
public:
    NFCWorldLogicModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

protected:

protected:
    NFIKernelModule* m_pKernelModule;
private:
};

#endif