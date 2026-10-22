// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <concepts>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include <ark/dynamic_library.hpp>
#include <ark/module_base.hpp>
#include <ark/plugin_base.hpp>
#include "server_test_types.hpp"

static_assert(std::derived_from<server_plugin, ark::plugin_base<server_plugin>>);
static_assert(std::derived_from<server_implementation, ark::module_base<server_module>>);
static_assert(std::same_as<decltype(&server_implementation::update), decltype(&ark::module::update)>);

namespace {

void require(bool condition, const std::string& message) {
    if (!condition)
    {
        std::cerr << message << std::endl;
        std::_Exit(1);
    }
}

} // namespace

int main(int argc, char* argv[]) {
    require(argc == 3, "Expected plugin directory and library stem");
    ark::logger log;
    ark::logger::init(&log);
    ark::dynamic_library library(argv[2]);
    require(library.load(argv[1]), "Cannot load actual server shared library");
    require(library.get_symbol("DllEntryPlugin") != nullptr, "Missing explicit DllEntryPlugin export");
    require(library.get_symbol("DllExitPlugin") != nullptr, "Missing explicit DllExitPlugin export");
    require(library.unload(), "Cannot unload server shared library");

    auto candidate = make_server_module();
    require(
        candidate && typeid(*candidate) == typeid(server_implementation), "Factory returned the wrong implementation");
    require(candidate->get_plugin_manager() == nullptr && candidate->get_name().empty(),
        "Factory published metadata early");
    candidate.reset();

    ark::plugin_manager manager;
    server_plugin owner;
    owner.set_plugin_manager(&manager);
    require(owner.get_plugin_version() == 0 && owner.get_plugin_name() == typeid(server_plugin).name(),
        "Plugin version or RTTI name changed");
    owner.set_plugin_conf("server-test.conf");
    require(server_plugin::get_plugin_conf() == "server-test.conf", "Plugin configuration storage changed");
    owner.install();
    auto* instance = manager.find_module<server_module>();
    require(instance && typeid(*instance) == typeid(server_implementation), "Assembly registered the wrong factory");
    require(instance->get_plugin_manager() == &manager && instance->get_name() == typeid(server_module).name(),
        "Assembly lost manager ownership or interface RTTI key");
    // The existing admission seam reports whether the module was already in the
    // update set. Remove this observation immediately to preserve disabled mode.
    require(manager.add_update_module(instance), "Server module was unexpectedly admitted to updates");
    manager.remove_update_module(instance->get_name());
    bool rejected = false;
    try
    { owner.install(); }
    catch (const std::runtime_error&)
    { rejected = true; }
    require(
        rejected && manager.find_module<server_module>() == instance, "Duplicate install replaced the owned module");
    owner.uninstall();
    require(manager.find_module(typeid(server_module).name()) == nullptr, "Uninstall retained the server module");
    owner.install();
    owner.uninstall();
    require(manager.update(1), "Uninstall left invalid update bookkeeping");
    std::cout
        << argv[2]
        << ": actual factory, interface key, disabled update, duplicate rejection, reinstall and DSO exports passed\n";
}
