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

#include "base/AFMacros.hpp"
#include "base/AFMap.hpp"
#include "base/AFDefine.hpp"
#include "base/AFEnum.hpp"
#include "AFDataNewMeta.hpp"
#include "AFRecordMeta.hpp"
#include "AFClassCallBackManager.hpp"

namespace ark {

class AFClassMeta final
{

public:
    using DataMetaList = AFNewSmartPtrHashmap<std::string, AFDataNewMeta>;
    using RecordMetaList = AFNewSmartPtrHashmap<std::string, AFRecordMeta>;
    using ClassList = std::vector<std::string>;
    using ContainerList = std::map<std::string, std::string>;
    using ContainerMetaList = AFNewSmartPtrHashmap<std::string, AFClassMeta>;

    AFClassMeta() = delete;

    explicit AFClassMeta(const std::string& name)
        : name_(name)
    {
        class_meta_call_back_ = std::make_shared<AFClassCallBackManager>();
    }

    virtual ~AFClassMeta()
    {
        class_list_.clear();
        container_list_.clear();
        container_meta_list_.clear();
        data_meta_list_.clear();
        record_meta_list_.clear();
    }

    const std::string& GetName() const
    {
        return name_;
    }

    void SetResPath(const std::string& value)
    {
        res_path_ = value;
    }

    const std::string& GetResPath() const
    {
        return res_path_;
    }

    // create data meta
    ARK_SHARE_PTR<AFDataNewMeta> CreateDataMeta(const std::string& name)
    {
        auto iter = data_meta_list_.find(name);
        ARK_ASSERT_RET_VAL(iter == data_meta_list_.end(), nullptr);

        // create new data meta
        ARK_SHARE_PTR<AFDataNewMeta> pMeta = std::make_shared<AFDataNewMeta>(name);
        data_meta_list_.insert(name, pMeta);

        return pMeta;
    }

    // add data meta
    bool AddDataMeta(const std::string& name, ARK_SHARE_PTR<AFDataNewMeta> pDataMeta)
    {
        ARK_ASSERT_RET_VAL(pDataMeta != nullptr, false);

        if (!data_meta_list_.insert(name, pDataMeta).second)
        {
            return false;
        }

        return true;
    }

    ARK_SHARE_PTR<AFDataNewMeta> FindDataMeta(const std::string& value)
    {
        return data_meta_list_.find_value(value);
    }

    // create record meta
    ARK_SHARE_PTR<AFRecordMeta> CreateRecordMeta(const std::string& name)
    {
        auto iter = record_meta_list_.find(name);
        ARK_ASSERT_RET_VAL(iter == record_meta_list_.end(), nullptr);

        ARK_SHARE_PTR<AFRecordMeta> pMeta = std::make_shared<AFRecordMeta>(name);
        record_meta_list_.insert(name, pMeta);

        return pMeta;
    }

    bool AddRecordMeta(ARK_SHARE_PTR<AFRecordMeta> pMeta)
    {
        return record_meta_list_.insert(pMeta->GetName(), pMeta).second;
    }

    ARK_SHARE_PTR<AFRecordMeta> FindRecordMeta(const std::string& value)
    {
        return record_meta_list_.find_value(value);
    }

    const DataMetaList& GetDataMetaList() const
    {
        return data_meta_list_;
    }

    const RecordMetaList& GetRecordMetaList() const
    {
        return record_meta_list_;
    }

    ARK_SHARE_PTR<AFClassCallBackManager> GetClassCallBackManager() const
    {
        return class_meta_call_back_;
    }

    bool AddContainer(const std::string& name, ARK_SHARE_PTR<AFClassMeta> pMeta)
    {
        ARK_ASSERT_RET_VAL(container_meta_list_.find_value(name) == nullptr, false);

        return container_meta_list_.insert(name, pMeta).second;
    }

    ARK_SHARE_PTR<AFClassMeta> FindContainer(const std::string& name)
    {
        return container_meta_list_.find_value(name);
    }

    bool AddClass(const std::string& name)
    {
        ARK_ASSERT_RET_VAL(std::find(class_list_.begin(), class_list_.end(), name) == class_list_.end(), false);

        class_list_.push_back(name);
        return true;
    }

    const ClassList& GetClassList()
    {
        return class_list_;
    }

    bool AddContainer(const std::string& name, const std::string& type_name)
    {
        ARK_ASSERT_RET_VAL(container_list_.find(name) == container_list_.end(), false);

        return container_list_.insert(std::make_pair(name, type_name)).second;
    }

    const ContainerList& GetContainerList() const
    {
        return container_list_;
    }

private:
    // class name
    std::string name_{NULL_STR};

    // res path
    std::string res_path_{NULL_STR};

    // class type list only used for reading config
    ClassList class_list_;

    // container type list only used for reading config
    ContainerList container_list_;

    // container meta list
    ContainerMetaList container_meta_list_;

    // data meta list
    DataMetaList data_meta_list_;

    // record meta list
    RecordMetaList record_meta_list_;

    // class meta call back
    ARK_SHARE_PTR<AFClassCallBackManager> class_meta_call_back_;
};

} // namespace ark
