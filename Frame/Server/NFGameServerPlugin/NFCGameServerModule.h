// -------------------------------------------------------------------------
//    @FileName			:    NFCGameServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    NFCGameServerModule
//
// -------------------------------------------------------------------------

#ifndef NFC_GAMESERVER_MODULE_H
#define NFC_GAMESERVER_MODULE_H

#include "SDK/Core/NFMap.h"
#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFIGameServerModule.h"
#include "SDK/Interface/NFIClassModule.h"
#include "SDK/Interface/NFIUUIDModule.h"

class NFCGameServerModule
    : public NFIGameServerModule
{
public:
    NFCGameServerModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~NFCGameServerModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();
    virtual bool BeforeShut();


protected:


protected:
    NFIUUIDModule* m_pUUIDModule;
    NFIClassModule* m_pClassModule;
    NFIKernelModule* m_pKernelModule;
private:
};

#endif
