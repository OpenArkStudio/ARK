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
#pragma once

#include "AFIModule.h"
#include "SDK/Core/AFIObject.h"
#include "SDK/Base/AFGUID.h"

class AFIKernelModule
    : public AFIModule
{
public:

    template<typename BaseType>
    bool AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const float, const int), const float fTime, const int nCount)
    {
        ARK_SHARE_PTR<AFIObject> pObject = GetObject(self);
        if(pObject.get())
        {
            return pObject->AddHeartBeat(strHeartBeatName, pBase, handler, fTime, nCount);
        }

        return false;
    }

    virtual bool FindHeartBeat(const AFGUID& self, const std::string& strHeartBeatName) = 0;

    virtual bool RemoveHeartBeat(const AFGUID& self, const std::string& strHeartBeatName) = 0;

    template<typename BaseType>
    bool AddRecordCallBack(const AFGUID& self, const std::string& strRecordName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const RECORD_EVENT_DATA&, const AFIData&, const AFIData&))
    {
        ARK_SHARE_PTR<AFIObject> pObject = GetObject(self);
        if(pObject.get())
        {
            return pObject->AddRecordCallBack(strRecordName, pBase, handler);
        }

        return false;
    }

    template<typename BaseType>
    bool AddPropertyCallBack(const AFGUID& self, const std::string& strPropertyName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
    {
        ARK_SHARE_PTR<AFIObject> pObject = GetObject(self);
        if(pObject.get())
        {
            return pObject->AddPropertyCallBack(strPropertyName, pBase, handler);
        }

        return false;
    }

    ////////////////event//////////////////////////////////////////////////////////
    template<typename BaseType>
    bool AddEventCallBack(const AFGUID& self, const int nEventID, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const int, const AFIDataList&))
    {
        EVENT_PROCESS_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        EVENT_PROCESS_FUNCTOR_PTR functorPtr(new EVENT_PROCESS_FUNCTOR(functor));
        return AddEventCallBack(self, nEventID, functorPtr);
    }

    template<typename BaseType>
    bool AddClassCallBack(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const CLASS_OBJECT_EVENT, const AFIDataList&))
    {
        CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        CLASS_EVENT_FUNCTOR_PTR functorPtr(new CLASS_EVENT_FUNCTOR(functor));
        return AddClassCallBack(strClassName, functorPtr);
    }

    virtual bool DoEvent(const AFGUID& self, const std::string& strClassName, CLASS_OBJECT_EVENT eEvent, const AFIDataList& valueList) = 0;
    virtual bool DoEvent(const AFGUID& self, const int nEventID, const AFIDataList& valueList) = 0;

    //////////////////////////////////////////////////////////////////////////
    //只能网络模块注册，回调用来同步对象类事件,所有的类对象都会回调
    template<typename BaseType>
    bool RegisterCommonClassEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const CLASS_OBJECT_EVENT, const AFIDataList&))
    {
        CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        CLASS_EVENT_FUNCTOR_PTR functorPtr(new CLASS_EVENT_FUNCTOR(functor));
        return RegisterCommonClassEvent(functorPtr);
    }

    //只能网络模块注册，回调用来同步对象属性事件,所有的类属性都会回调
    template<typename BaseType>
    bool RegisterCommonPropertyEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFIData&, const AFIData&))
    {
        PROPERTY_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        PROPERTY_EVENT_FUNCTOR_PTR functorPtr(new PROPERTY_EVENT_FUNCTOR(functor));
        return RegisterCommonPropertyEvent(functorPtr);
    }

    //只能网络模块注册，回调用来同步对象类表事件,所有的类表都会回调
    template<typename BaseType>
    bool RegisterCommonRecordEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const RECORD_EVENT_DATA&, const AFIData&, const AFIData&))
    {
        RECORD_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        RECORD_EVENT_FUNCTOR_PTR functorPtr(new RECORD_EVENT_FUNCTOR(functor));
        return RegisterCommonRecordEvent(functorPtr);
    }

    /////////////////////////////////////////////////////////////////

    virtual bool IsContainer(const AFGUID& self) = 0;
    virtual bool ExistContainer(const int nContainerIndex) = 0;

    virtual ARK_SHARE_PTR<AFIObject> GetObject(const AFGUID& ident) = 0;
    virtual ARK_SHARE_PTR<AFIObject> CreateObject(const AFGUID& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const AFIDataList& arg) = 0;

    virtual bool DestroyObject(const AFGUID& self) = 0;
    virtual bool DestroyAll() = 0;
    //////////////////////////////////////////////////////////////////////////
    virtual bool FindProperty(const AFGUID& self, const std::string& strPropertyName) = 0;

    virtual bool SetPropertyBool(const AFGUID& self, const std::string& strPropertyName, const bool value) = 0;
    virtual bool SetPropertyInt(const AFGUID& self, const std::string& strPropertyName, const int32_t value) = 0;
    virtual bool SetPropertyInt64(const AFGUID& self, const std::string& strPropertyName, const int64_t value) = 0;
    virtual bool SetPropertyFloat(const AFGUID& self, const std::string& strPropertyName, const float value) = 0;
    virtual bool SetPropertyDouble(const AFGUID& self, const std::string& strPropertyName, const double value) = 0;
    virtual bool SetPropertyString(const AFGUID& self, const std::string& strPropertyName, const std::string& value) = 0;
    virtual bool SetPropertyObject(const AFGUID& self, const std::string& strPropertyName, const AFGUID& value) = 0;

    virtual bool GetPropertyBool(const AFGUID& self, const std::string& strPropertyName) = 0;
    virtual int32_t GetPropertyInt(const AFGUID& self, const std::string& strPropertyName) = 0;
    virtual int64_t GetPropertyInt64(const AFGUID& self, const std::string& strPropertyName) = 0;
    virtual float GetPropertyFloat(const AFGUID& self, const std::string& strPropertyName) = 0;
    virtual double GetPropertyDouble(const AFGUID& self, const std::string& strPropertyName) = 0;
    virtual const std::string& GetPropertyString(const AFGUID& self, const std::string& strPropertyName) = 0;
    virtual const AFGUID& GetPropertyObject(const AFGUID& self, const std::string& strPropertyName) = 0;
    //////////////////////////////////////////////////////////////////////////
    virtual ARK_SHARE_PTR<AFIRecord> FindRecord(const AFGUID& self, const std::string& strRecordName) = 0;
    virtual bool ClearRecord(const AFGUID& self, const std::string& strRecordName) = 0;

    virtual bool SetRecordBool(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const bool value) = 0;
    virtual bool SetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const int32_t value) = 0;
    virtual bool SetRecordInt64(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const int64_t value) = 0;
    virtual bool SetRecordFloat(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const float value) = 0;
    virtual bool SetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const double value) = 0;
    virtual bool SetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const std::string& value) = 0;
    virtual bool SetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const AFGUID& value) = 0;

    virtual bool SetRecordBool(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const bool value) = 0;
    virtual bool SetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const int32_t value) = 0;
    virtual bool SetRecordInt64(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64_t value) = 0;
    virtual bool SetRecordFloat(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const float value) = 0;
    virtual bool SetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value) = 0;
    virtual bool SetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value) = 0;
    virtual bool SetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const AFGUID& value) = 0;

    virtual bool GetRecordBool(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual int32_t GetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual int64_t GetRecordInt64(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual float GetRecordFloat(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual double GetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual const std::string& GetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual const AFGUID& GetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;

    virtual bool GetRecordBool(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual int32_t GetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual int64_t GetRecordInt64(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual float GetRecordFloat(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual double GetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual const std::string& GetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual const AFGUID& GetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    //////////////////////////////////////////////////////////////////////////
    virtual bool SwitchScene(const AFGUID& self, const int nTargetSceneID, const int nTargetGroupID, const float fX, const float fY, const float fZ, const float fOrient, const AFIDataList& arg) = 0;

    virtual bool CreateScene(const int nSceneID) = 0;
    virtual bool DestroyScene(const int nSceneID) = 0;

    virtual int GetOnLineCount() = 0;
    virtual int GetMaxOnLineCount() = 0;
    virtual int GetSceneOnLineCount(const int nSceneID) = 0;
    virtual int GetSceneOnLineCount(const int nSceneID, const int nGroupID) = 0;
    virtual int GetSceneOnLineList(const int nSceneID, AFIDataList& var) = 0;

    virtual int RequestGroupScene(const int nSceneID) = 0;
    virtual bool ReleaseGroupScene(const int nSceneID, const int nGroupID) = 0;
    virtual bool ExitGroupScene(const int nSceneID, const int nGroupID) = 0;

    virtual bool GetGroupObjectList(const int nSceneID, const int nGroupID, AFIDataList& list) = 0;
    virtual int GetObjectByProperty(const int nSceneID, const std::string& strPropertyName, const AFIDataList& valueArg, AFIDataList& list) = 0;

    virtual void Random(int nStart, int nEnd, int nCount, AFIDataList& valueList) = 0;
    virtual bool LogInfo(const AFGUID& ident) = 0;

protected:
    virtual bool AddEventCallBack(const AFGUID& self, const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb) = 0;
    virtual bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;

protected:
    //只能网络模块注册，回调用来同步对象类事件,所有的类对象都会回调
    virtual bool RegisterCommonClassEvent(const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;

    //只能网络模块注册，回调用来同步对象属性事件,所有的类属性都会回调
    virtual bool RegisterCommonPropertyEvent(const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;

    //只能网络模块注册，回调用来同步对象类表事件,所有的类表都会回调
    virtual bool RegisterCommonRecordEvent(const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;
};