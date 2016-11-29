// -------------------------------------------------------------------------
//    @FileName         :    NFIComponent.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2014-02-17
//    @Module           :    NFIComponent
//
// -----------------------------------------------------------------------

#ifndef NFI_COMPONENT_H
#define NFI_COMPONENT_H

#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFPluginModule/NFIModule.h"

/**
 * @class   NFIComponent
 *
 * @brief   A nfi component.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

class NFIComponent : public NFIModule
{
private:

    /**
     * @fn  NFIComponent::NFIComponent()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    NFIComponent()
    {
    }

public:

    /**
     * @fn  NFIComponent::NFIComponent(NFGUID self, const std::string& strName)
     *
     * @brief   Constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @param   self    The class instance that this method operates on.
     * @param   strName The name.
     */

    NFIComponent(NFGUID self, const std::string& strName)
    {
        mbHasInit = false;
        mbEnable = true;
        mSelf = self;
        mstrName = strName;
    }

    /**
     * @fn  virtual NFIComponent::~NFIComponent()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    virtual ~NFIComponent() {}

    /**
     * @fn  template <typename T> NF_SHARE_PTR<T> NFIComponent::CreateNewInstance()
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
        NF_SHARE_PTR<NFIComponent> pComponent = CreateNewInstance();
        if (nullptr != pComponent)
        {
            if (TIsDerived<T, NFIComponent>::Result)
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
     * @fn  virtual bool NFIComponent::SetEnable(const bool bEnable)
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
     * @fn  virtual bool NFIComponent::Enable()
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
     * @fn  virtual bool NFIComponent::SetHasInit(const bool bEnable)
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
     * @fn  virtual bool NFIComponent::HasInit()
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
     * @fn  virtual NFGUID NFIComponent::Self()
     *
     * @brief   Gets the self.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A NFGUID.
     */

    virtual NFGUID Self()
    {
        return NULL_GUID;
    }

    /**
     * @fn  virtual const std::string& NFIComponent::GetComponentName() const
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
     * @fn  virtual int NFIComponent::OnASyncEvent(const NFGUID& self, const int event, std::string& arg)
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

    virtual int OnASyncEvent(const NFGUID& self, const int event, std::string& arg)
    {
        return 0;
    }

protected:

    /**
     * @fn  virtual NF_SHARE_PTR<NFIComponent> NFIComponent::CreateNewInstance()
     *
     * @brief   Creates new instance.
     *
     * @author  flyicegood
     * @date    2016/11/29
     *
     * @return  The new new instance.
     */

    virtual NF_SHARE_PTR<NFIComponent> CreateNewInstance()
    {
        return nullptr;
    };

private:
    /** @brief   True if megabytes enable. */
    bool mbEnable;
    /** @brief   True if megabytes has init. */
    bool mbHasInit;
    /** @brief   The self. */
    NFGUID mSelf;
    /** @brief   Name of the mstr. */
    std::string mstrName;
};

#endif
