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

#include "ark/kernel/class_meta_module_impl.hpp"
#include "ark/plugin_manager.hpp"
#include "ark/xml.hpp"

namespace ark {

bool class_meta_module_impl::init() {
    m_pClassMetaManager = ARK_NEW class_meta_manager;

    ARK_ASSERT_RET_VAL(load(), false);

    return true;
}

bool class_meta_module_impl::shut() {
    ARK_DELETE(m_pClassMetaManager);

    return true;
}

bool class_meta_module_impl::load() {
    if (!load_config())
    {
        return false;
    }

    if (!load_entity())
    {
        return false;
    }

    return true;
}

bool class_meta_module_impl::load_config() {
    // TODO: will put the configuration fileds in kernel.plugin.conf
    const static std::string res_dir = "resource";
    std::string file_path = res_dir + CONFIG_CLASS_FILE_PATH;

    ARK_LOG_INFO("Load config class files: {}", file_path);

    xml xml_doc(file_path);
    auto root_node = xml_doc.get_root_node();
    ARK_ASSERT_RET_VAL(root_node.is_valid(), false);

    for (auto meta_node = root_node.find_node("config"); meta_node.is_valid(); meta_node.next_node())
    {
        std::string meta_name = meta_node.get_string("id");
        std::string schema_path = meta_node.get_string("meta");
        std::string res_path = meta_node.get_string("res");

        auto pClassMeta = m_pClassMetaManager->create_static_meta(meta_name);
        ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

        pClassMeta->set_res_path(res_path);
        ARK_ASSERT_RET_VAL(load_config_meta(res_dir + schema_path, pClassMeta), false);
    }

    return true;
}

bool class_meta_module_impl::load_config_meta(const std::string& schema_path, std::shared_ptr<class_meta> pClassMeta) {
    ARK_LOG_INFO("Load meta files: {}", schema_path);

    xml xml_doc(schema_path);
    auto root_node = xml_doc.get_root_node();
    ARK_ASSERT_RET_VAL(root_node.is_valid(), false);

    const std::string& class_name = pClassMeta->get_name();
    for (auto meta_node = root_node.find_node("meta"); meta_node.is_valid(); meta_node.next_node())
    {
        std::string name = meta_node.get_string("field");
        std::string type_name = meta_node.get_string("type");
        uint32_t index = AFMetaNameIndex::GetIndex(class_name, name);

        data_type data_type = convert_data_type(type_name);
        ARK_ASSERT_RET_VAL(data_type != data_type::DT_EMPTY, false);

        auto pDataMeta = pClassMeta->create_data_meta(name, index);
        ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

        pDataMeta->set_type(data_type);
    }

    return true;
}

bool class_meta_module_impl::load_entity() {
    // TODO: will put the configuration fileds in kernel.plugin.conf
    const static std::string res_dir = "resource";
    std::string file_path = res_dir + ENTITY_CLASS_FILE_PATH;

    ARK_LOG_INFO("Load entity class files: {}", file_path);

    xml xml_doc(file_path);
    auto root_node = xml_doc.get_root_node();
    ARK_ASSERT_RET_VAL(root_node.is_valid(), false);

    for (auto meta_node = root_node.find_node("config"); meta_node.is_valid(); meta_node.next_node())
    {
        std::string schema_path = meta_node.get_string("meta");
        ARK_ASSERT_RET_VAL(load_entity_meta(res_dir + schema_path), false);
    }

    // exact table meta and object meta of a class meta after all loaded
    ARK_ASSERT_RET_VAL(m_pClassMetaManager->after_loaded(), false);

    return true;
}

bool class_meta_module_impl::load_entity_meta(const std::string& schema_path) {
    ARK_LOG_INFO("Load entity class files: {}", schema_path);

    xml xml_doc(schema_path);
    auto root_node = xml_doc.get_root_node();
    ARK_ASSERT_RET_VAL(root_node.is_valid(), false);

    for (auto meta_node = root_node.find_node("meta"); meta_node.is_valid(); meta_node.next_node())
    {
        std::string class_name = meta_node.get_string("class");
        std::string data_name = meta_node.get_string("field_name");
        std::string type_name = meta_node.get_string("type");
        std::string type_class = meta_node.get_string("type_class");
        uint32_t index = AFMetaNameIndex::GetIndex(class_name, data_name);

        // data mask
        uint32_t feature_sync_view = meta_node.get_uint32("sync_view");
        uint32_t feature_sync_self = meta_node.get_uint32("sync_self");
        uint32_t feature_save = meta_node.get_uint32("save");
        uint32_t feature_real_time = meta_node.get_uint32("real_time");

        // class meta
        auto pClassMeta = m_pClassMetaManager->create_meta(class_name);
        ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

        if (type_name == "class")
        {
            continue;
        }

        // data mask
        mask_type mask;
        mask[(size_t)data_mask::PF_SYNC_VIEW] = (feature_sync_view > 0 ? 1 : 0);
        mask[(size_t)data_mask::PF_SYNC_SELF] = (feature_sync_self > 0 ? 1 : 0);
        mask[(size_t)data_mask::PF_SAVE] = (feature_save > 0 ? 1 : 0);
        mask[(size_t)data_mask::PF_REAL_TIME] = (feature_real_time > 0 ? 1 : 0);

        if (type_name == "container")
        {
            auto pContainerMeta = pClassMeta->create_container_meta(data_name, index, type_class, mask);
            ARK_ASSERT_RET_VAL(pContainerMeta != nullptr, false);
            continue;
        }

        // convert type
        data_type data_type = convert_data_type(type_name);
        ARK_ASSERT_RET_VAL(data_type != data_type::DT_EMPTY, false);

        if (data_type::DT_TABLE == data_type)
        {
            // create table meta
            auto pTableMeta = pClassMeta->create_table_meta(data_name, index);
            ARK_ASSERT_RET_VAL(pTableMeta != nullptr, false);

            pTableMeta->set_type_name(type_class);
            pTableMeta->set_mask(mask);
        }
        else
        {
            // default create data meta
            auto pDataMeta = pClassMeta->create_data_meta(data_name, index);
            ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

            pDataMeta->set_mask(mask);
            pDataMeta->set_type(data_type);
        }
    }

    return true;
}

data_type class_meta_module_impl::convert_data_type(const std::string& type_name) {
    data_type data_type = data_type::DT_EMPTY;
    if (type_name == "bool")
    {
        data_type = data_type::DT_BOOLEAN;
    }
    else if (type_name == "int")
    {
        data_type = data_type::DT_INT32;
    }
    else if (type_name == "int32")
    {
        data_type = data_type::DT_INT32;
    }
    else if (type_name == "int64")
    {
        data_type = data_type::DT_INT64;
    }
    else if (type_name == "uint32")
    {
        data_type = data_type::DT_UINT32;
    }
    else if (type_name == "uint64")
    {
        data_type = data_type::DT_UINT64;
    }
    else if (type_name == "float")
    {
        data_type = data_type::DT_FLOAT;
    }
    else if (type_name == "double")
    {
        data_type = data_type::DT_DOUBLE;
    }
    else if (type_name == "string")
    {
        data_type = data_type::DT_STRING;
    }
    else if (type_name == "table")
    {
        data_type = data_type::DT_TABLE;
    }
    else if (type_name == "object")
    {
        data_type = data_type::DT_GUID;
    }

    return data_type;
}

bool class_meta_module_impl::add_class_call_back(
    const std::string& class_name, class_event_functor&& cb, const int32_t prio) {
    auto pClassMeta = m_pClassMetaManager->find_meta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pCallBack = pClassMeta->get_class_call_back_manager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    return pCallBack->add_class_call_back(std::move(cb), prio);
}

bool class_meta_module_impl::do_class_event(
    const guid_t& id, const std::string& class_name, const entity_event class_event, const data_list& args) {
    auto pClassMeta = m_pClassMetaManager->find_meta(class_name);
    ARK_ASSERT_RET_VAL(pClassMeta != nullptr, false);

    auto pCallBack = pClassMeta->get_class_call_back_manager();
    ARK_ASSERT_RET_VAL(pCallBack != nullptr, false);

    return pCallBack->on_class_event(id, class_name, class_event, args);
}

std::shared_ptr<class_meta> class_meta_module_impl::find_meta(const std::string& class_name) const {
    return m_pClassMetaManager->find_meta(class_name);
}

const class_meta_manager::class_meta_list& class_meta_module_impl::get_meta_list() const {
    return m_pClassMetaManager->get_meta_list();
}

std::shared_ptr<class_meta> class_meta_module_impl::find_static_meta(const std::string& class_name) const {
    return m_pClassMetaManager->find_static_meta(class_name);
}

const class_meta_manager::class_meta_list& class_meta_module_impl::get_static_meta_list() const {
    return m_pClassMetaManager->get_static_meta_list();
}

} // namespace ark
