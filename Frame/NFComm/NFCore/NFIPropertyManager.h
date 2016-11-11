// -------------------------------------------------------------------------
//    @FileName         :    NFIPropertyManager.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFIPropertyManager
//
// -------------------------------------------------------------------------

#ifndef NFI_PROPERTYMANAGER_H
#define NFI_PROPERTYMANAGER_H

#include "NFMap.h"
#include "NFIProperty.h"
#include <map>

class NFIPropertyManager
    : public NFMapEx<std::string, NFIProperty>
{
public:
    virtual ~NFIPropertyManager() {}

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty) = 0;

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, const std::string& strPropertyName, const TDATA_TYPE varType) = 0;

    virtual const NFGUID& Self() = 0;

    //////////////////////////////////////////////////////////////////////////
    virtual bool SetProperty(const std::string& strPropertyName, const NFIDataList::TData& TData) = 0;

    virtual bool SetPropertyInt(const std::string& strPropertyName, const NFINT64 value) = 0;
    virtual bool SetPropertyDouble(const std::string& strPropertyName, const double value) = 0;
    virtual bool SetPropertyString(const std::string& strPropertyName, const std::string& value) = 0;
    virtual bool SetPropertyObject(const std::string& strPropertyName, const NFGUID& value) = 0;
    virtual bool SetPropertyPoint(const std::string& strPropertyName, const Point3D& value) = 0;

    virtual NFINT64 GetPropertyInt(const std::string& strPropertyName) = 0;
    virtual double GetPropertyDouble(const std::string& strPropertyName) = 0;
    virtual const std::string& GetPropertyString(const std::string& strPropertyName) = 0;
    virtual const NFGUID& GetPropertyObject(const std::string& strPropertyName) = 0;
    virtual const Point3D& GetPropertyPoint(const std::string& strPropertyName) = 0;

};

#endif
