#include "AFCClassModule.h"
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_print.hpp"
#include "SDK/Base/AFIData.h"

bool AFCClassModule::Init()
{
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();

    Load();

    return true;
}

bool AFCClassModule::Shut()
{
    ClearAll();

    return true;
}

AFCClassModule::AFCClassModule(AFIPluginManager* p)
{
    pPluginManager = p;

    msConfigFileName = "DataConfig/Struct/LogicClass.xml";

    std::cout << "Using [" << pPluginManager->GetConfigPath() + msConfigFileName << "]" << std::endl;
}

AFCClassModule::~AFCClassModule()
{
    ClearAll();
}

int AFCClassModule::ComputerType(const char* pstrTypeName, AFIData& var)
{
    if(0 == ARK_STRICMP(pstrTypeName, "bool"))
    {
        var.SetBool(NULL_BOOLEAN);
        return var.GetType();
    }
    if(0 == ARK_STRICMP(pstrTypeName, "int"))
    {
        var.SetInt(NULL_INT);
        return var.GetType();
    }
    if(0 == ARK_STRICMP(pstrTypeName, "int64"))
    {
        var.SetInt64(NULL_INT64);
        return var.GetType();
    }
    else if(0 == ARK_STRICMP(pstrTypeName, "float"))
    {
        var.SetFloat(NULL_FLOAT);
        return var.GetType();
    }
    else if(0 == ARK_STRICMP(pstrTypeName, "double"))
    {
        var.SetDouble(NULL_DOUBLE);
        return var.GetType();
    }
    else if(0 == ARK_STRICMP(pstrTypeName, "string"))
    {
        var.SetString(NULL_STR.c_str());
        return var.GetType();
    }
    else if(0 == ARK_STRICMP(pstrTypeName, "object"))
    {
        var.SetObject(NULL_GUID);
        return var.GetType();
    }

    return DT_UNKNOWN;
}

bool AFCClassModule::AddPropertys(rapidxml::xml_node<>* pPropertyRootNode, ARK_SHARE_PTR<AFIClass> pClass)
{
    for(rapidxml::xml_node<>* pPropertyNode = pPropertyRootNode->first_node(); pPropertyNode; pPropertyNode = pPropertyNode->next_sibling())
    {
        if(NULL == pPropertyNode)
        {
            continue;
        }

        const char* strPropertyName = pPropertyNode->first_attribute("Id")->value();
        if(NULL != pClass->GetPropertyManager()->GetProperty(strPropertyName))
        {
            ARK_ASSERT(0, strPropertyName, __FILE__, __FUNCTION__);
            continue;
        }

        const char* pstrType = pPropertyNode->first_attribute("Type")->value();
        bool bPublic = ARK_LEXICAL_CAST<bool>(pPropertyNode->first_attribute("Public")->value());
        bool bPrivate = ARK_LEXICAL_CAST<bool>(pPropertyNode->first_attribute("Private")->value());
        bool bSave = ARK_LEXICAL_CAST<bool>(pPropertyNode->first_attribute("Save")->value());
        bool bRealTime = ARK_LEXICAL_CAST<bool>(pPropertyNode->first_attribute("Cache")->value());

        AFCData varProperty;
        if(DT_UNKNOWN == ComputerType(pstrType, varProperty))
        {
            ARK_ASSERT(0, strPropertyName, __FILE__, __FUNCTION__);
        }

        int8_t feature(0);
        if(bPublic)
        {
            BitValue<int8_t>::SetBitValue(feature, AFProperty::PF_PUBLIC);
        }

        if(bPrivate)
        {
            BitValue<int8_t>::SetBitValue(feature, AFProperty::PF_PRIVATE);
        }

        if(bRealTime)
        {
            BitValue<int8_t>::SetBitValue(feature, AFProperty::PF_REAL_TIME);
        }

        if(bSave)
        {
            BitValue<int8_t>::SetBitValue(feature, AFProperty::PF_SAVE);
        }

        pClass->GetPropertyManager()->AddProperty(strPropertyName, varProperty, feature);
    }

    return true;
}

