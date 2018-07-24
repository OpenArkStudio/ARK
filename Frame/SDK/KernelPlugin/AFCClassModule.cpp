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
#include "rapidxml/rapidxml_print.hpp"
#include "SDK/Core/AFIData.h"
#include "SDK/Core/AFDataNode.h"
#include "AFCClassModule.h"

AFCClassModule::AFCClassModule(AFIPluginManager* p) : m_pElementModule(nullptr)
{
    pPluginManager = p;
}

AFCClassModule::~AFCClassModule()
{
    ClearAll();
}

bool AFCClassModule::Init()
{
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();

    msConfigFileName = "resource/Struct/LogicClass.xml";
    ARK_LOG_INFO("Using file [{}]", msConfigFileName);

    bool bRet = Load();
    ARK_ASSERT_RET_VAL(bRet, false);

    return true;
}

bool AFCClassModule::Shut()
{
    bool bRet = ClearAll();
    ARK_ASSERT_RET_VAL(bRet, false);

    return true;
}

int AFCClassModule::ComputerType(const char* pstrTypeName, AFIData& var)
{
    if (0 == ARK_STRICMP(pstrTypeName, "bool"))
    {
        var.SetBool(NULL_BOOLEAN);
        return var.GetType();
    }

    if (0 == ARK_STRICMP(pstrTypeName, "int"))
    {
        var.SetInt(NULL_INT);
        return var.GetType();
    }

    if (0 == ARK_STRICMP(pstrTypeName, "int64"))
    {
        var.SetInt64(NULL_INT64);
        return var.GetType();
    }
    else if (0 == ARK_STRICMP(pstrTypeName, "float"))
    {
        var.SetFloat(NULL_FLOAT);
        return var.GetType();
    }
    else if (0 == ARK_STRICMP(pstrTypeName, "double"))
    {
        var.SetDouble(NULL_DOUBLE);
        return var.GetType();
    }
    else if (0 == ARK_STRICMP(pstrTypeName, "string"))
    {
        var.SetString(NULL_STR.c_str());
        return var.GetType();
    }
    else if (0 == ARK_STRICMP(pstrTypeName, "object"))
    {
        var.SetObject(NULL_GUID);
        return var.GetType();
    }

    return DT_UNKNOWN;
}

bool AFCClassModule::AddNodes(rapidxml::xml_node<>* pNodeRootNode, ARK_SHARE_PTR<AFIClass> pClass)
{
    for (rapidxml::xml_node<>* pNode = pNodeRootNode->first_node(); pNode != nullptr; pNode = pNode->next_sibling())
    {
        if (pNode == nullptr)
        {
            continue;
        }

        const char* strNodeName = pNode->first_attribute("Id")->value();

        if (pClass->GetNodeManager()->GetNode(strNodeName) != nullptr)
        {
            ARK_ASSERT(0, strNodeName, __FILE__, __FUNCTION__);
            continue;
        }

        const char* pstrType = pNode->first_attribute("Type")->value();
        bool bPublic = ARK_LEXICAL_CAST<bool>(pNode->first_attribute("Public")->value());
        bool bPrivate = ARK_LEXICAL_CAST<bool>(pNode->first_attribute("Private")->value());
        bool bSave = ARK_LEXICAL_CAST<bool>(pNode->first_attribute("Save")->value());
        bool bRealTime = ARK_LEXICAL_CAST<bool>(pNode->first_attribute("Cache")->value()); //will change to real-time

        AFCData varNode;

        if (DT_UNKNOWN == ComputerType(pstrType, varNode))
        {
            ARK_ASSERT(0, strNodeName, __FILE__, __FUNCTION__);
        }

        AFFeatureType feature;
        feature[AFDataNode::PF_PUBLIC] = (int)bPublic;
        feature[AFDataNode::PF_PRIVATE] = (int)bPrivate;
        feature[AFDataNode::PF_REAL_TIME] = (int)bRealTime;
        feature[AFDataNode::PF_SAVE] = (int)bSave;

        pClass->GetNodeManager()->AddNode(strNodeName, varNode, feature);
    }

    return true;
}

