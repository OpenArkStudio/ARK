// -------------------------------------------------------------------------
//    @FileName         :    NFCElementModule.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-15
//    @Module           :    NFCElementModule
//
// -------------------------------------------------------------------------

#include <algorithm>
#include <ctype.h>
#include "NFConfigPlugin.h"
#include "NFCElementModule.h"
#include "NFCClassModule.h"

////

NFCElementModule::NFCElementModule(NFIPluginManager* p)
{
    pPluginManager = p;
    mbLoaded = false;
}

NFCElementModule::~NFCElementModule()
{

}

bool NFCElementModule::Init()
{
    m_pClassModule = pPluginManager->FindModule<NFIClassModule>();

    Load();

    return true;
}

bool NFCElementModule::Shut()
{
    Clear();
    return true;
}

bool NFCElementModule::Load()
{
    if(mbLoaded)
    {
        return false;
    }

    NF_SHARE_PTR<NFIClass> pLogicClass = m_pClassModule->First();
    while(nullptr != pLogicClass)
    {
        const std::string& strInstancePath = pLogicClass->GetInstancePath();
        if(strInstancePath.empty())
        {
            pLogicClass = m_pClassModule->Next();
            continue;
        }
        //////////////////////////////////////////////////////////////////////////
        rapidxml::xml_document<> xDoc;
        char* pData = NULL;
        int nDataSize = 0;

        std::string strFile = pPluginManager->GetConfigPath() + strInstancePath;
        rapidxml::file<> fdoc(strFile.c_str());
        nDataSize = fdoc.size();
        pData = new char[nDataSize + 1];
        strncpy(pData, fdoc.data(), nDataSize);

        pData[nDataSize] = 0;
        xDoc.parse<0>(pData);
        //////////////////////////////////////////////////////////////////////////
        //support for unlimited layer class inherits
        rapidxml::xml_node<>* root = xDoc.first_node();
        for(rapidxml::xml_node<>* attrNode = root->first_node(); attrNode; attrNode = attrNode->next_sibling())
        {
            Load(attrNode, pLogicClass);
        }

        mbLoaded = true;
        //////////////////////////////////////////////////////////////////////////
        if(NULL != pData)
        {
            delete []pData;
        }
        //////////////////////////////////////////////////////////////////////////
        pLogicClass = m_pClassModule->Next();
    }

    return true;
}

bool NFCElementModule::Load(rapidxml::xml_node<>* attrNode, NF_SHARE_PTR<NFIClass> pLogicClass)
{
    //attrNode is the node of a object
    std::string strConfigID = attrNode->first_attribute("Id")->value();
    if(strConfigID.empty())
    {
        NFASSERT(0, strConfigID, __FILE__, __FUNCTION__);
        return false;
    }

    if(ExistElement(strConfigID))
    {
        NFASSERT(0, strConfigID, __FILE__, __FUNCTION__);
        return false;
    }

    NF_SHARE_PTR<ElementConfigInfo> pElementInfo(NF_NEW ElementConfigInfo());
    AddElement(strConfigID, pElementInfo);

    //can find all config id by class name
    pLogicClass->AddConfigName(strConfigID);

    //ElementConfigInfo* pElementInfo = CreateElement( strConfigID, pElementInfo );
    NF_SHARE_PTR<NFIPropertyManager> pElementPropertyManager = pElementInfo->GetPropertyManager();
    NF_SHARE_PTR<NFIRecordManager> pElementRecordManager = pElementInfo->GetRecordManager();

    //1.add property
    //2.set the default value  of them
    NF_SHARE_PTR<NFIPropertyManager> pClassPropertyManager = pLogicClass->GetPropertyManager();
    NF_SHARE_PTR<NFIRecordManager> pClassRecordManager = pLogicClass->GetRecordManager();
    if(nullptr != pClassPropertyManager && nullptr != pClassRecordManager)
    {
        NF_SHARE_PTR<NFIProperty> pProperty = pClassPropertyManager->First();
        while(nullptr != pProperty)
        {
            pElementPropertyManager->AddProperty(NULL_GUID, pProperty);
            pProperty = pClassPropertyManager->Next();
        }

        NF_SHARE_PTR<NFIRecord> pRecord = pClassRecordManager->First();
        while(nullptr != pRecord)
        {
            NF_SHARE_PTR<NFIRecord> xRecord = pElementRecordManager->AddRecord(NULL_GUID, pRecord->GetName(), pRecord->GetInitData(), pRecord->GetTag(), pRecord->GetRows());

            xRecord->SetPublic(pRecord->GetPublic());
            xRecord->SetPrivate(pRecord->GetPrivate());
            xRecord->SetSave(pRecord->GetSave());
            xRecord->SetCache(pRecord->GetCache());

            pRecord = pClassRecordManager->Next();
        }
    }

    //3.set the config value to them

    //const char* pstrConfigID = attrNode->first_attribute( "ID" );
    for(rapidxml::xml_attribute<>* pAttribute = attrNode->first_attribute(); pAttribute; pAttribute = pAttribute->next_attribute())
    {
        const char* pstrConfigName = pAttribute->name();
        const char* pstrConfigValue = pAttribute->value();
        //printf( "%s : %s\n", pstrConfigName, pstrConfigValue );

        NF_SHARE_PTR<NFIProperty> temProperty = pElementPropertyManager->GetElement(pstrConfigName);
        if(!temProperty)
        {
            continue;
        }

        NFIDataList::TData var;
        const TDATA_TYPE eType = temProperty->GetType();
        switch(eType)
        {
        case TDATA_INT:
            {
                if(!LegalNumber(pstrConfigValue))
                {
                    NFASSERT(0, temProperty->GetKey(), __FILE__, __FUNCTION__);
                }
                var.SetInt(lexical_cast<NFINT64>(pstrConfigValue));
            }
            break;
        case TDATA_DOUBLE:
            {
                if(strlen(pstrConfigValue) <= 0)
                {
                    NFASSERT(0, temProperty->GetKey(), __FILE__, __FUNCTION__);
                }
                var.SetDouble(lexical_cast<double>(pstrConfigValue));
            }
            break;
        case TDATA_STRING:
            {
                var.SetString(pstrConfigValue);
            }
            break;
        case TDATA_OBJECT:
            {
                if(strlen(pstrConfigValue) <= 0)
                {
                    NFASSERT(0, temProperty->GetKey(), __FILE__, __FUNCTION__);
                }
                var.SetObject(NULL_GUID);
            }
            break;
        case TDATA_POINT:
            {
                if(strlen(pstrConfigValue) <= 0)
                {
                    NFASSERT(0, temProperty->GetKey(), __FILE__, __FUNCTION__);
                }
                var.SetPoint(NULL_POINT);
            }
            break;
        default:
            NFASSERT(0, temProperty->GetKey(), __FILE__, __FUNCTION__);
            break;
        }

        temProperty->SetValue(var);
        if(eType == TDATA_STRING)
        {
            temProperty->DeSerialization();
        }
    }

    NFIDataList::TData xData;
    xData.SetString(pLogicClass->GetClassName());
    pElementPropertyManager->SetProperty("ClassName", xData);

    return true;
}

