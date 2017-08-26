/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @file  	AFCHeartBeatManager.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFCHeartBeatManager                                                  *
*****************************************************************************/
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