bool AFCClassModule::AddRecords(rapidxml::xml_node<>* pRecordRootNode, ARK_SHARE_PTR<AFIClass> pClass)
{
    for(rapidxml::xml_node<>* pRecordNode = pRecordRootNode->first_node(); pRecordNode;  pRecordNode = pRecordNode->next_sibling())
    {
        if(pRecordNode)
        {
            const char* pstrRecordName = pRecordNode->first_attribute("Id")->value();

            if(pClass->GetRecordManager()->GetElement(pstrRecordName))
            {
                ARK_ASSERT(0, pstrRecordName, __FILE__, __FUNCTION__);
                continue;
            }

            const char* pstrRow = pRecordNode->first_attribute("Row")->value();
            const char* pstrCol = pRecordNode->first_attribute("Col")->value();

            const char* pstrPublic = pRecordNode->first_attribute("Public")->value();
            const char* pstrPrivate = pRecordNode->first_attribute("Private")->value();
            const char* pstrSave = pRecordNode->first_attribute("Save")->value();
            const char* pstrCache = pRecordNode->first_attribute("Cache")->value();

            std::string strView;
            if(pRecordNode->first_attribute("View") != NULL)
            {
                strView = pRecordNode->first_attribute("View")->value();
            }

            bool bPublic = ARK_LEXICAL_CAST<bool>(pstrPublic);
            bool bPrivate = ARK_LEXICAL_CAST<bool>(pstrPrivate);
            bool bSave = ARK_LEXICAL_CAST<bool>(pstrSave);
            bool bCache = ARK_LEXICAL_CAST<bool>(pstrCache);

            ARK_SHARE_PTR<AFIDataList> recordVar(ARK_NEW AFCDataList());
            ARK_SHARE_PTR<AFIDataList> recordTag(ARK_NEW AFCDataList());

            for(rapidxml::xml_node<>* recordColNode = pRecordNode->first_node(); recordColNode;  recordColNode = recordColNode->next_sibling())
            {
                AFCData AFIData;
                const char* pstrColType = recordColNode->first_attribute("Type")->value();
                if(DT_UNKNOWN == ComputerType(pstrColType, AFIData))
                {
                    //assert(0);
                    ARK_ASSERT(0, pstrRecordName, __FILE__, __FUNCTION__);
                }

                recordVar->Append(AFIData);
            }

            ARK_SHARE_PTR<AFIRecord> xRecord = pClass->GetRecordManager()->AddRecord(NULL_GUID, pstrRecordName, recordVar, recordTag, atoi(pstrRow));

            xRecord->SetPublic(bPublic);
            xRecord->SetPrivate(bPrivate);
            xRecord->SetSave(bSave);
            xRecord->SetCache(bCache);
        }
    }

    return true;
}

bool AFCClassModule::AddComponents(rapidxml::xml_node<>* pComponentRootNode, ARK_SHARE_PTR<AFIClass> pClass)
{
    //for(rapidxml::xml_node<>* pComponentNode = pComponentRootNode->first_node(); pComponentNode; pComponentNode = pComponentNode->next_sibling())
    //{
    //    if(pComponentNode)
    //    {
    //        const char* strComponentName = pComponentNode->first_attribute("Name")->value();
    //        const char* strLanguage = pComponentNode->first_attribute("Language")->value();
    //        const char* strEnable = pComponentNode->first_attribute("Enable")->value();
    //        bool bEnable = ARK_LEXICAL_CAST<bool>(strEnable);
    //        if(bEnable)
    //        {
    //            if(pClass->GetComponentManager()->GetElement(strComponentName))
    //            {
    //                //error
    //                ARK_ASSERT(0, strComponentName, __FILE__, __FUNCTION__);
    //                continue;
    //            }
    //            ARK_SHARE_PTR<AFIComponent> xComponent(ARK_NEW AFIComponent(NULL_GUID, strComponentName));
    //            pClass->GetComponentManager()->AddComponent(strComponentName, xComponent);
    //        }
    //    }
    //}

    return true;
}

