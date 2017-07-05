// -------------------------------------------------------------------------
//    @FileName         :    AFIComponent.h
//    @Author           :    Ark Game Tech
//    @Date             :    2014-02-17
//    @Module           :    AFIComponent
//
// -----------------------------------------------------------------------

#ifndef AFI_COMPONENT_H
#define AFI_COMPONENT_H

#include "SDK/Interface/AFPlatform.h"
#include "SDK/Core/AFGUID.h"
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
     * @fn  template <typename T> NF_SHARE_PTR<T> AFIComponent::CreateNewInstance()
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
    NF_SHARE_PTR<T> CreateNewInstance()
    {
        NF_SHARE_PTR<AFIComponent> pComponent = CreateNewInstance();
        if (nullptr != pComponent)
        {
            if (TIsDerived<T, AFIComponent>::Result)
            {
                NF_SHARE_PTR<T> pT = std::dynamic_pointer_cast<T>(pComponent);
                if (nullptr != pT)
                {
                    return pT;
                }
            }
        }

        return NF_SHARE_PTR<T>();
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
     * @fn  virtual NF_SHARE_PTR<AFIComponent> AFIComponent::CreateNewInstance()
     *
     * @brief   Creates new instance.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The new new instance.
     */

    virtual NF_SHARE_PTR<AFIComponent> CreateNewInstance()
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
