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

#include "Sample3Module.h"
#include "base/AFMetaDefine.hpp"
#include "kernel/include/AFCDataList.hpp"
#include "kernel/interface/AFIData.hpp"
#include "kernel/include/AFClassMeta.hpp"
#include "base/AFMetaDefine.hpp"

static const int MPA_ID = 1;
static const size_t ITEM_CONFIG_ID = 100001;
static const std::string RECORD_NAME = ark::AFEntityMetaPlayer::items();
static const uint32_t BAG = ark::AFEntityMetaPlayer::bag_index();
static const uint32_t col_count = ark::AFEntityMetaItemBag::count_index();
static const uint32_t col_guid = ark::AFEntityMetaItemBag::guid_index();

namespace ark {

bool Sample3Module::Init()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", Init" << std::endl;
    return true;
}

bool Sample3Module::PostInit()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", PostInit" << std::endl;

    m_pKernelModule = FindModule<AFIKernelModule>();
    m_pConfigModule = FindModule<AFIConfigModule>();
    m_pLogModule = FindModule<AFILogModule>();
    m_pClassModule = FindModule<AFIClassMetaModule>();
    m_pMapModule = FindModule<AFIMapModule>();

    ARK_LOG_INFO("Init finished...");

    // Create scene, all entity need in scene
    m_pMapModule->CreateMap(MPA_ID);

    DestroyTest();
    //CalssCallBackTest();
    //DataCallBackTest();
    //TableCallBackTest();
    //EventTest();

    CreateEntityTest();
    DataTest();
    TableTest();

    m_pKernelModule->DestroyAll();

    return true;
}

bool Sample3Module::PreShut()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", PreShut" << std::endl;
    m_pKernelModule->DestroyAll();

    return true;
}

bool Sample3Module::Shut()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", Shut" << std::endl;
    return true;
}

// create object test
void Sample3Module::CreateEntityTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", CreateEntityTest" << std::endl;

    // create player
    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    std::cout << "Player Name = " << pEntity->GetString(AFEntityMetaPlayer::name()).c_str() << std::endl;

    const AFGUID& id = pEntity->GetID();

    // create an item for player
    auto pItem = m_pKernelModule->CreateContainerEntity(id, BAG, AFConfigMetaItem::self_name(), ITEM_CONFIG_ID);
    ARK_ASSERT_RET_NONE(pItem != nullptr);

    std::cout << "Item Config = " << pItem->GetConfigID() << std::endl;
}

void Sample3Module::DataTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", DataTest" << std::endl;

    // create player
    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    const AFGUID& id = pEntity->GetID();
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

void Sample3Module::TableTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", TableTest" << std::endl;

    // create player
    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    AFITable* pTable = pEntity->FindTable(RECORD_NAME);
    ARK_ASSERT_RET_NONE(pTable != nullptr);

    auto pRowData = pTable->AddRow(0);
    ARK_ASSERT_RET_NONE(pRowData != nullptr);

    pRowData->SetInt32(col_count, 22);
}

void Sample3Module::DestroyTest()
{
    ARK_LOG_DEBUG("DestroyTest Start...");

    size_t count = 1;
    for (size_t i = 0; i < count; i++)
    {
        auto pEntity = CreateAnPlayerAndInit();
        m_pKernelModule->DestroyEntity(pEntity->GetID());
    }

    ARK_LOG_DEBUG("DestroyTest End...");
}

void Sample3Module::CalssCallBackTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", TableTest" << std::endl;

    m_pKernelModule->AddClassCallBack(AFEntityMetaPlayer::self_name(), this, &Sample3Module::OnClassCallBackEvent);

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    m_pKernelModule->DestroyEntity(pEntity->GetID());
}

void Sample3Module::DataCallBackTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", TableTest" << std::endl;

    m_pKernelModule->AddDataCallBack(
        AFEntityMetaPlayer::self_name(), AFEntityMetaPlayer::gender(), this, &Sample3Module::OnDataCallBackEvent);

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    pEntity->SetInt32(AFEntityMetaPlayer::gender(), 1);
    pEntity->SetInt32(AFEntityMetaPlayer::gender(), 2);
}

