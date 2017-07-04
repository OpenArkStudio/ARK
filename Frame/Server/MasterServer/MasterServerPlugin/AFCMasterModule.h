// -------------------------------------------------------------------------
//    @FileName         £º    NFCMasterModule.h
//    @Author           £º    Ark Game Tech
//    @Date             £º    2012-12-15
//    @Module           £º    NFCMasterModule
//
// -------------------------------------------------------------------------

#ifndef NFC_MASTER_MODULE_H
#define NFC_MASTER_MODULE_H

#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFIMasterModule.h"
#include "SDK/Interface/NFIMasterNet_ServerModule.h"

class NFCMasterModule
    : public NFIMasterModule
{

public:
    NFCMasterModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
protected:


private:

    NFIKernelModule* m_pKernelModule;
};

#endif