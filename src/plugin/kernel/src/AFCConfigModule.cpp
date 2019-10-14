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

#include "kernel/include/AFCConfigModule.hpp"
#include "interface/AFIModule.hpp"
#include "kernel/interface/AFIStaticEntity.hpp"
#include "log/interface/AFILogModule.hpp"
#include "base/AFPluginManager.hpp"
#include "base/AFMacros.hpp"
#include "kernel/include/AFCStaticEntity.hpp"
#include "kernel/include/AFCNode.hpp"
#include "base/AFXml.hpp"
#include "kernel/include/AFNodeManager.hpp"

namespace ark {

bool AFCConfigModule::Init()
{
    m_pLogModule = FindModule<AFILogModule>();
    m_pClassModule = FindModule<AFIClassMetaModule>();

    m_pStaticEntityManager = ARK_NEW AFStaticEntityManager();

    return Load();
}

bool AFCConfigModule::Shut()
{
    return Clear();
}

bool AFCConfigModule::Load()
{
    if (loaded_)
    {
        return false;
    }

    auto& class_meta_list = m_pClassModule->GetMetaList();
    for (auto iter : class_meta_list)
    {
        auto class_meta = iter.second;
        ARK_ASSERT_RET_VAL(LoadConfig(class_meta), false);
    }

    loaded_ = true;

    return true;
}

bool AFCConfigModule::Save()
{
    return false;
}

bool AFCConfigModule::Clear()
{
    ARK_DELETE(m_pStaticEntityManager);
    return true;
}

bool AFCConfigModule::LoadConfig(std::shared_ptr<AFClassMeta> pClassMeta)
{
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, true);

    const std::string& res_path = pClassMeta->GetResPath();
    if (res_path.empty())
    {
        return true;
    }

    std::string file_path = GetPluginManager()->GetResPath() + res_path;

    ARK_LOG_INFO("Load data config files: {}", file_path);

    AFXml xml_doc(file_path);
    auto root_node = xml_doc.GetRootNode();
    ARK_ASSERT_RET_VAL(root_node.IsValid(), false);

    auto& data_meta_list = pClassMeta->GetDataMetaList();
    for (auto data_node = root_node.FindNode("data"); data_node.IsValid(); data_node.NextNode())
    {
        // must have id
        auto idx = pClassMeta->GetIndex("id");
        auto pIDMeta = data_meta_list.find_value(idx);
        ARK_ASSERT_RET_VAL(pIDMeta != nullptr, false);

        AFINode* pIDData = CreateDataByMeta(pIDMeta);
        ARK_ASSERT_RET_VAL(pIDData != nullptr, false);

        pIDData->FromString(data_node.GetString(pIDData->GetName().c_str()));

        // create a new object
        auto pNodeManager = std::make_shared<AFNodeManager>(pClassMeta);
        ARK_ASSERT_RET_VAL(pNodeManager != nullptr, false);

        auto pObj = m_pStaticEntityManager->CreateStaticEntity(pClassMeta, pNodeManager, pIDData->GetValue());
        ARK_ASSERT_RET_VAL(pObj != nullptr, false);

        // delete id data do not need it
        ARK_DELETE(pIDData);

        // read by class meta
        for (auto iter : data_meta_list)
        {
            auto& data_name = iter.first;
            auto pDataMeta = iter.second;
            ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

            AFINode* pData = pNodeManager->CreateData(pDataMeta);
            ARK_ASSERT_RET_VAL(pData != nullptr, false);

            pData->FromString(data_node.GetString(pDataMeta->GetName().c_str()));
        }
    }

    return true;
}

std::shared_ptr<AFIStaticEntity> AFCConfigModule::FindStaticEntity(const ID_TYPE config_id)
{
    ARK_ASSERT_RET_VAL(m_pStaticEntityManager != nullptr, nullptr);

    return m_pStaticEntityManager->FindObject(config_id);
}

} // namespace ark