void Sample3Module::TableCallBackTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", TableTest" << std::endl;

    m_pKernelModule->AddTableCallBack(
        AFEntityMetaPlayer::self_name(), AFEntityMetaPlayer::items(), this, &Sample3Module::OnTableCallBackEvent);

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    AFITable* pTable = pEntity->FindTable(RECORD_NAME);
    ARK_ASSERT_RET_NONE(pTable != nullptr);

    auto pRowData = pTable->AddRow(0);
    ARK_ASSERT_RET_NONE(pRowData != nullptr);

    pRowData->SetInt32(col_count, 22);

    pRowData->SetInt32(col_count, 33);
}

void Sample3Module::EventTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", TableTest" << std::endl;

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);
}

//-----------------------------inner interface--------------------------------------------
ARK_SHARE_PTR<ark::AFIEntity> Sample3Module::CreateAnPlayerAndInit()
{
    // create object
    auto pEntity =
        m_pKernelModule->CreateEntity(NULL_GUID, MPA_ID, 0, AFEntityMetaPlayer::self_name(), NULL_INT, AFCDataList());
    ARK_ASSERT_RET_VAL(pEntity != nullptr, nullptr);

    // init data
    pEntity->SetString(AFEntityMetaPlayer::name(), "Xiaoming");
    pEntity->SetInt32(AFEntityMetaPlayer::gender(), 1);
    pEntity->SetInt32(AFEntityMetaPlayer::career(), 1);
    pEntity->SetInt32(AFEntityMetaPlayer::camp(), 1);
    pEntity->SetInt32(AFEntityMetaPlayer::last_map_id(), 1);
    pEntity->SetInt32(AFEntityMetaPlayer::level(), 1);

    AFITable* pTable = pEntity->FindTable(RECORD_NAME);
    ARK_ASSERT_RET_VAL(pTable != nullptr, nullptr);

    auto pRow = pTable->AddRow(0);
    ARK_ASSERT_RET_VAL(pRow != nullptr, nullptr);
    pRow->SetInt64(col_guid, 1111);

    if (!pEntity->FindCustomData("test"))
    {
        pEntity->AddCustomInt32("test", 111);
    }

    std::cout << "CreatePlayer name " << pEntity->GetString(AFEntityMetaPlayer::name()).c_str() << " "
              << AFEntityMetaPlayer::gender() << pEntity->GetInt32(AFEntityMetaPlayer::gender()) << std::endl;

    return pEntity;
}

void Sample3Module::QueryStringTest(const AFGUID& guid, size_t count, const uint32_t index)
{
    auto pEntity = m_pKernelModule->GetEntity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    std::cout << "----------[" << index << "] time [query string] test start----------" << std::endl;

    // query string data with int key
    auto begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto& value = pEntity->GetString(AFEntityMetaPlayer::name_index());
    }
    auto end = std::chrono::system_clock::now().time_since_epoch();
    auto time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // query string data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto& value = pEntity->GetString(AFEntityMetaPlayer::name());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    auto time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] query with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    // query string data with int key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->GetEntity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        auto& value = pObj->GetString(AFEntityMetaPlayer::name_index());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // query string data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->GetEntity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        auto& value = pObj->GetString(AFEntityMetaPlayer::name());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] find then query with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    std::cout << "----------[" << index << "] time [query string] test end----------" << std::endl;
}

void Sample3Module::QueryIntTest(const AFGUID& guid, size_t count, const uint32_t index)
{
    auto pEntity = m_pKernelModule->GetEntity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    std::cout << "----------[" << index << "] time [query int] test start----------" << std::endl;

    // query string data with int key
    auto begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto value = pEntity->GetInt32(AFEntityMetaPlayer::gender_index());
    }
    auto end = std::chrono::system_clock::now().time_since_epoch();
    auto time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // query string data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto value = pEntity->GetInt32(AFEntityMetaPlayer::gender());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    auto time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] query with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    // query string data with int key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->GetEntity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        auto value = pObj->GetInt32(AFEntityMetaPlayer::gender_index());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // query string data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->GetEntity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        auto value = pObj->GetInt32(AFEntityMetaPlayer::gender());
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] find then query with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    std::cout << "----------[" << index << "] time [query int] test end----------" << std::endl;
}

