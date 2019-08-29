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

static const int MPA_ID = 1;
static const size_t ITEM_CONFIG_ID = 100001;
static const std::string RECORD_NAME = "items";
static const std::string BAG = "bag";

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

    // CreateEntityTest();
    // DataTest();
    // TableTest();
    // CalssCallBackTest();
    // DataCallBackTest();
    // TableCallBackTest();
    // EventTest();

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
    std::cout << GET_CLASS_NAME(Sample3Module) << ", CreateEntityTest Start" << std::endl;

    // create player
    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    std::cout << "Player Name = " << pEntity->GetString(AFEntityMetaPlayer::name()).c_str() << std::endl;

    const AFGUID& id = pEntity->GetID();

    // create an item for player
    auto pItem = m_pKernelModule->CreateContainerEntity(id, BAG, AFConfigMetaItem::self_name(), ITEM_CONFIG_ID);
    ARK_ASSERT_RET_NONE(pItem != nullptr);

    std::cout << "Item Config = " << pItem->GetConfigID() << std::endl;

    std::cout << GET_CLASS_NAME(Sample3Module) << ", CreateEntityTest End" << std::endl;
}

void Sample3Module::DataTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", QueryDataTest Start" << std::endl;

    // create player
    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    const AFGUID& id = pEntity->GetID();
    const size_t count = 5000000;

    CustomDataTest(id, count);

    std::cout << GET_CLASS_NAME(Sample3Module) << ", QueryDataTest Start 1" << std::endl;
    QueryDataStringTest(id, count);

    std::cout << GET_CLASS_NAME(Sample3Module) << ", QueryDataTest Start 2" << std::endl;
    QueryDataStringTest(id, count);

    std::cout << GET_CLASS_NAME(Sample3Module) << ", QueryDataTest Start 3" << std::endl;
    QueryDataStringTest(id, count);
    // QueryDataIntTest(id, count);
    // SetDataTest(id, count);

    std::cout << GET_CLASS_NAME(Sample3Module) << ", QueryDataTest Start" << std::endl;
}

void Sample3Module::TableTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", TableTest Start" << std::endl;

    // create player
    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    AFITable* pTable = pEntity->FindTable(RECORD_NAME);
    ARK_ASSERT_RET_NONE(pTable != nullptr);

    auto pRowData = pTable->AddRow(0);
    ARK_ASSERT_RET_NONE(pRowData != nullptr);

    pRowData->SetInt32(AFEntityMetaItemBag::count(), 22);
    std::cout << GET_CLASS_NAME(Sample3Module)
              << ", TableTest value = " << pRowData->GetInt32(AFEntityMetaItemBag::count()) << std::endl;

    std::cout << GET_CLASS_NAME(Sample3Module) << ", TableTest End" << std::endl;
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
    std::cout << GET_CLASS_NAME(Sample3Module) << ", CalssCallBackTest Start" << std::endl;

    m_pKernelModule->AddClassCallBack(AFEntityMetaPlayer::self_name(), this, &Sample3Module::OnClassCallBackEvent);

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    std::cout << GET_CLASS_NAME(Sample3Module) << ", CalssCallBackTest End" << std::endl;
}

void Sample3Module::DataCallBackTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", DataCallBackTest Start" << std::endl;

    m_pKernelModule->AddDataCallBack(
        AFEntityMetaPlayer::self_name(), AFEntityMetaPlayer::gender(), this, &Sample3Module::OnDataCallBackEvent);

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    pEntity->SetInt32(AFEntityMetaPlayer::gender(), 1);
    pEntity->SetInt32(AFEntityMetaPlayer::gender(), 2);

    std::cout << GET_CLASS_NAME(Sample3Module) << ", DataCallBackTest End" << std::endl;
}

void Sample3Module::TableCallBackTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", TableCallBackTest Start" << std::endl;

    m_pKernelModule->AddTableCallBack(
        AFEntityMetaPlayer::self_name(), AFEntityMetaPlayer::items(), this, &Sample3Module::OnTableCallBackEvent);

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    AFITable* pTable = pEntity->FindTable(RECORD_NAME);
    ARK_ASSERT_RET_NONE(pTable != nullptr);

    auto pRowData = pTable->AddRow(0);
    ARK_ASSERT_RET_NONE(pRowData != nullptr);

    pRowData->SetInt32(AFEntityMetaItemBag::count(), 22);

    pRowData->SetInt32(AFEntityMetaItemBag::count(), 33);

    std::cout << GET_CLASS_NAME(Sample3Module) << ", TableCallBackTest End" << std::endl;
}

void Sample3Module::EventTest()
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", EventTest Start" << std::endl;

    auto pEntity = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pEntity != nullptr);

    std::cout << GET_CLASS_NAME(Sample3Module) << ", EventTest End" << std::endl;
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
    pRow->SetInt64("guid", 1111);

    pEntity->AddCustomInt32("test", 111);

    std::cout << "CreatePlayer name " << pEntity->GetString(AFEntityMetaPlayer::name()).c_str() << " gender "
              << pEntity->GetInt32(AFEntityMetaPlayer::gender()) << std::endl;

    ARK_LOG_DEBUG("CreatePlayer...");

    return pEntity;
}

