// -------------------------------------------------------------------------
//    @FileName         :    NFCKernelModule.cpp
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-15
//    @Module           :    NFCKernelModule
//    @Desc             :
// -------------------------------------------------------------------------

#include "NFCKernelModule.h"
#include "NFComm/NFCore/NFDefine.h"
#include "NFComm/NFCore/NFCObject.h"
#include "NFComm/NFCore/NFCDataList.h"
#include "NFComm/NFCore/NFCRecord.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFCore/NFCMemManger.h"
#include "NFComm/NFMessageDefine/NFProtocolDefine.hpp"

NFCKernelModule::NFCKernelModule(NFIPluginManager* p)
{
    pPluginManager = p;

    nLastTime = pPluginManager->GetNowTime();
    InitRandom();
}

NFCKernelModule::~NFCKernelModule()
{
    ClearAll();
}

void NFCKernelModule::InitRandom()
{
    mvRandom.clear();

    int nRandomMax = 100000;
    mnRandomPos = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0f, 1.0f);

    for(int i = 0; i < nRandomMax; i++)
    {
        mvRandom.push_back(dis(gen));
    }
}

bool NFCKernelModule::Init()
{
    mtDeleteSelfList.clear();

    m_pSceneModule = pPluginManager->FindModule<NFISceneModule>();
    m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
    m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
    m_pLogModule = pPluginManager->FindModule<NFILogModule>();
    m_pUUIDModule = pPluginManager->FindModule<NFIUUIDModule>();

    return true;
}

bool NFCKernelModule::Shut()
{
    return true;
}

bool NFCKernelModule::Execute()
{
    ProcessMemFree();

    mnCurExeObject = NULL_GUID;

    if(mtDeleteSelfList.size() > 0)
    {
        std::list<NFGUID>::iterator it = mtDeleteSelfList.begin();
        for(it; it != mtDeleteSelfList.end(); it++)
        {
            DestroyObject(*it);
        }
        mtDeleteSelfList.clear();
    }

    m_pSceneModule->Execute();

    NF_SHARE_PTR<NFIObject> pObject = First();
    while(pObject)
    {
        mnCurExeObject = pObject->Self();
        pObject->Execute();
        mnCurExeObject = NULL_GUID;

        pObject = Next();
    }

    return true;
}

bool NFCKernelModule::FindHeartBeat(const NFGUID& self, const std::string& strHeartBeatName)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(pObject)
    {
        return pObject->FindHeartBeat(strHeartBeatName);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return false;
}

bool NFCKernelModule::RemoveHeartBeat(const NFGUID& self, const std::string& strHeartBeatName)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(pObject)
    {
        return pObject->RemoveHeartBeat(strHeartBeatName);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);

    return false;
}

