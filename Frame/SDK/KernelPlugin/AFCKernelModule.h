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

#include "SDK/Core/AFIObject.h"
#include "SDK/Base/AFCDataList.h"
#include "SDK/Core/AFRecord.h"
#include "SDK/Base/AFGUID.h"
#include "SDK/Interface/AFIGUIDModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFISceneModule.h"
#include "SDK/Base/AFPlatform.hpp"
#include "SDK/Base/AFMapEx.h"

class AFCKernelModule
    : public AFIKernelModule,
      public AFMapEx<AFGUID, AFIObject>
{
public:
    AFCKernelModule(AFIPluginManager* p);
    virtual ~AFCKernelModule();

    virtual bool Init();
    virtual bool Shut();

    virtual bool BeforeShut();
    virtual bool AfterInit();

    virtual bool Execute();

    ///////////////////////////////////////////////////////////////////////

    virtual bool FindHeartBeat(const AFGUID& self, const std::string& strHeartBeatName);
    virtual bool RemoveHeartBeat(const AFGUID& self, const std::string& strHeartBeatName);

    virtual bool IsContainer(const AFGUID& self);
    virtual bool ExistContainer(const int nSceneID);

    virtual ARK_SHARE_PTR<AFIObject> GetObject(const AFGUID& ident);
    virtual ARK_SHARE_PTR<AFIObject> CreateObject(const AFGUID& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const AFIDataList& arg);

    virtual bool DestroyAll();
    virtual bool DestroySelf(const AFGUID& self);
    virtual bool DestroyObject(const AFGUID& self);

    //////////////////////////////////////////////////////////////////////////
    virtual bool FindProperty(const AFGUID& self, const std::string& strPropertyName);

    virtual bool SetPropertyBool(const AFGUID& self, const std::string& strPropertyName, const bool value);
    virtual bool SetPropertyInt(const AFGUID& self, const std::string& strPropertyName, const int32_t value);
    virtual bool SetPropertyInt64(const AFGUID& self, const std::string& strPropertyName, const int64_t value);
    virtual bool SetPropertyFloat(const AFGUID& self, const std::string& strPropertyName, const float value);
    virtual bool SetPropertyDouble(const AFGUID& self, const std::string& strPropertyName, const double value);
    virtual bool SetPropertyString(const AFGUID& self, const std::string& strPropertyName, const std::string& value);
    virtual bool SetPropertyObject(const AFGUID& self, const std::string& strPropertyName, const AFGUID& value);

    virtual bool GetPropertyBool(const AFGUID& self, const std::string& strPropertyName);
    virtual int32_t GetPropertyInt(const AFGUID& self, const std::string& strPropertyName);
    virtual int64_t GetPropertyInt64(const AFGUID& self, const std::string& strPropertyName);
    virtual float GetPropertyFloat(const AFGUID& self, const std::string& strPropertyName);
    virtual double GetPropertyDouble(const AFGUID& self, const std::string& strPropertyName);
    virtual const char*  GetPropertyString(const AFGUID& self, const std::string& strPropertyName);
    virtual const AFGUID& GetPropertyObject(const AFGUID& self, const std::string& strPropertyName);
    //////////////////////////////////////////////////////////////////////////
    virtual AFRecord* FindRecord(const AFGUID& self, const std::string& strRecordName);
    virtual bool ClearRecord(const AFGUID& self, const std::string& strRecordName);

    virtual bool SetRecordBool(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const bool value);
    virtual bool SetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const int32_t value);
    virtual bool SetRecordInt64(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const int64_t value);
    virtual bool SetRecordFloat(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const float value);
    virtual bool SetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const double value);
    virtual bool SetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const std::string& value);
    virtual bool SetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const AFGUID& value);

    virtual bool GetRecordBool(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual int32_t GetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual int64_t GetRecordInt64(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual float GetRecordFloat(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual double GetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual const char* GetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual const AFGUID& GetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);

    //////////////////////////////////////////////////////////////////////////
    virtual bool SwitchScene(const AFGUID& self, const int nTargetSceneID, const int nTargetGroupID, const float fX, const float fY, const float fZ, const float fOrient, const AFIDataList& arg);

    virtual bool CreateScene(const int nSceneID);
    virtual bool DestroyScene(const int nSceneID);

    virtual int GetOnLineCount();
    virtual int GetMaxOnLineCount();
    virtual int GetSceneOnLineCount(const int nSceneID);
    virtual int GetSceneOnLineCount(const int nSceneID, const int nGroupID);

    virtual int GetSceneOnLineList(const int nSceneID, AFIDataList& var);

    virtual int RequestGroupScene(const int nSceneID);
    virtual bool ReleaseGroupScene(const int nSceneID, const int nGroupID);
    virtual bool ExitGroupScene(const int nSceneID, const int nGroupID);

    virtual bool GetGroupObjectList(const int nSceneID, const int nGroupID, AFIDataList& list);
    virtual int GetObjectByProperty(const int nSceneID, const std::string& strPropertyName, const AFIDataList& valueArgArg, AFIDataList& list);

    virtual void Random(int nStart, int nEnd, int nCount, AFIDataList& valueList);

    //////////////////////////////////////////////////////////////////////////
    virtual bool LogStack();
    virtual bool LogInfo(const AFGUID& ident);
    virtual bool LogSelfInfo(const AFGUID& ident);

    //////////////////////////////////////////////////////////////////////////

    virtual bool DoEvent(const AFGUID& self, const std::string& strClassName, CLASS_OBJECT_EVENT eEvent, const AFIDataList& valueList);
    virtual bool DoEvent(const AFGUID& self, const int nEventID, const AFIDataList& valueList);

protected:

    //注册通用类事件
    virtual bool RegisterCommonClassEvent(const CLASS_EVENT_FUNCTOR_PTR& cb);

    //注册通用属性事件
    virtual bool RegisterCommonPropertyEvent(const PROPERTY_EVENT_FUNCTOR_PTR& cb);

    //注册通用类事件
    virtual bool RegisterCommonRecordEvent(const RECORD_EVENT_FUNCTOR_PTR& cb);

protected:

    virtual bool AddEventCallBack(const AFGUID& self, const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb);
    virtual bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb);

    void InitRandom();

    int OnClassCommonEvent(const AFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& var);
    int OnPropertyCommonEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar);
    int OnRecordCommonEvent(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar);

    void ProcessMemFree();

protected:

    std::list<AFGUID> mtDeleteSelfList;

    //////////////////////////////////////////////////////////////////////////
    //閫氱敤瀵硅薄绫讳簨浠跺洖璋?浠ヤ究鍚屾
    std::list<CLASS_EVENT_FUNCTOR_PTR> mtCommonClassCallBackList;
    //閫氱敤灞炴�у彉鍔ㄥ洖璋?浠ヤ究鍚屾
    std::list<PROPERTY_EVENT_FUNCTOR_PTR> mtCommonPropertyCallBackList;
    //閫氱敤琛ㄥ彉鍔ㄥ洖璋?浠ヤ究鍚屾
    std::list<RECORD_EVENT_FUNCTOR_PTR> mtCommonRecordCallBackList;

private:
    std::vector<float> mvRandom;
    int mnRandomPos;

    AFGUID mnCurExeObject;
    int64_t nLastTime;

    AFISceneModule* m_pSceneModule;
    AFILogModule* m_pLogModule;
    AFIClassModule* m_pClassModule;
    AFIElementModule* m_pElementModule;
    AFIGUIDModule* m_pGUIDModule;
};


