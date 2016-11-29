// -------------------------------------------------------------------------
//    @FileName         :    NFCHeartBeatManager.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-17
//    @Module           :    NFCHeartBeatManager
//
// -------------------------------------------------------------------------

#ifndef _GT_HEARTBEATMANAGER_H
#define _GT_HEARTBEATMANAGER_H

#include "NFMap.h"
#include "NFList.h"
#include "NFDefine.h"
#include "NFIDataList.h"
#include "NFIHeartBeatManager.h"

/**
 * @class   NFCHeartBeatManager
 *
 * @brief   Manager for nfc heart beats.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFCHeartBeatManager : public NFIHeartBeatManager
{
public:

    /**
     * @fn  NFCHeartBeatManager::NFCHeartBeatManager()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    NFCHeartBeatManager()
    {
        mSelf = NULL_GUID;
    }

    /**
     * @fn  NFCHeartBeatManager::NFCHeartBeatManager(const NFGUID& self)
     *
     * @brief   Constructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   self    The class instance that this method operates on.
     */

    NFCHeartBeatManager(const NFGUID& self)
    {
        mSelf = self;
    }

    /**
     * @fn  virtual NFCHeartBeatManager::~NFCHeartBeatManager();
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual ~NFCHeartBeatManager();

    /**
     * @fn  virtual NFGUID NFCHeartBeatManager::Self();
     *
     * @brief   Gets the self.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A NFGUID.
     */

    virtual NFGUID Self();

    /**
     * @fn  virtual bool NFCHeartBeatManager::Execute();
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
     * @fn  virtual bool NFCHeartBeatManager::Exist(const std::string& strHeartBeatName);
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

    virtual bool Exist(const std::string& strHeartBeatName);

    /**
     * @fn  virtual bool NFCHeartBeatManager::AddHeartBeat(const NFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount);
     *
     * @brief   Adds a heart beat.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   self                The class instance that this method operates on.
     * @param   strHeartBeatName    Name of the heart beat.
     * @param   cb                  The cb.
     * @param   fTime               The time.
     * @param   nCount              Number of.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddHeartBeat(const NFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount);

    /**
     * @fn  virtual bool NFCHeartBeatManager::RemoveHeartBeat(const std::string& strHeartBeatName);
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

    virtual bool RemoveHeartBeat(const std::string& strHeartBeatName);

protected:
    /** @brief   The self. */
    NFGUID mSelf;

    /** @brief   The remove list ex. */
    NFList<std::string> mRemoveListEx;
    /** @brief   The add list ex. */
    std::list<NFCHeartBeatElement> mAddListEx;
    /** @brief   The heart beat element map ex. */
    NFMapEx<std::string, NFCHeartBeatElement> mHeartBeatElementMapEx;
};

#endif