bool AFCClassModule::AddTables(rapidxml::xml_node<>* pTableRootNode, ARK_SHARE_PTR<AFIClass> pClass)
{
    for (rapidxml::xml_node<>* pTableNode = pTableRootNode->first_node(); pTableNode != nullptr;  pTableNode = pTableNode->next_sibling())
    {
        const char* pTableName = pTableNode->first_attribute("Id")->value();

        if (pClass->GetTableManager()->GetTable(pTableName) != nullptr)
        {
            ARK_ASSERT(0, pTableName, __FILE__, __FUNCTION__);
            continue;
        }

        bool bPublic = ARK_LEXICAL_CAST<bool>(pTableNode->first_attribute("Public")->value());
        bool bPrivate = ARK_LEXICAL_CAST<bool>(pTableNode->first_attribute("Private")->value());
        bool bSave = ARK_LEXICAL_CAST<bool>(pTableNode->first_attribute("Save")->value());
        bool bRealtime = ARK_LEXICAL_CAST<bool>(pTableNode->first_attribute("Cache")->value());//will change to real-time

        AFCDataList col_type_list;

        for (rapidxml::xml_node<>* pTableColNode = pTableNode->first_node(); pTableColNode != nullptr;  pTableColNode = pTableColNode->next_sibling())
        {
            AFCData data;
            const char* pstrColType = pTableColNode->first_attribute("Type")->value();

            if (DT_UNKNOWN == ComputerType(pstrColType, data))
            {
                ARK_ASSERT(0, pTableName, __FILE__, __FUNCTION__);
            }

            col_type_list.Append(data);
        }

        AFFeatureType feature(0);
        feature[AFDataTable::TABLE_PUBLIC] = bPublic;
        feature[AFDataTable::TABLE_PRIVATE] = bPrivate;
        feature[AFDataTable::TABLE_REAL_TIME] = bRealtime;
        feature[AFDataTable::TABLE_SAVE] = bSave;

        bool result = pClass->GetTableManager()->AddTable(NULL_GUID, pTableName, col_type_list, feature);
        ARK_ASSERT(result, "add table failed, please check", __FILE__, __FUNCTION__);
    }

    return true;
}

bool AFCClassModule::AddComponents(rapidxml::xml_node<>* pComponentRootNode, ARK_SHARE_PTR<AFIClass> pClass)
{
    //Components will be added in Ark-ECS
    return true;
}

