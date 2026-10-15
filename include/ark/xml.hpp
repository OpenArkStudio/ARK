/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#pragma once

#include <memory>

#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

#include "ark/define.hpp"
#include "ark/misc.hpp"
#include "ark/platform.hpp"

namespace ark {

class xml;

class xml_node
{
public:
    xml_node(xml* xml) {
        xml_ = xml;
    }

    ~xml_node() {
        xml_node_ = nullptr;
    }

    bool is_valid() {
        return xml_node_ != nullptr;
    }

    void next_node(const char* key = nullptr) {
        rapidxml::xml_node<>* node = reinterpret_cast<rapidxml::xml_node<>*>(xml_node_);
        xml_node_ = node->next_sibling(key);
    }

    xml_node find_node(const char* key) {
        ARK_ASSERT_NO_EFFECT(is_valid());
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<>*>(xml_node_);

        xml_node node(xml_);
        node.xml_node_ = xmlnode->first_node(key);
        return node;
    }

    bool get_bool(const char* key, bool optional = false, bool default_value = false) {
        rapidxml::xml_node<>* xmlnode = reinterpret_cast<rapidxml::xml_node<>*>(xml_node_);

        auto attr = xmlnode->first_attribute(key);
        if (attr == nullptr)
        {
            if (optional == false)
            {
                return default_value;
            }
            else
            {
                ARK_ASSERT_NO_EFFECT(0);
                return default_value;
            }
        }
        else
        {
            return misc::from_string<bool>(attr->value());
        }
    }

    uint32_t get_uint32(const char* key, bool optional = false, uint32_t default_value = NULL_INT) {
        rapidxml::xml_node<>* node = reinterpret_cast<rapidxml::xml_node<>*>(xml_node_);

        auto attr = node->first_attribute(key);
        if (attr == nullptr)
        {
            if (optional == false)
            {
                return default_value;
            }
            else
            {
                ARK_ASSERT_NO_EFFECT(0);
                return default_value;
            }
        }
        else
        {
            return misc::from_string<uint32_t>(attr->value());
        }
    }

    float get_float(const char* key, bool optional = false, float default_value = NULL_FLOAT) {
        rapidxml::xml_node<>* node = reinterpret_cast<rapidxml::xml_node<>*>(xml_node_);

        auto attr = node->first_attribute(key);
        if (attr == nullptr)
        {
            if (optional == false)
            {
                return default_value;
            }
            else
            {
                ARK_ASSERT_NO_EFFECT(0);
                return default_value;
            }
        }
        else
        {
            return misc::from_string<float>(attr->value());
        }
    }

    std::string get_string(const char* key, bool optional = false, std::string default_value = NULL_STR) {
        rapidxml::xml_node<>* node = reinterpret_cast<rapidxml::xml_node<>*>(xml_node_);

        auto attr = node->first_attribute(key);
        if (attr == nullptr)
        {
            if (optional == false)
            {
                return default_value;
            }
            else
            {
                ARK_ASSERT_NO_EFFECT(0);
                return default_value;
            }
        }
        else
        {
            return attr->value();
        }
    }

private:
    friend class xml;
    xml* xml_{nullptr};
    void* xml_node_{nullptr};
};

class xml
{
public:
    xml(const std::string& file_path)
        : file_path_(file_path) {
        xml_document_ = std::make_unique<rapidxml::xml_document<>>();

        parse_file();
    }

    ~xml() = default;

    xml_node get_root_node() {
        xml_node node(this);
        node.xml_node_ = xml_document_->first_node();
        return node;
    }

    xml_node find_node(const char* key) {
        xml_node node(this);
        node.xml_node_ = xml_document_->first_node(key);
        return node;
    }

    const char* get_file() const {
        return file_path_.c_str();
    }

    const char* get_data() {
        return xml_file_->data();
    }

    uint32_t get_size() {
        return xml_file_->size();
    }

protected:
    void parse_file() {
        xml_file_ = std::make_unique<rapidxml::file<>>(file_path_.c_str());
        xml_document_->parse<0>(xml_file_->data());
    }

private:
    std::unique_ptr<rapidxml::xml_document<>> xml_document_;
    std::unique_ptr<rapidxml::file<>> xml_file_;
    std::string file_path_;
};

} // namespace ark
