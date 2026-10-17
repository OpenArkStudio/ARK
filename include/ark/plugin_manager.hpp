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

#include <type_traits>

#include "ark/dynamic_library.hpp"
#include "ark/logger.hpp"
#include "ark/module.hpp"
#include "ark/noncopyable.hpp"
#include "ark/plugin.hpp"

namespace ark {

class plugin_manager final : private noncopyable
{
public:
    static auto instance() -> plugin_manager* {
        static plugin_manager* instance_ptr = nullptr;
        if (instance_ptr == nullptr)
        {
            instance_ptr = ARK_NEW plugin_manager();
        }

        return instance_ptr;
    }

    auto start(int64_t const timestamp, std::string const& lib_dir, std::string const& conf_dir,
        std::unordered_map<std::string, std::string> const& plugins) -> bool {
        timestamp_ = timestamp;

        plugin_library_dir_ = lib_dir;
        plugin_conf_dir_ = conf_dir;
        plugins_ = plugins;

        if (!init())
        {
            return false;
        }
        if (!post_init())
        {
            return false;
        }
        if (!check_config())
        {
            return false;
        }
        if (!pre_update())
        {
            return false;
        }

        return true;
    }

    auto stop(const int64_t timestamp) -> bool {
        timestamp_ = timestamp;

        pre_shut();
        shut();

        return true;
    }

    auto update(const int64_t timestamp) -> bool {
        timestamp_ = timestamp;

        // Just loop the modules which have update function.
        for (const auto& iter : module_updates_)
        {
            module* module_ptr = iter.second;
            ARK_ASSERT_CONTINUE(module_ptr != nullptr);

            module_ptr->update();
        }

        return true;
    }

    template<typename module_type>
        requires std::is_base_of_v<module, module_type>
    auto find_module() -> module_type* {
        module* logic_module = find_module(GET_CLASS_NAME(module_type));
        ARK_ASSERT_RET_VAL(logic_module != nullptr, nullptr);

        auto derived_module = dynamic_cast<module_type*>(logic_module);
        ARK_ASSERT_RET_VAL(derived_module != nullptr, nullptr);

        return derived_module;
    }

    template<typename plugin_type>
    void register_plugin(std::string const& plugin_name) {
        plugin* new_plugin = ARK_NEW plugin_type();
        std::string runtime_plugin_name = GET_CLASS_NAME(plugin_type);
        register_plugin(new_plugin, plugin_name, runtime_plugin_name);
    }

    template<typename plugin_type>
    void unregister_plugin() {
        //! Delete plugin memory in Unregister
        unregister_plugin(GET_CLASS_NAME(plugin_type));
    }

    auto add_module(const std::string& module_name, module* module_ptr) -> bool {
        ARK_ASSERT_RET_VAL(!module_name.empty(), false);
        ARK_ASSERT_RET_VAL(find_module(module_name) == nullptr, false);
        return module_instances_.insert(std::make_pair(module_name, module_ptr)).second;
    }

    void remove_module(const std::string& module_name) {
        auto iter = module_instances_.find(module_name);
        if (iter == module_instances_.end())
        {
            return;
        }

        module_instances_.erase(module_name);
    }

    auto add_update_module(module* module_ptr) -> bool {
        ARK_ASSERT_RET_VAL(module_ptr != nullptr, false);
        return module_updates_.insert(std::make_pair(module_ptr->get_name(), module_ptr)).second;
    }

    void remove_update_module(const std::string& module_name) {
        module_updates_.erase(module_name);
    }

    auto get_now_time() const -> int64_t {
        return timestamp_;
    }

    auto get_bus_id() const -> bus_id_t {
        return bus_id_;
    }

    void set_bus_id(const bus_id_t value) {
        bus_id_ = value;
    }

    auto get_app_name() const -> const std::string& {
        return app_name_;
    }

    void set_app_name(const std::string& value) {
        app_name_ = value;
    }