NF_SHARE_PTR<NFIObject> NFCKernelModule::CreateObject(const NFGUID& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const NFIDataList& arg)
{
    NF_SHARE_PTR<NFIObject> pObject;
    NFGUID ident = self;

    NF_SHARE_PTR<NFCSceneInfo> pContainerInfo = m_pSceneModule->GetElement(nSceneID);
    if(!pContainerInfo)
    {
        m_pLogModule->LogError(NFGUID(0, nSceneID), "There is no scene", nSceneID, __FUNCTION__, __LINE__);
        return pObject;
    }

    if(!pContainerInfo->GetElement(nGroupID))
    {
        m_pLogModule->LogError(NFGUID(0, nSceneID), "There is no group", nGroupID, __FUNCTION__, __LINE__);
        return pObject;
    }

    //  if (!m_pElementModule->ExistElement(strConfigIndex))
    //  {
    //      m_pLogModule->LogError(NFGUID(0, nSceneID), "There is no group", nGroupID, __FUNCTION__, __LINE__);
    //      return pObject;
    //  }

    //默认为1分组，0则是所有分组都看得见,-1则是容器
    if(ident.IsNull())
    {
        ident = m_pUUIDModule->CreateGUID();
    }

    if(GetElement(ident))
    {
        m_pLogModule->LogError(ident, "The object has Exists", NULL_STR, __FUNCTION__, __LINE__);
        return pObject;
    }

    NF_SHARE_PTR<NFIPropertyManager> pStaticClassPropertyManager = m_pClassModule->GetClassPropertyManager(strClassName);
    NF_SHARE_PTR<NFIRecordManager> pStaticClassRecordManager = m_pClassModule->GetClassRecordManager(strClassName);
    NF_SHARE_PTR<NFIComponentManager> pStaticClasComponentManager = m_pClassModule->GetClassComponentManager(strClassName);
    if(pStaticClassPropertyManager && pStaticClassRecordManager && pStaticClasComponentManager)
    {

        pObject = NF_SHARE_PTR<NFIObject>(NF_NEW NFCObject(ident, pPluginManager));
        //是否是应该晚点等到事件2时才加入容器，这样能保证进入容器的对象都是有完整数据的，否则因为协程的原因，其他对象找到他时他却没数据或者部分数据
        AddElement(ident, pObject);
        pContainerInfo->AddObjectToGroup(nGroupID, ident, strClassName == NFrame::Player::ThisName() ? true : false);

        NF_SHARE_PTR<NFIPropertyManager> pPropertyManager = pObject->GetPropertyManager();
        NF_SHARE_PTR<NFIRecordManager> pRecordManager = pObject->GetRecordManager();
        NF_SHARE_PTR<NFIComponentManager> pComponentManager = pObject->GetComponentManager();

        //默认属性
        NF_SHARE_PTR<NFIProperty> pStaticConfigPropertyInfo = pStaticClassPropertyManager->First();
        while(nullptr != pStaticConfigPropertyInfo)
        {
            NF_SHARE_PTR<NFIProperty> xProperty = pPropertyManager->AddProperty(ident, pStaticConfigPropertyInfo->GetKey(), pStaticConfigPropertyInfo->GetType());

            xProperty->SetPublic(pStaticConfigPropertyInfo->GetPublic());
            xProperty->SetPrivate(pStaticConfigPropertyInfo->GetPrivate());
            xProperty->SetSave(pStaticConfigPropertyInfo->GetSave());
            xProperty->SetCache(pStaticConfigPropertyInfo->GetCache());
            xProperty->SetRelationValue(pStaticConfigPropertyInfo->GetRelationValue());

            //通用回调，方便NET同步
            pObject->AddPropertyCallBack(pStaticConfigPropertyInfo->GetKey(), this, &NFCKernelModule::OnPropertyCommonEvent);

            pStaticConfigPropertyInfo = pStaticClassPropertyManager->Next();
        }

        NF_SHARE_PTR<NFIRecord> pConfigRecordInfo = pStaticClassRecordManager->First();
        while(nullptr != pConfigRecordInfo)
        {
            NF_SHARE_PTR<NFIRecord> xRecord =  pRecordManager->AddRecord(ident,
                                               pConfigRecordInfo->GetName(),
                                               pConfigRecordInfo->GetInitData(),
                                               pConfigRecordInfo->GetTag(),
                                               pConfigRecordInfo->GetRows());

            xRecord->SetPublic(pConfigRecordInfo->GetPublic());
            xRecord->SetPrivate(pConfigRecordInfo->GetPrivate());
            xRecord->SetSave(pConfigRecordInfo->GetSave());
            xRecord->SetCache(pConfigRecordInfo->GetCache());



            //通用回调，方便NET同步
            pObject->AddRecordCallBack(pConfigRecordInfo->GetName(), this, &NFCKernelModule::OnRecordCommonEvent);

            pConfigRecordInfo = pStaticClassRecordManager->Next();
        }

        //////////////////////////////////////////////////////////////////////////
        //配置属性
        NF_SHARE_PTR<NFIPropertyManager> pConfigPropertyManager = m_pElementModule->GetPropertyManager(strConfigIndex);
        NF_SHARE_PTR<NFIRecordManager> pConfigRecordManager = m_pElementModule->GetRecordManager(strConfigIndex);

        if(nullptr != pConfigPropertyManager && nullptr != pConfigRecordManager)
        {
            NF_SHARE_PTR<NFIProperty> pConfigPropertyInfo = pConfigPropertyManager->First();
            while(nullptr != pConfigPropertyInfo)
            {
                if(pConfigPropertyInfo->Changed())
                {
                    pPropertyManager->SetProperty(pConfigPropertyInfo->GetKey(), pConfigPropertyInfo->GetValue());
                }

                pConfigPropertyInfo = pConfigPropertyManager->Next();
            }
        }

        DoEvent(ident, strClassName, CLASS_OBJECT_EVENT::COE_CREATE_NODATA, arg);

        //传入的属性赋值
        for(int i = 0; i < arg.GetCount() - 1; i += 2)
        {
            const std::string& strPropertyName = arg.String(i);
            if(NFrame::IObject::ConfigID() != strPropertyName
                    && NFrame::IObject::ClassName() != strPropertyName
                    && NFrame::IObject::SceneID() != strPropertyName
                    && NFrame::IObject::GroupID() != strPropertyName)
            {
                NF_SHARE_PTR<NFIProperty> pArgProperty = pStaticClassPropertyManager->GetElement(strPropertyName);
                if(pArgProperty)
                {
                    switch(pArgProperty->GetType())
                    {
                    case TDATA_INT:
                        pObject->SetPropertyInt(strPropertyName, arg.Int(i + 1));
                        break;
                    case TDATA_DOUBLE:
                        pObject->SetPropertyDouble(strPropertyName, arg.Double(i + 1));
                        break;
                    case TDATA_STRING:
                        pObject->SetPropertyString(strPropertyName, arg.String(i + 1));
                        break;
                    case TDATA_OBJECT:
                        pObject->SetPropertyObject(strPropertyName, arg.Object(i + 1));
                        break;
                    case TDATA_POINT:
                        pObject->SetPropertyPoint(strPropertyName, arg.Point(i + 1));
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        //放进容器//先进入场景，再进入层
        pObject->SetPropertyString(NFrame::IObject::ConfigID(), strConfigIndex);
        pObject->SetPropertyString(NFrame::IObject::ClassName(), strClassName);
        pObject->SetPropertyInt(NFrame::IObject::SceneID(), nSceneID);
        pObject->SetPropertyInt(NFrame::IObject::GroupID(), nGroupID);

        DoEvent(ident, strClassName, COE_CREATE_LOADDATA, arg);
        DoEvent(ident, strClassName, COE_CREATE_BEFORE_EFFECT, arg);
        DoEvent(ident, strClassName, COE_CREATE_EFFECTDATA, arg);
        DoEvent(ident, strClassName, COE_CREATE_AFTER_EFFECT, arg);
        DoEvent(ident, strClassName, COE_CREATE_HASDATA, arg);
    }

    return pObject;
}

bool NFCKernelModule::DestroyObject(const NFGUID& self)
{
    if(self == mnCurExeObject
            && !self.IsNull())
    {
        //自己的循环过程中，不能删除自己，得等下一帧才行
        return DestroySelf(self);
    }

    //需要同时从容器中删掉
    NFINT64 nGroupID = GetPropertyInt(self, NFrame::IObject::GroupID());
    NFINT64 nSceneID = GetPropertyInt(self, NFrame::IObject::SceneID());

    NF_SHARE_PTR<NFCSceneInfo> pContainerInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pContainerInfo)
    {
        const std::string& strClassName = GetPropertyString(self, NFrame::IObject::ClassName());

        pContainerInfo->RemoveObjectFromGroup(nGroupID, self, strClassName == NFrame::Player::ThisName() ? true : false);

        DoEvent(self, strClassName, COE_BEFOREDESTROY, NFCDataList());
        DoEvent(self, strClassName, COE_DESTROY, NFCDataList());

        RemoveElement(self);

        return true;

    }

    m_pLogModule->LogError(self, "There is no scene", nSceneID, __FUNCTION__, __LINE__);

    return false;
}

bool NFCKernelModule::FindProperty(const NFGUID& self, const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->FindProperty(strPropertyName);
    }

    m_pLogModule->LogError(self, strPropertyName, "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool NFCKernelModule::SetPropertyInt(const NFGUID& self, const std::string& strPropertyName, const NFINT64 value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->SetPropertyInt(strPropertyName, value);
    }

    m_pLogModule->LogError(self, strPropertyName, "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool NFCKernelModule::SetPropertyDouble(const NFGUID& self, const std::string& strPropertyName, const double value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->SetPropertyDouble(strPropertyName, value);
    }

    m_pLogModule->LogError(self, strPropertyName, "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool NFCKernelModule::SetPropertyString(const NFGUID& self, const std::string& strPropertyName, const std::string& value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->SetPropertyString(strPropertyName, value);
    }

    m_pLogModule->LogError(self, strPropertyName, "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool NFCKernelModule::SetPropertyObject(const NFGUID& self, const std::string& strPropertyName, const NFGUID& value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->SetPropertyObject(strPropertyName, value);
    }

    m_pLogModule->LogError(self, strPropertyName, "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

bool NFCKernelModule::SetPropertyPoint(const NFGUID& self, const std::string& strPropertyName, const Point3D& value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->SetPropertyPoint(strPropertyName, value);
    }

    m_pLogModule->LogError(self, strPropertyName, "| There is no object", __FUNCTION__, __LINE__);

    return false;
}

NFINT64 NFCKernelModule::GetPropertyInt(const NFGUID& self, const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetPropertyInt(strPropertyName);
    }

    m_pLogModule->LogError(self, strPropertyName, "| There is no object", __FUNCTION__, __LINE__);

    return NULL_INT;
}

double NFCKernelModule::GetPropertyDouble(const NFGUID& self, const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetPropertyDouble(strPropertyName);
    }

    m_pLogModule->LogError(self, strPropertyName, "| There is no object", __FUNCTION__, __LINE__);

    return NULL_DOUBLE;
}

const std::string& NFCKernelModule::GetPropertyString(const NFGUID& self, const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetPropertyString(strPropertyName);
    }

    m_pLogModule->LogError(self, strPropertyName, "| There is no object", __FUNCTION__, __LINE__);

    return NULL_STR;
}

