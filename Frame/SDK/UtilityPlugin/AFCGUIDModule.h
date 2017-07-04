// -------------------------------------------------------------------------
//    @FileName         :    AFCGUIDModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCGUIDModule
//    @Desc             :
// -------------------------------------------------------------------------

#pragma once

#include "SDK/Interface/AFIGUIDModule.h"

namespace GUIDModule
{
    extern uint64_t GetNowInMsec();
    class IdWorkerUnThreadSafe;
    class IdWorkerThreadSafe;
}


class AFCGUIDModule
    : public AFIGUIDModule
{
public:

    AFCGUIDModule(NFIPluginManager* p);
    virtual ~AFCGUIDModule() {}

    virtual bool Init();
    virtual bool AfterInit();
    virtual bool Execute();
    virtual bool BeforeShut();
    virtual bool Shut();

    virtual void SetWorkerAndDatacenter(uint16_t worker_id, uint16_t data_center_id);
    virtual uint64_t CreateGUID();

private:
#ifdef AF_THREAD_SAFE
    GUIDModule::IdWorkerThreadSafe* m_pIDWoker;
#else
    GUIDModule::IdWorkerUnThreadSafe* m_pIDWoker;
#endif // AF_THREAD_SAFE

    
};
