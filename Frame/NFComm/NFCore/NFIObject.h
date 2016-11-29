// -------------------------------------------------------------------------
//    @FileName         :    NFIObject.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-17
//    @Module           :    NFIObject
//
// -------------------------------------------------------------------------

#ifndef NFI_OBJECT_H
#define NFI_OBJECT_H


#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFIDataList.h"
#include "NFIRecord.h"
#include "NFIRecordManager.h"
#include "NFIHeartBeatManager.h"
#include "NFIPropertyManager.h"
#include "NFIComponentManager.h"
#include "NFIEventManager.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

/**
 * @class   NFIObject
 *
 * @brief   A nfi object.
 *
 * @author  flyicegood
 * @date    2016/11/22
 */

class NFIObject
{
public:

    /**
     * @fn  NFIObject::NFIObject(NFGUID self)
     *
     * @brief   Constructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   self    The class instance that this method operates on.
     */

    NFIObject(NFGUID self)
    {

    }

    /**
     * @fn  virtual NFIObject::~NFIObject()
     *
     * @brief   Destructor.
     *
     * @author  flyicegood
     * @date    2016/11/22
     */

    virtual ~NFIObject() {}

    /**
     * @fn  virtual bool NFIObject::Execute() = 0;
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
     * @fn  virtual NFGUID NFIObject::Self() = 0;
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
     * @fn  template<typename BaseType> bool NFIObject::AddPropertyCallBack(const std::string& strPropertyName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const NFIDataList::TData&, const NFIDataList::TData&))
     *
     * @brief   Adds a property call back.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  BaseType    Type of the base type.
     * @param           strPropertyName Name of the property.
     * @param [in,out]  pBase           If non-null, the base.
     * @param [in,out]  handler         If non-null, the handler.
     *
     * @return  True if it succeeds, false if it fails.
     */