const NFGUID& NFCKernelModule::GetPropertyObject(const NFGUID& self, const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetPropertyObject(strPropertyName);
    }

    m_pLogModule->LogError(self, strPropertyName, "| There is no object", __FUNCTION__, __LINE__);

    return NULL_GUID;
}

const Point3D& NFCKernelModule::GetPropertyPoint(const NFGUID& self, const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetPropertyPoint(strPropertyName);
    }

    m_pLogModule->LogError(self, strPropertyName, "| There is no object", __FUNCTION__, __LINE__);

    return NULL_POINT;
}

NF_SHARE_PTR<NFIRecord> NFCKernelModule::FindRecord(const NFGUID& self, const std::string& strRecordName)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetRecordManager()->GetElement(strRecordName);
    }

    m_pLogModule->LogError(self, strRecordName, "| There is no object", __FUNCTION__, __LINE__);

    return nullptr;
}

bool NFCKernelModule::ClearRecord(const NFGUID& self, const std::string& strRecordName)
{
    NF_SHARE_PTR<NFIRecord> pRecord =  FindRecord(self, strRecordName);
    if(nullptr != pRecord)
    {
        return pRecord->Clear();
    }

    m_pLogModule->LogError(self, strRecordName, "| There is no record", __FUNCTION__, __LINE__);
    return false;
}

