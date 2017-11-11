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

#include "SDK/Base/AFPlatform.hpp"
#include "SDK/Base/AFList.h"
#include "SDK/Base/AFDefine.h"
#include "SDK/Base/AFCDataList.h"

class  AFCHeartBeatElement
    : public NFList<HEART_BEAT_FUNCTOR_PTR>
{
public:
    bool operator==(const AFCHeartBeatElement& src)
    {
        if(strBeatName == src.strBeatName)
        {
            return true;
        }

        return false;
    }

    AFCHeartBeatElement()
    {
        nBeatTime = 0;
        nNextTriggerTime = 0;
        nCount = 0;
        strBeatName = NULL_STR;
        bStop = false;
        bForever = false;
    };

    virtual ~AFCHeartBeatElement()
    {
    }

    void DoHeartBeatEvent(int64_t nNowTime);
    bool CheckTime(int64_t nNowTime);
    bool IsStop()
    {
        return bStop;
    }

    AFGUID self;
    uint64_t id;
    int64_t nBeatTime;
    int64_t nNextTriggerTime;//next trigger time, millisecond
    int nCount;
    bool bStop;
    bool bForever;
    std::string strBeatName;
};

class AFIHeartBeatManager
{
public:
    virtual ~AFIHeartBeatManager() {}
    virtual AFGUID Self() = 0;
    virtual bool Execute() = 0;
    virtual bool Exist(const std::string& strHeartBeatName) = 0;

    virtual bool AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const int64_t nTime, const int nCount, const bool bForever = false) = 0;
    virtual bool RemoveHeartBeat(const std::string& strHeartBeatName) = 0;

    template<typename BaseType>
    bool AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const int64_t, const int), const int64_t nTime, const int nCount, const bool bForever = false)
    {
        HEART_BEAT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        HEART_BEAT_FUNCTOR_PTR functorPtr(ARK_NEW HEART_BEAT_FUNCTOR(functor));
        return AddHeartBeat(self, strHeartBeatName, functorPtr, nTime, nCount,  bForever);
    }
};