/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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
#include "SDK/Core/AFIData.hpp"
#include "SDK/Core/AFDataNode.hpp"
#include "AFCMetaClassModule.h"

namespace ark
{

    AFCMetaClassModule::~AFCMetaClassModule()
    {
        ClearAll();
    }

    bool AFCMetaClassModule::Init()
    {
        m_pConfigModule = pPluginManager->FindModule<AFIConfigModule>();
        m_pLogModule = pPluginManager->FindModule<AFILogModule>();

        schema_file_ = "schema/LogicClass.xml";
        ARK_LOG_INFO("Using file [{}{}]", pPluginManager->GetResPath(), schema_file_);

        bool bRet = Load();
        ARK_ASSERT_RET_VAL(bRet, false);

        return true;
    }

    bool AFCMetaClassModule::Shut()
    {
        bool bRet = ClearAll();
        ARK_ASSERT_RET_VAL(bRet, false);

        return true;
    }

    int AFCMetaClassModule::ComputerType(const char* pstrTypeName, AFIData& var)
    {
        if (0 == ARK_STRICMP(pstrTypeName, "bool"))
        {
            var.SetBool(NULL_BOOLEAN);
            return var.GetType();
        }
        else if (0 == ARK_STRICMP(pstrTypeName, "int"))
        {
            var.SetInt(NULL_INT);
            return var.GetType();
        }
        else if (0 == ARK_STRICMP(pstrTypeName, "int64"))
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

        return DT_UNKNOWN;
    }

    bool AFCMetaClassModule::AddNodes(rapidxml::xml_node<>* pNodeRootNode, ARK_SHARE_PTR<AFIMetaClass>& pClass)
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

