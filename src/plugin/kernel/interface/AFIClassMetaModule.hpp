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

#pragma once

#include "interface/AFIModule.hpp"
#include "base/AFEnum.hpp"
#include "base/AFDefine.hpp"
#include "kernel/include/AFClassMeta.hpp"
#include "kernel/include/AFClassMetaManager.hpp"

namespace ark {

class AFIClassMetaModule : public AFIModule
{
public:
    virtual bool Load() = 0;

    template<typename BaseType>
    bool AddClassCallBack(const std::string& class_name, BaseType* pBase,
        int (BaseType::*handler)(const AFGUID&, const std::string&, const ArkEntityEvent, const AFIDataList&))
    {
        return AddClassCallBack(
            class_name, std::make_shared<CLASS_EVENT_FUNCTOR>(std::bind(handler, pBase, std::placeholders::_1,
                            std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
    }

    virtual bool AddClassCallBack(const std::string& class_name, CLASS_EVENT_FUNCTOR&& cb) = 0;

    virtual bool DoEvent(
        const AFGUID& id, const std::string& class_name, const ArkEntityEvent class_event, const AFIDataList& args) = 0;

    virtual ARK_SHARE_PTR<AFClassMeta> FindMeta(const std::string& class_name) = 0;

    virtual const AFClassMetaManager::ClassMetaList& GetMetaList() const = 0;
};

} // namespace ark
