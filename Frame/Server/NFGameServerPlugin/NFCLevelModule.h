// -------------------------------------------------------------------------
//    @FileName			:    NFCLevelModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-09-28
//    @Module           :    NFCLevelModule
//
// -------------------------------------------------------------------------

#ifndef NFC_LEVEL_MODULE_H
#define NFC_LEVEL_MODULE_H

#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFIGameLogicModule.h"
#include "SDK/Interface/NFILevelModule.h"
#include "SDK/Interface/NFILogModule.h"
#include "SDK/Interface/NFIPropertyConfigModule.h"
#include "SDK/Interface/NFIPluginManager.h"
#include "NFComm/NFMessageDefine/NFProtocolDefine.hpp"
#include "SDK/Interface/NFIElementModule.h"

class NFCLevelModule
    : public NFILevelModule
{
public:
    NFCLevelModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~NFCLevelModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

    virtual int AddExp(const AFGUID& self, const int nExp);

private:
    NFIPropertyConfigModule* m_pPropertyConfigModule;
    NFIKernelModule* m_pKernelModule;
    NFILogModule* m_pLogModule;
    NFIElementModule* m_pElementModule;
};

#endif
