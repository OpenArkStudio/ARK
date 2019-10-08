import zipfile
import tarfile
import os
import plugin_and_module_param
import shutil


def create_folder(folder):
    if not os.path.exists(folder):
        os.makedirs(folder)
        print("---new folder[%s] ok ---\n" % (folder))


def write_plugin(res_path, plugin):
    # include/AFNamePlugin.hpp
    dir_path = os.path.join(res_path, plugin,
                            plugin_and_module_param.include_path)
    create_folder(dir_path)
    file_name = "AF" + plugin.capitalize() + "Plugin"
    with open(
            os.path.join(dir_path,
                         file_name + plugin_and_module_param.hpp_ext),
            'w') as f:
        f.write(u'''/*
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

#include "interface/AFIPlugin.hpp"
#include "base/AFPluginManager.hpp"

namespace ark {

ARK_DECLARE_PLUGIN(AF''' + plugin.capitalize() + '''Plugin)

} // namespace ark''')

    # interface
    dir_path = os.path.join(res_path, plugin,
                            plugin_and_module_param.interface_path)
    create_folder(dir_path)

    # src/AFNamePlugin.cpp
    dir_path = os.path.join(res_path, plugin, plugin_and_module_param.src_path)
    create_folder(dir_path)
    file_name = "AF" + plugin.capitalize() + "Plugin"
    with open(
            os.path.join(dir_path,
                         file_name + plugin_and_module_param.cpp_ext),
            'w') as f:
        f.write(u'''/*
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

#include "''' + plugin + '''/include/AF''' + plugin.capitalize() +
                '''Plugin.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(AF''' + plugin.capitalize() + '''Plugin)

void AF''' + plugin.capitalize() + '''Plugin::Install()
{
    // install module
}

void AF''' + plugin.capitalize() + '''Plugin::Uninstall()
{
    // uninstall module
}

} // namespace ark
''')

    # src/dllmain.cpp
    with open(os.path.join(dir_path, plugin_and_module_param.dllmain_file),
              'w') as f:
        f.write(u'''/*
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

#include "base/AFDLLHeader.hpp"

''')

    # name/CMakeLists.txt
    with open(
            os.path.join(res_path, plugin, plugin_and_module_param.cmake_file),
            'w') as f:
        f.write(u'''if(WIN32)
    if (_type STREQUAL debug)
        BUILD_PLUGIN_MACRO(AF''' + plugin.capitalize() + '''Plugin)
    else()
        BUILD_PLUGIN_MACRO(AF''' + plugin.capitalize() + '''Plugin)
    endif()
else(UNIX)
    BUILD_PLUGIN_MACRO(AF''' + plugin.capitalize() + '''Plugin)
endif()

''')

    # plugin/CMakeLists.txt
    f = open(os.path.join(res_path, plugin_and_module_param.cmake_file), 'r')
    lines = []
    for line in f:
        lines.append(line)
    s = "add_subdirectory(" + plugin + ")"
    if lines[len(lines) - 1].find(s) == -1:
        s = "\n" + s
        lines.append(s)
    s = ''.join(lines)
    f = open(os.path.join(res_path, plugin_and_module_param.cmake_file), 'w+')
    f.write(s)
    f.close()
    del lines[:]


def write_module(module, plugin, res_path):
    # include/AFCNameModule.hpp
    dir_path = os.path.join(res_path, plugin,
                            plugin_and_module_param.include_path)
    file_name = "AFC" + module.capitalize() + "Module"

    with open(
            os.path.join(dir_path,
                         file_name + plugin_and_module_param.hpp_ext),
            'w') as f:
        f.write(u'''/*
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

#include "base/AFPluginManager.hpp"
#include "''' + plugin + '''/interface/AFI''' + module.capitalize() +
                '''Module.hpp"

namespace ark {

class AFC''' + module.capitalize() + '''Module final : public AFI''' +
                module.capitalize() + '''Module
{
    ARK_DECLARE_MODULE_FUNCTIONS
public:
    //ToDo

private:
    //Todo

};

} // namespace ark

''')

# interface/AFINameModule.hpp
    dir_path = os.path.join(res_path, plugin,
                            plugin_and_module_param.interface_path)
    file_name = "AFI" + module.capitalize() + "Module"

    with open(
            os.path.join(dir_path,
                         file_name + plugin_and_module_param.hpp_ext),
            'w') as f:
        f.write(u'''/*
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

#include "interface/AFIModule.hpp"

namespace ark {

class AFI''' + module.capitalize() + '''Module : public AFIModule 
{
public:
    //virtual method
};

} // namespace ark

''')

