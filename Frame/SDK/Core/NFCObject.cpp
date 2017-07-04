// -------------------------------------------------------------------------
//    @FileName         :    NFCObject.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-03-01
//    @Module           :    NFCObject
//
// -------------------------------------------------------------------------

#include "NFCObject.h"
#include "NFCRecordManager.h"
#include "NFCHeartBeatManager.h"
#include "NFCPropertyManager.h"
//#include "NFCComponentManager.h"
#include "NFCEventManager.h"

NFCObject::NFCObject(const AFGUID& self, NFIPluginManager* pLuginManager)
    : NFIObject(self)
{
    mSelf = self;
    m_pPluginManager = pLuginManager;

    m_pRecordManager = NF_SHARE_PTR<NFCRecordManager>(NF_NEW NFCRecordManager(mSelf));
    m_pHeartBeatManager = NF_SHARE_PTR<NFCHeartBeatManager>(NF_NEW NFCHeartBeatManager(mSelf));
    m_pPropertyManager = NF_SHARE_PTR<NFCPropertyManager>(NF_NEW NFCPropertyManager(mSelf));
    //m_pComponentManager = NF_SHARE_PTR<NFCComponentManager>(NF_NEW NFCComponentManager(mSelf));
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
    //GetComponentManager()->Execute();
    GetEventManager()->Execute();

    return true;
}

bool NFCObject::AddHeartBeat(const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const NFINT64 nTime, const int nCount)
{
    return GetHeartBeatManager()->AddHeartBeat(mSelf , strHeartBeatName, cb, nTime, nCount);
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

bool NFCObject::SetPropertyBool(const std::string& strPropertyName, const bool value)
{
    return GetPropertyManager()->SetPropertyBool(strPropertyName, value);
}

bool NFCObject::SetPropertyInt(const std::string& strPropertyName, const int32_t value)
{
    return GetPropertyManager()->SetPropertyInt(strPropertyName, value);
}

bool NFCObject::SetPropertyInt64(const std::string& strPropertyName, const int64_t value)
{
    return GetPropertyManager()->SetPropertyInt64(strPropertyName, value);
}

bool NFCObject::SetPropertyFloat(const std::string& strPropertyName, const float value)
{
    return GetPropertyManager()->SetPropertyFloat(strPropertyName, value);
}

bool NFCObject::SetPropertyDouble(const std::string& strPropertyName, const double value)
{
    return GetPropertyManager()->SetPropertyDouble(strPropertyName, value);
}

bool NFCObject::SetPropertyString(const std::string& strPropertyName, const std::string& value)
{
    return GetPropertyManager()->SetPropertyString(strPropertyName, value);
}

bool NFCObject::SetPropertyObject(const std::string& strPropertyName, const AFGUID& value)
{
    return GetPropertyManager()->SetPropertyObject(strPropertyName, value);
}


//bool NFCObject::SetPropertyPoint(const std::string& strPropertyName, const Point3D& value)
//{
//    return GetPropertyManager()->SetPropertyPoint(strPropertyName, value);
//}

bool NFCObject::GetPropertyBool(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyBool(strPropertyName);
}

int32_t NFCObject::GetPropertyInt(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyInt(strPropertyName);
}

int64_t NFCObject::GetPropertyInt64(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyInt64(strPropertyName);
}

float NFCObject::GetPropertyFloat(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyFloat(strPropertyName);
}

double NFCObject::GetPropertyDouble(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyDouble(strPropertyName);
}

const std::string& NFCObject::GetPropertyString(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyString(strPropertyName);
}

const AFGUID& NFCObject::GetPropertyObject(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyObject(strPropertyName);
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

bool NFCObject::SetRecordBool(const std::string& strRecordName, const int nRow, const int nCol, const bool value)
{
    return GetRecordManager()->SetRecordBool(strRecordName, nRow, nCol, value);
}

bool NFCObject::SetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag, const bool value)
{
    return GetRecordManager()->SetRecordBool(strRecordName, nRow, strColTag, value);
}

bool NFCObject::SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int32_t value)
{
    return GetRecordManager()->SetRecordInt(strRecordName, nRow, nCol, value);
}

bool NFCObject::SetRecordInt(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int32_t value)
{
    return GetRecordManager()->SetRecordInt(strRecordName, nRow, strColTag, value);
}

bool NFCObject::SetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol, const int64_t value)
{
    return GetRecordManager()->SetRecordInt64(strRecordName, nRow, nCol, value);
}

bool NFCObject::SetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64_t value)
{
    return GetRecordManager()->SetRecordInt64(strRecordName, nRow, strColTag, value);
}

bool NFCObject::SetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const float value)
{
    return GetRecordManager()->SetRecordFloat(strRecordName, nRow, nCol, value);
}

bool NFCObject::SetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag, const float value)
{
    return GetRecordManager()->SetRecordFloat(strRecordName, nRow, strColTag, value);
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

bool NFCObject::SetRecordObject(const std::string & strRecordName, const int nRow, const int nCol, const AFGUID & value)
{
    return GetRecordManager()->SetRecordObject(strRecordName, nRow, nCol, value);
}

bool NFCObject::SetRecordObject(const std::string & strRecordName, const int nRow, const std::string & strColTag, const AFGUID & value)
{
    return GetRecordManager()->SetRecordObject(strRecordName, nRow, strColTag, value);
}

bool NFCObject::GetRecordBool(const std::string& strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordBool(strRecordName, nRow, nCol);
}

bool NFCObject::GetRecordBool(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    return GetRecordManager()->GetRecordBool(strRecordName, nRow, strColTag);
}

int32_t NFCObject::GetRecordInt(const std::string & strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordInt(strRecordName, nRow, nCol);
}

int32_t NFCObject::GetRecordInt(const std::string & strRecordName, const int nRow, const std::string & strColTag)
{
    return GetRecordManager()->GetRecordInt(strRecordName, nRow, strColTag);
}

int64_t NFCObject::GetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordInt64(strRecordName, nRow, nCol);
}

int64_t NFCObject::GetRecordInt64(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    return GetRecordManager()->GetRecordInt64(strRecordName, nRow, strColTag);
}

float NFCObject::GetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordFloat(strRecordName, nRow, nCol);
}

float NFCObject::GetRecordFloat(const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    return GetRecordManager()->GetRecordFloat(strRecordName, nRow, strColTag);
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

const AFGUID& NFCObject::GetRecordObject(const std::string & strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordObject(strRecordName, nRow, nCol);
}

const AFGUID& NFCObject::GetRecordObject(const std::string & strRecordName, const int nRow, const std::string & strColTag)
{
    return GetRecordManager()->GetRecordObject(strRecordName, nRow, strColTag);
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

const AFGUID& NFCObject::Self()
{
    return mSelf;
}

//NF_SHARE_PTR<NFIComponentManager> NFCObject::GetComponentManager()
//{
//    return m_pComponentManager;
//}

NF_SHARE_PTR<NFIEventManager> NFCObject::GetEventManager()
{
    return m_pEventManager;
}