    template<typename BaseType>
    bool AddPropertyCallBack(const std::string& strPropertyName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const NFIDataList::TData&, const NFIDataList::TData&))
    {
        PROPERTY_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        PROPERTY_EVENT_FUNCTOR_PTR functorPtr(NF_NEW PROPERTY_EVENT_FUNCTOR(functor));
        return AddPropertyCallBack(strPropertyName, functorPtr);
    }

    /**
     * @fn  template<typename BaseType> bool NFIObject::AddRecordCallBack(const std::string& strRecordName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const RECORD_EVENT_DATA&, const NFIDataList::TData&, const NFIDataList::TData&))
     *
     * @brief   Adds a record call back.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  BaseType    Type of the base type.
     * @param           strRecordName   Name of the record.
     * @param [in,out]  pBase           If non-null, the base.
     * @param [in,out]  handler         If non-null, the handler.
     *
     * @return  True if it succeeds, false if it fails.
     */

    template<typename BaseType>
    bool AddRecordCallBack(const std::string& strRecordName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const RECORD_EVENT_DATA&, const NFIDataList::TData&, const NFIDataList::TData&))
    {
        RECORD_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        RECORD_EVENT_FUNCTOR_PTR functorPtr(NF_NEW RECORD_EVENT_FUNCTOR(functor));
        return AddRecordCallBack(strRecordName, functorPtr);
    }

    /**
     * @fn  template<typename BaseType> bool NFIObject::AddHeartBeat(const std::string& strHeartBeatName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const float, const int), const float fTime, const int nCount)
     *
     * @brief   Adds a heart beat.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  BaseType    Type of the base type.
     * @param           strHeartBeatName    Name of the heart beat.
     * @param [in,out]  pBase               If non-null, the base.
     * @param [in,out]  handler             If non-null, the handler.
     * @param           fTime               The time.
     * @param           nCount              Number of.
     *
     * @return  True if it succeeds, false if it fails.
     */

    template<typename BaseType>
    bool AddHeartBeat(const std::string& strHeartBeatName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const float, const int), const float fTime, const int nCount)
    {
        HEART_BEAT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        HEART_BEAT_FUNCTOR_PTR functorPtr(NF_NEW HEART_BEAT_FUNCTOR(functor));
        return AddHeartBeat(strHeartBeatName, functorPtr, fTime, nCount);
    }

    /**
     * @fn  template <typename T> bool NFIObject::AddComponent()
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
        return GetComponentManager()->AddComponent<T>();
    }

    /**
     * @fn  template <typename T> NF_SHARE_PTR<T> NFIObject::AddComponent(const std::string& strComponentName)
     *
     * @brief   Adds a component.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  T   Generic type parameter.
     * @param   strComponentName    Name of the component.
     *
     * @return  A NF_SHARE_PTR&lt;T&gt;
     */

    template <typename T>
    NF_SHARE_PTR<T> AddComponent(const std::string& strComponentName)
    {
        NF_SHARE_PTR<NFIComponent> pComponent = GetComponentManager()->FindComponent<NFIComponent>(strComponentName);
        if(pComponent)
        {
            NF_SHARE_PTR<T> pNewCOmponent = pComponent->CreateNewInstance<T>();
            if(nullptr != pNewCOmponent && GetComponentManager()->AddComponent(strComponentName, pNewCOmponent))
            {
                return pNewCOmponent;
            }
        }

        return NF_SHARE_PTR<T>();
    }

    /**
     * @fn  template <typename T> NF_SHARE_PTR<T> NFIObject::FindComponent(const std::string& strComponentName)
     *
     * @brief   Searches for the first component.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @tparam  T   Generic type parameter.
     * @param   strComponentName    Name of the component.
     *
     * @return  The found component.
     */

    template <typename T>
    NF_SHARE_PTR<T> FindComponent(const std::string& strComponentName)
    {
        return GetComponentManager()->FindComponent<T>(strComponentName);
    }

    /**
     * @fn  virtual bool NFIObject::FindHeartBeat(const std::string& strHeartBeatName) = 0;
     *
     * @brief   Searches for the first heart beat.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strHeartBeatName    Name of the heart beat.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool FindHeartBeat(const std::string& strHeartBeatName) = 0;

    /**
     * @fn  virtual bool NFIObject::RemoveHeartBeat(const std::string& strHeartBeatName) = 0;
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
     * @fn  virtual bool NFIObject::FindProperty(const std::string& strPropertyName) = 0;
     *
     * @brief   //////////////////////////////////////////////////////////////.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool FindProperty(const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual bool NFIObject::SetPropertyInt(const std::string& strPropertyName, const NFINT64 value) = 0;
     *
     * @brief   Sets property int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyInt(const std::string& strPropertyName, const NFINT64 value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetPropertyDouble(const std::string& strPropertyName, const double value) = 0;
     *
     * @brief   Sets property double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyDouble(const std::string& strPropertyName, const double value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetPropertyString(const std::string& strPropertyName, const std::string& value) = 0;
     *
     * @brief   Sets property string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyString(const std::string& strPropertyName, const std::string& value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetPropertyObject(const std::string& strPropertyName, const NFGUID& value) = 0;
     *
     * @brief   Sets property object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyObject(const std::string& strPropertyName, const NFGUID& value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetPropertyPoint(const std::string& strPropertyName, const Point3D& value) = 0;
     *
     * @brief   Sets property point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetPropertyPoint(const std::string& strPropertyName, const Point3D& value) = 0;

    /**
     * @fn  virtual NFINT64 NFIObject::GetPropertyInt(const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property int.
     */

    virtual NFINT64 GetPropertyInt(const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual double NFIObject::GetPropertyDouble(const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property double.
     */

    virtual double GetPropertyDouble(const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual const std::string& NFIObject::GetPropertyString(const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property string.
     */

    virtual const std::string& GetPropertyString(const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual const NFGUID& NFIObject::GetPropertyObject(const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property object.
     */

    virtual const NFGUID& GetPropertyObject(const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual const Point3D& NFIObject::GetPropertyPoint(const std::string& strPropertyName) = 0;
     *
     * @brief   Gets property point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     *
     * @return  The property point.
     */

    virtual const Point3D& GetPropertyPoint(const std::string& strPropertyName) = 0;

    /**
     * @fn  virtual bool NFIObject::FindRecord(const std::string& strRecordName) = 0;
     *
     * @brief   ///////////////////////////////////////////////////////////////////////.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool FindRecord(const std::string& strRecordName) = 0;

    /**
     * @fn  virtual bool NFIObject::SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 value) = 0;
     *
     * @brief   Sets record int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double value) = 0;
     *
     * @brief   Sets record double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& value) = 0;
     *
     * @brief   Sets record string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& value) = 0;
     *
     * @brief   Sets record object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol, const Point3D& value) = 0;
     *
     * @brief   Sets record point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol, const Point3D& value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value) = 0;
     *
     * @brief   Sets record int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value) = 0;
     *
     * @brief   Sets record double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value) = 0;
     *
     * @brief   Sets record string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFGUID& value) = 0;
     *
     * @brief   Sets record object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFGUID& value) = 0;

    /**
     * @fn  virtual bool NFIObject::SetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Point3D& value) = 0;
     *
     * @brief   Sets record point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     * @param   value           The value.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool SetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Point3D& value) = 0;

    /**
     * @fn  virtual NFINT64 NFIObject::GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol) = 0;
     *
     * @brief   Gets record int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record int.
     */

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol) = 0;

    /**
     * @fn  virtual double NFIObject::GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol) = 0;
     *
     * @brief   Gets record double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record double.
     */

    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol) = 0;

    /**
     * @fn  virtual const std::string& NFIObject::GetRecordString(const std::string& strRecordName, const int nRow, const int nCol) = 0;
     *
     * @brief   Gets record string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record string.
     */

    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const int nCol) = 0;

    /**
     * @fn  virtual const NFGUID& NFIObject::GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol) = 0;
     *
     * @brief   Gets record object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record object.
     */

    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol) = 0;

    /**
     * @fn  virtual const Point3D& NFIObject::GetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol) = 0;
     *
     * @brief   Gets record point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   nCol            The col.
     *
     * @return  The record point.
     */

    virtual const Point3D& GetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol) = 0;

    /**
     * @fn  virtual NFINT64 NFIObject::GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
     *
     * @brief   Gets record int.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record int.
     */

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

    /**
     * @fn  virtual double NFIObject::GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
     *
     * @brief   Gets record double.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record double.
     */

    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

    /**
     * @fn  virtual const std::string& NFIObject::GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
     *
     * @brief   Gets record string.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record string.
     */

    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

    /**
     * @fn  virtual const NFGUID& NFIObject::GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
     *
     * @brief   Gets record object.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record object.
     */

    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

    /**
     * @fn  virtual const Point3D& NFIObject::GetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
     *
     * @brief   Gets record point.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   nRow            The row.
     * @param   strColTag       The col tag.
     *
     * @return  The record point.
     */

    virtual const Point3D& GetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIRecordManager> NFIObject::GetRecordManager() = 0;
     *
     * @brief   Gets record manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The record manager.
     */

    virtual NF_SHARE_PTR<NFIRecordManager> GetRecordManager() = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIHeartBeatManager> NFIObject::GetHeartBeatManager() = 0;
     *
     * @brief   Gets heart beat manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The heart beat manager.
     */

    virtual NF_SHARE_PTR<NFIHeartBeatManager> GetHeartBeatManager() = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIPropertyManager> NFIObject::GetPropertyManager() = 0;
     *
     * @brief   Gets property manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The property manager.
     */

    virtual NF_SHARE_PTR<NFIPropertyManager> GetPropertyManager() = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIComponentManager> NFIObject::GetComponentManager() = 0;
     *
     * @brief   Gets component manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The component manager.
     */

    virtual NF_SHARE_PTR<NFIComponentManager> GetComponentManager() = 0;

    /**
     * @fn  virtual NF_SHARE_PTR<NFIEventManager> NFIObject::GetEventManager() = 0;
     *
     * @brief   Gets event manager.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @return  The event manager.
     */

    virtual NF_SHARE_PTR<NFIEventManager> GetEventManager() = 0;

protected:

    /**
     * @fn  virtual bool NFIObject::AddRecordCallBack(const std::string& strRecordName, const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;
     *
     * @brief   Adds a record call back to 'cb'.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strRecordName   Name of the record.
     * @param   cb              The cb.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddRecordCallBack(const std::string& strRecordName, const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;

    /**
     * @fn  virtual bool NFIObject::AddPropertyCallBack(const std::string& strPropertyName, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
     *
     * @brief   Adds a property call back to 'cb'.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strPropertyName Name of the property.
     * @param   cb              The cb.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddPropertyCallBack(const std::string& strPropertyName, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;

    /**
     * @fn  virtual bool NFIObject::AddHeartBeat(const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount) = 0;
     *
     * @brief   Adds a heart beat.
     *
     * @author  flyicegood
     * @date    2016/11/22
     *
     * @param   strHeartBeatName    Name of the heart beat.
     * @param   cb                  The cb.
     * @param   fTime               The time.
     * @param   nCount              Number of.
     *
     * @return  True if it succeeds, false if it fails.
     */

    virtual bool AddHeartBeat(const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount) = 0;

protected:
    /** @brief   Manager for plugin. */
    NFIPluginManager* m_pPluginManager;
};

#endif
