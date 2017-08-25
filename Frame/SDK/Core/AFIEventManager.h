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
// * @FileName  AFIEventManager.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFIEventManager                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFIEventManager                                                  *
*****************************************************************************/
#pragma once

#include "SDK/Base/AFPlatform.hpp"
#include "SDK/Base/AFDefine.h"
#include "SDK/Base/AFList.h"
#include "SDK/Base/AFMapEx.h"
#include "SDK/Base/AFIDataList.h"
#include "SDK/EventDefine/AFEventDefine.h"

using namespace ArkFrame;
/**
 * @class   AFIEventManager
 *
 * @brief   Manager for nfi events.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class AFIEventManager
{
public:

    /**
     * @fn  virtual AFIEventManager::~AFIEventManager()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual ~AFIEventManager() {}

    /**
     * @fn  virtual bool AFIEventManager::Execute() = 0;
     *
     * @brief   Executes this object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Execute() = 0;

    /**
     * @fn  template<typename BaseType> bool AFIEventManager::AddEventCallBack(const int nEventID, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const int, const AFIDataList&))
     *
     * @brief   Adds an event call back.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  BaseType    Type of the base type.
     * @param           nEventID    Identifier for the event.
     * @param [in,out]  pBase       If non-null, the base.
     * @param [in,out]  handler     If non-null, the handler.
     *
     * @return  True if it succeeds, false if it fails.
     */

    template<typename BaseType>
    bool AddEventCallBack(const int nEventID, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const int, const AFIDataList&))
    {
        EVENT_PROCESS_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        EVENT_PROCESS_FUNCTOR_PTR functorPtr(new EVENT_PROCESS_FUNCTOR(functor));
        return AddEventCallBack(nEventID, functorPtr);
    }

    /**
     * @fn  virtual bool AFIEventManager::RemoveEventCallBack(const int nEventID) = 0;
     *
     * @brief   Removes the event call back described by nEventID.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nEventID    Identifier for the event.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool RemoveEventCallBack(const int nEventID) = 0;

    /**
     * @fn  virtual bool AFIEventManager::DoEvent(const int nEventID, const AFIDataList& valueList) = 0;
     *
     * @brief   Executes the event operation.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nEventID    Identifier for the event.
     * @param   valueList   List of values.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool DoEvent(const int nEventID, const AFIDataList& valueList) = 0;

    /**
     * @fn  virtual bool AFIEventManager::AddEventCallBack(const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb) = 0;
     *
     * @brief   Adds an event call back to 'cb'.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nEventID    Identifier for the event.
     * @param   cb          The cb.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddEventCallBack(const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb) = 0;

    /**
     * @fn  virtual bool AFIEventManager::HasEventCallBack(const int nEventID) = 0;
     *
     * @brief   ///////////////////////////////////////////////////////////////////////.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nEventID    Identifier for the event.
     *
     * @return  True if event call back, false if not.
     */

protected:
    virtual bool HasEventCallBack(const int nEventID) = 0;

};
