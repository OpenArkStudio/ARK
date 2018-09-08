/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "AFCConfigModule.h"
#include "AFCClassModule.h"
#include "SDK/Core/AFDataNode.hpp"

AFCConfigModule::AFCConfigModule(AFIPluginManager* p)
{
    pPluginManager = p;
}

bool AFCConfigModule::Init()
{
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();

    return Load();
}

bool AFCConfigModule::Shut()
{
    return Clear();
}

bool AFCConfigModule::Load()
{
    if (mbLoaded)
    {
        return false;
    }

    for (ARK_SHARE_PTR<AFIClass> pLogicClass = m_pClassModule->First();  nullptr != pLogicClass; pLogicClass = m_pClassModule->Next())
    {
        const std::string& strInstancePath = pLogicClass->GetResPath();

        if (strInstancePath.empty())
        {
            continue;
        }

        //////////////////////////////////////////////////////////////////////////
        rapidxml::xml_document<> xDoc;
        std::string strFile = pPluginManager->GetResPath() + strInstancePath;
        rapidxml::file<> fdoc(strFile.c_str());
        xDoc.parse<0>(fdoc.data());
        //////////////////////////////////////////////////////////////////////////
        //support for unlimited layer class inherits
        rapidxml::xml_node<>* root = xDoc.first_node();

        for (rapidxml::xml_node<>* attrNode = root->first_node(); attrNode != nullptr; attrNode = attrNode->next_sibling())
        {
            if (!Load(attrNode, pLogicClass))
            {
                return false;
            }
        }

        mbLoaded = true;
    }

    return true;
}

bool AFCConfigModule::Load(rapidxml::xml_node<>* attrNode, ARK_SHARE_PTR<AFIClass> pLogicClass)
{
    //attrNode is the node of a object
    std::string strConfigID = attrNode->first_attribute("Id")->value();

    if (strConfigID.empty())
    {
        ARK_ASSERT(0, strConfigID, __FILE__, __FUNCTION__);
        return false;
    }

    if (ExistConfig(strConfigID))
    {
        ARK_ASSERT(0, strConfigID, __FILE__, __FUNCTION__);
        return false;
    }

    ElementConfigInfo* pElementInfo = ARK_NEW ElementConfigInfo();
    mxElementConfigMap.AddElement(strConfigID, pElementInfo);

    //can find all config id by class name
    pLogicClass->AddConfigName(strConfigID);

    ARK_SHARE_PTR<AFIDataNodeManager> pElementNodeManager = pElementInfo->GetNodeManager();
    ARK_SHARE_PTR<AFIDataTableManager> pElementTableManager = pElementInfo->GetTableManager();

    //1.add DataNode
    //2.set the default value of them
    ARK_SHARE_PTR<AFIDataNodeManager> pClassNodeManager = pLogicClass->GetNodeManager();
    ARK_SHARE_PTR<AFIDataTableManager> pClassTableManager = pLogicClass->GetTableManager();

    if (pClassNodeManager != nullptr && pClassTableManager != nullptr)
    {
        //////////////////////////////////////////////////////////////////////////
        size_t tableCount = pClassTableManager->GetCount();

        for (size_t i = 0; i < tableCount; ++i)
        {
            AFDataTable* pTable = pClassTableManager->GetTableByIndex(i);

            if (pTable != nullptr)
            {
                continue;
            }

            AFCDataList col_type_list;
            pTable->GetColTypeList(col_type_list);
            pElementTableManager->AddTable(NULL_GUID, pTable->GetName(), col_type_list, pTable->GetFeature());
        }
    }

    //3.set the config value to them
    for (rapidxml::xml_attribute<>* pAttribute = attrNode->first_attribute(); pAttribute; pAttribute = pAttribute->next_attribute())
    {
        const char* pstrConfigName = pAttribute->name();
        const char* pstrConfigValue = pAttribute->value();

        AFDataNode TmpNode;

		// null data don't add node;
        const int eType = TmpNode.GetType();
        switch (eType)
        {
        case DT_BOOLEAN:
            TmpNode.value.SetBool(ARK_LEXICAL_CAST<bool>(pstrConfigValue));
            break;

        case DT_INT:
            {
                if (!AFMisc::IsDigit(pstrConfigValue))
                {
                    ARK_ASSERT(0, TmpNode.name.c_str(), __FILE__, __FUNCTION__);
                }

                TmpNode.value.SetInt(ARK_LEXICAL_CAST<int32_t>(pstrConfigValue));
            }
            break;

        case DT_INT64:
            {
                if (!AFMisc::IsDigit(pstrConfigValue))
                {
                    ARK_ASSERT(0, TmpNode.name.c_str(), __FILE__, __FUNCTION__);
                }

                TmpNode.value.SetInt64(ARK_LEXICAL_CAST<int64_t>(pstrConfigValue));
            }
            break;

        case DT_FLOAT:
            {
                if (strlen(pstrConfigValue) <= 0)
                {
                    ARK_ASSERT(0, TmpNode.name.c_str(), __FILE__, __FUNCTION__);
                }

                TmpNode.value.SetFloat(ARK_LEXICAL_CAST<float>(pstrConfigValue));
            }
            break;

        case DT_DOUBLE:
            {
                if (strlen(pstrConfigValue) <= 0)
                {
                    ARK_ASSERT(0, TmpNode.name.c_str(), __FILE__, __FUNCTION__);
                }

                TmpNode.value.SetDouble(ARK_LEXICAL_CAST<double>(pstrConfigValue));
            }
            break;

        case DT_STRING:
            TmpNode.value.SetString(pstrConfigValue);
            break;

        case DT_OBJECT:
            {
                if (strlen(pstrConfigValue) <= 0)
                {
                    ARK_ASSERT(0, TmpNode.name.c_str(), __FILE__, __FUNCTION__);
                }

                TmpNode.value.SetObject(NULL_GUID);
            }
            break;

        default:
            ARK_ASSERT_NO_EFFECT(0);
            break;
        }

		if (!TmpNode.GetValue().IsNullValue())
		{
			pElementNodeManager->AddNode(TmpNode.name.c_str(), TmpNode.value, TmpNode.feature);
		}
    }

    pElementNodeManager->SetNodeString("ClassName", pLogicClass->GetClassName().c_str());

    return true;
}