    void set_app_conf(const std::string& value) {
        ARK_ASSERT_RET_NONE(!value.empty());
        ARK_ASSERT_RET_NONE(value.find(".app.conf") != std::string::npos || value.ends_with(".toml"));

        app_conf_path_ = value;
    }

    auto get_app_conf() const -> std::string const& {
        return app_conf_path_;
    }

    auto get_plugin_conf(std::string const& plugin_name) -> std::string const& {
        const static std::string null_str;
        ARK_ASSERT_RET_VAL(!plugin_name.empty(), null_str);
        auto iter = plugins_.find(plugin_name);
        return iter != plugins_.end() ? iter->second : null_str;
    }

    template<typename plugin_type>
    auto get_plugin_conf() -> const std::string& {
        auto plugin = find_plugin<plugin_type>();
        ARK_ASSERT_RET_VAL(plugin != nullptr, null_str);

        return plugin->get_plugin_conf();
    }

protected:
    void register_plugin(plugin* plugin, std::string const& plugin_name, std::string const& runtime_plugin_name) {
        // e.g. plugin_name = class ark::AFKernelPlugin
        // It's a runtime name, so we cannot use it to find the plugin.conf file.
        //std::string plugin_name = plugin->GetPluginName();

        ARK_ASSERT_RET_NONE(find_plugin(plugin_name) == nullptr);

        auto iter = plugins_.find(plugin_name);
        ARK_ASSERT_NO_EFFECT(iter != plugins_.end());

        plugin->set_plugin_manager(this);
        // set plugin self configuration
        plugin->set_plugin_conf(plugin_conf_dir_ + iter->second);
        // Install modules
        plugin->install();
        // manage this plugin
        plugin_instances_.insert(std::make_pair(runtime_plugin_name, plugin));
    }

    void unregister_plugin(const std::string& plugin_name) {
        ARK_ASSERT_RET_NONE(!plugin_name.empty());

        auto plugin = find_plugin(plugin_name);
        ARK_ASSERT_RET_NONE(plugin != nullptr);

        plugin->uninstall();
        plugin_instances_.erase(plugin_name);
        ARK_DELETE(plugin);
    }

    auto find_plugin(const std::string& plugin_name) -> plugin* {
        auto iter = plugin_instances_.find(plugin_name);
        if (iter != plugin_instances_.end())
        {
            return iter->second;
        }
        else
        {
            return nullptr;
        }
    }

    template<typename plugin_type>
        requires std::is_base_of_v<plugin, plugin_type>
    auto find_plugin() -> plugin* {
        auto plugin_name = GET_CLASS_NAME(plugin_type);
        auto iter = plugin_instances_.find(plugin_name);
        if (iter != plugin_instances_.end())
        {
            return iter->second;
        }
        else
        {
            return nullptr;
        }
    }

    auto find_module(const std::string& module_name) -> module* {
        auto iter = module_instances_.find(module_name);
        return ((iter != module_instances_.end()) ? iter->second : nullptr);
    }

    auto init() -> bool {
        // load plugin dynamic libraries
        for (auto const& iter : plugins_)
        {
            bool ret = load_plugin_library(iter.first);
            ARK_ASSERT_RET_VAL(ret, false);
        }

        // initialize all modules
        for (auto const& iter : module_instances_)
        {
            module* module_ptr = iter.second;
            ARK_ASSERT_CONTINUE(module_ptr != nullptr);

            if (!module_ptr->init())
            {
                return false;
            }
        }

        return true;
    }

    auto post_init() -> bool {
        for (auto const& iter : module_instances_)
        {
            module* module_ptr = iter.second;
            ARK_ASSERT_CONTINUE(module_ptr != nullptr);

            if (!module_ptr->post_init())
            {
                return false;
            }
        }

        return true;
    }

    auto check_config() -> bool {
        for (auto const& iter : module_instances_)
        {
            module* module_ptr = iter.second;
            ARK_ASSERT_CONTINUE(module_ptr != nullptr);

            if (!module_ptr->check_config())
            {
                return false;
            }
        }

        return true;
    }

