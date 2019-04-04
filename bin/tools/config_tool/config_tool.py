#!/usr/bin/python
# encoding=utf-8

# author: NickYang
# date: 2019/04/02
# version: 0.1
# changelog:
#   1. first version

# TODO:
#   1. array

import os
import sys
import datetime
import os.path
import shutil
import commands
import argparse
from xml.dom.minidom import Document
from xml.dom import minidom
# self module
import config_param
import config_xml
import config_excel

reload(sys)
sys.setdefaultencoding("utf-8")


# parse command arguments
def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--path', type=str,
                        required=True, help="excel file path")

    return vars(parser.parse_args())


# generate entity class meta
def generate_entity_meta(res_path):
    entity_filepath = os.path.join(
        res_path, config_param.excel_path, config_param.special_file)
    print "start to generate [%s]" % entity_filepath
    excel = config_excel.my_excel(entity_filepath)
    excel.set_sheet_by_index(0)  # set default sheet
    min_row_no = excel.get_min_row_no()
    max_row_no = excel.get_max_row_no()
    min_col_no = excel.get_min_col_no()
    max_col_no = excel.get_max_col_no()

    meta_doc = Document()
    meta_root_node = meta_doc.createElement('metas')
    meta_doc.appendChild(meta_root_node)

    for row in range(min_row_no + config_param.entity_form_head_row, max_row_no):
        data_node = meta_doc.createElement('meta')
        meta_root_node.appendChild(data_node)
        for col in range(min_col_no, max_col_no):
            # if 2nd row cell value is 'unused', ignore
            second_cell_value = excel.get_cell_content(min_row_no + 1, col)
            if second_cell_value != None and str.lower(str(second_cell_value)) == config_param.entity_unused_flag:
                continue
            # ignore empty cell in first row
            if excel.get_cell_content(min_row_no, col) == None:
                continue
            if excel.get_cell_content(row, col) == None:
                data_node.setAttribute(
                    str(excel.get_cell_content(min_row_no, col)), "")
            else:
                data_node.setAttribute(str(excel.get_cell_content(min_row_no, col)),
                                       str(excel.get_cell_content(row, col)))
    with open(os.path.join(res_path, config_param.meta_path, config_param.special_file_name + config_param.meta_ext), 'w') as f:
        meta_doc.writexml(f, indent="\n",
                          addindent="\t", encoding='utf-8')

    # open meta define file
    cpp_file = open(os.path.join(res_path, config_param.cpp_meta_file), 'a')
    cs_file = open(os.path.join(res_path, config_param.cs_meta_file), 'a')
    cpp_file.write(u"\t//////////////////////////////////\n\t//Entity meta\n")
    cs_file.write(u"\t//////////////////////////////////\n\t//Entity meta\n")
    # class field_name type
    entity_map = {}
    for row in range(min_row_no + config_param.entity_form_head_row, max_row_no):
        meta_class = excel.get_cell_content(row, min_col_no)
        if meta_class == None:
            continue
        if not entity_map.has_key(str(meta_class)):
            entity_map[str(meta_class)] = []
        # filed type sub_class
        for col in [min_col_no + 1, min_col_no + 2, min_col_no + 6]:
            cell_content = excel.get_cell_content(row, col)
            if cell_content == None:
                entity_map[str(meta_class)].append("")
            else:
                entity_map[str(meta_class)].append(str(cell_content))
    # print entity_map
    for k in entity_map:
        cpp_file.write(u"\tclass AFEntityMeta" + k + "\n\t{\n\tpublic:\n")
        cpp_file.write(u'''\t\tstatic const std::string& self_name() { static const std:: string meta_%s_ = "%s"; return meta_%s_; }\n\n'''
                       % (k, k, k))

        cs_file.write(u"\tpublic class AFEntityMeta" + k + "\n\t{\n")
        cs_file.write(
            u'''\t\tpublic static readonly String self_name = "%s";\n\n''' % (k))
        member_list = entity_map[k]
        for i in xrange(0, len(member_list), 3):
            filed_name = member_list[i]
            type_name = member_list[i + 1]
            sub_class_name = member_list[i + 2]
            if sub_class_name != "":
                cpp_file.write(u'''\t\tstatic const std::string& %s() { static const std::string %s_ = "%s"; return %s_; } // %s - AFEntityMeta%s\n''' %
                               (filed_name, filed_name, filed_name, filed_name, type_name, sub_class_name))
                cs_file.write(u'''\t\tpublic static readonly String %s = "%s"; // %s - %s\n''' %
                              (filed_name, filed_name, type_name, sub_class_name))
            else:
                cpp_file.write(u'''\t\tstatic const std::string& %s() { static const std::string %s_ = "%s"; return %s_; } // %s\n''' %
                               (filed_name, filed_name, filed_name, filed_name, type_name))
                cs_file.write(u'''\t\tpublic static readonly String %s = "%s"; // %s\n''' %
                              (filed_name, filed_name, type_name))

        cpp_file.write(u"\t};\n\n")
        cs_file.write(u"\t}\n\n")
    # sperate line
    cpp_file.write(u"\t//////////////////////////////////\n\t//Config meta\n")
    cs_file.write(u"\t//////////////////////////////////\n\t//Config meta\n")
    # close meta define files
    cpp_file.close()
    cs_file.close()
    print "generate [%s] finished" % entity_filepath


