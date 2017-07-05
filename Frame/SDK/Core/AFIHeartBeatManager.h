// -------------------------------------------------------------------------
//    @FileName         :    AFIHeartBeatManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-17
//    @Module           :    AFIHeartBeatManager
//
// -------------------------------------------------------------------------

#ifndef AFI_HEARTBEATMANAGER_H
#define AFI_HEARTBEATMANAGER_H

#include <string>
#include "AFMap.h"
#include "AFList.h"
#include "AFDefine.h"
#include "AFCDataList.h"
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
    AFINT64 nBeatTime;
    AFINT64 nNextTriggerTime;//next trigger time, millisecond
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

    virtual bool AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const AFINT64 nTime, const int nCount) = 0;

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
    bool AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFINT64, const int), const AFINT64 nTime, const int nCount)
    {
        HEART_BEAT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        HEART_BEAT_FUNCTOR_PTR functorPtr(NF_NEW HEART_BEAT_FUNCTOR(functor));
        return AddHeartBeat(self, strHeartBeatName, functorPtr, nTime, nCount);
    }
};

#endif
