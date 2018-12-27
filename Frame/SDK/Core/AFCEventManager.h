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

#include "SDK/Core/AFDefine.hpp"
#include "SDK/Core/AFCDataList.hpp"
#include "AFIEventManager.h"

namespace ark
{

    class AFCEventManager : public AFIEventManager
    {
    public:
        AFCEventManager() = delete;

        explicit AFCEventManager(AFGUID self);
        ~AFCEventManager() override;

        void Update() override;

        bool AddEventCallBack(const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb) override;
        bool RemoveEventCallBack(const int nEventID) override;

        bool DoEvent(const int nEventID, const AFIDataList& valueList) override;

    protected:
        bool HasEventCallBack(const int nEventID) override;

        bool Shut();

    private:
        AFGUID mSelf;

        AFList<int> mRemoveEventListEx;
        AFMapEx<int, AFList<EVENT_PROCESS_FUNCTOR_PTR>> mObjectEventInfoMapEx;
    };

}