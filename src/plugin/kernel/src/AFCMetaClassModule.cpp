/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

#include "base/AFXml.hpp"
#include "base/AFPluginManager.hpp"
#include "kernel/interface/AFIData.hpp"
#include "kernel/include/AFDataNode.hpp"
#include "kernel/include/AFCMetaClassModule.hpp"

namespace ark {

bool AFCMetaClassModule::Init()
{
    m_pConfigModule = FindModule<AFIConfigModule>();
    m_pLogModule = FindModule<AFILogModule>();

    ARK_ASSERT_RET_VAL(Load(), false);
    return true;
}

bool AFCMetaClassModule::Shut()
{
    metaclasses_.clear();
    return true;
}

int AFCMetaClassModule::ComputerType(const char* type_name, AFIData& data)
{
    if (0 == ARK_STRICMP(type_name, "bool"))
    {
        data.SetBool(NULL_BOOLEAN);
        return data.GetType();
    }
    else if (0 == ARK_STRICMP(type_name, "int"))
    {
        data.SetInt(NULL_INT);
        return data.GetType();
    }
    else if (0 == ARK_STRICMP(type_name, "int64"))
    {
        data.SetInt64(NULL_INT64);
        return data.GetType();
    }
    else if (0 == ARK_STRICMP(type_name, "float"))
    {
        data.SetFloat(NULL_FLOAT);
        return data.GetType();
    }
    else if (0 == ARK_STRICMP(type_name, "double"))
    {
        data.SetDouble(NULL_DOUBLE);
        return data.GetType();
    }
    else if (0 == ARK_STRICMP(type_name, "string"))
    {
        data.SetString(NULL_STR.c_str());
        return data.GetType();
    }

    return DT_UNKNOWN;
}

bool AFCMetaClassModule::AddNodes(rapidxml::xml_node<>* pNodeRootNode, ARK_SHARE_PTR<AFIMetaClass> pClass)
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
        bool bRealTime = ARK_LEXICAL_CAST<bool>(pNode->first_attribute("Cache")->value()); // will change to real-time

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

bool AFCMetaClassModule::AddTables(rapidxml::xml_node<>* pTableRootNode, ARK_SHARE_PTR<AFIMetaClass> pClass)
{
    for (rapidxml::xml_node<>* pTableNode = pTableRootNode->first_node(); pTableNode != nullptr;
         pTableNode = pTableNode->next_sibling())
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
        bool bRealtime =
            ARK_LEXICAL_CAST<bool>(pTableNode->first_attribute("Cache")->value()); // will change to real-time

        AFCDataList col_type_list;

        for (rapidxml::xml_node<>* pTableColNode = pTableNode->first_node(); pTableColNode != nullptr;
             pTableColNode = pTableColNode->next_sibling())
        {
            AFCData data;
            const char* col_type = pTableColNode->first_attribute("Type")->value();

            if (DT_UNKNOWN == ComputerType(col_type, data))
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

bool AFCMetaClassModule::AddComponents(rapidxml::xml_node<>* pComponentRootNode, ARK_SHARE_PTR<AFIMetaClass> pClass)
{
    // Components will be added in Ark-ECS
    return true;
}

bool AFCMetaClassModule::AddClassInclude(const char* pstrClassFilePath, ARK_SHARE_PTR<AFIMetaClass> pClass)
{
    if (pClass->ExistInclude(pstrClassFilePath))
    {
        return false;
    }

    // Todo: will fix by AFXml of stack size.
    //////////////////////////////////////////////////////////////////////////
    std::string strFile = GetPluginManager()->GetResPath() + pstrClassFilePath;
    rapidxml::file<> fdoc(strFile.c_str());
    rapidxml::xml_document<> xDoc;
    xDoc.parse<0>(fdoc.data());
    //////////////////////////////////////////////////////////////////////////

    // support for unlimited layer class inherits
    rapidxml::xml_node<>* root = xDoc.first_node();

    rapidxml::xml_node<>* pRootNodeDataNode = root->first_node("DataNodes");

    if (pRootNodeDataNode != nullptr && !AddNodes(pRootNodeDataNode, pClass))
    {
        ARK_ASSERT(0, "Add Nodes failed", __FILE__, __FUNCTION__);
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    // Add table
    rapidxml::xml_node<>* pRootNodeDataTable = root->first_node("DataTables");

    if (pRootNodeDataTable != nullptr && !AddTables(pRootNodeDataTable, pClass))
    {
        ARK_ASSERT(0, "AddTables failed", __FILE__, __FUNCTION__);
        return false;
    }

    // add include file
    rapidxml::xml_node<>* pIncludeRootNode = root->first_node("Includes");

    if (pIncludeRootNode != nullptr)
    {
        for (rapidxml::xml_node<>* includeNode = pIncludeRootNode->first_node(); includeNode;
             includeNode = includeNode->next_sibling())
        {
            const char* pstrIncludeFile = includeNode->first_attribute("Id")->value();

            if (AddClassInclude(pstrIncludeFile, pClass))
            {
                pClass->AddInclude(pstrIncludeFile);
            }
        }
    }

    return true;
}

bool AFCMetaClassModule::AddClass(const char* pstrSchemaFilePath, ARK_SHARE_PTR<AFIMetaClass> pClass)
{
    for (ARK_SHARE_PTR<AFIMetaClass> pParent = pClass->GetParent(); nullptr != pParent; pParent = pParent->GetParent())
    {
        // inherited some properties form class of parent
        auto include_files = pParent->GetIncludeFiles();
        for (auto iter : include_files)
        {
            const std::string& include_file = iter;
            if (pClass->ExistInclude(include_file))
            {
                continue;
            }

            if (AddClassInclude(include_file.c_str(), pClass))
            {
                pClass->AddInclude(include_file);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    if (AddClassInclude(pstrSchemaFilePath, pClass))
    {
        pClass->AddInclude(pstrSchemaFilePath);
    }

    return true;
}

bool AFCMetaClassModule::AddClass(const std::string& class_name, const std::string& parent_name)
{
    auto parent = metaclasses_.find_value(parent_name);
    auto child = metaclasses_.find_value(class_name);
    if (child != nullptr)
    {
        return false;
    }
    else
    {
        std::shared_ptr<AFIMetaClass> pChildClass = std::make_shared<AFCMetaClass>(class_name);
        metaclasses_.insert(class_name, pChildClass);

        pChildClass->SetTypeName(NULL_STR.c_str());
        pChildClass->SetResPath(NULL_STR.c_str());

        if (parent != nullptr)
        {
            pChildClass->SetParent(parent);
        }
    }

    return true;
}

std::shared_ptr<ark::AFIMetaClass> AFCMetaClassModule::GetMetaClass(const std::string& class_name)
{
    return metaclasses_.find_value(class_name);
}

ark::AFMapEx<std::string, ark::AFIMetaClass>& AFCMetaClassModule::GetAllMetaClass()
{
    return metaclasses_;
}

bool AFCMetaClassModule::AddNodeCallBack(
    const std::string& class_name, const std::string& name, DATA_NODE_EVENT_FUNCTOR&& cb)
{
    auto pClass = metaclasses_.find_value(class_name);
    return ((pClass != nullptr) ? pClass->AddNodeCallBack(name, std::forward<DATA_NODE_EVENT_FUNCTOR>(cb)) : false);
}

bool AFCMetaClassModule::AddTableCallBack(
    const std::string& class_name, const std::string& name, DATA_TABLE_EVENT_FUNCTOR&& cb)
{
    auto pClass = metaclasses_.find_value(class_name);
    return ((pClass != nullptr) ? pClass->AddTableCallBack(name, std::forward<DATA_TABLE_EVENT_FUNCTOR>(cb)) : false);
}

bool AFCMetaClassModule::AddCommonNodeCallback(const std::string& class_name, DATA_NODE_EVENT_FUNCTOR&& cb)
{
    auto pClass = metaclasses_.find_value(class_name);
    if (pClass != nullptr)
    {
        return pClass->AddCommonNodeCallback(std::forward<DATA_NODE_EVENT_FUNCTOR>(cb));
    }
    else
    {
        return false;
    }
}

bool AFCMetaClassModule::AddCommonTableCallback(const std::string& class_name, DATA_TABLE_EVENT_FUNCTOR&& cb)
{
    auto pClass = metaclasses_.find_value(class_name);
    return ((pClass != nullptr) ? pClass->AddCommonTableCallback(std::forward<DATA_TABLE_EVENT_FUNCTOR>(cb)) : false);
}

bool AFCMetaClassModule::Load(rapidxml::xml_node<>* attrNode, ARK_SHARE_PTR<AFIMetaClass> pParentClass)
{
    const char* metaclass_name = attrNode->first_attribute("id")->value();
    // const char* type = attrNode->first_attribute("Type")->value();
    const char* schema_path = attrNode->first_attribute("meta")->value();
    const char* res_path = attrNode->first_attribute("res")->value();

    ARK_SHARE_PTR<AFIMetaClass> pClass = std::make_shared<AFCMetaClass>(metaclass_name);
    metaclasses_.insert(metaclass_name, pClass);
    pClass->SetParent(pParentClass);
    pClass->SetTypeName("");
    pClass->SetResPath(res_path);

    if (!AddClass(schema_path, pClass))
    {
        return false;
    }

    for (rapidxml::xml_node<>* pDataNode = attrNode->first_node(); pDataNode; pDataNode = pDataNode->next_sibling())
    {
        // children
        if (!Load(pDataNode, pClass))
        {
            return false;
        }
    }

    return true;
}

bool AFCMetaClassModule::Load()
{
    std::string file_path = GetPluginManager()->GetResPath() + config_class_file;

    ARK_LOG_INFO("Load config files: {}", file_path);

    // Todo: Will fix by AFXml
    rapidxml::file<> fdoc(file_path.c_str());

    rapidxml::xml_document<> xDoc;
    xDoc.parse<0>(fdoc.data());
    //////////////////////////////////////////////////////////////////////////
    // support for unlimited layer class inherits
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
    auto pClass = metaclasses_.find_value(class_name);
    return ((pClass != nullptr) ? pClass->GetNodeManager() : nullptr);
}

ARK_SHARE_PTR<AFIDataTableManager> AFCMetaClassModule::GetTableManager(const std::string& class_name)
{
    auto pClass = metaclasses_.find_value(class_name);
    return ((pClass != nullptr) ? pClass->GetTableManager() : nullptr);
}

bool AFCMetaClassModule::InitDataNodeManager(
    const std::string& class_name, ARK_SHARE_PTR<AFIDataNodeManager> pNodeManager)
{
    auto pClass = metaclasses_.find_value(class_name);
    return ((pClass != nullptr) ? pClass->InitDataNodeManager(pNodeManager) : false);
}

bool AFCMetaClassModule::InitDataTableManager(
    const std::string& class_name, ARK_SHARE_PTR<AFIDataTableManager> pTableManager)
{
    auto pClass = metaclasses_.find_value(class_name);
    return ((pClass != nullptr) ? pClass->InitDataTableManager(pTableManager) : false);
}

bool AFCMetaClassModule::AddClassCallBack(const std::string& class_name, CLASS_EVENT_FUNCTOR&& cb)
{
    auto pClass = metaclasses_.find_value(class_name);
    return ((pClass != nullptr) ? pClass->AddClassCallBack(std::forward<CLASS_EVENT_FUNCTOR>(cb)) : false);
}

bool AFCMetaClassModule::DoEvent(
    const AFGUID& id, const std::string& class_name, const ArkEntityEvent class_event, const AFIDataList& args)
{
    auto pClass = metaclasses_.find_value(class_name);
    return ((pClass != nullptr) ? pClass->DoEvent(id, class_event, args) : false);
}

//////////////////////////////////////////////////////////////////////////
// bool AFCNewMetaConfigModule::Init()
//{
//    m_pLogModule = FindModule<AFILogModule>();

//    ARK_ASSERT_RET_VAL(Load(), false);
//    return true;
//}

// bool AFCNewMetaConfigModule::Load()
//{
//    std::string file_path = GetResPath() + config_class_file;

//    ARK_LOG_INFO("Load config files: {}", file_path);

//    AFXml xml_file(file_path);
//    auto root_node = xml_file.GetRootNode();
//    for (auto config_node = root_node.FindNode("config"); config_node.IsValid(); config_node.NextNode())
//    {
//        std::string id = config_node.GetString("id");
//        std::string meta_file = config_node.GetString("meta");
//        std::string res_file = config_node.GetString("res");

//        std::shared_ptr<AFIMetaConfigInfo> meta_config_info = std::make_shared<AFCMetaConfigInfo>(id);
//        if (!LoadConfigMeta(meta_file, meta_config_info))
//        {
//            ARK_LOG_ERROR("Load config meta failed, file = {}", meta_file);
//            ARK_ASSERT_RET_VAL(0, false);
//        }

//        meta_config_classes_.insert(id, meta_config_info);
//    }

//    return true;
//}

// std::shared_ptr<AFIMetaConfigInfo> AFCNewMetaConfigModule::GetMetaClass(const std::string& class_name)
//{
//    return meta_config_classes_.find_value(class_name);
//}

// AFNewSmartPtrMap<std::string, AFIMetaConfigInfo>& AFCNewMetaConfigModule::GetAllMetaConfigInfo()
//{
//    return meta_config_classes_;
//}

// bool AFCNewMetaConfigModule::LoadConfigMeta(const std::string& config_meta_file, std::shared_ptr<AFIMetaConfigInfo>
// meta_config_info)
//{
//    //TODO
//    return true;
//}

} // namespace ark
