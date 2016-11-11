// -------------------------------------------------------------------------
//    @FileName         :    NFCPropertyManager.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFCPropertyManager
//
// -------------------------------------------------------------------------

#ifndef NFC_PROPERTYMANAGER_H
#define NFC_PROPERTYMANAGER_H

#include "NFDefine.h"
#include "NFMap.h"
#include "NFIDataList.h"
#include "NFIPropertyManager.h"

class NFCPropertyManager : public NFIPropertyManager
{
public:
    NFCPropertyManager(const NFGUID& self)
    {
        mSelf = self;
    };

    virtual ~NFCPropertyManager();

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb);

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty);

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, const std::string& strPropertyName, const TDATA_TYPE varType);

    virtual const NFGUID& Self();

    virtual bool SetProperty(const std::string& strPropertyName, const NFIDataList::TData& TData);

    virtual bool SetPropertyInt(const std::string& strPropertyName, const NFINT64 value);
    virtual bool SetPropertyDouble(const std::string& strPropertyName, const double value);
    virtual bool SetPropertyString(const std::string& strPropertyName, const std::string& value);
    virtual bool SetPropertyObject(const std::string& strPropertyName, const NFGUID& value);
    virtual bool SetPropertyPoint(const std::string& strPropertyName, const Point3D& value);

    virtual NFINT64 GetPropertyInt(const std::string& strPropertyName);
    virtual double GetPropertyDouble(const std::string& strPropertyName);
    virtual const std::string& GetPropertyString(const std::string& strPropertyName);
    virtual const NFGUID& GetPropertyObject(const std::string& strPropertyName);
    virtual const Point3D& GetPropertyPoint(const std::string& strPropertyName);

private:
    NFGUID mSelf;
    std::map<std::string, int> mxPropertyIndexMap;
};


#endif
