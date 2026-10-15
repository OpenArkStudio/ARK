/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "ark/kernel/config_module_impl.hpp"
#include "ark/kernel/node_impl.hpp"
#include "ark/kernel/node_manager.hpp"
#include "ark/kernel/static_entity.hpp"
#include "ark/kernel/static_entity_impl.hpp"
#include "ark/macros.hpp"
#include "ark/module.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/xml.hpp"

namespace ark {

bool config_module_impl::init() {
    m_pClassModule = find_module<class_meta_module>();

    m_pStaticEntityManager = ARK_NEW static_entity_manager();

    return load();
} // namespace ark

bool config_module_impl::shut() {
    return clear();
}

bool config_module_impl::load() {
    if (loaded_)
    {
        return false;
    }

    auto& class_meta_list = m_pClassModule->get_static_meta_list();
    for (auto iter : class_meta_list)
    {
        auto class_meta = iter.second;
        ARK_ASSERT_RET_VAL(load_config(class_meta), false);
    }

    loaded_ = true;

    return true;
}

bool config_module_impl::save() {
    return false;
}

bool config_module_impl::clear() {
    ARK_DELETE(m_pStaticEntityManager);
    return true;
}

bool config_module_impl::load_config(std::shared_ptr<class_meta> pClassMeta) {
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, true);

    const std::string& res_path = pClassMeta->get_res_path();
    if (res_path.empty())
    {
        return true;
    }

    // TODO: put the config filed in kernel.plgin.conf
    std::string file_path = "resource" + res_path;

    ARK_LOG_INFO("Load data config files: {}", file_path);

    xml xml_doc(file_path);
    auto root_node = xml_doc.get_root_node();
    ARK_ASSERT_RET_VAL(root_node.is_valid(), false);

    auto& data_meta_list = pClassMeta->get_data_meta_list();
    for (auto data_node = root_node.find_node("data"); data_node.is_valid(); data_node.next_node())
    {
        // must have id
        auto idx = pClassMeta->get_index("id");
        auto pIDMeta = data_meta_list.find_value(idx);
        ARK_ASSERT_RET_VAL(pIDMeta != nullptr, false);

        node* pIDData = create_data_by_meta(pIDMeta);
        ARK_ASSERT_RET_VAL(pIDData != nullptr, false);

        pIDData->from_string(data_node.get_string(pIDData->get_name().c_str()));

        // create a new object
        auto pNodeManager = std::make_shared<node_manager>(pClassMeta);
        ARK_ASSERT_RET_VAL(pNodeManager != nullptr, false);

        auto pObj = m_pStaticEntityManager->create_static_entity(pClassMeta, pNodeManager, pIDData->get_value());
        ARK_ASSERT_RET_VAL(pObj != nullptr, false);

        // delete id data do not need it
        ARK_DELETE(pIDData);

        // read by class meta
        for (auto iter : data_meta_list)
        {
            auto& data_name = iter.first;
            auto pDataMeta = iter.second;
            ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

            node* pData = pNodeManager->create_data(pDataMeta);
            ARK_ASSERT_RET_VAL(pData != nullptr, false);

            pData->from_string(data_node.get_string(pDataMeta->get_name().c_str()));
        }
    }

    return true;
}

std::shared_ptr<static_entity> config_module_impl::find_static_entity(const id_type config_id) {
    ARK_ASSERT_RET_VAL(m_pStaticEntityManager != nullptr, nullptr);

    return m_pStaticEntityManager->find_object(config_id);
}

} // namespace ark
