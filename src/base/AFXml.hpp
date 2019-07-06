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

#pragma once

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "base/AFPlatform.hpp"
#include "base/AFMisc.hpp"
#include "base/AFDefine.hpp"

namespace ark {

class AFXml;
class AFXmlNode
{
public:
    AFXmlNode(AFXml *xml)
    {
        xml_ = xml;
    }

    ~AFXmlNode()
    {
        xml_node_ = nullptr;
    }

    bool IsValid()
    {
        return xml_node_ != nullptr;
    }

    void NextNode(const char *key = nullptr)
    {
        rapidxml::xml_node<> *node = reinterpret_cast<rapidxml::xml_node<> *>(xml_node_);
        xml_node_ = node->next_sibling(key);
    }

    AFXmlNode FindNode(const char *key)
    {
        auto xmlnode = reinterpret_cast<rapidxml::xml_node<> *>(xml_node_);

        AFXmlNode node(xml_);
        node.xml_node_ = xmlnode->first_node(key);
        return node;
    }

    bool GetBool(const char *key, bool optional = false, bool default_value = false)
    {
        rapidxml::xml_node<> *xmlnode = reinterpret_cast<rapidxml::xml_node<> *>(xml_node_);

        auto attr = xmlnode->first_attribute(key);
        if (attr == nullptr)
        {
            ARK_ASSERT_RET_VAL(optional == false, default_value);
        }
        else
        {
            return AFMisc::FromString<bool>(attr->value());
        }
    }

    uint32_t GetUint32(const char *key, bool optional = false, uint32_t default_value = NULL_INT)
    {
        rapidxml::xml_node<> *node = reinterpret_cast<rapidxml::xml_node<> *>(xml_node_);

        auto attr = node->first_attribute(key);
        if (attr == nullptr)
        {
            ARK_ASSERT_RET_VAL(optional == false, default_value);
        }
        else
        {
            return AFMisc::FromString<uint32_t>(attr->value());
        }
    }

    float GetFloat(const char *key, bool optional = false, float default_value = NULL_FLOAT)
    {
        rapidxml::xml_node<> *node = reinterpret_cast<rapidxml::xml_node<> *>(xml_node_);

        auto attr = node->first_attribute(key);
        if (attr == nullptr)
        {
            ARK_ASSERT_RET_VAL(optional == false, default_value);
        }
        else
        {
            return AFMisc::FromString<float>(attr->value());
        }
    }

    std::string GetString(const char *key, bool optional = false, std::string default_value = NULL_STR)
    {
        rapidxml::xml_node<> *node = reinterpret_cast<rapidxml::xml_node<> *>(xml_node_);

        auto attr = node->first_attribute(key);
        if (attr == nullptr)
        {
            ARK_ASSERT_RET_VAL(optional == false, default_value);
        }
        else
        {
            return attr->value();
        }
    }

private:
    friend class AFXml;
    AFXml *xml_{nullptr};
    void *xml_node_{nullptr};
};

class AFXml
{
public:
    AFXml(const std::string &file_path)
        : file_path_(file_path)
    {
        xml_file_ = ARK_NEW rapidxml::file<>();
        xml_document_ = ARK_NEW rapidxml::xml_document<>();

        ParseFile();
    }

    ~AFXml()
    {
        ARK_DELETE(xml_file_);
        ARK_DELETE(xml_document_);
    }

    AFXmlNode GetRootNode()
    {
        AFXmlNode node(this);
        node.xml_node_ = xml_document_->first_node();
        return node;
    }

    AFXmlNode FindNode(const char *key)
    {
        AFXmlNode node(this);
        node.xml_node_ = xml_document_->first_node(key);
        return node;
    }

    const char *GetFile() const
    {
        return file_path_.c_str();
    }

    const char *GetData()
    {
        return xml_file_->data();
    }

    uint32_t GetSize()
    {
        return xml_file_->size();
    }

protected:
    void ParseFile()
    {
        xml_file_->from(file_path_.c_str());
        xml_document_->parse<0>(xml_file_->data());
    }

private:
    rapidxml::xml_document<> *xml_document_;
    rapidxml::file<> *xml_file_;
    std::string file_path_;
};

} // namespace ark