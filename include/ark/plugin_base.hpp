// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>

#include "ark/plugin.hpp"
#include "ark/plugin_manager.hpp"

namespace ark {

enum class module_update
{
    disabled,
    enabled
};

template<typename derived_type>
class plugin_base : public plugin
{
public:
    int get_plugin_version() override {
        return 0;
    }

    const std::string get_plugin_name() override {
        return typeid(derived_type).name();
    }

    plugin_manager* get_plugin_manager() const override {
        return plugin_manager_;
    }

    void set_plugin_manager(plugin_manager* value) override {
        ARK_ASSERT_RET_NONE(value != nullptr);
        plugin_manager_ = value;
    }

    static const std::string& get_plugin_conf() {
        return plugin_conf_;
    }

    void set_plugin_conf(const std::string& value) override {
        plugin_conf_ = value;
    }

protected:
    // Successful registration transfers ownership to the explicit unregister path.
    // Like the legacy macros, destruction alone does not uninstall modules.
    template<std::derived_from<module> interface_type>
    bool register_module(std::unique_ptr<interface_type> candidate, module_update update) {
        get_plugin_manager()->check_registration();
        if (!candidate)
        {
            return false;
        }
        const std::string name = typeid(interface_type).name();
        candidate->set_plugin_manager(get_plugin_manager());
        candidate->set_name(name);
        const auto [entry, inserted] = modules_.emplace(name, candidate.get());
        if (!inserted)
        {
            return false;
        }
        try
        {
            if (!get_plugin_manager()->register_owned_module(name, *candidate, update == module_update::enabled))
            {
                modules_.erase(entry);
                return false;
            }
        }
        catch (...)
        {
            modules_.erase(entry);
            throw;
        }
        candidate.release();
        return true;
    }

    template<std::derived_from<module> interface_type>
    bool unregister_module() {
        get_plugin_manager()->check_registry_mutation();
        const auto entry = modules_.find(typeid(interface_type).name());
        if (entry == modules_.end() || !get_plugin_manager()->unregister_owned_module(entry->first, *entry->second))
        {
            return false;
        }
        auto* instance = entry->second;
        modules_.erase(entry);
        delete instance;
        return true;
    }

private:
    std::unordered_map<std::string, module*> modules_;
    plugin_manager* plugin_manager_{nullptr};
    inline static std::string plugin_conf_;
};

} // namespace ark
