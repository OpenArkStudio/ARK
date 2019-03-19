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

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_iterators.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "AFPlatform.hpp"

namespace ark
{

    class AFXml;
    class AFXmlNode
    {
    public:
        AFXmlNode(AFXml* xml)
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

        void NextNode(const char* key = nullptr)
        {
            xml_node_ = xml_node_->next_sibling(key);
        }

        AFXmlNode FindNode(const char* key)
        {
            AFXmlNode node(xml_);
            node.xml_node_ = xml_node_->first_node(key);
            return node;
        }

        bool GetBool(const char* key, bool optional = false, bool default_value = false)
        {
            auto attr = xml_node_.first_attribute(key);
            if (attr == nullptr)
            {
                ARK_ASSERT_RET_VAL(optional == false, default_value);
            }
            else
            {
                return AFMisc::FromString<bool>(attr->value());
            }
        }

        uint32_t GetUint32(const char* key, bool optional = false, uint32_t default_value = NULL_INT)
        {
            auto attr = xml_node_.first_attribute(key);
            if (attr == nullptr)
            {
                ARK_ASSERT_RET_VAL(optional == false, default_value);
            }
            else
            {
                return AFMisc::FromString<uint32_t>(attr->value());
            }
        }

        float GetFloat(const char* key, bool optional = false, float default_value = NULL_FLOAT)
        {
            auto attr = xml_node_.first_attribute(key);
            if (attr == nullptr)
            {
                ARK_ASSERT_RET_VAL(optional == false, default_value);
            }
            else
            {
                return AFMisc::FromString<float>(attr->value());
            }
        }

        std::string GetString(const char* key, bool optional = false, std::string default_value = NULL_STR)
        {
            auto attr = xml_node_.first_attribute(key);
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
        AFXml* xml_{ nullptr };
        rapidxml::xml_node<>* xml_node_{ nullptr };
    };

    class AFXml
    {
    public:
        AFXml(const std::string& file_path) :
            file_path_(file_path),
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

        AFXmlNode FindNode(const char* key)
        {
            AFXmlNode node(this);
            node.xml_node_ = xml_document->first_node(key);
            return node;
        }

        const char* GetFile() const
        {
            return file_path_.c_str();
        }

        const char* GetData()
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
        }

    private:
        rapidxml::xml_document<>* xml_document_;
        rapidxml::file<>* xml_file_;
        std::string file_path_;
    };

}