# generate single config
def genrate_single_config(res_path, filepath, filename):
    excel = config_excel.my_excel(filepath)
    excel.set_sheet_by_index(0)  # set default sheet
    min_row_no = excel.get_min_row_no()
    max_row_no = excel.get_max_row_no()
    min_col_no = excel.get_min_col_no()
    max_col_no = excel.get_max_col_no()

    '''config meta xml'''
    meta_doc = Document()
    meta_root_node = meta_doc.createElement('metas')
    meta_doc.appendChild(meta_root_node)

    for col in range(min_col_no, max_col_no):
        data_node = meta_doc.createElement('meta')
        for row in range(min_row_no, min_row_no + config_param.config_form_head_row):
            if config_param.config_form_head_list[row - min_row_no + 1] == 'target':
                target_value = str.upper(str(excel.get_cell_content(row, col)))
                # S(erver) or A(all) will be generate for server side
                if target_value != config_param.target_all or target_value != config_param.target_server:
                    continue
            data_node.setAttribute(
                config_param.config_form_head_list[row - min_row_no + 1], excel.get_cell_content(row, col))
        if data_node.hasAttributes():
            meta_root_node.appendChild(data_node)
    with open(os.path.join(res_path, config_param.meta_path, filename + config_param.meta_ext), 'w') as f:
        meta_doc.writexml(f, indent="\n", addindent="\t", encoding='utf-8')

    '''config data xml'''
    config_doc = Document()
    config_root_node = config_doc.createElement('data')
    config_doc.appendChild(config_root_node)
    for row in range(min_row_no + config_param.config_form_head_row, max_row_no):
        data_node = config_doc.createElement('data')
        config_root_node.appendChild(data_node)
        for col in range(min_col_no, max_col_no):
            # ignore empty cell in first row
            if excel.get_cell_content(min_row_no, col) == None:
                continue
            data_node.setAttribute(str(excel.get_cell_content(min_row_no, col)),
                                   str(excel.get_cell_content(row, col)))

    with open(os.path.join(res_path, config_param.server_res_path, filename + config_param.config_ext), 'w') as f:
        config_doc.writexml(
            f, indent="\n", addindent="\t", encoding='utf-8')

    # open meta define file
    cpp_file = open(os.path.join(res_path, config_param.cpp_meta_file), 'a')
    cpp_file.write(u"\tclass AFConfigMeta" +
                   filename.capitalize() + "\n\t{\n\tpublic:\n")
    cpp_file.write(
        u'''\t\tstatic const std::string& self_name() { static const std::string meta_%s_ = "%s"; return meta_%s_; }\n\n''' %
        (filename, filename, filename))

    cs_file = open(os.path.join(res_path, config_param.cs_meta_file), 'a')
    cs_file.write(u"\tpublic class AFConfigMeta" +
                  filename.capitalize() + "\n\t{\n")
    cs_file.write(
        u'''\t\tpublic static readonly String self_name = "%s";\n\n''' % filename)

    for col in range(min_col_no, max_col_no):
        filed_name = str(excel.get_cell_content(min_row_no, col))
        type_name = str(excel.get_cell_content(min_row_no + 1, col))
        cpp_file.write(u'''\t\tstatic const std::string& %s() { static const std::string %s_ = "%s"; return %s_; } // %s\n''' %
                       (filed_name, filed_name, filed_name, filed_name, type_name))
        cs_file.write(u'''\t\tpublic static readonly String %s = "%s"; // %s\n''' %
                      (filed_name, filed_name, type_name))

    cpp_file.write(u"\t};\n\n")
    cs_file.write(u"\t}\n\n")
    # close meta define files
    cpp_file.close()
    cs_file.close()
    return True


