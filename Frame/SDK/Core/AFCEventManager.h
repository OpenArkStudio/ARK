// -------------------------------------------------------------------------
//    @FileName         :    AFCEventManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCEventManager
//
// -------------------------------------------------------------------------

#ifndef AFC_EVENT_MNG_H
#define AFC_EVENT_MNG_H

#include "AFIEventManager.h"
#include "SDK/Base/AFDefine.h"
#include "SDK/Base/AFCDataList.h"

using namespace ArkFrame;

class AFCEventManager
    : public AFIEventManager
{
public:
    AFCEventManager(AFGUID self);
    virtual ~AFCEventManager();

    virtual bool Init();
    virtual bool Shut();

    virtual bool Execute();

    virtual bool RemoveEventCallBack(const int nEventID);

    virtual bool DoEvent(const int nEventID, const AFIDataList& valueList);

    virtual bool AddEventCallBack(const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb);

protected:
    virtual bool HasEventCallBack(const int nEventID);

private:
    AFGUID mSelf;

    NFList<int> mRemoveEventListEx;
    AFMapEx<int, NFList<EVENT_PROCESS_FUNCTOR_PTR>> mObjectEventInfoMapEx;
};


#endif