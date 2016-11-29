// -------------------------------------------------------------------------
//    @FileName         :    NFIHeartBeatManager.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-17
//    @Module           :    NFIHeartBeatManager
//
// -------------------------------------------------------------------------

#ifndef NFI_HEARTBEATMANAGER_H
#define NFI_HEARTBEATMANAGER_H

#include <string>
#include "NFMap.h"
#include "NFList.h"
#include "NFDefine.h"
#include "NFCDataList.h"

/**
 * @class   NFCHeartBeatElement
 *
 * @brief   A nfc heart beat element.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class  NFCHeartBeatElement
    : public NFList<HEART_BEAT_FUNCTOR_PTR>
{
public:
    bool operator==(const NFCHeartBeatElement& src)
    {
        if (strBeatName == src.strBeatName)
        {
            return true;
        }

        return false;
    }

    NFCHeartBeatElement()
    {
        fBeatTime = 0.0f;
        nNextTriggerTime = 0;
        nCount = 0;
        strBeatName = "";
    };

    virtual ~NFCHeartBeatElement()
    {
    }

    void DoHeartBeatEvent();

    float fBeatTime;
    NFINT64 nNextTriggerTime;//next trigger time, millisecond
    int nCount;
    std::string strBeatName;
    NFGUID self;
};

/**
 * @class   NFIHeartBeatManager
 *
 * @brief   Manager for nfi heart beats.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFIHeartBeatManager
{
public:

    /**
     * @fn  virtual NFIHeartBeatManager::~NFIHeartBeatManager()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual ~NFIHeartBeatManager() {}

    /**
     * @fn  virtual NFGUID NFIHeartBeatManager::Self() = 0;
     *
     * @brief   Gets the self.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  A NFGUID.
     */

    virtual NFGUID Self() = 0;

    /**
     * @fn  virtual bool NFIHeartBeatManager::Execute() = 0;
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
     * @fn  virtual bool NFIHeartBeatManager::Exist(const std::string& strHeartBeatName) = 0;
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
     * @fn  virtual bool NFIHeartBeatManager::AddHeartBeat(const NFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount) = 0;
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

    virtual bool AddHeartBeat(const NFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount) = 0;

    /**
     * @fn  virtual bool NFIHeartBeatManager::RemoveHeartBeat(const std::string& strHeartBeatName) = 0;
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
     * @fn  template<typename BaseType> bool NFIHeartBeatManager::AddHeartBeat(const NFGUID self, const std::string& strHeartBeatName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const float, const int), const float fTime, const int nCount)
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
    bool AddHeartBeat(const NFGUID self, const std::string& strHeartBeatName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const float, const int), const float fTime, const int nCount)
    {
        HEART_BEAT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        HEART_BEAT_FUNCTOR_PTR functorPtr(NF_NEW HEART_BEAT_FUNCTOR(functor));
        return AddHeartBeat(self, strHeartBeatName, functorPtr, fTime, nCount);
    }
};

#endif
