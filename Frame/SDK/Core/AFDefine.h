// -------------------------------------------------------------------------
//    @FileName         :    AFDefine.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    NFDefine
//
// -------------------------------------------------------------------------
#pragma once

#include <functional>
#include "AFCData.h"
#include "AFCDataList.h"
#include "SDK/Core/AFGUID.h"
#include "SDK/Interface/AFPlatform.h"

namespace ArkFrame
{

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
 * @typedef std::function<int(const AFGUID&, const std::string&, const float, const int)> HEART_BEAT_FUNCTOR
 *
 * @brief   functor.
 */

using HEART_BEAT_FUNCTOR = std::function<int(const AFGUID&, const std::string&, const AFINT64, const int)>;

/**
 * @typedef std::function<void()> MODULE_HEART_BEAT_FUNCTOR
 *
 * @brief   Defines an alias representing the module heart beat functor.
 */

using MODULE_HEART_BEAT_FUNCTOR = std::function<void()>;

/**
 * @typedef std::function<int(const AFGUID&, const std::string&, const AFIData&, const AFIData&)> PROPERTY_EVENT_FUNCTOR
 *
 * @brief   Defines an alias representing the property event functor.
 */

using PROPERTY_EVENT_FUNCTOR = std::function<int(const AFGUID&, const std::string&, const AFIData&, const AFIData&)>;

/**
 * @typedef std::function<int(const AFGUID&, const RECORD_EVENT_DATA&, const AFIData&, const AFIData&)> RECORD_EVENT_FUNCTOR
 *
 * @brief   Defines an alias representing the record event functor.
 */

using RECORD_EVENT_FUNCTOR = std::function<int(const AFGUID&, const RECORD_EVENT_DATA&, const AFIData&, const AFIData&)>;

/**
 * @typedef std::function<int(const AFGUID&, const std::string&, const CLASS_OBJECT_EVENT, const AFIDataList&)> CLASS_EVENT_FUNCTOR
 *
 * @brief   Defines an alias representing the class event functor.
 */

using CLASS_EVENT_FUNCTOR = std::function<bool(const AFGUID&, const std::string&, const CLASS_OBJECT_EVENT, const AFIDataList&)>;

/**
 * @typedef std::function<int(const AFGUID&, const int, const AFIDataList&)> EVENT_PROCESS_FUNCTOR
 *
 * @brief   Defines an alias representing the event process functor.
 */

using EVENT_PROCESS_FUNCTOR = std::function<int(const AFGUID&, const int, const AFIDataList&)>;

/**
 * @typedef std::function<int(const AFGUID&, const int, std::string&)> EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR
 *
 * @brief   Defines an alias representing the event asynchronous process begin functor.
 */

using EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR = std::function<int(const AFGUID&, const int, std::string&)>;

/**
 * @typedef std::function<int(const AFGUID&, const int, const int, const std::string&)> EVENT_ASYNC_PROCESS_END_FUNCTOR
 *
 * @brief   Defines an alias representing the event asynchronous process end functor.
 */

using EVENT_ASYNC_PROCESS_END_FUNCTOR = std::function<int(const AFGUID&, const int, const int, const std::string&)>;

/**
 * @typedef NF_SHARE_PTR<HEART_BEAT_FUNCTOR> HEART_BEAT_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the heart beat functor pointer.
 */

using HEART_BEAT_FUNCTOR_PTR = NF_SHARE_PTR<HEART_BEAT_FUNCTOR>;

/**
 * @typedef NF_SHARE_PTR<MODULE_HEART_BEAT_FUNCTOR> MODULE_HEART_BEAT_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the module heart beat functor pointer.
 */

using MODULE_HEART_BEAT_FUNCTOR_PTR = NF_SHARE_PTR<MODULE_HEART_BEAT_FUNCTOR>;

/**
 * @typedef NF_SHARE_PTR<PROPERTY_EVENT_FUNCTOR> PROPERTY_EVENT_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the property event functor pointer.
 */

using PROPERTY_EVENT_FUNCTOR_PTR = NF_SHARE_PTR<PROPERTY_EVENT_FUNCTOR>;

/**
 * @typedef NF_SHARE_PTR<RECORD_EVENT_FUNCTOR> RECORD_EVENT_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the record event functor pointer.
 */

using RECORD_EVENT_FUNCTOR_PTR = NF_SHARE_PTR<RECORD_EVENT_FUNCTOR>;

/**
 * @typedef NF_SHARE_PTR<CLASS_EVENT_FUNCTOR> CLASS_EVENT_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the class event functor pointer.
 */

using CLASS_EVENT_FUNCTOR_PTR = NF_SHARE_PTR<CLASS_EVENT_FUNCTOR> ;

/**
 * @typedef NF_SHARE_PTR<EVENT_PROCESS_FUNCTOR> EVENT_PROCESS_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the event process functor pointer.
 */

using EVENT_PROCESS_FUNCTOR_PTR = NF_SHARE_PTR<EVENT_PROCESS_FUNCTOR>;

/**
 * @typedef NF_SHARE_PTR<EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR> EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the event asynchronous process begin functor
 *          pointer.
 */

using EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR_PTR = NF_SHARE_PTR<EVENT_ASYNC_PROCESS_BEGIN_FUNCTOR>;

/**
 * @typedef NF_SHARE_PTR<EVENT_ASYNC_PROCESS_END_FUNCTOR> EVENT_ASYNC_PROCESS_END_FUNCTOR_PTR
 *
 * @brief   Defines an alias representing the event asynchronous process end functor pointer.
 */

using EVENT_ASYNC_PROCESS_END_FUNCTOR_PTR = NF_SHARE_PTR<EVENT_ASYNC_PROCESS_END_FUNCTOR>;

}