# src/AFCNameModule.cpp
    dir_path = os.path.join(res_path, plugin, plugin_and_module_param.src_path)
    file_name = "AFC" + module.capitalize() + "Module"

    with open(
            os.path.join(dir_path,
                         file_name + plugin_and_module_param.cpp_ext),
            'w') as f:
        f.write(u'''/*
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

#include "''' + plugin + '''/include/AFC''' + module.capitalize() +
                '''Module.hpp"

namespace ark {
    //method realization

} // namespace ark

''')


# src/AFNamePlugin.cpp
    file_name = "AF" + plugin.capitalize() + "Plugin"
    path = os.path.join(dir_path, file_name + plugin_and_module_param.cpp_ext)
    file = open(path, "r", encoding="utf-8")
    lines = []
    for line in file:
        lines.append(line)

    i = 0
    flag = False
    for line in lines:  #依次读取每行
        i = i + 1
        if not flag and line[0:1] != "#":
            continue
        flag = True
        if flag and line[0:1] != "#":
            if line == "\n":
                continue
            else:
                break
    lines.insert(
        i - 2, "#include \"" + plugin + "/include/AFC" + module.capitalize() +
        "Module.hpp\"\n")

    for i in range(i, len(lines)):
        i = i + 1
        if lines[i].find("Install") != -1:
            while True:
                i = i + 1
                if lines[i].find("}") != -1:
                    break
            lines.insert(
                i, "\tARK_REGISTER_MODULE(AFI" + module.capitalize() +
                "Module, AFC" + module.capitalize() + "Module);\n")
            while True:
                if lines[i].find("{") != -1 and lines[i - 1].find(
                        "Uninstall") != -1:
                    break
                i = i + 1
            i = i + 1
            if lines[i].find("// uninstall module") != -1:
                i = i + 1
            lines.insert(
                i, "\tARK_DEREGISTER_MODULE(AFI" + module.capitalize() +
                "Module, AFC" + module.capitalize() + "Module);\n")
            break

    s = ''.join(lines)
    f = open(path, 'w+')
    f.write(s)
    f.close()
    del lines[:]

if __name__ == "__main__":
    files = os.listdir(plugin_and_module_param.plugin_path)
    plugin_list = []
    for i in files:
        if i != plugin_and_module_param.cmake_file and i != plugin_and_module_param.readme_file:
            plugin_list.append(i)
    # plugin name-id
    while True:
        val = input(
            '\nplease choose generating plugin type.\n\n0. quit\n\n1. common plugin\n\n2. plugin in specified project\n\n3. add module\n\nYou choose : '
        )
        if val == "0":
            break
        if val == "3":
            while True:
                module = input('\nplease input module name : ')
                if module.isalpha():
                    s = '\nPlease choose which plugin is the module served for.\n\n0. back\n\n1. common plugin\n\n2. plugin in specified project\n\nYou choose : '
                    plugin_type = input(s)
                    if plugin_type == "0":
                        continue
                    else:
                        if plugin_type == "1":
                            path = plugin_and_module_param.plugin_path
                        if plugin_type == "2":
                            path = plugin_and_module_param.server_path
                        files = os.listdir(path)
                        plugin_list = []
                        for i in files:
                            if i != plugin_and_module_param.cmake_file and i != plugin_and_module_param.readme_file:
                                plugin_list.append(i)
                        s = "There are such plugins:\n\n"
                        s = s + str(0) + '. ' + 'quit\n\n'
                        for i in range(0, len(plugin_list)):
                            s = s + str(i + 1) + '. ' + plugin_list[i] + '\n\n'
                        s = s + 'You choose : '
                        plugin = input(s)
                        if int(plugin) in range(1, len(plugin_list) + 1):
                            write_module(module, plugin_list[int(plugin) - 1],
                                         path)
                            break
                        else:
                            if int(plugin) == 0:
                                break
                            else:
                                print("Please choose correct number.\n")
                        break
        if val == "1":
            res_path = plugin_and_module_param.plugin_path
            print("res_path: " + res_path)
            while True:
                val = input('\nplease input plugin name : ')
                if val.isalpha():
                    print("plugin %s start to generate  --------" % (val))
                    write_plugin(res_path, val)
                    break
        if val == "2":
            res_path = plugin_and_module_param.server_path
            print("res_path: " + res_path)
            while True:
                val = input('\nplease input plugin name : ')
                if val.isalpha():
                    print("plugin %s start to generate  --------" % (val))
                    write_plugin(res_path, val)
                    break