void Sample3Module::SetDataTest(const AFGUID& guid, size_t count, const uint32_t index)
{
    auto pEntity = m_pKernelModule->GetEntity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    std::cout << "----------[" << index << "] time [set int] test start----------" << std::endl;

    // set data with int key
    auto begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        pEntity->SetInt32(AFEntityMetaPlayer::gender_index(), 22);
    }
    auto end = std::chrono::system_clock::now().time_since_epoch();
    auto time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // set data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        pEntity->SetInt32(AFEntityMetaPlayer::gender(), 22);
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    auto time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] set with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    // set data with int key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->GetEntity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        pObj->SetInt32(AFEntityMetaPlayer::gender_index(), 11);
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_int_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    // set data with string key
    begin = std::chrono::system_clock::now().time_since_epoch();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->GetEntity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        pObj->SetInt32(AFEntityMetaPlayer::gender(), 11);
    }
    end = std::chrono::system_clock::now().time_since_epoch();
    time_string_key = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    std::cout << "[" << count << "] find then set with [int key] time = " << time_int_key << " ms"
              << " with [string key] time = " << time_string_key << std::endl;

    std::cout << "----------[" << index << "] time [set int] test end----------" << std::endl;
}

void Sample3Module::CustomDataTest(const AFGUID& guid, size_t count, const uint32_t index)
{
    std::cout << "----------[index] time [custom data] test start----------" << std::endl;

    auto pEntity = m_pKernelModule->GetEntity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    if (!pEntity->FindCustomData("test") && !pEntity->AddCustomInt32("test", 0))
    {
        return;
    }

    std::cout << "CustomDataTest find test = " << (pEntity->FindCustomData("test") ? "true" : "false")
              << " value = " << pEntity->GetCustomInt32("test") << std::endl;

    std::cout << "----------[index] time [custom data] test end----------" << std::endl;
}

int Sample3Module::OnClassCallBackEvent(
    const AFGUID& self, const std::string& strClassName, const ArkEntityEvent event, const AFIDataList& arg)
{
    std::cout << "---------- OnClassCallBackEvent start----------" << std::endl;

    std::cout << "ClassName: " << strClassName << " ID: " << self << " Event: " << (int)event << std::endl;

    if (event == ArkEntityEvent::ENTITY_EVT_DATA_FINISHED)
    {
        //
    }

    std::cout << "---------- OnClassCallBackEvent end----------" << std::endl;

    return 0;
}

int Sample3Module::OnDataCallBackEvent(const AFGUID& self, const std::string& data_name, const uint32_t index,
    const AFIData& old_value, const AFIData& new_value)
{
    std::cout << "---------- OnDataCallBackEvent start----------" << std::endl;

    std::cout << "data name: " << data_name << " old value : " << old_value.ToString()
              << " new value : " << new_value.ToString() << std::endl;

    std::cout << "---------- OnDataCallBackEvent end----------" << std::endl;

    return 0;
}

int Sample3Module::OnTableCallBackEvent(
    const AFGUID& self, const TABLE_EVENT_DATA& data, const AFIData& old_value, const AFIData& new_value)
{
    std::cout << "---------- OnTableCallBackEvent start----------" << std::endl;

    if (data.op_type_ == (size_t)ArkTableOpType::TABLE_ADD)
    {
        std::cout << "table name: " << data.table_name_ << " row : " << data.row_ << std::endl;
    }
    else
    {
        std::cout << "table name: " << data.table_name_ << " row : " << data.row_
                  << " data index : " << data.data_index_ << " old value : " << old_value.ToString()
                  << " new value : " << new_value.ToString() << std::endl;
    }

    std::cout << "---------- OnTableCallBackEvent end----------" << std::endl;

    return 0;
}

} // namespace ark
