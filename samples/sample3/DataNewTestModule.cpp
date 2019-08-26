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

#include "DataNewTestModule.h"
#include "base/AFMetaDefine.hpp"
#include "kernel/include/AFCDataList.hpp"
#include "kernel/interface/AFIDataNewType.hpp"
#include "kernel/include/AFClassMeta.hpp"

static const int MPA_ID = 1;
static const size_t ITEM_CONFIG_ID = 100001;
static const std::string RECORD_NAME = "items";
static const std::string BAG = "bag";

namespace ark {

bool DataNewTestModule::Init()
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", Init" << std::endl;
    return true;
}

bool DataNewTestModule::PostInit()
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", PostInit" << std::endl;

    m_pKernelModule = FindModule<AFIKernelNewModule>();
    m_pConfigModule = FindModule<AFIConfigNewModule>();
    m_pLogModule = FindModule<AFILogModule>();
    m_pClassModule = FindModule<AFIClassNewMetaModule>();
    m_pMapModule = FindModule<AFIMapModule>();

    ARK_LOG_INFO("Init finished...");

    // Create scene, all entity need in scene
    m_pMapModule->CreateMap(MPA_ID);

    ARK_LOG_DEBUG("DestroyTest...");
    DestroyTest();

    // CreateObjectTest();
    // DataTest();
    // RecordTest();
    // CalssCallBackTest();
    // DataCallBackTest();
    // RecordCallBackTest();
    // EventTest();

    return true;
}

bool DataNewTestModule::PreShut()
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", PreShut" << std::endl;
    m_pKernelModule->DestroyAll();

    return true;
}

bool DataNewTestModule::Shut()
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", Shut" << std::endl;
    return true;
}

// create object test
void DataNewTestModule::CreateObjectTest()
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", CreateObjectTest Start" << std::endl;

    // create player
    auto pObject = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pObject != nullptr);

    std::cout << "Player Name = " << pObject->GetString(AFEntityMetaPlayer::name()).c_str() << std::endl;

    const AFGUID& id = pObject->GetID();

    // create an item for player
    auto pItem = m_pKernelModule->CreateContainerObject(id, BAG, AFConfigMetaItem::self_name(), ITEM_CONFIG_ID);
    ARK_ASSERT_RET_NONE(pItem != nullptr);

    std::cout << "Item Config = " << pItem->GetConfigID() << std::endl;

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", CreateObjectTest End" << std::endl;
}

void DataNewTestModule::DataTest()
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", QueryDataTest Start" << std::endl;

    // create player
    auto pObject = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pObject != nullptr);

    const AFGUID& id = pObject->GetID();
    const size_t count = 5000000;

    CustomDataTest(id, count);

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", QueryDataTest Start 1" << std::endl;
    QueryDataStringTest(id, count);

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", QueryDataTest Start 2" << std::endl;
    QueryDataStringTest(id, count);

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", QueryDataTest Start 3" << std::endl;
    QueryDataStringTest(id, count);
    // QueryDataIntTest(id, count);
    // SetDataTest(id, count);

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", QueryDataTest Start" << std::endl;
}

void DataNewTestModule::RecordTest()
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", RecordTest Start" << std::endl;

    // create player
    auto pObject = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pObject != nullptr);

    AFIRecord* pRecord = pObject->FindRecord(RECORD_NAME);
    ARK_ASSERT_RET_NONE(pRecord != nullptr);

    auto pRowData = pRecord->AddRow(0);
    ARK_ASSERT_RET_NONE(pRowData != nullptr);

    pRowData->SetInt32(AFEntityMetaItemBag::count(), 22);
    std::cout << GET_CLASS_NAME(DataNewTestModule)
              << ", RecordTest value = " << pRowData->GetInt32(AFEntityMetaItemBag::count()) << std::endl;

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", RecordTest End" << std::endl;
}

void DataNewTestModule::DestroyTest()
{
    ARK_LOG_DEBUG("DestroyTest Start...");

    size_t count = 1;
    for (size_t i = 0; i < count; i++)
    {
        auto pObject = CreateAnPlayerAndInit();
        m_pKernelModule->DestroyEntity(pObject->GetID());
    }

    ARK_LOG_DEBUG("DestroyTest End...");
}

