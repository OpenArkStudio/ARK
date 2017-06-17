// -------------------------------------------------------------------------
//    @FileName         :    NFIKernelModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    NFIKernelModule
//
// -------------------------------------------------------------------------

#ifndef NFI_KERNEL_MODULE_H
#define NFI_KERNEL_MODULE_H

#include <iostream>
#include <string>
#include <functional>
#include "NFIModule.h"
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/AFGUID.h"

class NFIKernelModule
    : public NFIModule
{
public:

    template<typename BaseType>
    bool AddHeartBeat(const AFGUID self, const std::string& strHeartBeatName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const float, const int), const float fTime, const int nCount)
    {
        NF_SHARE_PTR<NFIObject> pObject = GetObject(self);
        if(pObject.get())
        {
            return pObject->AddHeartBeat(strHeartBeatName, pBase, handler, fTime, nCount);
        }

        return false;
    }

    virtual bool FindHeartBeat(const AFGUID& self, const std::string& strHeartBeatName) = 0;

    virtual bool RemoveHeartBeat(const AFGUID& self, const std::string& strHeartBeatName) = 0;

    template<typename BaseType>
    bool AddRecordCallBack(const AFGUID& self, const std::string& strRecordName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const RECORD_EVENT_DATA&, const AFDataList::TData&, const AFDataList::TData&))
    {
        NF_SHARE_PTR<NFIObject> pObject = GetObject(self);
        if(pObject.get())
        {
            return pObject->AddRecordCallBack(strRecordName, pBase, handler);
        }

        return false;
    }

    template<typename BaseType>
    bool AddPropertyCallBack(const AFGUID& self, const std::string& strPropertyName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFDataList::TData&, const AFDataList::TData&))
    {
        NF_SHARE_PTR<NFIObject> pObject = GetObject(self);
        if(pObject.get())
        {
            return pObject->AddPropertyCallBack(strPropertyName, pBase, handler);
        }

        return false;
    }

    ////////////////event//////////////////////////////////////////////////////////
    template<typename BaseType>
    bool AddEventCallBack(const AFGUID& self, const int nEventID, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const int, const AFDataList&))
    {
        EVENT_PROCESS_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        EVENT_PROCESS_FUNCTOR_PTR functorPtr(new EVENT_PROCESS_FUNCTOR(functor));
        return AddEventCallBack(self, nEventID, functorPtr);
    }

    template<typename BaseType>
    bool AddClassCallBack(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const CLASS_OBJECT_EVENT, const AFDataList&))
    {
        CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        CLASS_EVENT_FUNCTOR_PTR functorPtr(new CLASS_EVENT_FUNCTOR(functor));
        return AddClassCallBack(strClassName, functorPtr);
    }

    virtual bool DoEvent(const AFGUID& self, const std::string& strClassName, CLASS_OBJECT_EVENT eEvent, const AFDataList& valueList) = 0;
    virtual bool DoEvent(const AFGUID& self, const int nEventID, const AFDataList& valueList) = 0;

    //////////////////////////////////////////////////////////////////////////
    //只能网络模块注册，回调用来同步对象类事件,所有的类对象都会回调
    template<typename BaseType>
    bool RegisterCommonClassEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const CLASS_OBJECT_EVENT, const AFDataList&))
    {
        CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        CLASS_EVENT_FUNCTOR_PTR functorPtr(new CLASS_EVENT_FUNCTOR(functor));
        return RegisterCommonClassEvent(functorPtr);
    }

    //只能网络模块注册，回调用来同步对象属性事件,所有的类属性都会回调
    template<typename BaseType>
    bool RegisterCommonPropertyEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const std::string&, const AFDataList::TData&, const AFDataList::TData&))
    {
        PROPERTY_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        PROPERTY_EVENT_FUNCTOR_PTR functorPtr(new PROPERTY_EVENT_FUNCTOR(functor));
        return RegisterCommonPropertyEvent(functorPtr);
    }

    //只能网络模块注册，回调用来同步对象类表事件,所有的类表都会回调
    template<typename BaseType>
    bool RegisterCommonRecordEvent(BaseType* pBase, int (BaseType::*handler)(const AFGUID&, const RECORD_EVENT_DATA&, const AFDataList::TData&, const AFDataList::TData&))
    {
        RECORD_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        RECORD_EVENT_FUNCTOR_PTR functorPtr(new RECORD_EVENT_FUNCTOR(functor));
        return RegisterCommonRecordEvent(functorPtr);
    }


    /////////////////////////////////////////////////////////////////


    virtual bool IsContainer(const AFGUID& self) = 0;
    virtual bool ExistContainer(const int nContainerIndex) = 0;

    virtual NF_SHARE_PTR<NFIObject> GetObject(const AFGUID& ident) = 0;
    virtual NF_SHARE_PTR<NFIObject> CreateObject(const AFGUID& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const AFDataList& arg) = 0;

    virtual bool DestroyObject(const AFGUID& self) = 0;
    virtual bool DestroyAll() = 0;
    //////////////////////////////////////////////////////////////////////////
    virtual bool FindProperty(const AFGUID& self, const std::string& strPropertyName) = 0;

    virtual bool SetPropertyInt(const AFGUID& self, const std::string& strPropertyName, const NFINT64 value) = 0;
    virtual bool SetPropertyDouble(const AFGUID& self, const std::string& strPropertyName, const double value) = 0;
    virtual bool SetPropertyString(const AFGUID& self, const std::string& strPropertyName, const std::string& value) = 0;
    virtual bool SetPropertyObject(const AFGUID& self, const std::string& strPropertyName, const AFGUID& value) = 0;
    virtual bool SetPropertyPoint(const AFGUID& self, const std::string& strPropertyName, const Point3D& value) = 0;

    virtual NFINT64 GetPropertyInt(const AFGUID& self, const std::string& strPropertyName) = 0;
    virtual double GetPropertyDouble(const AFGUID& self, const std::string& strPropertyName) = 0;
    virtual const std::string& GetPropertyString(const AFGUID& self, const std::string& strPropertyName) = 0;
    virtual const AFGUID& GetPropertyObject(const AFGUID& self, const std::string& strPropertyName) = 0;
    virtual const Point3D& GetPropertyPoint(const AFGUID& self, const std::string& strPropertyName) = 0;
    //////////////////////////////////////////////////////////////////////////
    virtual NF_SHARE_PTR<NFIRecord> FindRecord(const AFGUID& self, const std::string& strRecordName) = 0;
    virtual bool ClearRecord(const AFGUID& self, const std::string& strRecordName) = 0;

    virtual bool SetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 value) = 0;
    virtual bool SetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const double value) = 0;
    virtual bool SetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const std::string& value) = 0;
    virtual bool SetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const AFGUID& value) = 0;
    virtual bool SetRecordPoint(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const Point3D& value) = 0;

    virtual bool SetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value) = 0;
    virtual bool SetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value) = 0;
    virtual bool SetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value) = 0;
    virtual bool SetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const AFGUID& value) = 0;
    virtual bool SetRecordPoint(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const Point3D& value) = 0;

    virtual NFINT64 GetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual double GetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual const std::string& GetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual const AFGUID& GetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual const Point3D& GetRecordPoint(const AFGUID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;

    virtual NFINT64 GetRecordInt(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual double GetRecordDouble(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual const std::string& GetRecordString(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual const AFGUID& GetRecordObject(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual const Point3D& GetRecordPoint(const AFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    ////////////////////////////////////////////////////////////////
    virtual bool SwitchScene(const AFGUID& self, const int nTargetSceneID, const int nTargetGroupID, const float fX, const float fY, const float fZ, const float fOrient, const AFDataList& arg) = 0;

    virtual bool CreateScene(const int nSceneID) = 0;
    virtual bool DestroyScene(const int nSceneID) = 0;

    virtual int GetOnLineCount() = 0;
    virtual int GetMaxOnLineCount() = 0;
    virtual int GetSceneOnLineCount(const int nSceneID) = 0;
    virtual int GetSceneOnLineCount(const int nSceneID, const int nGroupID) = 0;
    virtual int GetSceneOnLineList(const int nSceneID, AFDataList& var) = 0;

    virtual int RequestGroupScene(const int nSceneID) = 0;
    virtual bool ReleaseGroupScene(const int nSceneID, const int nGroupID) = 0;
    virtual bool ExitGroupScene(const int nSceneID, const int nGroupID) = 0;

    virtual bool GetGroupObjectList(const int nSceneID, const int nGroupID, AFDataList& list) = 0;
    virtual int GetObjectByProperty(const int nSceneID, const std::string& strPropertyName, const AFDataList& valueArg, AFDataList& list) = 0;

    virtual void Random(int nStart, int nEnd, int nCount, AFDataList& valueList) = 0;
    virtual bool LogInfo(const AFGUID ident) = 0;

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

#endif
