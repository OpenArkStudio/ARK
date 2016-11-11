// -------------------------------------------------------------------------
//    @FileName         :    NFCObject.cpp
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-03-01
//    @Module           :    NFCObject
//
// -------------------------------------------------------------------------

#include "NFCObject.h"
#include "NFCRecordManager.h"
#include "NFCHeartBeatManager.h"
#include "NFCPropertyManager.h"
#include "NFCComponentManager.h"
#include "NFCEventManager.h"

NFCObject::NFCObject(NFGUID self, NFIPluginManager* pLuginManager)
    : NFIObject(self)
{
    mSelf = self;
    m_pPluginManager = pLuginManager;

    m_pRecordManager = NF_SHARE_PTR<NFCRecordManager>(NF_NEW NFCRecordManager(mSelf));
    m_pHeartBeatManager = NF_SHARE_PTR<NFCHeartBeatManager>(NF_NEW NFCHeartBeatManager(mSelf));
    m_pPropertyManager = NF_SHARE_PTR<NFCPropertyManager>(NF_NEW NFCPropertyManager(mSelf));
    m_pComponentManager = NF_SHARE_PTR<NFCComponentManager>(NF_NEW NFCComponentManager(mSelf));
    m_pEventManager = NF_SHARE_PTR<NFIEventManager>(NF_NEW NFCEventManager(mSelf));
}

NFCObject::~NFCObject()
{

}

bool NFCObject::Init()
{
    return true;
}

bool NFCObject::Shut()
{
    return true;
}

bool NFCObject::Execute()
{
    GetHeartBeatManager()->Execute();
    GetComponentManager()->Execute();
    GetEventManager()->Execute();

    return true;
}

bool NFCObject::AddHeartBeat(const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount)
{
    return GetHeartBeatManager()->AddHeartBeat(mSelf , strHeartBeatName, cb, fTime, nCount);
}

bool NFCObject::FindHeartBeat(const std::string& strHeartBeatName)
{
    return GetHeartBeatManager()->Exist(strHeartBeatName);
}

bool NFCObject::RemoveHeartBeat(const std::string& strHeartBeatName)
{
    return GetHeartBeatManager()->RemoveHeartBeat(strHeartBeatName);
}

bool NFCObject::AddRecordCallBack(const std::string& strRecordName, const RECORD_EVENT_FUNCTOR_PTR& cb)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetRecordManager()->GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        pRecord->AddRecordHook(cb);

        return true;
    }

    return false;
}

bool NFCObject::AddPropertyCallBack(const std::string& strCriticalName, const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strCriticalName);
    if(nullptr != pProperty)
    {
        pProperty->RegisterCallback(cb);

        return true;
    }

    return false;
}

bool NFCObject::FindProperty(const std::string& strPropertyName)
{
    if(GetPropertyManager()->GetElement(strPropertyName))
    {
        return true;
    }

    return false;
}

bool NFCObject::SetPropertyInt(const std::string& strPropertyName, const NFINT64 value)
{
    return GetPropertyManager()->SetPropertyInt(strPropertyName, value);
}

bool NFCObject::SetPropertyDouble(const std::string& strPropertyName, const double value)
{
    return GetPropertyManager()->SetPropertyDouble(strPropertyName, value);
}

bool NFCObject::SetPropertyString(const std::string& strPropertyName, const std::string& value)
{
    return GetPropertyManager()->SetPropertyString(strPropertyName, value);
}

bool NFCObject::SetPropertyObject(const std::string& strPropertyName, const NFGUID& value)
{
    return GetPropertyManager()->SetPropertyObject(strPropertyName, value);
}

bool NFCObject::SetPropertyPoint(const std::string& strPropertyName, const Point3D& value)
{
    return GetPropertyManager()->SetPropertyPoint(strPropertyName, value);
}

NFINT64 NFCObject::GetPropertyInt(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyInt(strPropertyName);
}

double NFCObject::GetPropertyDouble(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyDouble(strPropertyName);
}

const std::string& NFCObject::GetPropertyString(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyString(strPropertyName);
}

const NFGUID& NFCObject::GetPropertyObject(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyObject(strPropertyName);
}

const Point3D& NFCObject::GetPropertyPoint(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyPoint(strPropertyName);
}

