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

#include "ark/kernel/class_meta.hpp"
#include "ark/kernel/data.hpp"
#include "ark/kernel/data_list_impl.hpp"
#include "ark/meta_define.hpp"
#include "sample3_module.hpp"

static const int MPA_ID = 1;
static const size_t ITEM_CONFIG_ID = 100001;
static const std::string RECORD_NAME = ark::AFEntityMetaPlayer::items();
static const uint32_t BAG = ark::AFEntityMetaPlayer::bag_index();
static const uint32_t col_count = ark::AFEntityMetaItemBag::count_index();
static const uint32_t col_guid = ark::AFEntityMetaItemBag::guid_index();

namespace ark {

bool sample3_module::init() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", Init" << std::endl;
    return true;
}

bool sample3_module::post_init() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", PostInit" << std::endl;

    m_pKernelModule = find_module<kernel_module>();
    m_pMapModule = find_module<map_module>();

    ARK_LOG_INFO("Init finished...");

    // Create scene, all entity need in scene
    m_pMapModule->create_map(MPA_ID);

    //DestroyTest();
    //CalssCallBackTest();
    //DataCallBackTest();
    //TableCallBackTest();
    //EventTest();
    ContainerCallBackTest();

    CreateEntityTest();
    DataTest();
    TableTest();

    // m_pKernelModule->DestroyAll();

    return true;
}

bool sample3_module::pre_shut() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", PreShut" << std::endl;
    m_pKernelModule->destroy_all();

    return true;
}

bool sample3_module::shut() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", Shut" << std::endl;
    return true;
}

// create object test
void sample3_module::CreateEntityTest() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", CreateEntityTest" << std::endl;

    // create player
    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    std::cout << "Player Name = " << pEntity->get_string(AFEntityMetaPlayer::name()).c_str() << std::endl;

    const guid_t& id = pEntity->get_id();

    // create an item for player
    //auto pItem = m_pKernelModule->CreateContainerEntity(id, BAG, AFConfigMetaItem::self_name(), ITEM_CONFIG_ID);
    //ARK_ASSERT_RET_NONE(pItem != nullptr);

    //pItem->SetInt32(AFConfigMetaItem::level(), 22);
    //std::cout << "Item Config = " << pItem->GetConfigID() << std::endl;
}

void sample3_module::DataTest() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", DataTest" << std::endl;

    // create player
    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    const guid_t& id = pEntity->get_id();
    const size_t count = 5000000;

    QueryStringTest(id, count, 1);
    //QueryStringTest(id, count, 2);
    //QueryStringTest(id, count, 3);

    QueryIntTest(id, count, 1);
    //QueryIntTest(id, count, 2);
    //QueryIntTest(id, count, 3);

    SetDataTest(id, count, 1);
    //SetDataTest(id, count, 2);
    //SetDataTest(id, count, 3);

    CustomDataTest(id, count, 1);
}

void sample3_module::TableTest() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", TableTest" << std::endl;

    // create player
    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    table* pTable = pEntity->find_table(RECORD_NAME);
    ARK_ASSERT_RET_NONE(pTable != nullptr);

    auto pRowData = pTable->add_row(0);
    ARK_ASSERT_RET_NONE(pRowData != nullptr);

    pRowData->set_int32(col_count, 22);
}

void sample3_module::DestroyTest() {
    ARK_LOG_DEBUG("DestroyTest Start...");

    size_t count = 1;
    for (size_t i = 0; i < count; i++)
    {
        auto pEntity = CreateAnPlayerAndInit();
        m_pKernelModule->destroy_entity(pEntity->get_id());
    }

    ARK_LOG_DEBUG("DestroyTest End...");
}

void sample3_module::CalssCallBackTest() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", TableTest" << std::endl;

    m_pKernelModule->add_class_call_back(AFEntityMetaPlayer::self_name(), this, &sample3_module::OnClassCallBackEvent);

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    m_pKernelModule->destroy_entity(pEntity->get_id());
}

void sample3_module::DataCallBackTest() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", TableTest" << std::endl;

    m_pKernelModule->add_node_call_back(
        AFEntityMetaPlayer::self_name(), AFEntityMetaPlayer::gender(), this, &sample3_module::OnDataCallBackEvent);

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    pEntity->set_int32(AFEntityMetaPlayer::gender(), 1);
    pEntity->set_int32(AFEntityMetaPlayer::gender(), 2);
}

