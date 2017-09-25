/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

#include "SDK/Base/AFList.h"
#include "SDK/Base/AFDefine.h"
#include "SDK/Base/AFCDataList.h"
#include "SDK/Base/AFIData.h"
#include "AFIHeartBeatManager.h"
#include "SDK/Base/AFMapEx.h"

using namespace ArkFrame;

class AFCHeartBeatManager : public AFIHeartBeatManager
{
public:

    AFCHeartBeatManager()
    {
        mSelf = NULL_GUID;
        mTimerIDIndex = NULL_GUID;
    }

    AFCHeartBeatManager(const AFGUID& self)
    {
        mSelf = self;
    }

    virtual ~AFCHeartBeatManager();

    virtual AFGUID Self();

    virtual bool Execute();

    virtual bool Exist(const std::string& strHeartBeatName);

    virtual bool AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const int64_t nTime, const int nCount, const bool bForever = false);
    virtual bool RemoveHeartBeat(const std::string& strHeartBeatName);

protected:
    AFGUID mSelf;

    NFList<std::string> mRemoveListEx;
    std::list<AFCHeartBeatElement> mAddListEx;
    AFMapEx<std::string, AFCHeartBeatElement> mHeartBeatElementMapEx;

    std::multimap<int64_t, ARK_SHARE_PTR<AFCHeartBeatElement>> mTimeList;
    AFGUID mTimerIDIndex;
};