bool AFCConfigModule::Save()
{
    return true;
}

bool AFCConfigModule::GetNodeBool(const std::string& strConfigName, const std::string& strDataNodeName)
{
    AFDataNode* pNode = GetNode(strConfigName, strDataNodeName);

    if (pNode != nullptr)
    {
        return pNode->value.GetBool();
    }
    else
    {
        return NULL_BOOLEAN;
    }
}

int32_t AFCConfigModule::GetNodeInt(const std::string& strConfigName, const std::string& strDataNodeName)
{
    AFDataNode* pNode = GetNode(strConfigName, strDataNodeName);

    if (pNode != nullptr)
    {
        return pNode->value.GetInt();
    }
    else
    {
        return NULL_INT;
    }
}

int64_t AFCConfigModule::GetNodeInt64(const std::string& strConfigName, const std::string& strDataNodeName)
{
    AFDataNode* pNode = GetNode(strConfigName, strDataNodeName);

    if (pNode != nullptr)
    {
        return pNode->value.GetInt64();
    }
    else
    {
        return NULL_INT64;
    }
}

float AFCConfigModule::GetNodeFloat(const std::string& strConfigName, const std::string& strDataNodeName)
{
    AFDataNode* pNode = GetNode(strConfigName, strDataNodeName);

    if (pNode != nullptr)
    {
        return pNode->value.GetFloat();
    }
    else
    {
        return NULL_FLOAT;
    }
}

double AFCConfigModule::GetNodeDouble(const std::string& strConfigName, const std::string& strDataNodeName)
{
    AFDataNode* pNode = GetNode(strConfigName, strDataNodeName);

    if (pNode != nullptr)
    {
        return pNode->value.GetDouble();
    }
    else
    {
        return NULL_DOUBLE;
    }
}

const char* AFCConfigModule::GetNodeString(const std::string& strConfigName, const std::string& strDataNodeName)
{
    AFDataNode* pNode = GetNode(strConfigName, strDataNodeName);

    if (pNode != nullptr)
    {
        return pNode->value.GetString();
    }
    else
    {
        return nullptr;
    }
}

AFDataNode* AFCConfigModule::GetNode(const std::string& strConfigName, const std::string& strDataNodeName)
{
    ElementConfigInfo* pElementInfo = mxElementConfigMap.GetElement(strConfigName);

    if (pElementInfo != nullptr)
    {
        return pElementInfo->GetNodeManager()->GetNode(strDataNodeName.c_str());
    }
    else
    {
        return nullptr;
    }
}

ARK_SHARE_PTR<AFIDataNodeManager> AFCConfigModule::GetNodeManager(const std::string& strConfigName)
{
    ElementConfigInfo* pElementInfo = mxElementConfigMap.GetElement(strConfigName);

    if (pElementInfo != nullptr)
    {
        return pElementInfo->GetNodeManager();
    }
    else
    {
        return nullptr;
    }
}

ARK_SHARE_PTR<AFIDataTableManager> AFCConfigModule::GetTableManager(const std::string& strConfigName)
{
    ElementConfigInfo* pElementInfo = mxElementConfigMap.GetElement(strConfigName);

    if (pElementInfo != nullptr)
    {
        return pElementInfo->GetTableManager();
    }
    else
    {
        return nullptr;
    }
}

bool AFCConfigModule::ExistConfig(const std::string& strConfigName)
{
    ElementConfigInfo* pElementInfo = mxElementConfigMap.GetElement(strConfigName);
    return (pElementInfo != nullptr);
}

bool AFCConfigModule::ExistConfig(const std::string& strClassName, const std::string& strConfigName)
{
    ElementConfigInfo* pElementInfo = mxElementConfigMap.GetElement(strConfigName);

    if (pElementInfo == nullptr)
    {
        return false;
    }

    const std::string strClass(pElementInfo->GetNodeManager()->GetNodeString("ClassName"));
    return (strClass == strClassName);
}

bool AFCConfigModule::Clear()
{
    mxElementConfigMap.Clear();

    mbLoaded = false;
    return true;
}
