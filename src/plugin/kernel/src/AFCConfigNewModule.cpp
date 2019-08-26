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

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "kernel/include/AFCConfigNewModule.hpp"
#include "interface/AFIModule.hpp"
#include "kernel/interface/AFIStaticObject.hpp"
#include "log/interface/AFILogModule.hpp"
#include "base/AFPluginManager.hpp"
#include "base/AFMacros.hpp"
#include "kernel/include/AFCStaticObjectInner.hpp"
#include "kernel/include/AFCDataNew.hpp"
#include "kernel/include/AFClassMetaManager.hpp"

namespace ark {

bool AFCConfigNewModule::Init()
{
    m_pLogModule = FindModule<AFILogModule>();
    m_pClassModule = FindModule<AFIClassNewMetaModule>();

    return Load();

    return true;
}

bool AFCConfigNewModule::Shut()
{
    return Clear();
}

bool AFCConfigNewModule::Load()
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

bool AFCConfigNewModule::Save()
{
    return false;
}

bool AFCConfigNewModule::Clear()
{
    static_obj_mgr_list_.clear();
    return true;
}

bool AFCConfigNewModule::LoadConfig(ARK_SHARE_PTR<AFClassMeta> pClassMeta)
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
    if (!root_node.IsValid())
    {
        ARK_ASSERT_NO_EFFECT(0);
        return false;
    }

    // create static object manager by name
    auto pStaticObjMgr = CreateStaticObjectManager(pClassMeta->GetName());

    auto data_meta_list = pClassMeta->GetDataMetaList();
    for (auto data_node = root_node.FindNode("data"); data_node.IsValid(); data_node.NextNode())
    {
        // must have id
        auto pIDMeta = data_meta_list.find_value("id");
        ARK_ASSERT_RET_VAL(pIDMeta != nullptr, false);

        AFIDataNew* pIDData = CreateDataByMeta(pIDMeta);
        ARK_ASSERT_RET_VAL(pIDData != nullptr, false);

        pIDData->FromString(data_node.GetString(pIDData->GetName().c_str()));

        // create a new object
        auto pObjBase = pStaticObjMgr->CreateObject(pIDData->GetUInt64(), pClassMeta);
        ARK_ASSERT_RET_VAL(pObjBase != nullptr, false);

        // delete id data do not need it
        ARK_DELETE(pIDData);

        auto pObj = std::dynamic_pointer_cast<AFCStaticObjectInner>(pObjBase);
        ARK_ASSERT_RET_VAL(pObj != nullptr, false);

        // read by class meta
        for (auto iter : data_meta_list)
        {
            auto& data_name = iter.first;
            auto pDataMeta = iter.second;
            ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

            AFIDataNew* pData = pObj->CreateData(pDataMeta);
            ARK_ASSERT_RET_VAL(pData != nullptr, false);

            pData->FromString(data_node.GetString(data_name.c_str()));
        }
    }

    return true;
}

ARK_SHARE_PTR<AFStaticObjectManager> AFCConfigNewModule::CreateStaticObjectManager(const std::string& name)
{
    ARK_SHARE_PTR<AFStaticObjectManager> pObjectManager = nullptr;
    auto iter = static_obj_mgr_list_.find(name);
    if (iter == static_obj_mgr_list_.end())
    {
        // create new class meta
        pObjectManager = std::make_shared<AFStaticObjectManager>();
        static_obj_mgr_list_.insert(name, pObjectManager);
    }
    else
    {
        pObjectManager = iter->second;
    }

    return pObjectManager;
}

ARK_SHARE_PTR<AFStaticObjectManager> AFCConfigNewModule::FindStaticObjectMgr(const std::string& class_name)
{
    return static_obj_mgr_list_.find_value(class_name);
}

ARK_SHARE_PTR<AFIStaticObject> AFCConfigNewModule::FindStaticObject(
    const std::string& class_name, const size_t config_id)
{
    auto pObjectManger = static_obj_mgr_list_.find_value(class_name);
    ARK_ASSERT_RET_VAL(pObjectManger != nullptr, nullptr);

    auto pStaticObject = pObjectManger->FindObject(config_id);
    return pStaticObject;
}

} // namespace ark
