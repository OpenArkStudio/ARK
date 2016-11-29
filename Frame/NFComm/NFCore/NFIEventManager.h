// -------------------------------------------------------------------------
//    @FileName         :    NFIEventManager.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-15
//    @Module           :    NFIEventManager
//
// -------------------------------------------------------------------------

#ifndef NFI_EVENT_MNG_H
#define NFI_EVENT_MNG_H

#include <iostream>
#include <functional>
#include "NFDefine.h"
#include "NFList.h"
#include "NFMapEx.h"
#include "NFComm/NFCore/NFIDataList.h"
#include "NFComm/NFEventDefine/NFEventDefine.h"

/**
 * @class   NFIEventManager
 *
 * @brief   Manager for nfi events.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFIEventManager
{
public:

    /**
     * @fn  virtual NFIEventManager::~NFIEventManager()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual ~NFIEventManager() {}

    /**
     * @fn  virtual bool NFIEventManager::Execute() = 0;
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
     * @fn  template<typename BaseType> bool NFIEventManager::AddEventCallBack(const int nEventID, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const int, const NFIDataList&))
     *
     * @brief   Adds an event call back.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  BaseType    Type of the base type.
     * @param           nEventID    Identifier for the event.
     * @param [in,out]  pBase       If non-null, the base.
     * @param [in,out]  handler     If non-null, the handler.
     *
     * @return  True if it succeeds, false if it fails.
     */

    template<typename BaseType>
    bool AddEventCallBack(const int nEventID, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const int, const NFIDataList&))
    {
        EVENT_PROCESS_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        EVENT_PROCESS_FUNCTOR_PTR functorPtr(new EVENT_PROCESS_FUNCTOR(functor));
        return AddEventCallBack(nEventID, functorPtr);
    }

    /**
     * @fn  virtual bool NFIEventManager::RemoveEventCallBack(const int nEventID) = 0;
     *
     * @brief   Removes the event call back described by nEventID.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nEventID    Identifier for the event.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool RemoveEventCallBack(const int nEventID) = 0;

    /**
     * @fn  virtual bool NFIEventManager::DoEvent(const int nEventID, const NFIDataList& valueList) = 0;
     *
     * @brief   Executes the event operation.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nEventID    Identifier for the event.
     * @param   valueList   List of values.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool DoEvent(const int nEventID, const NFIDataList& valueList) = 0;

    /**
     * @fn  virtual bool NFIEventManager::AddEventCallBack(const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb) = 0;
     *
     * @brief   Adds an event call back to 'cb'.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nEventID    Identifier for the event.
     * @param   cb          The cb.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddEventCallBack(const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb) = 0;

    /**
     * @fn  virtual bool NFIEventManager::HasEventCallBack(const int nEventID) = 0;
     *
     * @brief   ///////////////////////////////////////////////////////////////////////.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   nEventID    Identifier for the event.
     *
     * @return  True if event call back, false if not.
     */

protected:
    virtual bool HasEventCallBack(const int nEventID) = 0;

};

#endif