bool NFCKernelModule::SetRecordInt(const NFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        if(!pObject->SetRecordInt(strRecordName, nRow, nCol, value))
        {
            m_pLogModule->LogError(self, strRecordName, "error for row or col", __FUNCTION__, __LINE__);
            return false;
        }

        return true;
    }

    m_pLogModule->LogError(self, strRecordName, "| There is no object", __FUNCTION__, __LINE__);
    return false;
}

bool NFCKernelModule::SetRecordInt(const NFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        if(!pObject->SetRecordInt(strRecordName, nRow, strColTag, value))
        {
            m_pLogModule->LogError(self, strRecordName, "error for row or col", __FUNCTION__, __LINE__);
            return false;
        }

        return true;
    }

    m_pLogModule->LogError(self, strRecordName, "| There is no object", __FUNCTION__, __LINE__);
    return false;
}

bool NFCKernelModule::SetRecordDouble(const NFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const double value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        if(!pObject->SetRecordDouble(strRecordName, nRow, nCol, value))
        {
            m_pLogModule->LogError(self, strRecordName, "error SetRecordFloat for row  or col", __FUNCTION__, __LINE__);
            return false;
        }

        return true;
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return false;
}

bool NFCKernelModule::SetRecordDouble(const NFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        if(!pObject->SetRecordDouble(strRecordName, nRow, strColTag, value))
        {
            m_pLogModule->LogError(self, strRecordName, "error SetRecordFloat for row  or col", __FUNCTION__, __LINE__);
            return false;
        }

        return true;
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return false;
}

bool NFCKernelModule::SetRecordString(const NFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const std::string& value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        if(!pObject->SetRecordString(strRecordName, nRow, nCol, value))
        {
            m_pLogModule->LogError(self, strRecordName, "error SetRecordString for row  or col", __FUNCTION__, __LINE__);
            return false;
        }

        return true;
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return false;
}

bool NFCKernelModule::SetRecordString(const NFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        if(!pObject->SetRecordString(strRecordName, nRow, strColTag, value))
        {
            m_pLogModule->LogError(self, strRecordName, "error SetRecordObject for row  or col", __FUNCTION__, __LINE__);
            return false;
        }

        return true;
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return false;
}

bool NFCKernelModule::SetRecordObject(const NFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        if(!pObject->SetRecordObject(strRecordName, nRow, nCol, value))
        {
            m_pLogModule->LogError(self, strRecordName, "error SetRecordObject for row  or col", __FUNCTION__, __LINE__);
            return false;
        }

        return true;
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return false;
}

bool NFCKernelModule::SetRecordObject(const NFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFGUID& value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        if(!pObject->SetRecordObject(strRecordName, nRow, strColTag, value))
        {
            m_pLogModule->LogError(self, strRecordName, "error SetRecordObject for row  or col", __FUNCTION__, __LINE__);
            return false;
        }

        return true;
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return false;
}

bool NFCKernelModule::SetRecordPoint(const NFGUID& self, const std::string& strRecordName, const int nRow, const int nCol, const Point3D& value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        if(!pObject->SetRecordPoint(strRecordName, nRow, nCol, value))
        {
            m_pLogModule->LogError(self, strRecordName, "error SetRecordPoint for row  or col", __FUNCTION__, __LINE__);
            return false;
        }

        return true;
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return false;

}

bool NFCKernelModule::SetRecordPoint(const NFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const Point3D& value)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        if(!pObject->SetRecordPoint(strRecordName, nRow, strColTag, value))
        {
            m_pLogModule->LogError(self, strRecordName, "error SetRecordPoint for row or col", __FUNCTION__, __LINE__);
            return false;
        }

        return true;
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return false;
}

