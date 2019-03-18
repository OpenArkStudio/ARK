/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
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

#include "AFSingleton.hpp"
#include "AFXml.hpp"
#include "AFDataSetting.hpp"
#include "AFMetaClassSetting.hpp"

namespace ark
{

    class AFDataConfig : public AFSingleton<AFDataConfig>
    {
    public:
        AFDataConfig() = default;
        ~AFDataConfig() = default;

        std::shared_ptr<AFMetaClassSetting> FindClassSetting(const std::string& class_name) const
        {
            return class_settings_.find_value(class_name);
        }

        const AFDataSetting* FindDataSetting(const std::string& class_name, const std::string& data_name) const
        {
            auto class_setting = FindClassSetting(class_name);
            if (class_setting == nullptr)
            {
                return nullptr;
            }

            return class_setting->FindDataSetting(data_name);
        }

        //just load meta class config
        bool LoadDataConfig(const std::string& file)
        {
            AFXml xml(file);
            auto root_node = xml.GetRootNode();
            for (auto xml_node = root_node.FindNode("setting"); xml_node.IsValid(); xml_node = xml_node.NextNode())
            {
                auto class_name = xml_node.GetString("class");
                auto meta_class_setting = class_settings_.find_value(class_name);
                if (meta_class_setting == nullptr)
                {
                    auto meta_class_setting = std::make_shared<AFMetaClassSetting>();
                    class_settings_.insert(class_name, meta_class_setting);
                }

                auto field_name = xml_node.GetString("filed_name");
                auto data_setting = std::make_shared<AFDataSetting>();
                data_setting->type_ = xml_node.GetString("type");
                data_setting->init_value_ = xml_node.GetString("init_value");
                data_setting->sub_meta_class_ = xml_node.GetString("sub_class");
                data_setting->key_name_ = xml_node.GetString("key_name");
                if (xml_node.GetBool("save"))
                {
                    data_setting->mask_[DMT_SAVE_DB] = true;
                }

                if (xml_node.GetBool("sync_self"))
                {
                    data_setting->mask_[DMT_SYNC_SELF] = true;
                }

                if (xml_node.GetBool("sync_view"))
                {
                    data_setting->mask_[DMT_SYNC_VIEW] = true;
                }

                if (xml_node.GetBool("log"))
                {
                    data_setting->mask_[DMT_LOG] = true;
                }

                //other mask
            }
        }

    private:
        AFMapEx<std::string, AFMetaClassSetting> class_settings_;
    };

}