// -------------------------------------------------------------------------
//    @FileName         :    NFIComponentManager.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2014-02-17
//    @Module           :    NFIComponentManager
//
// -------------------------------------------------------------------------

#ifndef NFI_COMPONENT_MANAGER_H
#define NFI_COMPONENT_MANAGER_H

#include <memory>
#include "NFMap.h"
#include "NFIComponent.h"
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFPluginModule/NFIModule.h"

class NFIComponentManager : public NFIModule, public NFMapEx<std::string, NFIComponent>
{
public:
    virtual ~NFIComponentManager() {}

    /**
     * @fn  template <typename T> bool NFIComponentManager::AddComponent()
     *
     * @brief   Adds component.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  T   Generic type parameter.
     *
     * @return  True if it succeeds, false if it fails.
     */

    template <typename T>
    bool AddComponent()
    {
        if (!TIsDerived<T, NFIComponent>::Result)
        {
            //BaseTypeComponent must inherit from NFIComponent;
            return false;
        }

        NFIComponent* pComponent = NF_NEW T();
        return AddComponent(pComponent->GetComponentName(), NF_SHARE_PTR<NFIComponent>(pComponent));
    }

    /**
     * @fn  template <typename T> NF_SHARE_PTR<T> NFIComponentManager::FindComponent(const std::string& strName)
     *
     * @brief   Searches for the first component.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  T   Generic type parameter.
     * @param   strName The name.
     *
     * @return  The found component.
     */

    template <typename T>
    NF_SHARE_PTR<T> FindComponent(const std::string& strName)
    {
        if (!TIsDerived<T, NFIComponent>::Result)
        {
            //BaseTypeComponent must inherit from NFIComponent;
            return NF_SHARE_PTR<T>();
        }

        NF_SHARE_PTR<NFIComponent> pComponent = First();
        while (nullptr != pComponent)
        {
            if (pComponent->GetComponentName() == strName)
            {
                NF_SHARE_PTR<T> pT = std::dynamic_pointer_cast<T>(pComponent);
                if (nullptr != pT)
                {
                    return pT;
                }
                else
                {
                    return NF_SHARE_PTR<T>();
                }
            }

            pComponent = Next();
        }

        return NF_SHARE_PTR<T>();
    }

    /**
     * @fn  virtual NFGUID NFIComponentManager::Self() = 0;
     *
     * @brief   Gets the self ID.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A NFGUID.
     */

    virtual NFGUID Self() = 0;

    /**
     * @fn  virtual bool NFIComponentManager::AddComponent(const std::string& strComponentName, NF_SHARE_PTR<NFIComponent> pNewComponent) = 0;
     *
     * @brief   Adds a component to 'pNewComponent'.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strComponentName    Name of the component.
     * @param   pNewComponent       The new component.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddComponent(const std::string& strComponentName, NF_SHARE_PTR<NFIComponent> pNewComponent) = 0;

private:
};

#endif