NFINT64 NFCKernelModule::GetRecordInt(const NFGUID& self, const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetRecordInt(strRecordName, nRow, nCol);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return 0;
}

NFINT64 NFCKernelModule::GetRecordInt(const NFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetRecordInt(strRecordName, nRow, strColTag);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return 0;
}

double NFCKernelModule::GetRecordDouble(const NFGUID& self, const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetRecordDouble(strRecordName, nRow, nCol);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return 0.0;
}

double NFCKernelModule::GetRecordDouble(const NFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetRecordDouble(strRecordName, nRow, strColTag);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return 0.0;
}

const std::string& NFCKernelModule::GetRecordString(const NFGUID& self, const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetRecordString(strRecordName, nRow, nCol);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return NULL_STR;
}

const std::string& NFCKernelModule::GetRecordString(const NFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetRecordString(strRecordName, nRow, strColTag);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return NULL_STR;
}

const NFGUID& NFCKernelModule::GetRecordObject(const NFGUID& self, const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetRecordObject(strRecordName, nRow, nCol);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return NULL_GUID;
}

const NFGUID& NFCKernelModule::GetRecordObject(const NFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetRecordObject(strRecordName, nRow, strColTag);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return NULL_GUID;
}

const Point3D& NFCKernelModule::GetRecordPoint(const NFGUID& self, const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetRecordPoint(strRecordName, nRow, nCol);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return NULL_POINT;
}

const Point3D& NFCKernelModule::GetRecordPoint(const NFGUID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetRecordPoint(strRecordName, nRow, strColTag);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return NULL_POINT;
}

bool NFCKernelModule::SwitchScene(const NFGUID& self, const int nTargetSceneID, const int nTargetGroupID, const float fX, const float fY, const float fZ, const float fOrient, const NFIDataList& arg)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr == pObject)
    {
        m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
        return false;
    }

    NFINT64 nOldSceneID = pObject->GetPropertyInt("SceneID");
    NFINT64 nOldGroupID = pObject->GetPropertyInt("GroupID");

    NF_SHARE_PTR<NFCSceneInfo> pOldSceneInfo = m_pSceneModule->GetElement(nOldSceneID);
    NF_SHARE_PTR<NFCSceneInfo> pNewSceneInfo = m_pSceneModule->GetElement(nTargetSceneID);
    if(nullptr == pOldSceneInfo)
    {
        m_pLogModule->LogError(self, "no this container", nOldSceneID);
        return false;
    }

    if(nullptr == pNewSceneInfo)
    {
        m_pLogModule->LogError(self, "no this container", nTargetSceneID);
        return false;
    }

    if(nullptr == pNewSceneInfo->GetElement(nTargetGroupID))
    {
        m_pLogModule->LogError(self, "no this group", nTargetGroupID);
        return false;
    }

    pOldSceneInfo->RemoveObjectFromGroup(nOldGroupID, self, true);

    //可以在同一场景切换到不同的层
    if(nTargetSceneID != nOldSceneID)
    {
        //真的切场景
        //先退回到0层，才能修改场景ID
        pObject->SetPropertyInt("GroupID", 0);

        pObject->SetPropertyInt("SceneID", nTargetSceneID);
        //进新的场景0层
    }

    pObject->SetPropertyInt("GroupID", nTargetGroupID);

    pObject->SetPropertyDouble("X", fX);
    pObject->SetPropertyDouble("Y", fY);
    pObject->SetPropertyDouble("Z", fZ);

    pNewSceneInfo->AddObjectToGroup(nTargetGroupID, self, true);

    return true;
}

bool NFCKernelModule::CreateScene(const int nSceneID)
{
    NF_SHARE_PTR<NFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pSceneInfo)
    {
        return false;
    }

    //容器nSceneIndex
    pSceneInfo = NF_SHARE_PTR<NFCSceneInfo>(NF_NEW NFCSceneInfo(nSceneID));
    if(nullptr != pSceneInfo)
    {
        m_pSceneModule->AddElement(nSceneID, pSceneInfo);

        //默认分组0
        NF_SHARE_PTR<NFCSceneGroupInfo> pGroupInfo = NF_SHARE_PTR<NFCSceneGroupInfo>(NF_NEW NFCSceneGroupInfo(nSceneID, 0));
        if(nullptr != pGroupInfo)
        {
            pSceneInfo->AddElement(0, pGroupInfo);
            m_pLogModule->LogInfo(NULL_GUID, "Create scene success, groupId:0, scene id:", nSceneID, __FUNCTION__, __LINE__);
            return true;
        }
    }

    return false;
}

bool NFCKernelModule::DestroyScene(const int nSceneID)
{
    m_pSceneModule->RemoveElement(nSceneID);

    return true;
}