void sample3_module::TableCallBackTest() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", TableTest" << std::endl;

    m_pKernelModule->add_table_call_back(
        AFEntityMetaPlayer::self_name(), AFEntityMetaPlayer::items(), this, &sample3_module::OnTableCallBackEvent);

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    table* pTable = pEntity->find_table(RECORD_NAME);
    ARK_ASSERT_RET_NONE(pTable != nullptr);

    auto pRowData = pTable->add_row(0);
    ARK_ASSERT_RET_NONE(pRowData != nullptr);

    pRowData->set_int32(col_count, 22);

    pRowData->set_int32(col_count, 33);
}

void sample3_module::ContainerCallBackTest() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", ContainerTest" << std::endl;

    m_pKernelModule->add_container_call_back(
        AFEntityMetaPlayer::self_name(), BAG, this, &sample3_module::OnContainerCallBackEvent);

    CreateEntityTest();
}

void sample3_module::EventTest() {
    std::cout << GET_CLASS_NAME(sample3_module) << ", TableTest" << std::endl;

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);
}

//-----------------------------inner interface--------------------------------------------
std::shared_ptr<ark::entity> sample3_module::CreateAnPlayerAndInit() {
    // create object
    auto pEntity = m_pKernelModule->create_entity(
        NULL_GUID, MPA_ID, 0, AFEntityMetaPlayer::self_name(), NULL_INT, data_list_impl());
    ARK_ASSERT_RET_VAL(pEntity != nullptr, nullptr);

    // init data
    pEntity->set_string(AFEntityMetaPlayer::name(), "Xiaoming");
    pEntity->set_int32(AFEntityMetaPlayer::gender(), 1);
    pEntity->set_int32(AFEntityMetaPlayer::career(), 1);
    pEntity->set_int32(AFEntityMetaPlayer::camp(), 1);
    pEntity->set_int32(AFEntityMetaPlayer::last_map_id(), 1);
    pEntity->set_int32(AFEntityMetaPlayer::level(), 1);

    table* pTable = pEntity->find_table(RECORD_NAME);
    ARK_ASSERT_RET_VAL(pTable != nullptr, nullptr);

    auto pRow = pTable->add_row(0);
    ARK_ASSERT_RET_VAL(pRow != nullptr, nullptr);
    pRow->set_int64(col_guid, 1111);

    if (!pEntity->find_custom_data("test"))
    {
        pEntity->add_custom_int32("test", 111);
    }

    std::cout << "CreatePlayer name " << pEntity->get_string(AFEntityMetaPlayer::name()).c_str() << " "
              << AFEntityMetaPlayer::gender() << pEntity->get_int32(AFEntityMetaPlayer::gender()) << std::endl;

    return pEntity;
}

void sample3_module::QueryStringTest(const guid_t& guid, size_t count, const uint32_t index) {
    auto pEntity = m_pKernelModule->get_entity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    std::cout << "----------[" << index << "] time [query string] test start----------" << std::endl;

    // query string data with int key
    auto begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto& value = pEntity->get_string(AFEntityMetaPlayer::name_index());
    }
    auto end = std::chrono::system_clock::now().time_since_epoch();
    auto time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // query string data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto& value = pEntity->get_string(AFEntityMetaPlayer::name());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    auto time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] query with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    // query string data with int key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->get_entity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        auto& value = pObj->get_string(AFEntityMetaPlayer::name_index());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // query string data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->get_entity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        auto& value = pObj->get_string(AFEntityMetaPlayer::name());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] find then query with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    std::cout << "----------[" << index << "] time [query string] test end----------" << std::endl;
}

void sample3_module::QueryIntTest(const guid_t& guid, size_t count, const uint32_t index) {
    auto pEntity = m_pKernelModule->get_entity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    std::cout << "----------[" << index << "] time [query int] test start----------" << std::endl;

    // query string data with int key
    auto begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto value = pEntity->get_int32(AFEntityMetaPlayer::gender_index());
    }
    auto end = std::chrono::system_clock::now().time_since_epoch();
    auto time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // query string data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto value = pEntity->get_int32(AFEntityMetaPlayer::gender());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    auto time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] query with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    // query string data with int key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->get_entity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        auto value = pObj->get_int32(AFEntityMetaPlayer::gender_index());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // query string data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->get_entity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        auto value = pObj->get_int32(AFEntityMetaPlayer::gender());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] find then query with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    std::cout << "----------[" << index << "] time [query int] test end----------" << std::endl;
}