void Sample3Module::QueryDataStringTest(const AFGUID& guid, size_t count)
{
    auto pEntity = m_pKernelModule->GetEntity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    std::cout << " query string, value = " << pEntity->GetString(AFEntityMetaPlayer::name()) << std::endl;

    AFDateTime now;
    int64_t begin = now.GetMilliseconds();
    int64_t end = begin;

    // query string data
    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        auto& value = pEntity->GetString(AFEntityMetaPlayer::name());
    }
    now.update();
    end = now.GetMilliseconds();

    std::cout << count << " query string data time = " << end - begin << std::endl;

    // query string data with find
    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->GetEntity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        auto& value = pObj->GetString(AFEntityMetaPlayer::name());
    }
    now.update();
    end = now.GetMilliseconds();

    std::cout << count << " query string data with find time = " << end - begin << std::endl;
}

void Sample3Module::QueryDataIntTest(const AFGUID& guid, size_t count)
{
    auto pEntity = m_pKernelModule->GetEntity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    std::cout << " query int, value = " << pEntity->GetInt32(AFEntityMetaPlayer::gender()) << std::endl;

    AFDateTime now;
    int64_t begin = now.GetMilliseconds();
    int64_t end = begin;

    // query int data
    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        auto value = pEntity->GetInt32(AFEntityMetaPlayer::gender());
    }
    now.update();
    end = now.GetMilliseconds();

    std::cout << count << " query int data time = " << end - begin << std::endl;

    // query int data with find
    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->GetEntity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        auto value = pObj->GetInt32(AFEntityMetaPlayer::gender());
    }
    now.update();
    end = now.GetMilliseconds();

    std::cout << count << " query int data with find time = " << end - begin << std::endl;
}

void Sample3Module::SetDataTest(const AFGUID& guid, size_t count)
{
    auto pEntity = m_pKernelModule->GetEntity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    std::cout << " set data, old value = " << pEntity->GetString(AFEntityMetaPlayer::name()) << std::endl;

    AFDateTime now;
    int64_t begin = now.GetMilliseconds();
    int64_t end = begin;

    // set data
    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        bool bSuccess = pEntity->SetString(AFEntityMetaPlayer::name(), "Xiaojun");
    }
    now.update();
    end = now.GetMilliseconds();

    std::cout << count << " set data time = " << end - begin << std::endl;

    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->GetEntity(guid);
        if (nullptr == pObj)
        {
            return;
        }

        bool bSuccess = pEntity->SetString(AFEntityMetaPlayer::name(), "Xiaojun");
    }
    now.update();
    end = now.GetMilliseconds();

    std::cout << " set data, new value = " << pEntity->GetString(AFEntityMetaPlayer::name()) << std::endl;

    std::cout << count << " set data with find time = " << end - begin << std::endl;
}

void Sample3Module::CustomDataTest(const AFGUID& guid, size_t count)
{
    std::cout << GET_CLASS_NAME(Sample3Module) << ", CustomDataTest Start" << std::endl;

    auto pEntity = m_pKernelModule->GetEntity(guid);
    if (nullptr == pEntity)
    {
        return;
    }

    if (!pEntity->AddCustomInt32("test", 0))
    {
        return;
    }

    std::cout << GET_CLASS_NAME(Sample3Module)
              << ", CustomDataTest find test = " << (pEntity->FindCustomData("test") ? "true" : "false") << std::endl;

    std::cout << GET_CLASS_NAME(Sample3Module) << ", CustomDataTest test = " << pEntity->GetCustomInt32("test")
              << std::endl;

    pEntity->RemoveCustomData("test");

    std::cout << GET_CLASS_NAME(Sample3Module)
              << ", CustomDataTest find test = " << (pEntity->FindCustomData("test") ? "true" : "false") << std::endl;

    std::cout << GET_CLASS_NAME(Sample3Module) << ", CustomDataTest End" << std::endl;
}

int Sample3Module::OnClassCallBackEvent(
    const AFGUID& self, const std::string& strClassName, const ArkEntityEvent event, const AFIDataList& arg)
{
    std::cout << "OnClassCallBackEvent ClassName: " << strClassName << " ID: " << self << " Event: " << (int)event
              << std::endl;

    if (event == ArkEntityEvent::ENTITY_EVT_DATA_FINISHED)
    {
        //
    }

    return 0;
}

int Sample3Module::OnDataCallBackEvent(
    const AFGUID& self, const std::string& data_name, const AFIData& old_value, const AFIData& new_value)
{
    std::cout << "OnDataCallBackEvent data name: " << data_name << " old value : " << old_value.GetInt()
              << " new value : " << new_value.GetInt() << std::endl;

    return 0;
}

int Sample3Module::OnTableCallBackEvent(
    const AFGUID& self, const DATA_RECORD_EVENT_DATA& data, const AFIData& old_value, const AFIData& new_value)
{
    if (data.op_type_ == ArkTableOpType::TABLE_ADD)
    {
        std::cout << "OnTableCallBackEvent table name: " << data.table_name_ << " row : " << data.row_ << std::endl;
    }
    else
    {
        std::cout << "OnTableCallBackEvent table name: " << data.table_name_ << " row : " << data.row_
                  << " data name : " << data.data_name_ << " old value : " << old_value.GetInt()
                  << " new value : " << new_value.GetInt() << std::endl;
    }

    return 0;
}

} // namespace ark