int NFCKernelModule::GetOnLineCount()
{
    int nCount = 0;
    NF_SHARE_PTR<NFCSceneInfo> pSceneInfo = m_pSceneModule->First();
    while(nullptr != pSceneInfo)
    {
        NF_SHARE_PTR<NFCSceneGroupInfo> pGroupInfo = pSceneInfo->First();
        while(nullptr != pGroupInfo)
        {
            nCount += pGroupInfo->mxPlayerList.Count();
            pGroupInfo = pSceneInfo->Next();
        }

        pSceneInfo = m_pSceneModule->Next();
    }

    return nCount;
}

int NFCKernelModule::GetMaxOnLineCount()
{
    // test count 5000
    // and it should be define in a config file
    return 10000;
}

int NFCKernelModule::GetSceneOnLineCount(const int nSceneID)
{
    int nCount = 0;

    NF_SHARE_PTR<NFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pSceneInfo)
    {
        NF_SHARE_PTR<NFCSceneGroupInfo> pGroupInfo = pSceneInfo->First();
        while(nullptr != pGroupInfo)
        {
            nCount += pGroupInfo->mxPlayerList.Count();
            pGroupInfo = pSceneInfo->Next();
        }
    }

    return nCount;
}

int NFCKernelModule::GetSceneOnLineCount(const int nSceneID, const int nGroupID)
{
    int nCount = 0;

    NF_SHARE_PTR<NFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pSceneInfo)
    {
        NF_SHARE_PTR<NFCSceneGroupInfo> pGroupInfo = pSceneInfo->GetElement(nGroupID);
        if(nullptr != pGroupInfo)
        {
            nCount = pGroupInfo->mxPlayerList.Count();
        }
    }

    return nCount;
}

//int NFCKernelModule::GetSceneOnLineList( const int nSceneID, type, NFIDataList& var )
int NFCKernelModule::GetSceneOnLineList(const int nSceneID, NFIDataList& var)
{
    NF_SHARE_PTR<NFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pSceneInfo)
    {
        NF_SHARE_PTR<NFCSceneGroupInfo> pGroupInfo = pSceneInfo->First();
        while(nullptr != pGroupInfo)
        {
            NFGUID ident;

            NF_SHARE_PTR<int> pRet  = pGroupInfo->mxPlayerList.First(ident);
            while(!ident.IsNull())
            {
                var.Add(ident);

                ident = NULL_GUID;
                pRet = pGroupInfo->mxPlayerList.Next(ident);
            }

            pGroupInfo = pSceneInfo->Next();
        }
    }

    return var.GetCount();
}

int NFCKernelModule::RequestGroupScene(const int nSceneID)
{
    NF_SHARE_PTR<NFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pSceneInfo)
    {
        int nNewGroupID = pSceneInfo->NewGroupID();
        NF_SHARE_PTR<NFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
        if(pSceneInfo)
        {
            if(!pSceneInfo->GetElement(nNewGroupID))
            {
                NF_SHARE_PTR<NFCSceneGroupInfo> pGroupInfo(NF_NEW NFCSceneGroupInfo(nSceneID, nNewGroupID, pSceneInfo->GetWidth()));
                if(pGroupInfo)
                {
                    pSceneInfo->AddElement(nNewGroupID, pGroupInfo);
                    return nNewGroupID;
                }
            }
        }
    }

    return -1;
}

bool NFCKernelModule::ReleaseGroupScene(const int nSceneID, const int nGroupID)
{
    NF_SHARE_PTR<NFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pSceneInfo)
    {
        if(nullptr != pSceneInfo->GetElement(nGroupID))
        {
            NFCDataList listObject;
            if(GetGroupObjectList(nSceneID, nGroupID, listObject))
            {
                for(int i = 0; i < listObject.GetCount(); ++i)
                {
                    NFGUID ident = listObject.Object(i);
                    DestroyObject(ident);
                }
            }

            pSceneInfo->RemoveElement(nGroupID);

            return true;
        }
    }

    return false;
}

bool NFCKernelModule::ExitGroupScene(const int nSceneID, const int nGroupID)
{
    NF_SHARE_PTR<NFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pSceneInfo)
    {
        NF_SHARE_PTR<NFCSceneGroupInfo> pGroupInfo = pSceneInfo->GetElement(nGroupID);
        return (nullptr != pGroupInfo);
    }

    return false;
}