bool AFCClassModule::AddClassInclude(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIClass> pClass)
{
    if (pClass->Find(pstrClassFilePath))
    {
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    std::string strFile = pPluginManager->GetConfigPath() + pstrClassFilePath;
    rapidxml::file<> fdoc(strFile.c_str());

    rapidxml::xml_document<> xDoc;
    xDoc.parse<0>(fdoc.data());
    //////////////////////////////////////////////////////////////////////////

    //support for unlimited layer class inherits
    rapidxml::xml_node<>* root = xDoc.first_node();

    rapidxml::xml_node<>* pRootNodeDataNode = root->first_node("DataNodes");

    if (pRootNodeDataNode != nullptr && !AddNodes(pRootNodeDataNode, pClass))
    {
        ARK_ASSERT(0, "Add Nodes failed", __FILE__, __FUNCTION__);
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //Add table
    rapidxml::xml_node<>* pRootNodeDataTable = root->first_node("DataTables");

    if (pRootNodeDataTable != nullptr && !AddTables(pRootNodeDataTable, pClass))
    {
        ARK_ASSERT(0, "AddTables failed", __FILE__, __FUNCTION__);
        return false;
    }

    //add include file
    rapidxml::xml_node<>* pIncludeRootNode = root->first_node("Includes");

    if (pIncludeRootNode != nullptr)
    {
        for (rapidxml::xml_node<>* includeNode = pIncludeRootNode->first_node(); includeNode; includeNode = includeNode->next_sibling())
        {
            const char* pstrIncludeFile = includeNode->first_attribute("Id")->value();

            if (AddClassInclude(pstrIncludeFile, pClass))
            {
                pClass->Add(pstrIncludeFile);
            }
        }
    }

    return true;
}

bool AFCClassModule::AddClass(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIClass> pClass)
{
    ARK_SHARE_PTR<AFIClass> pParent = pClass->GetParent();

    while (nullptr != pParent)
    {
        //inherited some properties form class of parent
        std::string strFileName;
        pParent->First(strFileName);

        while (!strFileName.empty())
        {
            if (pClass->Find(strFileName))
            {
                strFileName.clear();
                continue;
            }

            if (AddClassInclude(strFileName.c_str(), pClass))
            {
                pClass->Add(strFileName);
            }

            strFileName.clear();
            pParent->Next(strFileName);
        }

        pParent = pParent->GetParent();
    }

    //////////////////////////////////////////////////////////////////////////
    if (AddClassInclude(pstrClassFilePath, pClass))
    {
        pClass->Add(pstrClassFilePath);
    }

    return true;
}

bool AFCClassModule::AddClass(const std::string& strClassName, const std::string& strParentName)
{
    ARK_SHARE_PTR<AFIClass> pParentClass = GetElement(strParentName);
    ARK_SHARE_PTR<AFIClass> pChildClass = GetElement(strClassName);

    if (nullptr == pChildClass)
    {
        pChildClass = std::make_shared<AFCClass>(strClassName);
        AddElement(strClassName, pChildClass);

        pChildClass->SetTypeName("");
        pChildClass->SetInstancePath("");

        if (pParentClass)
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
    const char* pstrInstancePath = attrNode->first_attribute("ResPath")->value();

    ARK_SHARE_PTR<AFIClass> pClass = std::make_shared<AFCClass>(pstrLogicClassName);
    AddElement(pstrLogicClassName, pClass);
    pClass->SetParent(pParentClass);
    pClass->SetTypeName(pstrType);
    pClass->SetInstancePath(pstrInstancePath);

    if (!AddClass(pstrPath, pClass))
    {
        return false;
    }

    for (rapidxml::xml_node<>* pDataNode = attrNode->first_node(); pDataNode; pDataNode = pDataNode->next_sibling())
    {
        //children
        if (!Load(pDataNode, pClass))
        {
            return false;
        }
    }

    return true;
}

bool AFCClassModule::Load()
{
    std::string strFile = pPluginManager->GetConfigPath() + msConfigFileName;
    rapidxml::file<> fdoc(strFile.c_str());

    rapidxml::xml_document<> xDoc;
    xDoc.parse<0>(fdoc.data());
    //////////////////////////////////////////////////////////////////////////
    //support for unlimited layer class inherits
    rapidxml::xml_node<>* root = xDoc.first_node();

    for (rapidxml::xml_node<>* attrNode = root->first_node(); attrNode; attrNode = attrNode->next_sibling())
    {
        if (!Load(attrNode, NULL))
        {
            return false;
        }
    }

    return true;
}

ARK_SHARE_PTR<AFIDataNodeManager> AFCClassModule::GetNodeManager(const std::string& strClassName)
{
    ARK_SHARE_PTR<AFIClass> pClass = GetElement(strClassName);
    return pClass != nullptr ? pClass->GetNodeManager() : nullptr;
}

ARK_SHARE_PTR<AFIDataTableManager> AFCClassModule::GetTableManager(const std::string& strClassName)
{
    ARK_SHARE_PTR<AFIClass> pClass = GetElement(strClassName);
    return pClass != nullptr ? pClass->GetTableManager() : nullptr;
}

bool AFCClassModule::InitDataNodeManager(const std::string& strClassName, ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager)
{
    ARK_SHARE_PTR<AFIDataNodeManager> pStaticClassNodeManager = GetNodeManager(strClassName);

    if (!pStaticClassNodeManager)
    {
        return false;
    }

    size_t staticNodeCount = pStaticClassNodeManager->GetNodeCount();

    for (size_t i = 0; i < staticNodeCount; ++i)
    {
        AFDataNode* pStaticConfigNode = pStaticClassNodeManager->GetNodeByIndex(i);

        if (pStaticConfigNode == nullptr)
        {
            continue;
        }

        bool bRet = pNodeManager->AddNode(pStaticConfigNode->GetName(), pStaticConfigNode->GetValue(), pStaticConfigNode->GetFeature());

        if (!bRet)
        {
            ARK_ASSERT_NO_EFFECT(0);
        }
    }

    return true;
}

bool AFCClassModule::InitDataTableManager(const std::string& strClassName, ARK_SHARE_PTR<AFIDataTableManager> pTableManager)
{
    ARK_SHARE_PTR<AFIDataTableManager> pStaticClassTableManager = GetTableManager(strClassName);

    if (!pStaticClassTableManager)
    {
        return false;
    }

    int staticTableCount = pStaticClassTableManager->GetCount();

    for (size_t i = 0; i < staticTableCount; ++i)
    {
        AFDataTable* pStaticTable = pStaticClassTableManager->GetTableByIndex(i);

        if (pStaticTable == nullptr)
        {
            continue;
        }

        AFCDataList col_type_list;
        pStaticTable->GetColTypeList(col_type_list);
        pTableManager->AddTable(NULL_GUID, pStaticTable->GetName(), col_type_list, pStaticTable->GetFeature());
    }

    return true;
}

bool AFCClassModule::Clear()
{
    return true;
}

bool AFCClassModule::AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb)
{
    ARK_SHARE_PTR<AFIClass> pClass = GetElement(strClassName);

    if (nullptr == pClass)
    {
        return false;
    }

    return pClass->AddClassCallBack(cb);
}

bool AFCClassModule::DoEvent(const AFGUID& objectID, const std::string& strClassName, const ARK_ENTITY_EVENT eClassEvent, const AFIDataList& valueList)
{
    ARK_SHARE_PTR<AFIClass> pClass = GetElement(strClassName);

    if (nullptr == pClass)
    {
        return false;
    }

    return pClass->DoEvent(objectID, eClassEvent, valueList);
}