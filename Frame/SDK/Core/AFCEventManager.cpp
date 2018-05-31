/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include "AFCEventManager.h"

AFCEventManager::AFCEventManager(AFGUID self) : mSelf(self)
{
    
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
    ARK_SHARE_PTR<AFList<EVENT_PROCESS_FUNCTOR_PTR>> pEventInfo = mObjectEventInfoMapEx.GetElement(nEventID);
    if(nullptr == pEventInfo)
    {
        pEventInfo = std::make_shared<AFList<EVENT_PROCESS_FUNCTOR_PTR>>();
        mObjectEventInfoMapEx.AddElement(nEventID, pEventInfo);
    }

    pEventInfo->Add(cb);

    return true;
}

void AFCEventManager::Update()
{
    int nEvent = 0;
    bool bRet = mRemoveEventListEx.First(nEvent);
    while(bRet)
    {
        mObjectEventInfoMapEx.RemoveElement(nEvent);

        bRet = mRemoveEventListEx.Next(nEvent);
    }

    mRemoveEventListEx.ClearAll();
}

bool AFCEventManager::RemoveEventCallBack(const int nEventID)
{
    mRemoveEventListEx.Add(nEventID);
    return true;
}

bool AFCEventManager::DoEvent(const int nEventID, const AFIDataList& valueList)
{
    ARK_SHARE_PTR<AFList<EVENT_PROCESS_FUNCTOR_PTR>> pEventInfo = mObjectEventInfoMapEx.GetElement(nEventID);
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
    ARK_SHARE_PTR<AFList<EVENT_PROCESS_FUNCTOR_PTR>> pEventInfo = mObjectEventInfoMapEx.GetElement(nEventID);
    return nullptr != pEventInfo;
}