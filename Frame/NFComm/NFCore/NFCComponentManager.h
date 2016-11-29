// -------------------------------------------------------------------------
//    @FileName         :    NFCComponentManager.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2014-02-17
//    @Module           :    NFCComponentManager
//
// -------------------------------------------------------------------------

#ifndef NFC_COMPONENT_MANAGER_H
#define NFC_COMPONENT_MANAGER_H

#include "NFIComponentManager.h"
#include "NFComm/NFPluginModule/NFGUID.h"

/**
 * @class   NFCComponentManager
 *
 * @brief   Manager for nfc components.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFCComponentManager : public NFIComponentManager
{
private:

    /**
     * @fn  NFCComponentManager::NFCComponentManager()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    NFCComponentManager()
    {

    }

public:

    /**
     * @fn  NFCComponentManager::NFCComponentManager(const NFGUID& self)
     *
     * @brief   Constructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   self    The class instance that this method operates on.
     */

    NFCComponentManager(const NFGUID& self)
    {
        mSelf = self;
    }

    /**
     * @fn  virtual NFCComponentManager::~NFCComponentManager()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual ~NFCComponentManager()
    {
        ClearAll();
    }

    /**
     * @fn  virtual bool NFCComponentManager::Init();
     *
     * @brief   S this object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Init();

    /**
     * @fn  virtual bool NFCComponentManager::AfterInit();
     *
     * @brief   Determines if we can after init.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AfterInit();

    /**
     * @fn  virtual bool NFCComponentManager::BeforeShut();
     *
     * @brief   Determines if we can before shut.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool BeforeShut();

    /**
     * @fn  virtual bool NFCComponentManager::Shut();
     *
     * @brief   Shuts this object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Shut();

    /**
     * @fn  virtual bool NFCComponentManager::Execute();
     *
     * @brief   Executes this object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool Execute();

    /**
     * @fn  virtual NFGUID NFCComponentManager::Self();
     *
     * @brief   Gets the self.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A NFGUID.
     */

    virtual NFGUID Self();

protected:

    /**
     * @fn  virtual bool NFCComponentManager::AddComponent(const std::string& strComponentName, NF_SHARE_PTR<NFIComponent> pNewComponent);
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

    virtual bool AddComponent(const std::string& strComponentName, NF_SHARE_PTR<NFIComponent> pNewComponent);

private:
    /** @brief   The self. */
    NFGUID mSelf;
};

#endif