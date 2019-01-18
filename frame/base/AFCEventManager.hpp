/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#pragma once

#include "AFDefine.hpp"
#include "AFCDataList.hpp"
#include "AFList.hpp"
#include "AFMap.hpp"
#include "interface/AFIEventManager.h"

namespace ark
{

    class AFCEventManager : public AFIEventManager
    {
    public:
        AFCEventManager() = delete;

        explicit AFCEventManager(AFGUID self) :
            mSelf(self)
        {
        }

        ~AFCEventManager() override
        {
            Shut();
        }

        void Update() override
        {
            int nEvent = 0;
            for (bool bRet = mRemoveEventListEx.First(nEvent); bRet; bRet = mRemoveEventListEx.Next(nEvent))
            {
                mObjectEventInfoMapEx.RemoveElement(nEvent);
            }

            mRemoveEventListEx.ClearAll();
        }

        bool AddEventCallBack(const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb) override
        {
            ARK_SHARE_PTR<AFList<EVENT_PROCESS_FUNCTOR_PTR>> pEventInfo = mObjectEventInfoMapEx.GetElement(nEventID);
            if (pEventInfo == nullptr)
            {
                pEventInfo = std::make_shared<AFList<EVENT_PROCESS_FUNCTOR_PTR>>();
                mObjectEventInfoMapEx.AddElement(nEventID, pEventInfo);
            }

            return pEventInfo->Add(cb);
        }

        bool RemoveEventCallBack(const int nEventID) override
        {
            mRemoveEventListEx.Add(nEventID);
            return true;
        }

        bool DoEvent(const int nEventID, const AFIDataList& valueList) override
        {
            ARK_SHARE_PTR<AFList<EVENT_PROCESS_FUNCTOR_PTR>> pEventInfo = mObjectEventInfoMapEx.GetElement(nEventID);

            if (pEventInfo == nullptr)
            {
                return false;
            }

            EVENT_PROCESS_FUNCTOR_PTR cb;
            for (bool bRet = pEventInfo->First(cb); bRet; bRet = pEventInfo->Next(cb))
            {
                (*cb)(mSelf, nEventID, valueList);
            }

            return true;
        }

    protected:
        bool HasEventCallBack(const int nEventID) override
        {
            ARK_SHARE_PTR<AFList<EVENT_PROCESS_FUNCTOR_PTR>> pEventInfo = mObjectEventInfoMapEx.GetElement(nEventID);
            return pEventInfo != nullptr;
        }

        bool Shut()
        {
            mRemoveEventListEx.ClearAll();
            mObjectEventInfoMapEx.ClearAll();

            return true;
        }

    private:
        AFGUID mSelf;

        AFList<int> mRemoveEventListEx;
        AFMapEx<int, AFList<EVENT_PROCESS_FUNCTOR_PTR>> mObjectEventInfoMapEx;
    };

}