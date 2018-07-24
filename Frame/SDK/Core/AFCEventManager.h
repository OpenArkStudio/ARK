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

#include "SDK/Core/AFDefine.h"
#include "SDK/Core/AFCDataList.h"
#include "AFIEventManager.h"

class AFCEventManager : public AFIEventManager
{
public:
    AFCEventManager() = delete;

    explicit AFCEventManager(AFGUID self);
    virtual ~AFCEventManager();

    virtual bool Init();
    virtual bool Shut();

    virtual void Update();

    virtual bool RemoveEventCallBack(const int nEventID);

    virtual bool DoEvent(const int nEventID, const AFIDataList& valueList);

    virtual bool AddEventCallBack(const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb);

protected:
    virtual bool HasEventCallBack(const int nEventID);

private:
    AFGUID mSelf;

    AFList<int> mRemoveEventListEx;
    AFMapEx<int, AFList<EVENT_PROCESS_FUNCTOR_PTR>> mObjectEventInfoMapEx;
};