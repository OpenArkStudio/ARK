/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @file      AFCElementModule.cpp                                              *
// * @author    Ark Game Tech                                                *
// * @date      2015-12-15                                                   *
// * @brief     AFCElementModule                                                  *
*****************************************************************************/
#include <algorithm>
#include <ctype.h>
#include "AFCElementModule.h"
#include "AFCClassModule.h"

AFCElementModule::AFCElementModule(AFIPluginManager* p)
{
    pPluginManager = p;
    mbLoaded = false;
}

AFCElementModule::~AFCElementModule()
{

}

bool AFCElementModule::Init()
{
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();

    Load();

    return true;
}

bool AFCElementModule::Shut()
{
    Clear();
    return true;
}

bool AFCElementModule::Load()
{
    if(mbLoaded)
    {
        return false;
    }

    ARK_SHARE_PTR<AFIClass> pLogicClass = m_pClassModule->First();
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

bool AFCElementModule::Load(rapidxml::xml_node<>* attrNode, ARK_SHARE_PTR<AFIClass> pLogicClass)
{
    //attrNode is the node of a object
    std::string strConfigID = attrNode->first_attribute("Id")->value();
    if(strConfigID.empty())
    {
        ARK_ASSERT(0, strConfigID, __FILE__, __FUNCTION__);
        return false;
    }

    if(ExistElement(strConfigID))
    {
        ARK_ASSERT(0, strConfigID, __FILE__, __FUNCTION__);
        return false;
    }

    ARK_SHARE_PTR<ElementConfigInfo> pElementInfo(ARK_NEW ElementConfigInfo());
    AddElement(strConfigID, pElementInfo);

    //can find all config id by class name
    pLogicClass->AddConfigName(strConfigID);

    //ElementConfigInfo* pElementInfo = CreateElement( strConfigID, pElementInfo );
    ARK_SHARE_PTR<AFIPropertyMgr> pElementPropertyManager = pElementInfo->GetPropertyManager();
    ARK_SHARE_PTR<AFIRecordMgr> pElementRecordManager = pElementInfo->GetRecordManager();

    //1.add property
    //2.set the default value of them
    ARK_SHARE_PTR<AFIPropertyMgr> pClassPropertyManager = pLogicClass->GetPropertyManager();
    ARK_SHARE_PTR<AFIRecordMgr> pClassRecordManager = pLogicClass->GetRecordManager();
    if(nullptr != pClassPropertyManager && nullptr != pClassRecordManager)
    {
        size_t property_count = pClassPropertyManager->GetPropertyCount();
        for(size_t i = 0; i < property_count; ++i)
        {
            AFProperty* pProperty = pClassPropertyManager->GetPropertyByIndex(i);
            if(NULL != pProperty)
            {
                pElementPropertyManager->AddProperty(pProperty->name.c_str(), pProperty->prop_value, pProperty->feature);
            }
        }
        //////////////////////////////////////////////////////////////////////////
        size_t record_count = pClassRecordManager->GetCount();
        for (size_t i = 0; i < record_count; ++i)
        {
            AFRecord* pRecord = pClassRecordManager->GetRecordByIndex(i);
            if (NULL == pRecord)
            {
                continue;
            }

            AFCDataList col_type_list;
            pRecord->GetColTypeList(col_type_list);
            int8_t feature = pRecord->GetFeature();
            pElementRecordManager->AddRecord(NULL_GUID, pRecord->GetName(), col_type_list, feature);
        }
    }

    //3.set the config value to them
    for(rapidxml::xml_attribute<>* pAttribute = attrNode->first_attribute(); pAttribute; pAttribute = pAttribute->next_attribute())
    {
        const char* pstrConfigName = pAttribute->name();
        const char* pstrConfigValue = pAttribute->value();

        AFProperty* pTmpProperty = pElementPropertyManager->GetProperty(pstrConfigName);
        if(!pTmpProperty)
        {
            continue;
        }

        const int eType = pTmpProperty->GetType();
        switch(eType)
        {
        case DT_BOOLEAN:
            {
                pTmpProperty->prop_value.SetInt(ARK_LEXICAL_CAST<bool>(pstrConfigValue));
            }
            break;
        case DT_INT:
            {
                if(!LegalNumber(pstrConfigValue))
                {
                    ARK_ASSERT(0, pTmpProperty->name.c_str(), __FILE__, __FUNCTION__);
                }
                pTmpProperty->prop_value.SetInt(ARK_LEXICAL_CAST<int32_t>(pstrConfigValue));
            }
            break;
        case DT_INT64:
            {
                pTmpProperty->prop_value.SetInt64(ARK_LEXICAL_CAST<int64_t>(pstrConfigValue));
            }
            break;
        case DT_FLOAT:
            {
                if(strlen(pstrConfigValue) <= 0)
                {
                    ARK_ASSERT(0, pTmpProperty->name.c_str(), __FILE__, __FUNCTION__);
                }
                pTmpProperty->prop_value.SetDouble(ARK_LEXICAL_CAST<float>(pstrConfigValue));
            }
            break;
        case DT_DOUBLE:
            {
                if(strlen(pstrConfigValue) <= 0)
                {
                    ARK_ASSERT(0, pTmpProperty->name.c_str(), __FILE__, __FUNCTION__);
                }
                pTmpProperty->prop_value.SetDouble(ARK_LEXICAL_CAST<double>(pstrConfigValue));
            }
            break;
        case DT_STRING:
            {
                pTmpProperty->prop_value.SetString(pstrConfigValue);
            }
            break;
        case DT_OBJECT:
            {
                if(strlen(pstrConfigValue) <= 0)
                {
                    ARK_ASSERT(0, pTmpProperty->name.c_str(), __FILE__, __FUNCTION__);
                }
                pTmpProperty->prop_value.SetObject(NULL_GUID);
            }
            break;
        default:
            ARK_ASSERT(0, pTmpProperty->name.c_str(), __FILE__, __FUNCTION__);
            break;
        }
    }

    AFCData xData;
    xData.SetString(pLogicClass->GetClassName().c_str());
    pElementPropertyManager->SetProperty("ClassName", xData);

    return true;
}

bool AFCElementModule::Save()
{
    return true;
}

bool AFCElementModule::GetPropertyBool(const std::string& strConfigName, const std::string& strPropertyName)
{
    AFProperty* pProperty = GetProperty(strConfigName, strPropertyName);
    if(NULL != pProperty)
    {
        return pProperty->prop_value.GetBool();
    }

    return NULL_BOOLEAN;
}

int32_t AFCElementModule::GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName)
{
    AFProperty* pProperty = GetProperty(strConfigName, strPropertyName);
    if(NULL != pProperty)
    {
        return pProperty->prop_value.GetInt();
    }

    return NULL_INT;
}

