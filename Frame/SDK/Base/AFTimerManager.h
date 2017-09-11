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

#include "AFPlatform.hpp"

class AFTimerManager;

enum class ETimerStatus : uint8_t
{
    Pending,
    Active,
    Paused,
    Executing,
};

struct AFTimerHandle
{
public:
    friend class AFTimerManager;
    
    AFTimerHandle()
        : handle(0)
    {}

    bool IsValid() const
    {
        return handle != 0;
    }

    void Invalidate()
    {
        handle = 0;
    }

    void MakeValid()
    {
        //AFTimerManager::ValidateHandle(*this);
    }

    bool operator==(const AFTimerHandle& other) const
    {
        return handle == other.handle;
    }

    bool operator!=(const AFTimerHandle& other) const
    {
        return handle != other.handle;
    }

    //std::string ToString()
    //{
    //    //TODO
    //}

private:
    uint64_t handle;
};

struct AFTimerData 
{
    uint8_t bLoop : 1;
    ETimerStatus nStatus;
    float fRate;
    double dExpire_time;
    AFTimerHandle xTimerHandle;

    AFTimerData()
        : bLoop(false)
        , nStatus(ETimerStatus::Active)
        , fRate(0)
        , dExpire_time(0)
    {}

    bool operator<(const AFTimerData& other) const
    {
        return dExpire_time < other.dExpire_time;
    }

    void clear()
    {
        xTimerHandle.Invalidate();
    }
};

class AFTimerManager
{
    //TODO:
};

