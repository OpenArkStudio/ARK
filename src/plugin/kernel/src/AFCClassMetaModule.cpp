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

#include "kernel/include/AFCClassMetaModule.hpp"
#include "base/AFXml.hpp"
#include "base/AFPluginManager.hpp"

namespace ark {

bool AFCClassMetaModule::Init()
{
    m_pLogModule = FindModule<AFILogModule>();

    m_pClassMetaManager = ARK_NEW AFClassMetaManager;

    ARK_ASSERT_RET_VAL(Load(), false);

    return true;
}

bool AFCClassMetaModule::Shut()
{
    ARK_DELETE(m_pClassMetaManager);

    return true;
}

bool AFCClassMetaModule::Load()
{
    if (!LoadConfig())
    {
        return false;
    }

    if (!LoadEntity())
    {
        return false;
    }

    return true;
}

bool AFCClassMetaModule::LoadConfig()
{
    std::string file_path = GetPluginManager()->GetResPath() + CONFIG_CLASS_FILE_PATH;

    ARK_LOG_INFO("Load config class files: {}", file_path);

    AFXml xml_doc(file_path);
    auto root_node = xml_doc.GetRootNode();
    ARK_ASSERT_RET_VAL(root_node.IsValid(), false);

    for (auto meta_node = root_node.FindNode("config"); meta_node.IsValid(); meta_node.NextNode())
    {
        std::string meta_name = meta_node.GetString("id");
        std::string schema_path = meta_node.GetString("meta");
        std::string res_path = meta_node.GetString("res");

        auto pClassMeta = m_pClassMetaManager->CreateMeta(meta_name);
        ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

        pClassMeta->SetResPath(res_path);
        ARK_ASSERT_RET_VAL(LoadConfigMeta(schema_path, pClassMeta), false);
    }

    return true;
}

bool AFCClassMetaModule::LoadConfigMeta(const std::string& schema_path, std::shared_ptr<AFClassMeta> pClassMeta)
{
    std::string file_path = GetPluginManager()->GetResPath() + schema_path;

    ARK_LOG_INFO("Load meta files: {}", schema_path);

    AFXml xml_doc(file_path);
    auto root_node = xml_doc.GetRootNode();
    ARK_ASSERT_RET_VAL(root_node.IsValid(), false);

    const std::string& class_name = pClassMeta->GetName();
    for (auto meta_node = root_node.FindNode("meta"); meta_node.IsValid(); meta_node.NextNode())
    {
        std::string name = meta_node.GetString("field");
        std::string type_name = meta_node.GetString("type");
        uint32_t index = AFMetaNameIndex::GetIndex(class_name, name);

        ArkDataType data_type = ConvertDataType(type_name);
        ARK_ASSERT_RET_VAL(data_type != ArkDataType::DT_EMPTY, false);

        auto pDataMeta = pClassMeta->CreateDataMeta(name, index);
        ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

        pDataMeta->SetType(data_type);
    }

    return true;
}

bool AFCClassMetaModule::LoadEntity()
{
    std::string file_path = GetPluginManager()->GetResPath() + ENTITY_CLASS_FILE_PATH;

    ARK_LOG_INFO("Load entity class files: {}", file_path);

    AFXml xml_doc(file_path);
    auto root_node = xml_doc.GetRootNode();
    ARK_ASSERT_RET_VAL(root_node.IsValid(), false);

    for (auto meta_node = root_node.FindNode("config"); meta_node.IsValid(); meta_node.NextNode())
    {
        std::string schema_path = meta_node.GetString("meta");
        ARK_ASSERT_RET_VAL(LoadEntityMeta(schema_path), false);
    }

    // exact table meta and object meta of a class meta after all loaded
    ARK_ASSERT_RET_VAL(m_pClassMetaManager->AfterLoaded(), false);

    return true;
}

bool AFCClassMetaModule::LoadEntityMeta(const std::string& schema_path)
{
    std::string file_path = GetPluginManager()->GetResPath() + schema_path;

    ARK_LOG_INFO("Load entity class files: {}", file_path);

    AFXml xml_doc(file_path);
    auto root_node = xml_doc.GetRootNode();
    ARK_ASSERT_RET_VAL(root_node.IsValid(), false);

    for (auto meta_node = root_node.FindNode("meta"); meta_node.IsValid(); meta_node.NextNode())
    {
        std::string class_name = meta_node.GetString("class");
        std::string data_name = meta_node.GetString("field_name");
        std::string type_name = meta_node.GetString("type");
        std::string type_class = meta_node.GetString("type_class");
        uint32_t index = AFMetaNameIndex::GetIndex(class_name, data_name);

        // data mask
        uint32_t feature_sync_view = meta_node.GetUint32("sync_view");
        uint32_t feature_sync_self = meta_node.GetUint32("sync_self");
        uint32_t feature_save = meta_node.GetUint32("save");
        uint32_t feature_real_time = meta_node.GetUint32("real_time");

        // class meta
        auto pClassMeta = m_pClassMetaManager->CreateMeta(class_name);
        ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

        if (type_name == "class")
        {
            continue;
        }
        else if (type_name == "container")
        {
            auto pContainerMeta = pClassMeta->CreateContainerMeta(data_name, index, type_class);
            ARK_ASSERT_RET_VAL(pContainerMeta != nullptr, false);
            continue;
        }

        // convert type
        ArkDataType data_type = ConvertDataType(type_name);
        ARK_ASSERT_RET_VAL(data_type != ArkDataType::DT_EMPTY, false);

        if (ArkDataType::DT_TABLE == data_type)
        {
            // create table meta
            auto pTableMeta = pClassMeta->CreateTableMeta(data_name, index);
            ARK_ASSERT_RET_VAL(pTableMeta != nullptr, false);

            pTableMeta->SetTypeName(type_class);

            ArkMaskType mask;
            mask[(size_t)ArkTableNodeMask::PF_PUBLIC] = (feature_sync_view > 0 ? 1 : 0);
            mask[(size_t)ArkTableNodeMask::PF_PRIVATE] = (feature_sync_self > 0 ? 1 : 0);
            mask[(size_t)ArkTableNodeMask::PF_SAVE] = (feature_save > 0 ? 1 : 0);
            mask[(size_t)ArkTableNodeMask::PF_REAL_TIME] = (feature_real_time > 0 ? 1 : 0);

            pTableMeta->SetMask(mask);
        }
        else
        {
            // default create data meta
            auto pDataMeta = pClassMeta->CreateDataMeta(data_name, index);
            ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

            ArkMaskType mask;
            mask[(size_t)ArkNodeMask::PF_PUBLIC] = (feature_sync_view > 0 ? 1 : 0);
            mask[(size_t)ArkNodeMask::PF_PRIVATE] = (feature_sync_self > 0 ? 1 : 0);
            mask[(size_t)ArkNodeMask::PF_SAVE] = (feature_save > 0 ? 1 : 0);
            mask[(size_t)ArkNodeMask::PF_REAL_TIME] = (feature_real_time > 0 ? 1 : 0);
            pDataMeta->SetMask(mask);

            pDataMeta->SetType(data_type);
        }
    }

    return true;
}

ArkDataType AFCClassMetaModule::ConvertDataType(const std::string& type_name)
{
    ArkDataType data_type = ArkDataType::DT_EMPTY;
    if (type_name == "bool")
    {
        data_type = ArkDataType::DT_BOOLEAN;
    }
    else if (type_name == "int")
    {
        data_type = ArkDataType::DT_INT32;
    }
    else if (type_name == "int32")
    {
        data_type = ArkDataType::DT_INT32;
    }
    else if (type_name == "int64")
    {
        data_type = ArkDataType::DT_INT64;
    }
    else if (type_name == "uint32")
    {
        data_type = ArkDataType::DT_UINT32;
    }
    else if (type_name == "uint64")
    {
        data_type = ArkDataType::DT_UINT64;
    }
    else if (type_name == "float")
    {
        data_type = ArkDataType::DT_FLOAT;
    }
    else if (type_name == "double")
    {
        data_type = ArkDataType::DT_DOUBLE;
    }
    else if (type_name == "string")
    {
        data_type = ArkDataType::DT_STRING;
    }
    else if (type_name == "table")
    {
        data_type = ArkDataType::DT_TABLE;
    }
    else if (type_name == "object")
    {
        data_type = ArkDataType::DT_GUID;
    }

    return data_type;
}

bool AFCClassMetaModule::AddClassCallBack(const std::string& class_name, CLASS_EVENT_FUNCTOR&& cb, const int32_t prio)
{
    auto pClassMeta = m_pClassMetaManager->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    return pCallBack->AddClassCallBack(std::forward<CLASS_EVENT_FUNCTOR>(cb), prio);
}

bool AFCClassMetaModule::DoClassEvent(
    const AFGUID& id, const std::string& class_name, const ArkEntityEvent class_event, const AFIDataList& args)
{
    auto pClassMeta = m_pClassMetaManager->FindMeta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pCallBack = pClassMeta->GetClassCallBackManager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    return pCallBack->OnClassEvent(id, class_name, class_event, args);
}

std::shared_ptr<AFClassMeta> AFCClassMetaModule::FindMeta(const std::string& class_name)
{
    return m_pClassMetaManager->FindMeta(class_name);
}

const AFClassMetaManager::ClassMetaList& AFCClassMetaModule::GetMetaList() const
{
    return m_pClassMetaManager->GetMetaList();
}

} // namespace ark