bool NFCKernelModule::GetGroupObjectList(const int nSceneID, const int nGroupID, NFIDataList& list)
{
    NF_SHARE_PTR<NFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    if(nullptr != pSceneInfo)
    {
        NF_SHARE_PTR<NFCSceneGroupInfo> pGroupInfo = pSceneInfo->GetElement(nGroupID);
        if(nullptr != pGroupInfo)
        {
            NFGUID ident = NULL_GUID;
            NF_SHARE_PTR<int> pRet = pGroupInfo->mxPlayerList.First(ident);
            while(!ident.IsNull())
            {
                list.Add(ident);

                ident = NULL_GUID;
                pRet = pGroupInfo->mxPlayerList.Next(ident);
            }

            pRet = pGroupInfo->mxOtherList.First(ident);
            while(!ident.IsNull())
            {
                list.Add(ident);

                ident = NULL_GUID;
                pRet = pGroupInfo->mxOtherList.Next(ident);
            }

            return true;
        }
    }

    return false;
}

bool NFCKernelModule::LogStack()
{
#if NF_PLATFORM == NF_PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
#endif

#if NF_PLATFORM == NF_PLATFORM_WIN
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
#endif // NF_PLATFORM

    return true;
}

bool NFCKernelModule::LogInfo(const NFGUID ident)
{
    //看是容器还是普通对象，容器则打印所有对象
    NF_SHARE_PTR<NFIObject> pObject = GetObject(ident);
    if(nullptr != pObject)
    {
        if(IsContainer(ident))
        {
            int nSceneID = GetPropertyInt(ident, "SceneID");

            m_pLogModule->LogInfo(ident, "//----------child object list-------- SceneID = ", nSceneID);

            NFCDataList valObjectList;
            int nCount = GetSceneOnLineList(nSceneID, valObjectList);
            for(int i  = 0; i < nCount; i++)
            {
                NFGUID targetIdent = valObjectList.Object(i);
                LogInfo(targetIdent);
            }
        }
        else
        {
            m_pLogModule->LogInfo(ident, "//---------printf object start-------- ", "");


            m_pLogModule->LogInfo(ident, "//---------printf object end-------- ", "");
        }
    }
    else
    {
        m_pLogModule->LogError(ident, "Cannot find this object", NULL_STR, __FUNCTION__, __LINE__);
    }

    return true;
}

int NFCKernelModule::OnPropertyCommonEvent(const NFGUID& self, const std::string& strPropertyName, const NFIDataList::TData& oldVar, const NFIDataList::TData& newVar)
{
    if(IsContainer(self))
    {
        return 0;
    }

    std::list<PROPERTY_EVENT_FUNCTOR_PTR>::iterator it = mtCommonPropertyCallBackList.begin();
    for(it; it != mtCommonPropertyCallBackList.end(); it++)
    {
        (**it)(self, strPropertyName, oldVar, newVar);
    }

    return 0;
}

NF_SHARE_PTR<NFIObject> NFCKernelModule::GetObject(const NFGUID& ident)
{
    return GetElement(ident);
}

bool NFCKernelModule::IsContainer(const NFGUID& self)
{
    NF_SHARE_PTR<NFIObject> pObject = GetObject(self);
    if(nullptr != pObject)
    {
        return (pObject->GetPropertyInt("GroupID") < 0);
    }

    m_pLogModule->LogError(self, "There is no object", NULL_STR, __FUNCTION__, __LINE__);
    return false;
}

int NFCKernelModule::GetObjectByProperty(const int nSceneID, const std::string& strPropertyName, const NFIDataList& valueArg, NFIDataList& list)
{
    NFCDataList varObjectList;
    GetSceneOnLineList(nSceneID, varObjectList);
    int nWorldCount = varObjectList.GetCount();
    for(int i = 0; i < nWorldCount; i++)
    {
        NFGUID ident = varObjectList.Object(i);
        if(FindProperty(ident, strPropertyName))
        {
            TDATA_TYPE eType = valueArg.Type(0);
            switch(eType)
            {
            case TDATA_INT:
                {
                    int nValue = GetPropertyInt(ident, strPropertyName.c_str());
                    if(valueArg.Int(0) == nValue)
                    {
                        list.Add(ident);
                    }
                }
                break;
            case TDATA_DOUBLE:
                {
                    double dValue = GetPropertyDouble(ident, strPropertyName.c_str());
                    double dCompareValue = valueArg.Double(0);
                    if(dValue == dCompareValue)
                    {
                        list.Add(ident);
                    }
                }
                break;
            case TDATA_STRING:
                {
                    std::string strValue = GetPropertyString(ident, strPropertyName.c_str());
                    std::string strCompareValue = valueArg.String(0);
                    if(strValue == strCompareValue)
                    {
                        list.Add(ident);
                    }
                }
                break;
            case TDATA_OBJECT:
                {
                    NFGUID identObject = GetPropertyObject(ident, strPropertyName.c_str());
                    if(valueArg.Object(0) == identObject)
                    {
                        list.Add(ident);
                    }
                }
                break;
            case TDATA_POINT:
                {
                    const Point3D& xPoint = GetPropertyPoint(ident, strPropertyName.c_str());
                    if(valueArg.Point(0) == xPoint)
                    {
                        list.Add(ident);
                    }
                }
                break;
            default:
                break;
            }
        }
    }

    return list.GetCount();
}

