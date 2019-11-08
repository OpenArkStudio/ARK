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

#include "base/AFDateTime.hpp"
#include "game/include/AFCAttributeModule.hpp"

namespace ark {

bool AFCAttributeModule::Init()
{
    m_pKernelModule = FindModule<AFIKernelModule>();
    m_pPropertyConfigModule = FindModule<AFIPropertyConfigModule>();
    m_pLevelModule = FindModule<AFILevelModule>();

    m_pKernelModule->AddClassCallBack(AFEntityMetaPlayer::self_name(), this, &AFCAttributeModule::OnObjectClassEvent);
    m_pKernelModule->AddNodeCallBack(
        AFEntityMetaPlayer::self_name(), AFEntityMetaPlayer::level(), this, &AFCAttributeModule::OnObjectLevelEvent);
    // m_pClassModule->AddTableCallBack(AFEntityMetaPlayer::self_name(), ark::Player::R_CommPropertyValue(), this,
    // &AFCPropertyModule::OnPropertyTableEvent);

    return true;
}

int AFCAttributeModule::GetAttributeValue(
    const AFGUID& self, const std::string& attr_name, const AttributeGroup eGroupType)
{
    if (AttributeGroup::APG_ALL != eGroupType)
    {
        // return m_pKernelModule->GetTableInt(self, ark::Player::R_CommPropertyValue(), eGroupType,
        // PropertyNameToCol(strPropertyName));
    }

    auto pEntity = m_pKernelModule->GetEntity(self);
    if (pEntity == nullptr)
    {
        return 0;
    }

    return pEntity->GetInt32(attr_name);
}

int AFCAttributeModule::SetAttributeValue(
    const AFGUID& self, const std::string& attr_name, const AttributeGroup attr_type, const int32_t value)
{
    // if (AttributeGroup::APG_ALL != eGroupType)
    //{
    //    std::shared_ptr<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

    //    if (pEntity != nullptr)
    //    {
    //        AFDataTable* pTable = m_pKernelModule->FindTable(self, ark::Player::R_CommPropertyValue());

    //        if (pTable != nullptr)
    //        {
    //            return pTable->SetInt(eGroupType, PropertyNameToCol(attr_name), value);
    //        }
    //    }
    //}

    // m_pKernelModule->SetNodeInt(self, attr_name, value);

    return 0;
}

int AFCAttributeModule::AddAttributeValue(
    const AFGUID& self, const std::string& attr_name, const AttributeGroup attr_type, const int32_t value)
{
    // if (AttributeGroup::APG_ALL != eGroupType)
    //{
    //    std::shared_ptr<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

    //    if (nullptr != pEntity)
    //    {
    //        AFDataTable* pTable = m_pKernelModule->FindTable(self, ark::Player::R_CommPropertyValue());

    //        if (nullptr != pTable)
    //        {
    //            int nPropertyValue = pTable->GetInt(eGroupType, PropertyNameToCol(strPropertyName));
    //            return pTable->SetInt(eGroupType, PropertyNameToCol(strPropertyName), nPropertyValue + value);
    //        }
    //    }
    //}

    return 0;
}

int AFCAttributeModule::SubAttributeValue(
    const AFGUID& self, const std::string& attr_name, const AttributeGroup attr_type, const int32_t value)
{
    // if (AttributeGroup::APG_ALL != eGroupType)
    //{
    //    std::shared_ptr<AFIEntity> pEntity = m_pKernelModule->GetEntity(self);

    //    if (nullptr != pEntity)
    //    {
    //        AFDataTable* pTable = m_pKernelModule->FindTable(self, ark::Player::R_CommPropertyValue());

    //        if (nullptr != pTable)
    //        {
    //            int nPropertyValue = pTable->GetInt(eGroupType, PropertyNameToCol(strPropertyName));

    //            return pTable->SetInt(eGroupType, PropertyNameToCol(strPropertyName), nPropertyValue - value);
    //        }
    //    }
    //}

    return 0;
}

int AFCAttributeModule::OnObjectLevelEvent(const AFGUID& self, const std::string& attr_name, const uint32_t index,
    const AFIData& old_var, const AFIData& new_var)
{
    RefreshBaseAttribute(self);

    FullHPMP(self);

    return 0;
}

int AFCAttributeModule::OnAttributeTableEvent(
    const AFGUID& self, const TABLE_EVENT_DATA& event_data, const AFIData& old_var, const AFIData& new_var)
{
    // const int nOpType = xEventData.nOpType;
    // const int nRow = xEventData.nRow;
    // const int nCol = xEventData.nCol;

    // int nAllValue = 0;
    // AFDataTable* pTable = m_pKernelModule->FindTable(self, ark::Player::R_CommPropertyValue());

    // for (size_t i = 0; i < (size_t)(AFIPropertyModule::APG_ALL) && i < pTable->GetRowCount(); ++i)
    //{
    //    int value = pTable->GetInt(i, nCol);
    //    nAllValue += value;
    //}

    // m_pKernelModule->SetNodeInt(self, ColToPropertyName(nCol), nAllValue);

    return 0;
}

const std::string& AFCAttributeModule::ColToAttributeName(const size_t nCol)
{
    if (nCol > col2names_.size())
    {
        return NULL_STR;
    }

    return col2names_[nCol];
}

int AFCAttributeModule::AttributeNameToCol(const std::string& strPropertyName)
{
    auto iter = name2col_.find(strPropertyName);

    if (iter != name2col_.end())
    {
        return iter->second;
    }

    return -1;
}

int AFCAttributeModule::OnObjectClassEvent(
    const AFGUID& self, const std::string& strClassName, const ArkEntityEvent eClassEvent, const AFIDataList& var)
{
    // if (ArkEntityEvent::ENTITY_EVT_PRE_LOAD_DATA == eClassEvent)
    //{
    //    AFDataTable* pTable = m_pKernelModule->FindTable(self, ark::Player::R_CommPropertyValue());

    //    if (nullptr != pTable)
    //    {
    //        for (int i = 0; i < APG_ALL; i++)
    //        {
    //            pTable->AddRow();
    //        }
    //    }

    //}
    // else if (ArkEntityEvent::ENTITY_EVT_EFFECT_DATA == eClassEvent)
    //{
    //    int nOnlineCount = m_pKernelModule->GetNodeInt(self, ark::Player::OnlineCount());

    //    if (nOnlineCount <= 0 && m_pKernelModule->GetNodeInt(self, ark::Player::MapID()) > 0)
    //    {
    //        // first create, set the level to 1
    //        m_pKernelModule->SetNodeInt(self, ark::Player::Level(), 1);
    //    }
    //}
    // else if (ArkEntityEvent::ENTITY_EVT_ALL_FINISHED == eClassEvent)
    //{
    //    int nOnlineCount = m_pKernelModule->GetNodeInt(self, ark::Player::OnlineCount());
    //    m_pKernelModule->SetNodeInt(self, ark::Player::OnlineCount(), (nOnlineCount + 1));
    //}

    return 0;
}

int AFCAttributeModule::RefreshBaseAttribute(const AFGUID& self)
{
    // AFDataTable* pTable = m_pKernelModule->FindTable(self, ark::Player::R_CommPropertyValue());

    // if (nullptr == pTable)
    //{
    //    return 1;
    //}

    //// base attribute + level attribute of career
    // int eJobType = m_pKernelModule->GetNodeInt(self, ark::Player::Career());
    // int nLevel = m_pKernelModule->GetNodeInt(self, ark::Player::Level());

    // for (size_t i = 0; i < pTable->GetColCount(); ++i)
    //{
    //    const std::string& strPropertyName = ColToPropertyName(i);
    //    int value = m_pPropertyConfigModule->CalculateBaseValue(eJobType, nLevel, strPropertyName);
    //    SetPropertyValue(self, strPropertyName, ArkPropertyGroup::APG_JOBLEVEL, value);
    //}

    return 0;
}

bool AFCAttributeModule::FullHPMP(const AFGUID& self)
{
    // int32_t nMaxHP = m_pKernelModule->GetNodeInt(self, ark::Player::HP());

    // if (nMaxHP > 0)
    //{
    //    m_pKernelModule->SetNodeInt(self, ark::Player::HP(), nMaxHP);
    //}

    // int32_t nMaxMP = m_pKernelModule->GetNodeInt(self, ark::Player::MP());

    // if (nMaxMP > 0)
    //{
    //    m_pKernelModule->SetNodeInt(self, ark::Player::MP(), nMaxMP);
    //}

    return true;
}

bool AFCAttributeModule::AddHP(const AFGUID& self, const int32_t& value)
{
    // if (value <= 0)
    //{
    //    return false;
    //}

    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, AFEntityMetaPlayer::hp());
    // int32_t nMaxValue = m_pKernelModule->GetNodeInt(self, AFEntityMetaPlayer::HP());