void DataNewTestModule::CalssCallBackTest()
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", CalssCallBackTest Start" << std::endl;

    m_pKernelModule->AddClassCallBack(AFEntityMetaPlayer::self_name(), this, &DataNewTestModule::OnClassCallBackEvent);

    auto pObject = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pObject != nullptr);

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", CalssCallBackTest End" << std::endl;
}

void DataNewTestModule::DataCallBackTest()
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", DataCallBackTest Start" << std::endl;

    m_pKernelModule->AddDataCallBack(
        AFEntityMetaPlayer::self_name(), AFEntityMetaPlayer::gender(), this, &DataNewTestModule::OnDataCallBackEvent);

    auto pObject = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pObject != nullptr);

    pObject->SetInt32(AFEntityMetaPlayer::gender(), 1);
    pObject->SetInt32(AFEntityMetaPlayer::gender(), 2);

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", DataCallBackTest End" << std::endl;
}

void DataNewTestModule::RecordCallBackTest()
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", RecordCallBackTest Start" << std::endl;

    m_pKernelModule->AddRecordCallBack(
        AFEntityMetaPlayer::self_name(), AFEntityMetaPlayer::items(), this, &DataNewTestModule::OnRecordCallBackEvent);

    auto pObject = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pObject != nullptr);

    AFIRecord* pRecord = pObject->FindRecord(RECORD_NAME);
    ARK_ASSERT_RET_NONE(pRecord != nullptr);

    auto pRowData = pRecord->AddRow(0);
    ARK_ASSERT_RET_NONE(pRowData != nullptr);

    pRowData->SetInt32(AFEntityMetaItemBag::count(), 22);

    pRowData->SetInt32(AFEntityMetaItemBag::count(), 33);

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", RecordCallBackTest End" << std::endl;
}

void DataNewTestModule::EventTest()
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", EventTest Start" << std::endl;

    auto pObject = CreateAnPlayerAndInit();
    ARK_ASSERT_RET_NONE(pObject != nullptr);

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", EventTest End" << std::endl;
}

//-----------------------------inner interface--------------------------------------------
ARK_SHARE_PTR<ark::AFIObject> DataNewTestModule::CreateAnPlayerAndInit()
{
    // create object
    auto pObject =
        m_pKernelModule->CreateObject(NULL_GUID, MPA_ID, 0, AFEntityMetaPlayer::self_name(), NULL_INT, AFCDataList());
    ARK_ASSERT_RET_VAL(pObject != nullptr, nullptr);

    // init data
    pObject->SetString(AFEntityMetaPlayer::name(), "Xiaoming");
    pObject->SetInt32(AFEntityMetaPlayer::gender(), 1);
    pObject->SetInt32(AFEntityMetaPlayer::career(), 1);
    pObject->SetInt32(AFEntityMetaPlayer::camp(), 1);
    pObject->SetInt32(AFEntityMetaPlayer::last_map_id(), 1);
    pObject->SetInt32(AFEntityMetaPlayer::level(), 1);

    AFIRecord* pRecord = pObject->FindRecord(RECORD_NAME);
    ARK_ASSERT_RET_VAL(pRecord != nullptr, nullptr);

    auto pRow = pRecord->AddRow(0);
    ARK_ASSERT_RET_VAL(pRow != nullptr, nullptr);
    pRow->SetInt64("guid", 1111);

    pObject->AddCustomInt32("test", 111);

    std::cout << "CreatePlayer name " << pObject->GetString(AFEntityMetaPlayer::name()).c_str() << " gender "
              << pObject->GetInt32(AFEntityMetaPlayer::gender()) << std::endl;

    ARK_LOG_DEBUG("CreatePlayer...");

    return pObject;
}

void DataNewTestModule::QueryDataStringTest(const AFGUID& guid, size_t count)
{
    auto pObject = m_pKernelModule->FindObject(guid);
    if (nullptr == pObject)
    {
        return;
    }

    std::cout << " query string, value = " << pObject->GetString(AFEntityMetaPlayer::name()) << std::endl;

    AFDateTime now;
    int64_t begin = now.GetMilliseconds();
    int64_t end = begin;

    // query string data
    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        auto& value = pObject->GetString(AFEntityMetaPlayer::name());
    }
    now.update();
    end = now.GetMilliseconds();

    std::cout << count << " query string data time = " << end - begin << std::endl;

    // query string data with find
    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->FindObject(guid);
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