void sample3_module::SetDataTest(const guid_t& guid, size_t count, const uint32_t index) {
    auto pEntity = m_pKernelModule->get_entity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    std::cout << "----------[" << index << "] time [set int] test start----------" << std::endl;

    // set data with int key
    auto begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        pEntity->set_int32(AFEntityMetaPlayer::gender_index(), 22);
    }
    auto end = std::chrono::system_clock::now().time_since_epoch();
    auto time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // set data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        pEntity->set_int32(AFEntityMetaPlayer::gender(), 22);
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    auto time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] set with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    // set data with int key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->get_entity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        pObj->set_int32(AFEntityMetaPlayer::gender_index(), 11);
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // set data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->get_entity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        pObj->set_int32(AFEntityMetaPlayer::gender(), 11);
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] find then set with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    std::cout << "----------[" << index << "] time [set int] test end----------" << std::endl;
}

void sample3_module::CustomDataTest(const guid_t& guid, size_t count, const uint32_t index) {
    std::cout << "----------[index] time [custom data] test start----------" << std::endl;

    auto pEntity = m_pKernelModule->get_entity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    if (!pEntity->find_custom_data("test") && !pEntity->add_custom_int32("test", 0))
    {
        return;
    }

    std::cout << "CustomDataTest find test = " << (pEntity->find_custom_data("test") ? "true" : "false")
              << " value = " << pEntity->get_custom_int32("test") << std::endl;

    std::cout << "----------[index] time [custom data] test end----------" << std::endl;
}

int sample3_module::OnClassCallBackEvent(
    const guid_t& self, const std::string& strClassName, const entity_event event, const data_list& arg) {
    std::cout << "---------- OnClassCallBackEvent start----------" << std::endl;

    std::cout << "ClassName: " << strClassName << " ID: " << self << " Event: " << (int)event << std::endl;

    if (event == entity_event::ENTITY_EVT_DATA_FINISHED)
    {
        //
    }

    std::cout << "---------- OnClassCallBackEvent end----------" << std::endl;

    return 0;
}

int sample3_module::OnDataCallBackEvent(const guid_t& self, const std::string& data_name, const uint32_t index,
    const ark::data& old_value, const ark::data& new_value) {
    std::cout << "---------- OnDataCallBackEvent start----------" << std::endl;

    std::cout << "data name: " << data_name << " old value : " << old_value.to_string()
              << " new value : " << new_value.to_string() << std::endl;

    std::cout << "---------- OnDataCallBackEvent end----------" << std::endl;

    return 0;
}

int sample3_module::OnTableCallBackEvent(
    const guid_t& self, const table_event_data& data, const ark::data& old_value, const ark::data& new_value) {
    std::cout << "---------- OnTableCallBackEvent start----------" << std::endl;

    if (data.op_type_ == (size_t)table_op_type::TABLE_ADD)
    {
        std::cout << "table name: " << data.table_name_ << " row : " << data.row_ << std::endl;
    }
    else
    {
        std::cout << "table name: " << data.table_name_ << " row : " << data.row_
                  << " data index : " << data.data_index_ << " old value : " << old_value.to_string()
                  << " new value : " << new_value.to_string() << std::endl;
    }

    std::cout << "---------- OnTableCallBackEvent end----------" << std::endl;

    return 0;
}

int sample3_module::OnContainerCallBackEvent(const guid_t& self, const uint32_t index, const container_op_type op_type,
    const uint32_t src_index, const uint32_t dest_index) {
    std::cout << "---------- OnContainerCallBackEvent start----------" << std::endl;

    std::cout << "container index: " << index << " op : " << (size_t)op_type << " src index : " << src_index
              << " dest index : " << dest_index << std::endl;

    std::cout << "---------- OnContainerCallBackEvent end----------" << std::endl;

    return 0;
}

} // namespace ark
