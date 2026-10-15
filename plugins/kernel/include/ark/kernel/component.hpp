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

#pragma once

#include <type_traits>

#include "ark/define.hpp"
#include "ark/module.hpp"

namespace ark {

class component : public module
{
public:
    component() = delete;

    component(guid_t self, const std::string& strName) {
        mbHasInit = false;
        mbEnable = true;
        mSelf = self;
        mstrName = strName;
    }

    template<typename T>
    std::shared_ptr<T> create_new_instance() {
        std::shared_ptr<component> pComponent = create_new_instance();

        if (nullptr != pComponent)
        {
            if (std::is_base_of_v<component, T>)
            {
                std::shared_ptr<T> pT = std::dynamic_pointer_cast<T>(pComponent);

                if (nullptr != pT)
                {
                    return pT;
                }
            }
        }

        return std::shared_ptr<T>();
    }

    virtual bool set_enable(const bool bEnable) {
        return mbEnable;
    }

    virtual bool enable() {
        return mbEnable;
    }

    virtual bool set_has_init(const bool bEnable) {
        mbHasInit = bEnable;
        return mbHasInit;
    }

    virtual bool has_init() {
        return mbHasInit;
    }

    virtual guid_t self() {
        return NULL_GUID;
    }

    virtual const std::string& get_component_name() const {
        return mstrName;
    }

    virtual int on_a_sync_event(const guid_t& self, const int event, std::string& arg) {
        return 0;
    }

protected:
    virtual std::shared_ptr<component> create_new_instance() {
        return nullptr;
    };

private:
    bool mbEnable;
    bool mbHasInit;
    guid_t mSelf;
    std::string mstrName;
};

} // namespace ark