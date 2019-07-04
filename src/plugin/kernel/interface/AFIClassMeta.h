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


namespace ark
{

    class AFIClassMeta
    {
    public:
        bool IsChildDataMeta(const std::string& name) const
        {
            return FindDataMeta(name) != nullptr;
        }

        virtual AFIDataMeta* FindDataMeta(const std::string& name) const = 0;

        void SetClassMetaName(const std::string& value)
        {
            class_meta_name_ = value;
        }

        const std::string& GetClassMetaName() const
        {
            return class_meta_name_;
        }

    private:
        std::string class_meta_name_{};
    };

}