bool AFCClassModule::AddClassInclude(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIClass> pClass)
{
    if(pClass->Find(pstrClassFilePath))
    {
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    rapidxml::xml_document<> xDoc;
    char* pData = NULL;
    int nDataSize = 0;

    std::string strFile = pPluginManager->GetConfigPath() + pstrClassFilePath;
    rapidxml::file<> fdoc(strFile.c_str());
    nDataSize = fdoc.size();
    pData = new char[nDataSize + 1];
    strncpy(pData, fdoc.data(), nDataSize);


    pData[nDataSize] = 0;
    xDoc.parse<0>(pData);
    //////////////////////////////////////////////////////////////////////////

    //support for unlimited layer class inherits
    rapidxml::xml_node<>* root = xDoc.first_node();

    rapidxml::xml_node<>* pRropertyRootNode = root->first_node("Propertys");
    if(pRropertyRootNode)
    {
        AddPropertys(pRropertyRootNode, pClass);
    }

    //////////////////////////////////////////////////////////////////////////
    //and record
    rapidxml::xml_node<>* pRecordRootNode = root->first_node("Records");
    if(pRecordRootNode)
    {
        AddRecords(pRecordRootNode, pClass);
    }

    rapidxml::xml_node<>* pComponentRootNode = root->first_node("Components");
    if(pComponentRootNode)
    {
        AddComponents(pComponentRootNode, pClass);
    }

    //pClass->mvIncludeFile.push_back( pstrClassFilePath );
    //and include file
    rapidxml::xml_node<>* pIncludeRootNode = root->first_node("Includes");
    if(pIncludeRootNode)
    {
        for(rapidxml::xml_node<>* includeNode = pIncludeRootNode->first_node(); includeNode; includeNode = includeNode->next_sibling())
        {
            const char* pstrIncludeFile = includeNode->first_attribute("Id")->value();
            //std::vector<std::string>::iterator it = std::find( pClass->mvIncludeFile.begin(), pClass->mvIncludeFile..end(), pstrIncludeFile );

            if(AddClassInclude(pstrIncludeFile, pClass))
            {
                pClass->Add(pstrIncludeFile);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    if(NULL != pData)
    {
        delete []pData;
    }
    //////////////////////////////////////////////////////////////////////////

    return true;
}

bool AFCClassModule::AddClass(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIClass> pClass)
{
    ARK_SHARE_PTR<AFIClass> pParent = pClass->GetParent();
    while(nullptr != pParent)
    {
        //inherited some properties form class of parent
        std::string strFileName = "";
        pParent->First(strFileName);
        while(!strFileName.empty())
        {
            if(pClass->Find(strFileName))
            {
                strFileName.clear();
                continue;
            }

            if(AddClassInclude(strFileName.c_str(), pClass))
            {
                pClass->Add(strFileName);
            }

            strFileName.clear();
            pParent->Next(strFileName);
        }

        pParent = pParent->GetParent();
    }

    //////////////////////////////////////////////////////////////////////////
    if(AddClassInclude(pstrClassFilePath, pClass))
    {
        pClass->Add(pstrClassFilePath);
    }

    //file.close();

    return true;
}

bool AFCClassModule::AddClass(const std::string& strClassName, const std::string& strParentName)
{
    ARK_SHARE_PTR<AFIClass> pParentClass = GetElement(strParentName);
    ARK_SHARE_PTR<AFIClass> pChildClass = GetElement(strClassName);
    if(nullptr == pChildClass)
    {
        pChildClass = ARK_SHARE_PTR<AFIClass>(ARK_NEW AFCClass(strClassName));
        AddElement(strClassName, pChildClass);
        //pChildClass = CreateElement( strClassName );

        pChildClass->SetTypeName("");
        pChildClass->SetInstancePath("");

        if(pParentClass)
        {
            pChildClass->SetParent(pParentClass);
        }
    }

    return true;
}

bool AFCClassModule::Load(rapidxml::xml_node<>* attrNode, ARK_SHARE_PTR<AFIClass> pParentClass)
{
    const char* pstrLogicClassName = attrNode->first_attribute("Id")->value();
    const char* pstrType = attrNode->first_attribute("Type")->value();
    const char* pstrPath = attrNode->first_attribute("Path")->value();
    const char* pstrInstancePath = attrNode->first_attribute("InstancePath")->value();

    //printf( "-----------------------------------------------------\n");
    //printf( "%s:\n", pstrLogicClassName );

    ARK_SHARE_PTR<AFIClass> pClass(ARK_NEW AFCClass(pstrLogicClassName));
    AddElement(pstrLogicClassName, pClass);
    pClass->SetParent(pParentClass);
    pClass->SetTypeName(pstrType);
    pClass->SetInstancePath(pstrInstancePath);

    AddClass(pstrPath, pClass);

    for(rapidxml::xml_node<>* pDataNode = attrNode->first_node(); pDataNode; pDataNode = pDataNode->next_sibling())
    {
        //her children
        Load(pDataNode, pClass);
    }
    //printf( "-----------------------------------------------------\n");
    return true;
}

bool AFCClassModule::Load()
{
    //////////////////////////////////////////////////////////////////////////
    rapidxml::xml_document<> xDoc;
    char* pData = NULL;
    int nDataSize = 0;

    std::string strFile = pPluginManager->GetConfigPath() + msConfigFileName;

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
        Load(attrNode, NULL);
    }

    //////////////////////////////////////////////////////////////////////////
    if(NULL != pData)
    {
        delete []pData;
    }
    //////////////////////////////////////////////////////////////////////////
    return true;
}

bool AFCClassModule::Save()
{
    return true;
}

ARK_SHARE_PTR<AFIPropertyMgr> AFCClassModule::GetClassPropertyManager(const std::string& strClassName)
{
    ARK_SHARE_PTR<AFIClass> pClass = GetElement(strClassName);
    if(nullptr != pClass)
    {
        return pClass->GetPropertyManager();
    }

    return NULL;
}

ARK_SHARE_PTR<AFIRecordManager> AFCClassModule::GetClassRecordManager(const std::string& strClassName)
{
    ARK_SHARE_PTR<AFIClass> pClass = GetElement(strClassName);
    if(nullptr != pClass)
    {
        return pClass->GetRecordManager();
    }

    return NULL;
}

//ARK_SHARE_PTR<AFIComponentManager> AFCClassModule::GetClassComponentManager(const std::string& strClassName)
//{
//    ARK_SHARE_PTR<AFIClass> pClass = GetElement(strClassName);
//    if(nullptr != pClass)
//    {
//        return pClass->GetComponentManager();
//    }
//
//    return NULL;
//}

bool AFCClassModule::Clear()
{
    return true;
}

bool AFCClassModule::AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb)
{
    ARK_SHARE_PTR<AFIClass> pClass = GetElement(strClassName);
    if(nullptr == pClass)
    {
        return false;
    }

    return pClass->AddClassCallBack(cb);
}

bool AFCClassModule::DoEvent(const AFGUID& objectID, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const AFIDataList& valueList)
{
    ARK_SHARE_PTR<AFIClass> pClass = GetElement(strClassName);
    if(nullptr == pClass)
    {
        return false;
    }

    return pClass->DoEvent(objectID, eClassEvent, valueList);
}