int64_t AFCElementModule::GetPropertyInt64(const std::string& strConfigName, const std::string& strPropertyName)
{
    AFProperty* pProperty = GetProperty(strConfigName, strPropertyName);
    if(NULL != pProperty)
    {
        return pProperty->prop_value.GetInt64();
    }

    return NULL_INT64;
}

float AFCElementModule::GetPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName)
{
    AFProperty* pProperty = GetProperty(strConfigName, strPropertyName);
    if(NULL != pProperty)
    {
        return pProperty->prop_value.GetFloat();
    }

    return NULL_FLOAT;
}

double AFCElementModule::GetPropertyDouble(const std::string& strConfigName, const std::string& strPropertyName)
{
    AFProperty* pProperty = GetProperty(strConfigName, strPropertyName);
    if(NULL != pProperty)
    {
        return pProperty->prop_value.GetFloat();
    }

    return NULL_DOUBLE;
}

const char*  AFCElementModule::GetPropertyString(const std::string& strConfigName, const std::string& strPropertyName)
{
    AFProperty* pProperty = GetProperty(strConfigName, strPropertyName);
    if(NULL != pProperty)
    {
        return pProperty->prop_value.GetString();
    }

    return nullptr;
}

AFProperty* AFCElementModule::GetProperty(const std::string& strConfigName, const std::string& strPropertyName)
{
    ARK_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
    if(nullptr != pElementInfo)
    {
        return pElementInfo->GetPropertyManager()->GetProperty(strPropertyName.c_str());
    }

    return NULL;
}

ARK_SHARE_PTR<AFIPropertyMgr> AFCElementModule::GetPropertyManager(const std::string& strConfigName)
{
    ARK_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
    if(nullptr != pElementInfo)
    {
        return pElementInfo->GetPropertyManager();
    }

    return nullptr;
}

ARK_SHARE_PTR<AFIRecordMgr> AFCElementModule::GetRecordManager(const std::string& strConfigName)
{
    ARK_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
    if(nullptr != pElementInfo)
    {
        return pElementInfo->GetRecordManager();
    }

    return nullptr;
}

bool AFCElementModule::ExistElement(const std::string& strConfigName)
{
    ARK_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
    if(nullptr != pElementInfo)
    {
        return true;
    }

    return false;
}

bool AFCElementModule::ExistElement(const std::string& strClassName, const std::string& strConfigName)
{
    ARK_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
    if(!pElementInfo)
    {
        return false;
    }

    const std::string strClass(pElementInfo->GetPropertyManager()->GetPropertyString("ClassName"));
    if(strClass != strClassName)
    {
        return false;
    }

    return true;
}

bool AFCElementModule::LegalNumber(const char* str)
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

bool AFCElementModule::AfterInit()
{
    return true;

}

bool AFCElementModule::BeforeShut()
{
    return true;

}

bool AFCElementModule::Execute()
{
    return true;

}

bool AFCElementModule::Clear()
{
    ClearAll();

    mbLoaded = false;
    return true;
}

//ARK_SHARE_PTR<AFIComponentManager> AFCElementModule::GetComponentManager(const std::string& strConfigName)
//{
//    ARK_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
//    if(nullptr != pElementInfo)
//    {
//        return pElementInfo->GetComponentManager();
//    }
//
//    return nullptr;
//}
