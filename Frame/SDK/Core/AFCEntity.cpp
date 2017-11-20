/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include "AFCEntity.h"
#include "AFCRecordMgr.h"
#include "AFCHeartBeatManager.h"
#include "AFCPropertyMgr.h"
#include "AFCEventManager.h"

AFCEntity::AFCEntity(const AFGUID& self, AFIPluginManager* pLuginManager)
    : AFIEntity(self)
{
    mSelf = self;
    m_pPluginManager = pLuginManager;

    m_pPropertyManager = ARK_SHARE_PTR<AFCPropertyMgr>(ARK_NEW AFCPropertyMgr(mSelf));
    m_pRecordManager = ARK_SHARE_PTR<AFCRecordMgr>(ARK_NEW AFCRecordMgr(mSelf));
    m_pHeartBeatManager = ARK_SHARE_PTR<AFCHeartBeatManager>(ARK_NEW AFCHeartBeatManager(mSelf));
    m_pEventManager = ARK_SHARE_PTR<AFIEventManager>(ARK_NEW AFCEventManager(mSelf));
}

AFCEntity::~AFCEntity()
{

}

bool AFCEntity::Init()
{
    return true;
}

bool AFCEntity::Shut()
{
    return true;
}

bool AFCEntity::Execute()
{
    GetHeartBeatManager()->Execute();
    GetEventManager()->Execute();

    return true;
}

bool AFCEntity::AddHeartBeat(const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const int64_t nTime, const int nCount, const bool bForever)
{
    return GetHeartBeatManager()->AddHeartBeat(mSelf, strHeartBeatName, cb, nTime, nCount,  bForever);
}

bool AFCEntity::FindHeartBeat(const std::string& strHeartBeatName)
{
    return GetHeartBeatManager()->Exist(strHeartBeatName);
}

bool AFCEntity::RemoveHeartBeat(const std::string& strHeartBeatName)
{
    return GetHeartBeatManager()->RemoveHeartBeat(strHeartBeatName);
}

bool AFCEntity::AddRecordCallBack(const std::string& strRecordName, const RECORD_EVENT_FUNCTOR_PTR& cb)
{
    ARK_SHARE_PTR<AFIRecordMgr> pRecordMgr = GetRecordManager();
    if (nullptr == pRecordMgr)
    {
        return false;
    }

    return pRecordMgr->AddRecordCallback(strRecordName.c_str(), cb);
}

bool AFCEntity::AddPropertyCallBack(const std::string& strCriticalName, const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
    return GetPropertyManager()->RegisterCallback(strCriticalName, cb);
}

bool AFCEntity::FindProperty(const std::string& strPropertyName)
{
    return (NULL != GetPropertyManager()->GetProperty(strPropertyName.c_str()));
}

bool AFCEntity::SetPropertyBool(const std::string& strPropertyName, const bool value)
{
    return GetPropertyManager()->SetPropertyBool(strPropertyName.c_str(), value);
}

bool AFCEntity::SetPropertyInt(const std::string& strPropertyName, const int32_t value)
{
    return GetPropertyManager()->SetPropertyInt(strPropertyName.c_str(), value);
}

bool AFCEntity::SetPropertyInt64(const std::string& strPropertyName, const int64_t value)
{
    return GetPropertyManager()->SetPropertyInt64(strPropertyName.c_str(), value);
}

bool AFCEntity::SetPropertyFloat(const std::string& strPropertyName, const float value)
{
    return GetPropertyManager()->SetPropertyFloat(strPropertyName.c_str(), value);
}

bool AFCEntity::SetPropertyDouble(const std::string& strPropertyName, const double value)
{
    return GetPropertyManager()->SetPropertyDouble(strPropertyName.c_str(), value);
}

bool AFCEntity::SetPropertyString(const std::string& strPropertyName, const std::string& value)
{
    return GetPropertyManager()->SetPropertyString(strPropertyName.c_str(), value);
}

bool AFCEntity::SetPropertyObject(const std::string& strPropertyName, const AFGUID& value)
{
    return GetPropertyManager()->SetPropertyObject(strPropertyName.c_str(), value);
}

