// -------------------------------------------------------------------------
//    @FileName         :    NFCObject.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-17
//    @Module           :    NFCObject
//
// -------------------------------------------------------------------------

#ifndef NFC_OBJECT_H
#define NFC_OBJECT_H

#include <iostream>
#include <string>
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFCore/NFIRecordManager.h"
#include "NFComm/NFCore/NFIHeartBeatManager.h"
#include "NFComm/NFCore/NFIPropertyManager.h"

class NFCObject
    : public NFIObject
{
private:
    NFCObject() : NFIObject(NULL_GUID)
    {

    }

public:
    NFCObject(NFGUID self, NFIPluginManager* pLuginManager);
    virtual ~NFCObject();

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    ///////////////////////////////////////////////////////////////////////
    virtual NFGUID Self();

    virtual bool FindHeartBeat(const std::string& strHeartBeatName);
    virtual bool RemoveHeartBeat(const std::string& strHeartBeatName);
    /////////////////////////////////////////////////////////////////
    virtual bool FindProperty(const std::string& strPropertyName);

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
    //////////////////////////////////////////////////////////////////////////
    virtual bool FindRecord(const std::string& strRecordName);

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 value);
    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double value);
    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& value);
    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& value);
    virtual bool SetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol, const Point3D& value);

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value);
    virtual bool SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value);
    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value);
    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFGUID& value);
    virtual bool SetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag, const Point3D& value);

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol);
    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol);
    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const int nCol);
    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol);
    virtual const Point3D& GetRecordPoint(const std::string& strRecordName, const int nRow, const int nCol);

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual double GetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual const Point3D& GetRecordPoint(const std::string& strRecordName, const int nRow, const std::string& strColTag);
    //////////////////////////////////////////////////////////////////////////
    virtual NF_SHARE_PTR<NFIRecordManager> GetRecordManager();
    virtual NF_SHARE_PTR<NFIHeartBeatManager> GetHeartBeatManager();
    virtual NF_SHARE_PTR<NFIPropertyManager> GetPropertyManager();
    virtual NF_SHARE_PTR<NFIComponentManager> GetComponentManager();
    virtual NF_SHARE_PTR<NFIEventManager> GetEventManager();

protected:
    virtual bool AddRecordCallBack(const std::string& strRecordName, const RECORD_EVENT_FUNCTOR_PTR& cb);

    virtual bool AddPropertyCallBack(const std::string& strCriticalName, const PROPERTY_EVENT_FUNCTOR_PTR& cb);

    virtual bool AddHeartBeat(const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount);

private:
    NFGUID mSelf;

    NF_SHARE_PTR<NFIRecordManager> m_pRecordManager;
    NF_SHARE_PTR<NFIHeartBeatManager> m_pHeartBeatManager;
    NF_SHARE_PTR<NFIPropertyManager> m_pPropertyManager;
    NF_SHARE_PTR<NFIComponentManager> m_pComponentManager;
    NF_SHARE_PTR<NFIEventManager> m_pEventManager;

};

#endif