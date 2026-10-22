# Retain source-consumer include paths after co-locating server headers.
# These build-tree forwarding headers are private to built-in server targets;
# public ark/... plugin mappings and installed headers are unchanged.
set(ark_server_legacy_headers
    "game/include/game_net_module_impl.hpp"
    "game/include/game_plugin.hpp"
    "game/interface/game_net_module.hpp"
    "login/include/login_net_module_impl.hpp"
    "login/include/login_plugin.hpp"
    "login/interface/login_net_module.hpp"
    "proxy/include/proxy_net_module_impl.hpp"
    "proxy/include/proxy_plugin.hpp"
    "proxy/interface/proxy_net_module.hpp"
    "world/include/world_net_module_impl.hpp"
    "world/include/world_plugin.hpp"
    "world/interface/world_net_module.hpp"
    "master/include/master_net_module_impl.hpp"
    "master/include/master_plugin.hpp"
    "master/interface/master_net_module.hpp"
    "dir/include/dir_net_module_impl.hpp"
    "dir/include/dir_plugin.hpp"
    "dir/interface/dir_net_module.hpp"
    "router/include/router_net_module_impl.hpp"
    "router/include/router_plugin.hpp"
    "router/interface/router_net_module.hpp"
    "simulate/include/simulate_define.hpp"
    "simulate/include/simulate_module_impl.hpp"
    "simulate/include/simulate_msg_define.hpp"
    "simulate/include/simulate_net_module_impl.hpp"
    "simulate/include/simulate_plugin.hpp"
    "simulate/interface/simulate_module.hpp"
    "simulate/interface/simulate_net_module.hpp"
)
set(ark_server_compat_include "${CMAKE_CURRENT_BINARY_DIR}/compat-include")
# Only this owned mapping directory is regenerated.
file(REMOVE_RECURSE "${ark_server_compat_include}")
foreach(header IN LISTS ark_server_legacy_headers)
    string(REGEX REPLACE "^([^/]+)/[^/]+/(.+)$" "\\1/\\2" source "${header}")
    get_filename_component(directory "${ark_server_compat_include}/${header}" DIRECTORY)
    file(MAKE_DIRECTORY "${directory}")
    file(WRITE "${ark_server_compat_include}/${header}"
        "// Generated compatibility forwarding header. Edit the source header.\n#pragma once\n#include \"${CMAKE_CURRENT_LIST_DIR}/${source}\"\n")
endforeach()
add_library(ark_server_compat_headers INTERFACE)
target_include_directories(ark_server_compat_headers INTERFACE "${ark_server_compat_include}")