bool AFCEntity::GetPropertyBool(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyBool(strPropertyName.c_str());
}

int32_t AFCEntity::GetPropertyInt(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyInt(strPropertyName.c_str());
}

int64_t AFCEntity::GetPropertyInt64(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyInt64(strPropertyName.c_str());
}

float AFCEntity::GetPropertyFloat(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyFloat(strPropertyName.c_str());
}

double AFCEntity::GetPropertyDouble(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyDouble(strPropertyName.c_str());
}

const char* AFCEntity::GetPropertyString(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyString(strPropertyName.c_str());
}

const AFGUID& AFCEntity::GetPropertyObject(const std::string& strPropertyName)
{
    return GetPropertyManager()->GetPropertyObject(strPropertyName.c_str());
}

bool AFCEntity::FindRecord(const std::string& strRecordName)
{
    AFRecord* pRecord = GetRecordManager()->GetRecord(strRecordName.c_str());
    return (nullptr != pRecord);
}

bool AFCEntity::SetRecordBool(const std::string& strRecordName, const int nRow, const int nCol, const bool value)
{
    return GetRecordManager()->SetRecordBool(strRecordName.c_str(), nRow, nCol, value);
}

bool AFCEntity::SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const int32_t value)
{
    return GetRecordManager()->SetRecordInt(strRecordName.c_str(), nRow, nCol, value);
}

bool AFCEntity::SetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol, const int64_t value)
{
    return GetRecordManager()->SetRecordInt64(strRecordName.c_str(), nRow, nCol, value);
}

bool AFCEntity::SetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const float value)
{
    return GetRecordManager()->SetRecordFloat(strRecordName.c_str(), nRow, nCol, value);
}

bool AFCEntity::SetRecordDouble(const std::string& strRecordName, const int nRow, const int nCol, const double value)
{
    return GetRecordManager()->SetRecordDouble(strRecordName.c_str(), nRow, nCol, value);
}

bool AFCEntity::SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& value)
{
    return GetRecordManager()->SetRecordString(strRecordName.c_str(), nRow, nCol, value.c_str());
}

bool AFCEntity::SetRecordObject(const std::string & strRecordName, const int nRow, const int nCol, const AFGUID & value)
{
    return GetRecordManager()->SetRecordObject(strRecordName.c_str(), nRow, nCol, value);
}

bool AFCEntity::GetRecordBool(const std::string& strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordBool(strRecordName.c_str(), nRow, nCol);
}

int32_t AFCEntity::GetRecordInt(const std::string & strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordInt(strRecordName.c_str(), nRow, nCol);
}

int64_t AFCEntity::GetRecordInt64(const std::string& strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordInt64(strRecordName.c_str(), nRow, nCol);
}

float AFCEntity::GetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordFloat(strRecordName.c_str(), nRow, nCol);
}

double AFCEntity::GetRecordDouble(const std::string & strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordDouble(strRecordName.c_str(), nRow, nCol);
}

const char* AFCEntity::GetRecordString(const std::string & strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordString(strRecordName.c_str(), nRow, nCol);
}

const AFGUID& AFCEntity::GetRecordObject(const std::string & strRecordName, const int nRow, const int nCol)
{
    return GetRecordManager()->GetRecordObject(strRecordName.c_str(), nRow, nCol);
}

ARK_SHARE_PTR<AFIRecordMgr> AFCEntity::GetRecordManager()
{
    return m_pRecordManager;
}

ARK_SHARE_PTR<AFIHeartBeatManager> AFCEntity::GetHeartBeatManager()
{
    return m_pHeartBeatManager;
}

ARK_SHARE_PTR<AFIPropertyMgr> AFCEntity::GetPropertyManager()
{
    return m_pPropertyManager;
}

const AFGUID& AFCEntity::Self()
{
    return mSelf;
}

ARK_SHARE_PTR<AFIEventManager> AFCEntity::GetEventManager()
{
    return m_pEventManager;
}

