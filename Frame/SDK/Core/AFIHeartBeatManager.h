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
// * @file  	AFIHeartBeatManager.h                                                *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFIHeartBeatManager                                                  *
*****************************************************************************/
#pragma once

#include "SDK/Base/AFPlatform.hpp"
#include "SDK/Base/AFMap.h"
#include "SDK/Base/AFList.h"
#include "SDK/Base/AFDefine.h"
#include "SDK/Base/AFCDataList.h"
using namespace ArkFrame;
/**
 * @class   AFCHeartBeatElement
 *
 * @brief   A nfc heart beat element.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

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
    };

    virtual ~AFCHeartBeatElement()
    {
    }

    void DoHeartBeatEvent();

    AFGUID self;
    int64_t nBeatTime;
    int64_t nNextTriggerTime;//next trigger time, millisecond
    int nCount;
    std::string strBeatName;
    //datalist
};

/**
 * @class   AFIHeartBeatManager
 *
 * @brief   Manager for nfi heart beats.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class AFIHeartBeatManager
{
public:

    /**
     * @fn  virtual AFIHeartBeatManager::~AFIHeartBeatManager()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual ~AFIHeartBeatManager() {}

    /**
     * @fn  virtual AFGUID AFIHeartBeatManager::Self() = 0;
     *
     * @brief   Gets the self.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A AFGUID.
     */

    virtual AFGUID Self() = 0;

    /**
     * @fn  virtual bool AFIHeartBeatManager::Execute() = 0;
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
     * @fn  virtual bool AFIHeartBeatManager::Exist(const std::string& strHeartBeatName) = 0;
     *
     * @brief   Exists the given string heart beat name.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strHeartBeatName    Name of the heart beat.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Exist(const std::string& strHeartBeatName) = 0;

    /**
     * @fn  virtual bool AFIHeartBeatManager::AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount) = 0;
     *
     * @brief   Adds a heart beat.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   self                The class instance that this method operates on.
     * @param   strHeartBeatName    Name of the heart beat.
     * @param   cb                  The cb.
     * @param   nTime               The dalay time.
     * @param   nCount              Number of.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const int64_t nTime, const int nCount) = 0;

    /**
     * @fn  virtual bool AFIHeartBeatManager::RemoveHeartBeat(const std::string& strHeartBeatName) = 0;
     *
     * @brief   Removes the heart beat described by strHeartBeatName.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strHeartBeatName    Name of the heart beat.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool RemoveHeartBeat(const std::string& strHeartBeatName) = 0;

    /**
     * @fn  template<typename BaseType> bool AFIHeartBeatManager::AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const float, const int), const float fTime, const int nCount)
     *
     * @brief   Adds a heart beat.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  BaseType    Type of the base type.
     * @param           self                The class instance that this method operates on.
     * @param           strHeartBeatName    Name of the heart beat.
     * @param [in,out]  pBase               If non-null, the base.
     * @param [in,out]  handler             If non-null, the handler.
     * @param           fTime               The time.
     * @param           nCount              Number of.
     *
     * @return  True if it succeeds, false if it fails.
     */

    template<typename BaseType>
    bool AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const int64_t, const int), const int64_t nTime, const int nCount)
    {
        HEART_BEAT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        HEART_BEAT_FUNCTOR_PTR functorPtr(ARK_NEW HEART_BEAT_FUNCTOR(functor));
        return AddHeartBeat(self, strHeartBeatName, functorPtr, nTime, nCount);
    }
};