# generate excel to config
def generate_config(res_path):
    print "Start to generate config..."
    print "---------------------------------------"
    meta_doc = Document()
    meta_root_node = meta_doc.createElement('configs')
    meta_doc.appendChild(meta_root_node)

    file_list = os.listdir(os.path.join(res_path, config_param.excel_path))
    for i in range(0, len(file_list)):
        file_path = os.path.join(
            res_path, config_param.excel_path, file_list[i])
        if not os.path.isfile(file_path):
            continue
        (filename, ext) = os.path.splitext(file_list[i])
        # file is entity_class or is not excel file
        if filename == config_param.special_file_name or (ext != config_param.excel_ext and ext != config_param.excel_new_ext):
            continue

        ret = genrate_single_config(res_path, file_path, filename)
        if ret == True:
            # insert into config_class.config
            config_node = meta_doc.createElement('config')
            config_node.setAttribute("id", filename)
            config_node.setAttribute("meta", os.path.join(
                config_param.meta_path, filename + config_param.meta_ext))
            config_node.setAttribute("res", os.path.join(
                config_param.server_res_path, filename + config_param.config_ext))
            meta_root_node.appendChild(config_node)
            print "generate file = [%s] success" % file_path
        else:
            raise Exception("generate file = [%s] failed" % file_path)

    with open(os.path.join(res_path, config_param.config_class_file), 'w') as f:
        meta_doc.writexml(f, indent="\n", addindent="\t", encoding='utf-8')
    print "---------------------------------------"
    print "generate config finished"


# write meta define files for cpp and c#
def write_meta_define_begin(res_path):
    with open(os.path.join(res_path, config_param.cpp_meta_file), 'w') as cpp_file:
        cpp_file.write(u'''/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
*
* Licensed under the Apache License, Version 2.0 (the 'License');
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an 'AS IS' BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#pragma once

#include <string>

namespace ark
{\n''')

    with open(os.path.join(res_path, config_param.cs_meta_file), 'w') as cs_file:
        cs_file.write(u'''/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2019 QuadHex authors.
*
* Licensed under the Apache License, Version 2.0 (the 'License');
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an 'AS IS' BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace ark
{\n''')


# write meta define files
def write_meta_define_end(res_path):
    with open(os.path.join(res_path, config_param.cpp_meta_file), 'a') as cpp_file:
        cpp_file.write(u"}")

    with open(os.path.join(res_path, config_param.cs_meta_file), 'a') as cs_file:
        cs_file.write(u"}")

    # main
if __name__ == "__main__":
    args = parse_args()
    res_path = args['path']
    print u"res_path = " + res_path
    # gen meta define files
    write_meta_define_begin(res_path)
    # gen entity meta
    generate_entity_meta(res_path)
    print "+++++++++++++++++++++++++++++++++++++++++"
    # gen config files
    generate_config(res_path)

    write_meta_define_end(res_path)