void DataNewTestModule::QueryDataIntTest(const AFGUID& guid, size_t count)
{
    auto pObject = m_pKernelModule->FindObject(guid);
    if (nullptr == pObject)
    {
        return;
    }

    std::cout << " query int, value = " << pObject->GetInt32(AFEntityMetaPlayer::gender()) << std::endl;

    AFDateTime now;
    int64_t begin = now.GetMilliseconds();
    int64_t end = begin;

    // query int data
    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        auto value = pObject->GetInt32(AFEntityMetaPlayer::gender());
    }
    now.update();
    end = now.GetMilliseconds();

    std::cout << count << " query int data time = " << end - begin << std::endl;

    // query int data with find
    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->FindObject(guid);
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

void DataNewTestModule::SetDataTest(const AFGUID& guid, size_t count)
{
    auto pObject = m_pKernelModule->FindObject(guid);
    if (nullptr == pObject)
    {
        return;
    }

    std::cout << " set data, old value = " << pObject->GetString(AFEntityMetaPlayer::name()) << std::endl;

    AFDateTime now;
    int64_t begin = now.GetMilliseconds();
    int64_t end = begin;

    // set data
    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        bool bSuccess = pObject->SetString(AFEntityMetaPlayer::name(), "Xiaojun");
    }
    now.update();
    end = now.GetMilliseconds();

    std::cout << count << " set data time = " << end - begin << std::endl;

    now.update();
    begin = now.GetMilliseconds();
    for (size_t i = 0; i < count; i++)
    {
        auto pObj = m_pKernelModule->FindObject(guid);
        if (nullptr == pObj)
        {
            return;
        }

        bool bSuccess = pObject->SetString(AFEntityMetaPlayer::name(), "Xiaojun");
    }
    now.update();
    end = now.GetMilliseconds();

    std::cout << " set data, new value = " << pObject->GetString(AFEntityMetaPlayer::name()) << std::endl;

    std::cout << count << " set data with find time = " << end - begin << std::endl;
}

void DataNewTestModule::CustomDataTest(const AFGUID& guid, size_t count)
{
    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", CustomDataTest Start" << std::endl;

    auto pObject = m_pKernelModule->FindObject(guid);
    if (nullptr == pObject)
    {
        return;
    }

    if (!pObject->AddCustomInt32("test", 0))
    {
        return;
    }

    std::cout << GET_CLASS_NAME(DataNewTestModule)
              << ", CustomDataTest find test = " << (pObject->FindCustomData("test") ? "true" : "false") << std::endl;

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", CustomDataTest test = " << pObject->GetCustomInt32("test")
              << std::endl;

    pObject->RemoveCustomData("test");

    std::cout << GET_CLASS_NAME(DataNewTestModule)
              << ", CustomDataTest find test = " << (pObject->FindCustomData("test") ? "true" : "false") << std::endl;

    std::cout << GET_CLASS_NAME(DataNewTestModule) << ", CustomDataTest End" << std::endl;
}

int DataNewTestModule::OnClassCallBackEvent(
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

int DataNewTestModule::OnDataCallBackEvent(
    const AFGUID& self, const std::string& data_name, const AFIDataNewType& old_value, const AFIDataNewType& new_value)
{
    std::cout << "OnDataCallBackEvent data name: " << data_name << " old value : " << old_value.GetInt()
              << " new value : " << new_value.GetInt() << std::endl;

    return 0;
}

int DataNewTestModule::OnRecordCallBackEvent(const AFGUID& self, const DATA_RECORD_EVENT_DATA& data,
    const AFIDataNewType& old_value, const AFIDataNewType& new_value)
{
    if (data.op_type_ == ArkTableOpType::TABLE_ADD)
    {
        std::cout << "OnRecordCallBackEvent record name: " << data.record_name_ << " row : " << data.row_ << std::endl;
    }
    else
    {
        std::cout << "OnRecordCallBackEvent record name: " << data.record_name_ << " row : " << data.row_
                  << " data name : " << data.data_name_ << " old value : " << old_value.GetInt()
                  << " new value : " << new_value.GetInt() << std::endl;
    }

    return 0;
}

} // namespace ark
