// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <initializer_list>
#include <string_view>
#include <utility>

#include <rapidxml.hpp>

#include <ark/application_manifest.hpp>
#include "configuration_reader.hpp"

namespace ark {
namespace {

using config_detail::fail_at;

[[noreturn]] void fail_at(const std::string& path, const char* reason, const toml::source_region& source) {
    throw application_manifest_error(path, reason, source.begin.line, source.begin.column);
}

void validate_keys(const toml::table& table, std::initializer_list<std::string_view> allowed, const std::string& path) {
    for (const auto& [key, value] : table)
    {
        bool known = false;
        for (const auto candidate : allowed)
        {
            known = known || key.str() == candidate;
        }
        if (!known)
        {
            fail_at(path, "unknown manifest field", key.source());
        }
    }
}

std::string read_string(
    const toml::table& table, std::string_view key, const std::string& path, bool required = false) {
    const auto* node = table.get(key);
    if (node == nullptr)
    {
        if (required)
        {
            fail_at(path, "missing plugin name", table.source());
        }
        return {};
    }
    const auto* value = node->as_string();
    if (value == nullptr)
    {
        fail_at(path, "manifest field must be a string", node->source());
    }
    if (value->get().find('\0') != std::string::npos)
    {
        fail_at(path, "manifest string contains NUL", node->source());
    }
    if (required && value->get().empty())
    {
        fail_at(path, "plugin name must not be empty", node->source());
    }
    return value->get();
}

application_manifest parse_toml(const std::string& path, std::string_view contents) {
    const auto document = config_detail::parse_toml(path, contents, "invalid TOML syntax or encoding");
    validate_keys(document, {"application", "plugins"}, path);
    const auto* application_node = document.get("application");
    if (application_node == nullptr)
    {
        throw application_manifest_error(path, "missing application table", 1, 1);
    }
    const auto* application = application_node->as_table();
    if (application == nullptr)
    {
        fail_at(path, "application must be a table", application_node->source());
    }
    validate_keys(*application, {"plugin_library_dir", "plugin_config_dir"}, path);
    application_manifest result;
    result.plugin_library_dir = read_string(*application, "plugin_library_dir", path);
    result.plugin_config_dir = read_string(*application, "plugin_config_dir", path);
    const auto* plugins_node = document.get("plugins");
    if (plugins_node == nullptr)
    {
        return result;
    }
    const auto* plugins = plugins_node->as_array();
    if (plugins == nullptr)
    {
        fail_at(path, "plugins must be an array of tables", plugins_node->source());
    }
    for (const auto& node : *plugins)
    {
        const auto* plugin = node.as_table();
        if (plugin == nullptr)
        {
            fail_at(path, "plugin entry must be a table", node.source());
        }
        validate_keys(*plugin, {"name", "config"}, path);
        auto name = read_string(*plugin, "name", path, true);
        auto config = read_string(*plugin, "config", path);
        if (!result.plugins.emplace(std::move(name), std::move(config)).second)
        {
            fail_at(path, "duplicate plugin name", plugin->source());
        }
    }
    return result;
}

application_manifest parse_xml(const std::string& path, const std::string& contents) {
    auto buffer = contents;
    buffer.push_back('\0');
    rapidxml::xml_document<> document;
    try
    { document.parse<rapidxml::parse_validate_closing_tags>(buffer.data()); }
    catch (const rapidxml::parse_error& error)
    { fail_at(path, "invalid XML syntax", contents, static_cast<std::size_t>(error.where<char>() - buffer.data())); }
    const auto* root = document.first_node();
    const auto* plugins = root == nullptr ? nullptr : root->first_node("plugins");
    if (plugins == nullptr)
    {
        throw application_manifest_error(path, "missing XML plugins element", 1, 1);
    }
    const auto read_attribute = [&](const rapidxml::xml_node<>* node, const char* key) {
        const auto* attribute = node->first_attribute(key);
        if (attribute == nullptr)
        {
            return std::string{};
        }
        std::string value(attribute->value(), attribute->value_size());
        if (value.find('\0') != std::string::npos)
        {
            fail_at(path, "manifest string contains NUL", contents,
                static_cast<std::size_t>(attribute->value() - buffer.data()));
        }
        return value;
    };
    application_manifest result;
    result.plugin_library_dir = read_attribute(plugins, "plugin_lib_dir");
    result.plugin_config_dir = read_attribute(plugins, "plugin_conf_dir");
    for (auto* plugin = plugins->first_node("plugin"); plugin != nullptr; plugin = plugin->next_sibling())
    {
        auto name = read_attribute(plugin, "name");
        if (name.empty())
        {
            fail_at(path, "plugin name must not be empty", contents,
                static_cast<std::size_t>(plugin->name() - buffer.data()));
        }
        if (!result.plugins.emplace(std::move(name), read_attribute(plugin, "conf")).second)
        {
            fail_at(path, "duplicate plugin name", contents, static_cast<std::size_t>(plugin->name() - buffer.data()));
        }
    }
    return result;
}

} // namespace

application_manifest_error::application_manifest_error(
    const std::string& path, const std::string& reason, std::size_t line, std::size_t column)
    : std::runtime_error(config_detail::error_message(path, reason, line, column))
    , path_(path)
    , line_(line)
    , column_(column) {
}

const std::string& application_manifest_error::path() const noexcept {
    return path_;
}

std::size_t application_manifest_error::line() const noexcept {
    return line_;
}

std::size_t application_manifest_error::column() const noexcept {
    return column_;
}

application_manifest load_application_manifest(const std::string& path) {
    try
    {
        const auto contents = config_detail::read_file(path, "manifest");
        return path.ends_with(".toml") ? parse_toml(path, contents) : parse_xml(path, contents);
    }
    catch (const config_detail::read_error& error)
    { throw application_manifest_error(path, error.reason, error.line, error.column); }
}

} // namespace ark
