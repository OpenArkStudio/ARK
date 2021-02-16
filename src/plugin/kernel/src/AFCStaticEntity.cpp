/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
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

#include "kernel/include/AFCStaticEntity.hpp"
#include "kernel/include/AFCNode.hpp"
#include "kernel/include/AFCTable.hpp"
#include "kernel/include/AFClassMeta.hpp"

namespace ark {

AFCStaticEntity::AFCStaticEntity(
    std::shared_ptr<AFClassMeta> pClassMeta, std::shared_ptr<AFNodeManager> pNodeManager, const ID_TYPE config_id)
    : config_id_(config_id)
{
    class_meta_ = pClassMeta;
    m_pNodeManager = pNodeManager;
}

// query data
const std::string& AFCStaticEntity::GetClassName() const
{
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, NULL_STR);

    return class_meta_->GetName();
}

ID_TYPE AFCStaticEntity::GetConfigID() const
{
    return config_id_;
}

bool AFCStaticEntity::GetBool(const std::string& name) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_BOOLEAN);

    return m_pNodeManager->GetBool(name);
}

int32_t AFCStaticEntity::GetInt32(const std::string& name) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->GetInt32(name);
}

uint32_t AFCStaticEntity::GetUInt32(const std::string& name) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->GetUInt32(name);
}

int64_t AFCStaticEntity::GetInt64(const std::string& name) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->GetInt64(name);
}

int64_t AFCStaticEntity::GetUInt64(const std::string& name) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->GetUInt64(name);
}

float AFCStaticEntity::GetFloat(const std::string& name) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_FLOAT);

    return m_pNodeManager->GetFloat(name);
}

double AFCStaticEntity::GetDouble(const std::string& name) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_DOUBLE);

    return m_pNodeManager->GetDouble(name);
}

const std::string& AFCStaticEntity::GetString(const std::string& name) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_STR);

    return m_pNodeManager->GetString(name);
}

const std::wstring& AFCStaticEntity::GetWString(const std::string& name) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_WIDESTR);

    return m_pNodeManager->GetWString(name);
}

bool AFCStaticEntity::GetBool(const uint32_t index) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_BOOLEAN);

    return m_pNodeManager->GetBool(index);
}

int32_t AFCStaticEntity::GetInt32(const uint32_t index) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->GetInt32(index);
}

uint32_t AFCStaticEntity::GetUInt32(const uint32_t index) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT);

    return m_pNodeManager->GetUInt32(index);
}

int64_t AFCStaticEntity::GetInt64(const uint32_t index) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->GetInt64(index);
}

int64_t AFCStaticEntity::GetUInt64(const uint32_t index) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_INT64);

    return m_pNodeManager->GetUInt64(index);
}

float AFCStaticEntity::GetFloat(const uint32_t index) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_FLOAT);

    return m_pNodeManager->GetFloat(index);
}

double AFCStaticEntity::GetDouble(const uint32_t index) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_DOUBLE);

    return m_pNodeManager->GetDouble(index);
}

const std::string& AFCStaticEntity::GetString(const uint32_t index) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_STR);

    return m_pNodeManager->GetString(index);
}

const std::wstring& AFCStaticEntity::GetWString(const uint32_t index) const
{
    ARK_ASSERT_RET_VAL(m_pNodeManager != nullptr, NULL_WIDESTR);

    return m_pNodeManager->GetWString(index);
}

std::shared_ptr<AFNodeManager> AFCStaticEntity::GetNodeManager() const
{
    return m_pNodeManager;
}

} // namespace ark