bool NFCKernelModule::ExistContainer(const int nSceneID)
{
    NF_SHARE_PTR<NFCSceneInfo> pSceneInfo = m_pSceneModule->GetElement(nSceneID);
    return (nullptr != pSceneInfo);
}

bool NFCKernelModule::DestroySelf(const NFGUID& self)
{
    mtDeleteSelfList.push_back(self);
    return true;
}

int NFCKernelModule::OnRecordCommonEvent(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFIDataList::TData& oldVar, const NFIDataList::TData& newVar)
{
    if(IsContainer(self))
    {
        return 0;
    }

    std::list<RECORD_EVENT_FUNCTOR_PTR>::iterator it = mtCommonRecordCallBackList.begin();
    for(it; it != mtCommonRecordCallBackList.end(); it++)
    {
        (**it)(self, xEventData, oldVar, newVar);
    }

    return 0;
}

int NFCKernelModule::OnClassCommonEvent(const NFGUID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const NFIDataList& var)
{
    if(IsContainer(self))
    {
        return 0;
    }

    std::list<CLASS_EVENT_FUNCTOR_PTR>::iterator it = mtCommonClassCallBackList.begin();
    for(it; it != mtCommonClassCallBackList.end(); it++)
    {
        (**it)(self, strClassName, eClassEvent, var);
    }

    return 0;
}

bool NFCKernelModule::RegisterCommonClassEvent(const CLASS_EVENT_FUNCTOR_PTR& cb)
{
    mtCommonClassCallBackList.push_back(cb);
    return true;
}

bool NFCKernelModule::RegisterCommonPropertyEvent(const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
    mtCommonPropertyCallBackList.push_back(cb);
    return true;
}

bool NFCKernelModule::RegisterCommonRecordEvent(const RECORD_EVENT_FUNCTOR_PTR& cb)
{
    mtCommonRecordCallBackList.push_back(cb);
    return true;
}

bool NFCKernelModule::LogSelfInfo(const NFGUID ident)
{
    return false;
}

bool NFCKernelModule::AfterInit()
{
    NF_SHARE_PTR<NFIClass> pClass = m_pClassModule->First();
    while(nullptr != pClass)
    {
        NFIKernelModule::AddClassCallBack(pClass->GetClassName(), this, &NFCKernelModule::OnClassCommonEvent);
        pClass = m_pClassModule->Next();
    }

    return true;
}

bool NFCKernelModule::DestroyAll()
{
    NF_SHARE_PTR<NFIObject> pObject = First();
    while(nullptr != pObject)
    {
        mtDeleteSelfList.push_back(pObject->Self());

        pObject = Next();
    }

    // 为了释放object
    Execute();

    m_pSceneModule->ClearAll();

    mvRandom.clear();
    mtCommonClassCallBackList.clear();
    mtCommonPropertyCallBackList.clear();
    mtCommonRecordCallBackList.clear();

    return true;
}

bool NFCKernelModule::BeforeShut()
{
    DestroyAll();
    return true;
}

void NFCKernelModule::Random(int nStart, int nEnd, int nCount, NFIDataList& valueList)
{
    if(mnRandomPos + nCount >= mvRandom.size())
    {
        mnRandomPos = 0;
    }

    for(int i = mnRandomPos; i < mnRandomPos + nCount; i++)
    {
        float fRanValue = mvRandom.at(i);
        int nValue = (nEnd - nStart) * fRanValue + nStart;
        valueList.Add((NFINT64)nValue);
    }

    mnRandomPos += nCount;
}

bool NFCKernelModule::AddEventCallBack(const NFGUID& self, const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetEventManager()->AddEventCallBack(nEventID, cb);
    }

    return false;
}

bool NFCKernelModule::AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb)
{
    return m_pClassModule->AddClassCallBack(strClassName, cb);
}

void NFCKernelModule::ProcessMemFree()
{
    if(nLastTime + 30 > pPluginManager->GetNowTime())
    {
        return;
    }

    nLastTime = pPluginManager->GetNowTime();

    NFCMemManger::GetSingletonPtr()->FreeMem();
}

bool NFCKernelModule::DoEvent(const NFGUID& self, const std::string& strClassName, CLASS_OBJECT_EVENT eEvent, const NFIDataList& valueList)
{
    return m_pClassModule->DoEvent(self, strClassName, eEvent, valueList);
}

bool NFCKernelModule::DoEvent(const NFGUID& self, const int nEventID, const NFIDataList& valueList)
{
    NF_SHARE_PTR<NFIObject> pObject = GetElement(self);
    if(nullptr != pObject)
    {
        return pObject->GetEventManager()->DoEvent(nEventID, valueList);
    }

    return false;
}
