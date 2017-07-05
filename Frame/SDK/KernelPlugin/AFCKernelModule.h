// -------------------------------------------------------------------------
//    @FileName         :    AFCKernelModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCKernelModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef AFC_KERNEL_MODULE_H
#define AFC_KERNEL_MODULE_H

#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include "SDK/Core/AFIObject.h"
#include "SDK/Core/AFCDataList.h"
#include "SDK/Core/AFIRecord.h"
#include "SDK/Core/AFGUID.h"
#include "SDK/Interface/AFIGUIDModule.h"
#include "SDK/Interface/AFILogModule.h"
#include "SDK/Interface/AFIKernelModule.h"
#include "SDK/Interface/AFIClassModule.h"
#include "SDK/Interface/AFIElementModule.h"
#include "SDK/Interface/AFISceneModule.h"
#include "SDK/Interface/AFPlatform.h"
#include "SDK/Core/AFMapEx.h"

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

    virtual NF_SHARE_PTR<AFIObject> GetObject(const AFGUID& ident);
    virtual NF_SHARE_PTR<AFIObject> CreateObject(const AFGUID& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const AFIDataList& arg);

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
    virtual const std::string& GetPropertyString(const AFGUID& self, const std::string& strPropertyName);
    virtual const AFGUID& GetPropertyObject(const AFGUID& self, const std::string& strPropertyName);
    //////////////////////////////////////////////////////////////////////////
    virtual NF_SHARE_PTR<AFIRecord> FindRecord(const AFGUID& self, const std::string& strRecordName);
    virtual bool ClearRecord(const AFGUID& self, const std::string& strRecordName);

    virtual bool SetRecordBool(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const bool value);
    virtual bool SetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const int32_t value);
    virtual bool SetRecordInt64(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const int64_t value);
    virtual bool SetRecordFloat(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const float value);
    virtual bool SetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const double value);
    virtual bool SetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const std::string& value);
    virtual bool SetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const AFGUID& value);

    virtual bool SetRecordBool(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const bool value);
    virtual bool SetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const int32_t value);
    virtual bool SetRecordInt64(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const int64_t value);
    virtual bool SetRecordFloat(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const float value);
    virtual bool SetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value);
    virtual bool SetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value);
    virtual bool SetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const AFGUID& value);

    virtual bool GetRecordBool(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual int32_t GetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual int64_t GetRecordInt64(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual float GetRecordFloat(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual double GetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual const std::string& GetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);
    virtual const AFGUID& GetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol);

    virtual bool GetRecordBool(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual int32_t GetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual int64_t GetRecordInt64(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual float GetRecordFloat(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual double GetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual const std::string& GetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag);
    virtual const AFGUID& GetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag);
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

    //只能网络[脚本]模块注册，回调用来同步对象类事件,所有的类对象都会回调
    virtual bool RegisterCommonClassEvent(const CLASS_EVENT_FUNCTOR_PTR& cb);

    //只能网络[脚本]模块注册，回调用来同步对象属性事件,所有的类属性都会回调
    virtual bool RegisterCommonPropertyEvent(const PROPERTY_EVENT_FUNCTOR_PTR& cb);

    //只能网络[脚本]模块注册，回调用来同步对象类表事件,所有的类表都会回调
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
    //通用对象类事件回调,以便同步
    std::list<CLASS_EVENT_FUNCTOR_PTR> mtCommonClassCallBackList;
    //通用属性变动回调,以便同步
    std::list<PROPERTY_EVENT_FUNCTOR_PTR> mtCommonPropertyCallBackList;
    //通用表变动回调,以便同步
    std::list<RECORD_EVENT_FUNCTOR_PTR> mtCommonRecordCallBackList;

private:
    //属性的KEY，比如HP=1，会以这个建立KEY，那么可以快速查询所有HP=1的对象而不用遍历
    //     std::map<std::string,std::map<TData, NFList<AFGUID>>>
    //     std::map<"Scene", std::map<10, NFList<AFGUID>>>

private:
    std::vector<float> mvRandom;
    int mnRandomPos;

    AFGUID mnCurExeObject;
    AFINT64 nLastTime;

    AFISceneModule* m_pSceneModule;
    AFILogModule* m_pLogModule;
    AFIClassModule* m_pClassModule;
    AFIElementModule* m_pElementModule;
    AFIGUIDModule* m_pGUIDModule;
};

#endif
