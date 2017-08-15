
#include "SDK/Base/AFTime.hpp"
#include "AFCHeartBeatManager.h"


AFCHeartBeatManager::~AFCHeartBeatManager()
{
    mHeartBeatElementMapEx.ClearAll();
}

void AFCHeartBeatElement::DoHeartBeatEvent()
{
    HEART_BEAT_FUNCTOR_PTR cb;
    bool bRet = First(cb);
    while(bRet)
    {
        (*cb)(self, strBeatName, nBeatTime, nCount);

        bRet = Next(cb);
    }
}
//////////////////////////////////////////////////////////////////////////
bool AFCHeartBeatManager::Execute()
{
    //millisecond
    int64_t nTime = AFCTimeBase::GetInstance().GetNowMillisecond();
    AFCHeartBeatElement* pElement = mHeartBeatElementMapEx.FirstNude();
    while(nullptr != pElement)
    {
        if(nTime > pElement->nNextTriggerTime && pElement->nCount > 0)
        {
            pElement->nCount--;

            pElement->DoHeartBeatEvent();

            if(pElement->nCount <= 0)
            {
                //等待删除
                mRemoveListEx.Add(pElement->strBeatName);
            }
            else
            {
                //Do Event
                pElement->nNextTriggerTime = nTime + pElement->nBeatTime;
            }
        }

        pElement = mHeartBeatElementMapEx.NextNude();
    }

    //删除所有过时心跳
    std::string strHeartBeatName;
    bool bRet = mRemoveListEx.First(strHeartBeatName);
    while(bRet)
    {
        mHeartBeatElementMapEx.RemoveElement(strHeartBeatName);

        bRet = mRemoveListEx.Next(strHeartBeatName);
    }

    mRemoveListEx.ClearAll();

    //////////////////////////////////////////////////////////////////////////
    //添加新心跳也是延时添加的
    for(std::list<AFCHeartBeatElement>::iterator iter = mAddListEx.begin(); iter != mAddListEx.end(); ++iter)
    {
        if(NULL == mHeartBeatElementMapEx.GetElement(iter->strBeatName))
        {
            ARK_SHARE_PTR<AFCHeartBeatElement> pHeartBeatEx(ARK_NEW AFCHeartBeatElement());
            *pHeartBeatEx = *iter;
            mHeartBeatElementMapEx.AddElement(pHeartBeatEx->strBeatName, pHeartBeatEx);
        }
    }

    mAddListEx.clear();

    return true;
}

bool AFCHeartBeatManager::RemoveHeartBeat(const std::string& strHeartBeatName)
{
    return mRemoveListEx.Add(strHeartBeatName);
}

AFGUID AFCHeartBeatManager::Self()
{
    return mSelf;
}

//////////////////////////////////////////////////////////////////////////
bool AFCHeartBeatManager::AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const int64_t nTime, const int nCount)
{
    AFCHeartBeatElement xHeartBeat;
    xHeartBeat.nNextTriggerTime = AFCTimeBase::GetInstance().GetNowMillisecond() + nTime;
    xHeartBeat.nBeatTime = nTime;
    xHeartBeat.nCount = nCount;
    xHeartBeat.self = self;
    xHeartBeat.strBeatName = strHeartBeatName;
    xHeartBeat.Add(cb);
    mAddListEx.push_back(xHeartBeat);

    return true;
}

bool AFCHeartBeatManager::Exist(const std::string& strHeartBeatName)
{
    if(mHeartBeatElementMapEx.GetElement(strHeartBeatName))
    {
        return true;
    }

    return false;
}
