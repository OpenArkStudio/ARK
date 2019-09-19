/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2019 ArkNX authors.
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

#include "kernel/include/AFTableMeta.hpp"
#include "kernel/include/AFNodeMeta.hpp"

namespace ark {

AFTableMeta::AFTableMeta(const std::string& name, const uint32_t index)
    : name_(name)
    , index_(index)
{
    //
}

const std::string& AFTableMeta::GetName() const
{
    return name_;
}

size_t AFTableMeta::GetColCount() const
{
    return class_meta_->GetNodeCount();
}

void AFTableMeta::SetClassMeta(ARK_SHARE_PTR<AFClassMeta> pClassMeta)
{
    class_meta_ = pClassMeta;
}

ARK_SHARE_PTR<AFNodeMeta> AFTableMeta::FindMeta(const uint32_t index)
{
    ARK_ASSERT_RET_VAL(class_meta_ != nullptr, nullptr);

    return class_meta_->FindDataMeta(index);
}

ARK_SHARE_PTR<AFClassMeta> AFTableMeta::GetClassMeta() const
{
    return class_meta_;
}

ArkDataType AFTableMeta::GetColType(const uint32_t index)
{
    auto pMeta = class_meta_->FindDataMeta(index);
    ARK_ASSERT_RET_VAL(pMeta != nullptr, ArkDataType::DT_EMPTY);

    return pMeta->GetType();
}

const AFFeatureType AFTableMeta::GetFeature() const
{
    return feature_;
}

bool AFTableMeta::HaveFeature(const ArkTableNodeFeature feature) const
{
    return feature_.test((size_t)feature);
}

void AFTableMeta::SetFeature(const AFFeatureType& feature)
{
    feature_ = feature;
}

bool AFTableMeta::IsPublic() const
{
    return feature_.test((size_t)ArkTableNodeFeature::PF_PUBLIC);
}

bool AFTableMeta::IsPrivate() const
{
    return feature_.test((size_t)ArkTableNodeFeature::PF_PRIVATE);
}

bool AFTableMeta::IsRealTime() const
{
    return feature_.test((size_t)ArkTableNodeFeature::PF_REAL_TIME);
}

bool AFTableMeta::IsSave() const
{
    return feature_.test((size_t)ArkTableNodeFeature::PF_SAVE);
}

void AFTableMeta::SetTypeName(const std::string& value)
{
    type_name_ = value;
}

const std::string& AFTableMeta::GetTypeName() const
{
    return type_name_;
}

uint32_t AFTableMeta::GetIndex() const
{
    return index_;
}

uint32_t AFTableMeta::GetIndex(const std::string& name)
{
    return class_meta_->GetIndex(name);
}

} // namespace ark
