// -------------------------------------------------------------------------
//    @FileName			:    AFCPropertyTrailModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-09-30
//    @Module           :    AFCPropertyTrailModule
//
// -------------------------------------------------------------------------

#ifndef AFC_PROPERTY_TRAIL_MODULE_H
#define AFC_PROPERTY_TRAIL_MODULE_H

#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIGameLogicModule.h"
#include "SDK/Interface/AFIPropertyModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIPropertyConfigModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Interface/AFIPropertyTrailModule.h"
#include "SDK/Interface/AFILogModule.h"

class AFCPropertyTrailModule
    : public AFIPropertyTrailModule
{
public:
    AFCPropertyTrailModule(AFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~AFCPropertyTrailModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

    virtual void StartTrail(const AFGUID self);
    virtual void EndTrail(const AFGUID self);

protected:

    int LogObjectData(const AFGUID& self);
    int TrailObjectData(const AFGUID& self);

    int OnObjectPropertyEvent(const AFGUID& self, const std::string& strPropertyName, const AFIDataList::TData& oldVar, const AFIDataList::TData& newVar);

    int OnObjectRecordEvent(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFIDataList::TData& oldVar, const AFIDataList::TData& newVar);

private:

    AFIKernelModule* m_pKernelModule;
    AFIElementModule* m_pElementModule;
    AFIClassModule* m_pClassModule;
    AFILogModule* m_pLogModule;
};


#endif
