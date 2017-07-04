// -------------------------------------------------------------------------
//    @FileName			:    NFCPropertyTrailModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-09-30
//    @Module           :    NFCPropertyTrailModule
//
// -------------------------------------------------------------------------

#ifndef NFC_PROPERTY_TRAIL_MODULE_H
#define NFC_PROPERTY_TRAIL_MODULE_H

#include "SDK/Interface/NFIKernelModule.h"
#include "SDK/Interface/NFIGameLogicModule.h"
#include "SDK/Interface/NFIPropertyModule.h"
#include "SDK/Interface/NFIElementModule.h"
#include "SDK/Interface/NFIClassModule.h"
#include "SDK/Interface/NFIPropertyConfigModule.h"
#include "SDK/Interface/NFIPluginManager.h"
#include "SDK/Interface/NFIPropertyTrailModule.h"
#include "SDK/Interface/NFILogModule.h"

class NFCPropertyTrailModule
    : public NFIPropertyTrailModule
{
public:
    NFCPropertyTrailModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~NFCPropertyTrailModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

    virtual void StartTrail(const AFGUID self);
    virtual void EndTrail(const AFGUID self);

protected:

    int LogObjectData(const AFGUID& self);
    int TrailObjectData(const AFGUID& self);

    int OnObjectPropertyEvent(const AFGUID& self, const std::string& strPropertyName, const AFDataList::TData& oldVar, const AFDataList::TData& newVar);

    int OnObjectRecordEvent(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFDataList::TData& oldVar, const AFDataList::TData& newVar);

private:

    NFIKernelModule* m_pKernelModule;
    NFIElementModule* m_pElementModule;
    NFIClassModule* m_pClassModule;
    NFILogModule* m_pLogModule;
};


#endif