    auto pre_update() -> bool {
        for (auto const& iter : module_instances_)
        {
            module* module_ptr = iter.second;
            ARK_ASSERT_CONTINUE(module_ptr != nullptr);

            if (!module_ptr->pre_update())
            {
                return false;
            }
        }

        return true;
    }

    auto pre_shut() -> bool {
        for (auto const& iter : module_instances_)
        {
            module* module_ptr = iter.second;
            ARK_ASSERT_CONTINUE(module_ptr != nullptr);

            module_ptr->pre_shut();
        }

        return true;
    }

    auto shut() -> bool {
        for (auto const& iter : module_instances_)
        {
            module* module_ptr = iter.second;
            ARK_ASSERT_CONTINUE(module_ptr != nullptr);

            module_ptr->shut();
        }

        for (auto& it : plugins_)
        {
            unload_plugin_library(it.first);
        }

        for (auto& iter : plugin_libs_)
        {
            iter.second->unload();
            ARK_DELETE(iter.second);
        }

        return true;
    }

    auto load_plugin_library(const std::string& plugin_name) -> bool {
        auto iter = plugin_libs_.find(plugin_name);
        ARK_ASSERT_RET_VAL(iter == plugin_libs_.end(), false);

        auto* library = ARK_NEW dynamic_library(plugin_name);
        ARK_ASSERT_RET_VAL(library != nullptr, false);

        bool load_ret = library->load(plugin_library_dir_);
        if (load_ret)
        {
            plugin_libs_.insert(std::make_pair(plugin_name, library));
            auto func = reinterpret_cast<dll_entry_plugin_func>(library->get_symbol("DllEntryPlugin"));
            ARK_ASSERT_RET_VAL(func != nullptr, false);
            func(this, plugin_name, logger::instance());

            return true;
        }
        else
        {
#ifdef ARK_PLATFORM_WIN
            auto error = GetLastError();
#else
            auto error = dlerror();
#endif
            ARK_LOG_ERROR("Load shared library[{}] failed, ErrorNo[{}]", library->get_name(), error);
            ARK_ASSERT_RET_VAL(0, false);
        }

        return true;
    }

    auto unload_plugin_library(const std::string& plugin_name) -> bool {
        auto iter = plugin_libs_.find(plugin_name);
        ARK_ASSERT_RET_VAL(iter != plugin_libs_.end(), false);

        dynamic_library* library = iter->second;
        ARK_ASSERT_RET_VAL(library != nullptr, false);

        auto func = reinterpret_cast<dll_exit_plugin_func>(library->get_symbol("DllExitPlugin"));
        ARK_ASSERT_RET_VAL(func != nullptr, false);
        func(this);

        return true;
    }

private:
    // Bus id
    int bus_id_{0};
    // Current time(ms)
    int64_t timestamp_{0};
    // plugin so/dll file dir
    std::string plugin_library_dir_{};
    // xx.plugin.conf file path
    std::string plugin_conf_dir_{};
    // xx.app.conf file path
    std::string app_conf_path_{};
    // app name
    std::string app_name_{"ark"};

    using dll_entry_plugin_func = void (*)(plugin_manager*, std::string const&, logger*);
    using dll_exit_plugin_func = void (*)(plugin_manager*);

    // plugin_code_name -> plugin_config
    std::unordered_map<std::string, std::string> plugins_;
    // plugin_code_name -> AFDynLib*
    std::unordered_map<std::string, dynamic_library*> plugin_libs_;
    // plugin_code_name -> AFIPlugin*
    std::unordered_map<std::string, plugin*> plugin_instances_;
    // module_runtime_name -> AFIModule*
    std::unordered_map<std::string, module*> module_instances_;
    // Only include the module with self Update function. module_runtime_name -> AFIModule*
    std::unordered_map<std::string, module*> module_updates_;
};

} // namespace ark