    // if (nCurValue > 0)
    //{
    //    nCurValue += value;

    //    if (nCurValue > nMaxValue)
    //    {
    //        nCurValue = nMaxValue;
    //    }

    //    m_pKernelModule->SetNodeInt(self, ark::Player::HP(), nCurValue);
    //}

    return true;
}

bool AFCAttributeModule::EnoughHP(const AFGUID& self, const int32_t& value)
{
    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, ark::Player::HP());

    // if ((nCurValue > 0) && (nCurValue - value >= 0))
    //{
    //    return true;
    //}

    return false;
}

bool AFCAttributeModule::ConsumeHP(const AFGUID& self, const int32_t& value)
{
    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, ark::Player::HP());

    // if ((nCurValue > 0) && (nCurValue - value >= 0))
    //{
    //    nCurValue -= value;
    //    m_pKernelModule->SetNodeInt(self, ark::Player::HP(), nCurValue);

    //    return true;
    //}

    return false;
}

bool AFCAttributeModule::AddMP(const AFGUID& self, const int32_t& value)
{
    // if (value <= 0)
    //{
    //    return false;
    //}

    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, ark::Player::MP());
    // int32_t nMaxValue = m_pKernelModule->GetNodeInt(self, ark::Player::MP());

    // nCurValue += value;

    // if (nCurValue > nMaxValue)
    //{
    //    nCurValue = nMaxValue;
    //}

    // m_pKernelModule->SetNodeInt(self, ark::Player::MP(), nCurValue);

    return true;
}