    bool AFCMetaClassModule::AddTables(rapidxml::xml_node<>* pTableRootNode, ARK_SHARE_PTR<AFIMetaClass>& pClass)
    {
        for (rapidxml::xml_node<>* pTableNode = pTableRootNode->first_node(); pTableNode != nullptr; pTableNode = pTableNode->next_sibling())
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

            for (rapidxml::xml_node<>* pTableColNode = pTableNode->first_node(); pTableColNode != nullptr; pTableColNode = pTableColNode->next_sibling())
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

    bool AFCMetaClassModule::AddComponents(rapidxml::xml_node<>* pComponentRootNode, ARK_SHARE_PTR<AFIMetaClass>& pClass)
    {
        //Components will be added in Ark-ECS
        return true;
    }

    bool AFCMetaClassModule::AddClassInclude(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIMetaClass>& pClass)
    {
        if (pClass->Find(pstrClassFilePath))
        {
            return false;
        }

        //////////////////////////////////////////////////////////////////////////
        std::string strFile = pPluginManager->GetResPath() + pstrClassFilePath;
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

    bool AFCMetaClassModule::AddClass(const char* pstrSchemaFilePath, ARK_SHARE_PTR<AFIMetaClass>& pClass)
    {
        for (ARK_SHARE_PTR<AFIMetaClass> pParent = pClass->GetParent(); nullptr != pParent; pParent = pParent->GetParent())
        {
            //inherited some properties form class of parent
            std::string strFileName;


            for (bool ret = pParent->First(strFileName); ret && !strFileName.empty(); ret = pParent->Next(strFileName))
            {
                if (pClass->Find(strFileName))
                {
                    continue;
                }

                if (AddClassInclude(strFileName.c_str(), pClass))
                {
                    pClass->Add(strFileName);
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////
        if (AddClassInclude(pstrSchemaFilePath, pClass))
        {
            pClass->Add(pstrSchemaFilePath);
        }

        return true;
    }

    bool AFCMetaClassModule::AddClass(const std::string& class_name, const std::string& strParentName)
    {
        ARK_SHARE_PTR<AFIMetaClass>& pParentClass = GetElement(strParentName);
        ARK_SHARE_PTR<AFIMetaClass>& pChildClass = GetElement(class_name);

        if (nullptr == pChildClass)
        {
            pChildClass = std::make_shared<AFCClass>(class_name);
            AddElement(class_name, pChildClass);

            pChildClass->SetTypeName("");
            pChildClass->SetResPath("");

            if (pParentClass)
            {
                pChildClass->SetParent(pParentClass);
            }
        }

        return true;
    }

    bool AFCMetaClassModule::AddNodeCallBack(const std::string& class_name, const std::string& name, const DATA_NODE_EVENT_FUNCTOR_PTR& cb)
    {
        ARK_SHARE_PTR<AFIMetaClass>& pClass = GetElement(class_name);
        if (pClass == nullptr)
        {
            return false;
        }
        else
        {
            return pClass->AddNodeCallBack(name, cb);
        }
    }

    bool AFCMetaClassModule::AddTableCallBack(const std::string& strClassName, const std::string& name, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb)
    {
        ARK_SHARE_PTR<AFIMetaClass>& pClass = GetElement(strClassName);
        if (pClass == nullptr)
        {
            return false;
        }
        else
        {
            return pClass->AddTableCallBack(name, cb);
        }
    }

    bool AFCMetaClassModule::AddCommonNodeCallback(const std::string& strClassName, const DATA_NODE_EVENT_FUNCTOR_PTR& cb)
    {
        ARK_SHARE_PTR<AFIMetaClass> pClass = GetElement(strClassName);
        if (pClass == nullptr)
        {
            return false;
        }
        else
        {
            return pClass->AddCommonNodeCallback(cb);
        }
    }

    bool AFCMetaClassModule::AddCommonTableCallback(const std::string& strClassName, const DATA_TABLE_EVENT_FUNCTOR_PTR& cb)
    {
        ARK_SHARE_PTR<AFIMetaClass> pClass = GetElement(strClassName);
        return ((pClass != nullptr) ? pClass->AddCommonTableCallback(cb) : false);
    }

    bool AFCMetaClassModule::Load(rapidxml::xml_node<>* attrNode, ARK_SHARE_PTR<AFIMetaClass> pParentClass)
    {
        const char* pstrLogicClassName = attrNode->first_attribute("Id")->value();
        const char* pstrType = attrNode->first_attribute("Type")->value();
        const char* pstrSchemaPath = attrNode->first_attribute("Path")->value();
        const char* pstrResPath = attrNode->first_attribute("ResPath")->value();

        ARK_SHARE_PTR<AFIMetaClass> pClass = std::make_shared<AFCClass>(pstrLogicClassName);
        AddElement(pstrLogicClassName, pClass);
        pClass->SetParent(pParentClass);
        pClass->SetTypeName(pstrType);
        pClass->SetResPath(pstrResPath);

        if (!AddClass(pstrSchemaPath, pClass))
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

    bool AFCMetaClassModule::Load()
    {
        std::string file_path = pPluginManager->GetResPath() + schema_file_;
        rapidxml::file<> fdoc(file_path.c_str());

        rapidxml::xml_document<> xDoc;
        xDoc.parse<0>(fdoc.data());
        //////////////////////////////////////////////////////////////////////////
        //support for unlimited layer class inherits
        rapidxml::xml_node<>* root = xDoc.first_node();
        for (rapidxml::xml_node<>* attrNode = root->first_node(); attrNode != nullptr; attrNode = attrNode->next_sibling())
        {
            if (!Load(attrNode, ARK_SHARE_PTR<AFIMetaClass>(nullptr)))
            {
                return false;
            }
        }

        return true;
    }

    ARK_SHARE_PTR<AFIDataNodeManager> AFCMetaClassModule::GetNodeManager(const std::string& class_name)
    {
        ARK_SHARE_PTR<AFIMetaClass> pClass = GetElement(class_name);
        return ((pClass != nullptr) ? pClass->GetNodeManager() : nullptr);
    }

    ARK_SHARE_PTR<AFIDataTableManager> AFCMetaClassModule::GetTableManager(const std::string& class_name)
    {
        ARK_SHARE_PTR<AFIMetaClass> pClass = GetElement(class_name);
        return ((pClass != nullptr) ? pClass->GetTableManager() : nullptr);
    }

    bool AFCMetaClassModule::InitDataNodeManager(const std::string& class_name, ARK_SHARE_PTR<AFIDataNodeManager>& pNodeManager)
    {
        ARK_SHARE_PTR<AFIMetaClass> pClass = GetElement(class_name);
        return ((pClass != nullptr) ? pClass->InitDataNodeManager(pNodeManager) : false);
    }

    bool AFCMetaClassModule::InitDataTableManager(const std::string& class_name, ARK_SHARE_PTR<AFIDataTableManager>& pTableManager)
    {
        ARK_SHARE_PTR<AFIMetaClass> pClass = GetElement(class_name);
        return ((pClass != nullptr) ? pClass->InitDataTableManager(pTableManager) : false);
    }

    bool AFCMetaClassModule::Clear()
    {
        return true;
    }

    bool AFCMetaClassModule::AddClassCallBack(const std::string& class_name, const CLASS_EVENT_FUNCTOR_PTR& cb)
    {
        ARK_SHARE_PTR<AFIMetaClass>& pClass = GetElement(class_name);
        return ((pClass != nullptr) ? pClass->AddClassCallBack(cb) : false);
    }

    bool AFCMetaClassModule::DoEvent(const AFGUID& id, const std::string& class_name, const ARK_ENTITY_EVENT class_event, const AFIDataList& args)
    {
        ARK_SHARE_PTR<AFIMetaClass>& pClass = GetElement(class_name);
        return ((pClass != nullptr) ? pClass->DoEvent(id, class_event, args) : false);
    }

}