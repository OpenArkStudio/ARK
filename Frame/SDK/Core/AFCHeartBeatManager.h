// -------------------------------------------------------------------------
//    @FileName         :    AFCHeartBeatManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-17
//    @Module           :    AFCHeartBeatManager
//
// -------------------------------------------------------------------------

#ifndef _GT_HEARTBEATMANAGER_H
#define _GT_HEARTBEATMANAGER_H

#include "AFMap.h"
#include "AFList.h"
#include "AFDefine.h"
#include "AFCDataList.h"
#include "AFIHeartBeatManager.h"
#include "AFIData.h"

using namespace ArkFrame;

/**
 * @class   AFCHeartBeatManager
 *
 * @brief   Manager for nfc heart beats.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class AFCHeartBeatManager : public AFIHeartBeatManager
{
public:

    /**
     * @fn  AFCHeartBeatManager::AFCHeartBeatManager()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    AFCHeartBeatManager()
    {
        mSelf = NULL_GUID;
    }

    /**
     * @fn  AFCHeartBeatManager::AFCHeartBeatManager(const AFGUID& self)
     *
     * @brief   Constructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   self    The class instance that this method operates on.
     */

    AFCHeartBeatManager(const AFGUID& self)
    {
        mSelf = self;
    }

    /**
     * @fn  virtual AFCHeartBeatManager::~AFCHeartBeatManager();
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual ~AFCHeartBeatManager();

    /**
     * @fn  virtual AFGUID AFCHeartBeatManager::Self();
     *
     * @brief   Gets the self.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A AFGUID.
     */

    virtual AFGUID Self();

    /**
     * @fn  virtual bool AFCHeartBeatManager::Execute();
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
     * @fn  virtual bool AFCHeartBeatManager::Exist(const std::string& strHeartBeatName);
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
     * @fn  virtual bool AFCHeartBeatManager::AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount);
     *
     * @brief   Adds a heart beat.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   self                The class instance that this method operates on.
     * @param   strHeartBeatName    Name of the heart beat.
     * @param   cb                  The cb.
     * @param   nTime               The delay time.
     * @param   nCount              Number of.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const AFINT64 nTime, const int nCount);

    /**
     * @fn  virtual bool AFCHeartBeatManager::RemoveHeartBeat(const std::string& strHeartBeatName);
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
    AFGUID mSelf;

    /** @brief   The remove list ex. */
    NFList<std::string> mRemoveListEx;
    /** @brief   The add list ex. */
    std::list<AFCHeartBeatElement> mAddListEx;
    /** @brief   The heart beat element map ex. */
    AFMapEx<std::string, AFCHeartBeatElement> mHeartBeatElementMapEx;
};

#endif