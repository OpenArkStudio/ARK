// -------------------------------------------------------------------------
//    @FileName         :    NFDefine.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFDefine
//
// -------------------------------------------------------------------------

#ifndef NF_DEFINE_H
#define NF_DEFINE_H

#include <functional>
#include "NFIDataList.h"
#include "NFComm/NFPluginModule/NFGUID.h"

/**
 * @enum    CLASS_OBJECT_EVENT
 *
 * @brief   Values that represent class object events.
 */

enum CLASS_OBJECT_EVENT
{
    COE_DESTROY = 0,
    COE_BEFOREDESTROY,
    COE_CREATE_NODATA,
    COE_CREATE_LOADDATA,
    COE_CREATE_BEFORE_EFFECT,
    COE_CREATE_EFFECTDATA,
    COE_CREATE_AFTER_EFFECT,
    COE_CREATE_HASDATA,
    COE_CREATE_FINISH,  // ToModify任务创建完成后再挂回调
};

/**
 * @struct  RECORD_EVENT_DATA
 *
 * @brief   A record event data.
 *
 * @author  flyicegood
 * @date    2016/11/29
 */

struct RECORD_EVENT_DATA
{
    /**
     * @fn  RECORD_EVENT_DATA()
     *
     * @brief   Default constructor.
     *
     * @author  flyicegood
     * @date    2016/11/29
     */

    RECORD_EVENT_DATA()
    {
        nOpType = 0;
        nRow = 0;
        nCol = 0;
    }

    /** @brief   Type of the operation. */
    int nOpType;
    /** @brief   The row. */
    int nRow;
    /** @brief   The col. */
    int nCol;
    /** @brief   Name of the record. */
    std::string strRecordName;
};

/**
 * @typedef std::function<int(const NFGUID&, const std::string&, const float, const int)> HEART_BEAT_FUNCTOR
 *
 * @brief   functor.
 */

typedef std::function<int(const NFGUID&, const std::string&, const float, const int)> HEART_BEAT_FUNCTOR;

/**
 * @typedef std::function<void()> MODULE_HEART_BEAT_FUNCTOR
 *
 * @brief   Defines an alias representing the module heart beat functor.
 */

typedef std::function<void()> MODULE_HEART_BEAT_FUNCTOR;

/**
 * @typedef std::function<int(const NFGUID&, const std::string&, const NFIDataList::TData&, const NFIDataList::TData&)> PROPERTY_EVENT_FUNCTOR
 *
 * @brief   Defines an alias representing the property event functor.
 */

typedef std::function<int(const NFGUID&, const std::string&, const NFIDataList::TData&, const NFIDataList::TData&)> PROPERTY_EVENT_FUNCTOR;

/**
 * @typedef std::function<int(const NFGUID&, const RECORD_EVENT_DATA&, const NFIDataList::TData&, const NFIDataList::TData&)> RECORD_EVENT_FUNCTOR
 *
 * @brief   Defines an alias representing the record event functor.
 */

typedef std::function<int(const NFGUID&, const RECORD_EVENT_DATA&, const NFIDataList::TData&, const NFIDataList::TData&)> RECORD_EVENT_FUNCTOR;

/**
 * @typedef std::function<int(const NFGUID&, const std::string&, const CLASS_OBJECT_EVENT, const NFIDataList&)> CLASS_EVENT_FUNCTOR
 *
 * @brief   Defines an alias representing the class event functor.
 */

typedef std::function<int(const NFGUID&, const std::string&, const CLASS_OBJECT_EVENT, const NFIDataList&)> CLASS_EVENT_FUNCTOR;

/**
 * @typedef std::function<int(const NFGUID&, const int, const NFIDataList&)> EVENT_PROCESS_FUNCTOR
 *
 * @brief   Defines an alias representing the event process functor.
 */

typedef std::function<int(const NFGUID&, const int, const NFIDataList&)> EVENT_PROCESS_FUNCTOR;

/**
 * @typedef std::function<int(const NFGUID&, const int, std::string&)> EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR
 *
 * @brief   Defines an alias representing the event asynchronous process begin functor.
 */

typedef std::function<int(const NFGUID&, const int, std::string&)> EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR;

/**
 * @typedef std::function<int(const NFGUID&, const int, const int, const std::string&)> EVENT_ASYNC_PROCESS_END_FUNCTOR
 *
 * @brief   Defines an alias representing the event asynchronous process end functor.
 */

typedef std::function<int(const NFGUID&, const int, const int, const std::string&)> EVENT_ASYNC_PROCESS_END_FUNCTOR;

/**
 * @typedef NF_SHARE_PTR<HEART_BEAT_FUNCTOR> HEART_BEAT_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the heart beat functor pointer.
 */

typedef NF_SHARE_PTR<HEART_BEAT_FUNCTOR> HEART_BEAT_FUNCTOR_PTR;

/**
 * @typedef NF_SHARE_PTR<MODULE_HEART_BEAT_FUNCTOR> MODULE_HEART_BEAT_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the module heart beat functor pointer.
 */

typedef NF_SHARE_PTR<MODULE_HEART_BEAT_FUNCTOR> MODULE_HEART_BEAT_FUNCTOR_PTR;

/**
 * @typedef NF_SHARE_PTR<PROPERTY_EVENT_FUNCTOR> PROPERTY_EVENT_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the property event functor pointer.
 */

typedef NF_SHARE_PTR<PROPERTY_EVENT_FUNCTOR> PROPERTY_EVENT_FUNCTOR_PTR;

/**
 * @typedef NF_SHARE_PTR<RECORD_EVENT_FUNCTOR> RECORD_EVENT_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the record event functor pointer.
 */

typedef NF_SHARE_PTR<RECORD_EVENT_FUNCTOR> RECORD_EVENT_FUNCTOR_PTR;

/**
 * @typedef NF_SHARE_PTR<CLASS_EVENT_FUNCTOR> CLASS_EVENT_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the class event functor pointer.
 */

typedef NF_SHARE_PTR<CLASS_EVENT_FUNCTOR> CLASS_EVENT_FUNCTOR_PTR;

/**
 * @typedef NF_SHARE_PTR<EVENT_PROCESS_FUNCTOR> EVENT_PROCESS_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the event process functor pointer.
 */

typedef NF_SHARE_PTR<EVENT_PROCESS_FUNCTOR> EVENT_PROCESS_FUNCTOR_PTR;

/**
 * @typedef NF_SHARE_PTR<EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR> EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the event asynchronous process begin functor
 *          pointer.
 */

typedef NF_SHARE_PTR<EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR> EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR_PTR;

/**
 * @typedef NF_SHARE_PTR<EVENT_ASYNC_PROCESS_END_FUNCTOR> EVENT_ASYNC_PROCESS_END_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the event asynchronous process end functor pointer.
 */

typedef NF_SHARE_PTR<EVENT_ASYNC_PROCESS_END_FUNCTOR> EVENT_ASYNC_PROCESS_END_FUNCTOR_PTR;

#endif