bool NFCElementModule::Save()
{
    return true;
}

NFINT64 NFCElementModule::GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetProperty(strConfigName, strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetInt();
    }

    return 0;
}

double NFCElementModule::GetPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetProperty(strConfigName, strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetDouble();
    }

    return 0.0;
}

const std::string& NFCElementModule::GetPropertyString(const std::string& strConfigName, const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetProperty(strConfigName, strPropertyName);
    if(nullptr != pProperty)
    {
        return pProperty->GetString();
    }

    return  NULL_STR;
}

NF_SHARE_PTR<NFIProperty> NFCElementModule::GetProperty(const std::string& strConfigName, const std::string& strPropertyName)
{
    NF_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
    if(nullptr != pElementInfo)
    {
        return pElementInfo->GetPropertyManager()->GetElement(strPropertyName);
    }

    return nullptr;
}

NF_SHARE_PTR<NFIPropertyManager> NFCElementModule::GetPropertyManager(const std::string& strConfigName)
{
    NF_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
    if(nullptr != pElementInfo)
    {
        return pElementInfo->GetPropertyManager();
    }

    return nullptr;
}

NF_SHARE_PTR<NFIRecordManager> NFCElementModule::GetRecordManager(const std::string& strConfigName)
{
    NF_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
    if(nullptr != pElementInfo)
    {
        return pElementInfo->GetRecordManager();
    }
    return nullptr;
}

bool NFCElementModule::ExistElement(const std::string& strConfigName)
{
    NF_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
    if(nullptr != pElementInfo)
    {
        return true;
    }

    return false;
}

bool NFCElementModule::ExistElement(const std::string& strClassName, const std::string& strConfigName)
{
    NF_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
    if(!pElementInfo)
    {
        return false;
    }

    const std::string& strClass = pElementInfo->GetPropertyManager()->GetPropertyString("ClassName");
    if(strClass != strClassName)
    {
        return false;
    }

    return true;
}

bool NFCElementModule::LegalNumber(const char* str)
{
    int nLen = int(strlen(str));
    if(nLen <= 0)
    {
        return false;
    }

    int nStart = 0;
    if('-' == str[0])
    {
        nStart = 1;
    }

    for(int i = nStart; i < nLen; ++i)
    {
        if(!isdigit(str[i]))
        {
            return false;
        }
    }

    return true;
}

bool NFCElementModule::AfterInit()
{
    return true;

}

bool NFCElementModule::BeforeShut()
{
    return true;

}

bool NFCElementModule::Execute()
{
    return true;

}

bool NFCElementModule::Clear()
{
    ClearAll();

    mbLoaded = false;
    return true;
}

NF_SHARE_PTR<NFIComponentManager> NFCElementModule::GetComponentManager(const std::string& strConfigName)
{
    NF_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
    if(nullptr != pElementInfo)
    {
        return pElementInfo->GetComponentManager();
    }

    return nullptr;
}