bool NFCObject::FindRecord(const std::string& strRecordName)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetRecordManager()->GetElement(strRecordName);
    if(nullptr != pRecord)
    {
        return true;
    }

    return false;
}

bool NFCObject::SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 value)
{
    return GetRecordManager()->SetRecordInt(strRecordName, nRow, nCol, value);
}

bool NFCObject::SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value)
{
    return GetRecordManager()->SetRecordInt(strRecordName, nRow, strColTag, value);
}

bool NFCObject::SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double value)
{
    return GetRecordManager()->SetRecordDouble(strRecordName, nRow, nCol, value);
}

bool NFCObject::SetRecordDouble(const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value)
{
    return GetRecordManager()->SetRecordDouble(strRecordName, nRow, strColTag, value);
}

bool NFCObject::SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& value)
{
    return GetRecordManager()->SetRecordString(strRecordName, nRow, nCol, value);
}

bool NFCObject::SetRecordString(const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value)
{
    return GetRecordManager()->SetRecordString(strRecordName, nRow, strColTag, value);
}

bool NFCObject::SetRecordObject(const std::string & strRecordName, const int nRow, const int nCol, const NFGUID & value)
{
    return GetRecordManager()->SetRecordObject(strRecordName, nRow, nCol, value);
}

bool NFCObject::SetRecordObject(const std::string & strRecordName, const int nRow, const std::string & strColTag, const NFGUID & value)
{
    return GetRecordManager()->SetRecordObject(strRecordName, nRow, strColTag, value);
}

bool NFCObject::SetRecordPoint(const std::string & strRecordName, const int nRow, const int nCol, const Point3D & value)
{
    return GetRecordManager()->SetRecordPoint(strRecordName, nRow, nCol, value);
}

bool NFCObject::SetRecordPoint(const std::string & strRecordName, const int nRow, const std::string & strColTag, const Point3D & value)
{
    return GetRecordManager()->SetRecordPoint(strRecordName, nRow, strColTag, value);
}

NFINT64 NFCObject::GetRecordInt(const std::string & strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordInt(strRecordName, nRow, nCol);
}

NFINT64 NFCObject::GetRecordInt(const std::string & strRecordName, const int nRow, const std::string & strColTag)
{
    return GetRecordManager()->GetRecordInt(strRecordName, nRow, strColTag);
}

double NFCObject::GetRecordDouble(const std::string & strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordDouble(strRecordName, nRow, nCol);
}

double NFCObject::GetRecordDouble(const std::string & strRecordName, const int nRow, const std::string & strColTag)
{
    return GetRecordManager()->GetRecordDouble(strRecordName, nRow, strColTag);
}

const std::string& NFCObject::GetRecordString(const std::string & strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordString(strRecordName, nRow, nCol);
}

const std::string& NFCObject::GetRecordString(const std::string & strRecordName, const int nRow, const std::string & strColTag)
{
    return GetRecordManager()->GetRecordString(strRecordName, nRow, strColTag);
}

const NFGUID& NFCObject::GetRecordObject(const std::string & strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordObject(strRecordName, nRow, nCol);
}

const NFGUID& NFCObject::GetRecordObject(const std::string & strRecordName, const int nRow, const std::string & strColTag)
{
    return GetRecordManager()->GetRecordObject(strRecordName, nRow, strColTag);
}

const Point3D& NFCObject::GetRecordPoint(const std::string & strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordPoint(strRecordName, nRow, nCol);
}

const Point3D& NFCObject::GetRecordPoint(const std::string & strRecordName, const int nRow, const std::string & strColTag)
{
    return GetRecordManager()->GetRecordPoint(strRecordName, nRow, strColTag);
}

NF_SHARE_PTR<NFIRecordManager> NFCObject::GetRecordManager()
{
    return m_pRecordManager;
}

NF_SHARE_PTR<NFIHeartBeatManager> NFCObject::GetHeartBeatManager()
{
    return m_pHeartBeatManager;
}

NF_SHARE_PTR<NFIPropertyManager> NFCObject::GetPropertyManager()
{
    return m_pPropertyManager;
}

NFGUID NFCObject::Self()
{
    return mSelf;
}

NF_SHARE_PTR<NFIComponentManager> NFCObject::GetComponentManager()
{
    return m_pComponentManager;
}

NF_SHARE_PTR<NFIEventManager> NFCObject::GetEventManager()
{
    return m_pEventManager;
}