bool AFCAttributeModule::ConsumeMP(const AFGUID& self, const int32_t& value)
{
    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, ark::Player::MP());

    // if ((nCurValue > 0) && (nCurValue - value >= 0))
    //{
    //    nCurValue -= value;
    //    m_pKernelModule->SetNodeInt(self, ark::Player::MP(), nCurValue);

    //    return true;
    //}

    return false;
}

bool AFCAttributeModule::EnoughMP(const AFGUID& self, const int32_t& value)
{
    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, ark::Player::MP());

    // if ((nCurValue > 0) && (nCurValue - value >= 0))
    //{
    //    return true;
    //}

    return false;
}

bool AFCAttributeModule::AddMoney(const AFGUID& self, const int32_t& value)
{
    if (value <= 0)
    {
        return false;
    }

    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, ark::Player::Gold());
    // nCurValue += value;
    // m_pKernelModule->SetNodeInt(self, ark::Player::Gold(), nCurValue);

    return false;
}

bool AFCAttributeModule::ConsumeMoney(const AFGUID& self, const int32_t& value)
{
    if (value <= 0)
    {
        return false;
    }

    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, ark::Player::Gold());
    // nCurValue -= value;

    // if (nCurValue >= 0)
    //{
    //    m_pKernelModule->SetNodeInt(self, ark::Player::Gold(), nCurValue);

    //    return true;
    //}

    return false;
}

bool AFCAttributeModule::EnoughMoney(const AFGUID& self, const int32_t& value)
{
    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, ark::Player::Gold());

    // if ((nCurValue > 0) && (nCurValue - value >= 0))
    //{
    //    return true;
    //}

    return false;
}

bool AFCAttributeModule::AddDiamond(const AFGUID& self, const int32_t& value)
{
    // if (value <= 0)
    //{
    //    return false;
    //}

    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, ark::Player::Money());
    // nCurValue += value;
    // m_pKernelModule->SetNodeInt(self, ark::Player::Money(), nCurValue);

    return false;
}

bool AFCAttributeModule::ConsumeDiamond(const AFGUID& self, const int32_t& value)
{
    // if (value <= 0)
    //{
    //    return false;
    //}

    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, ark::Player::Money());
    // nCurValue -= value;

    // if (nCurValue >= 0)
    //{
    //    m_pKernelModule->SetNodeInt(self, ark::Player::Money(), nCurValue);

    //    return true;
    //}

    return false;
}

bool AFCAttributeModule::EnoughDiamond(const AFGUID& self, const int32_t& value)
{
    // int32_t nCurValue = m_pKernelModule->GetNodeInt(self, ark::Player::Money());
    // if ((nCurValue > 0) && (nCurValue - value >= 0))
    //{
    //    return true;
    //}

    return false;
}

} // namespace ark
