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

#pragma once

#include "SDK/Core/AFList.hpp"
#include "SDK/Core/AFDefine.hpp"
#include "SDK/Core/AFCDataList.hpp"
#include "SDK/Core/AFIData.hpp"
#include "SDK/Core/AFMap.hpp"
#include "AFIHeartBeatManager.h"

namespace ark
{

    class AFCHeartBeatManager : public AFIHeartBeatManager
    {
    public:
        AFCHeartBeatManager() = delete;

        explicit AFCHeartBeatManager(const AFGUID& self) : mSelf(self), mTimerIDIndex(0)
        {

        }

        virtual ~AFCHeartBeatManager();

        virtual AFGUID Self();

        virtual void Update();

        virtual bool Exist(const std::string& strHeartBeatName);

        virtual bool AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const int64_t nTime, const int nCount, const bool bForever = false);
        virtual bool RemoveHeartBeat(const std::string& strHeartBeatName);

    protected:
        bool ProcessFinishHeartBeat(AFCHeartBeatElement* pTarget);
        bool ProcessFinishHeartBeat();
        bool ProcessAddHeartBeat();

    private:
        AFGUID mSelf;

        AFList<std::string> mRemoveListEx;
        std::list<AFCHeartBeatElement> mAddListEx;
        AFMap<std::string, AFCHeartBeatElement> mHeartBeatElementMapEx;

        std::multimap<int64_t, AFCHeartBeatElement*> mTimeList;
        uint64_t mTimerIDIndex;
    };

}