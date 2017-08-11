// -------------------------------------------------------------------------
//    @FileName         :    AFCEventProcessModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCEventProcessModule
//
// -------------------------------------------------------------------------

#include "AFCEventManager.h"

AFCEventManager::AFCEventManager(AFGUID self)
{
    mSelf = self;
}

AFCEventManager::~AFCEventManager()
{
    mRemoveEventListEx.ClearAll();

    mObjectEventInfoMapEx.ClearAll();
}

bool AFCEventManager::Init()
{
    return true;
}

bool AFCEventManager::Shut()
{
    mRemoveEventListEx.ClearAll();
    mObjectEventInfoMapEx.ClearAll();

    return true;
}

bool AFCEventManager::AddEventCallBack(const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb)
{
    ARK_SHARE_PTR<NFList<EVENT_PROCESS_FUNCTOR_PTR>> pEventInfo = mObjectEventInfoMapEx.GetElement(nEventID);
    if(nullptr == pEventInfo)
    {
        pEventInfo = ARK_SHARE_PTR<NFList<EVENT_PROCESS_FUNCTOR_PTR>>(ARK_NEW NFList<EVENT_PROCESS_FUNCTOR_PTR>());
        mObjectEventInfoMapEx.AddElement(nEventID, pEventInfo);
    }

    pEventInfo->Add(cb);

    return true;
}

bool AFCEventManager::Execute()
{
    int nEvent = 0;
    bool bRet = mRemoveEventListEx.First(nEvent);
    while(bRet)
    {
        mObjectEventInfoMapEx.RemoveElement(nEvent);

        bRet = mRemoveEventListEx.Next(nEvent);
    }

    mRemoveEventListEx.ClearAll();

    return true;
}

bool AFCEventManager::RemoveEventCallBack(const int nEventID)
{
    mRemoveEventListEx.Add(nEventID);
    return true;
}

bool AFCEventManager::DoEvent(const int nEventID, const AFIDataList& valueList)
{
    ARK_SHARE_PTR<NFList<EVENT_PROCESS_FUNCTOR_PTR>> pEventInfo = mObjectEventInfoMapEx.GetElement(nEventID);
    if(nullptr == pEventInfo)
    {
        return false;
    }

    EVENT_PROCESS_FUNCTOR_PTR cb;
    bool bRet = pEventInfo->First(cb);
    while(bRet)
    {
        (*cb)(mSelf, nEventID,  valueList);

        bRet = pEventInfo->Next(cb);
    }

    return true;
}

bool AFCEventManager::HasEventCallBack(const int nEventID)
{
    ARK_SHARE_PTR<NFList<EVENT_PROCESS_FUNCTOR_PTR>> pEventInfo = mObjectEventInfoMapEx.GetElement(nEventID);
    return nullptr != pEventInfo;
}
