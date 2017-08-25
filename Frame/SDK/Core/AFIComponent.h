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
// * @FileName  AFIComponent.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFIComponent                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFIComponent                                                  *
*****************************************************************************/
#ifndef AFI_COMPONENT_H
#define AFI_COMPONENT_H

#include "SDK/Base/AFPlatform.hpp"
#include "SDK/Base/AFGUID.h"
#include "SDK/Interface/AFIModule.h"

/**
 * @class   AFIComponent
 *
 * @brief   A nfi component.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class AFIComponent : public AFIModule
{
private:

    /**
     * @fn  AFIComponent::AFIComponent()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    AFIComponent()
    {
    }

public:

    /**
     * @fn  AFIComponent::AFIComponent(AFGUID self, const std::string& strName)
     *
     * @brief   Constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   self    The class instance that this method operates on.
     * @param   strName The name.
     */

    AFIComponent(AFGUID self, const std::string& strName)
    {
        mbHasInit = false;
        mbEnable = true;
        mSelf = self;
        mstrName = strName;
    }

    /**
     * @fn  virtual AFIComponent::~AFIComponent()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    virtual ~AFIComponent() {}

    /**
     * @fn  template <typename T> ARK_SHARE_PTR<T> AFIComponent::CreateNewInstance()
     *
     * @brief   Creates new instance.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  T   Generic type parameter.
     *
     * @return  The new new instance.
     */

    template <typename T>
    ARK_SHARE_PTR<T> CreateNewInstance()
    {
        ARK_SHARE_PTR<AFIComponent> pComponent = CreateNewInstance();
        if (nullptr != pComponent)
        {
            if (TIsDerived<T, AFIComponent>::Result)
            {
                ARK_SHARE_PTR<T> pT = std::dynamic_pointer_cast<T>(pComponent);
                if (nullptr != pT)
                {
                    return pT;
                }
            }
        }

        return ARK_SHARE_PTR<T>();
    }

    /**
     * @fn  virtual bool AFIComponent::SetEnable(const bool bEnable)
     *
     * @brief   Sets an enable.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   bEnable The enable.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetEnable(const bool bEnable)
    {
        return mbEnable;
    }

    /**
     * @fn  virtual bool AFIComponent::Enable()
     *
     * @brief   Enables this object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Enable()
    {
        return mbEnable;
    }

    /**
     * @fn  virtual bool AFIComponent::SetHasInit(const bool bEnable)
     *
     * @brief   Sets has init.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   bEnable The enable.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetHasInit(const bool bEnable)
    {
        mbHasInit = bEnable;
        return mbHasInit;
    }

    /**
     * @fn  virtual bool AFIComponent::HasInit()
     *
     * @brief   Query if this object has init.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if init, false if not.
     */

    virtual bool HasInit()
    {
        return mbHasInit;
    }

    /**
     * @fn  virtual AFGUID AFIComponent::Self()
     *
     * @brief   Gets the self.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A AFGUID.
     */

    virtual AFGUID Self()
    {
        return NULL_GUID;
    }

    /**
     * @fn  virtual const std::string& AFIComponent::GetComponentName() const
     *
     * @brief   Gets component name.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The component name.
     */

    virtual const std::string& GetComponentName() const
    {
        return mstrName;
    };

    /**
     * @fn  virtual int AFIComponent::OnASyncEvent(const AFGUID& self, const int event, std::string& arg)
     *
     * @brief   for actor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param           self    The class instance that this method operates on.
     * @param           event   The event.
     * @param [in,out]  arg     The argument.
     *
     * @return  An int.
     */

    virtual int OnASyncEvent(const AFGUID& self, const int event, std::string& arg)
    {
        return 0;
    }

protected:

    /**
     * @fn  virtual ARK_SHARE_PTR<AFIComponent> AFIComponent::CreateNewInstance()
     *
     * @brief   Creates new instance.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The new new instance.
     */

    virtual ARK_SHARE_PTR<AFIComponent> CreateNewInstance()
    {
        return nullptr;
    };

private:
    /** @brief   True if megabytes enable. */
    bool mbEnable;
    /** @brief   True if megabytes has init. */
    bool mbHasInit;
    /** @brief   The self. */
    AFGUID mSelf;
    /** @brief   Name of the mstr. */
    std::string mstrName;
};

#endif

