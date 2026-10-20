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
import subprocess
import argparse
import re
from xml.dom.minidom import Document
from xml.dom import minidom
# self module
import config_param
import config_xml
import config_excel
import importlib
from openpyxl import load_workbook

importlib.reload(sys)
# coding=utf-8

# parse command arguments
def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--path', type=str,
                        required=True, help="excel file path")

    return vars(parser.parse_args())

def cpp_identifier(name):
    """Normalize C++ names without changing reflection keys or C# identifiers."""
    name = re.sub(r'([A-Z]+)([A-Z][a-z])', r'\1_\2', name)
    return re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', name).lower()

def write_entity_enum_head(file, name,cpp_or_cs=True):
    if cpp_or_cs:
        file.write(u"enum class " + cpp_identifier(name) + " : std::uint32_t\n{\n    enum_empty,\n")
    else:
        file.write(u"\tenum " + name + "\n\t{\n\t\tmeta_empty,\n")

def write_entity_enum_member(file, field_name,cpp_or_cs=True):
    if cpp_or_cs:
        file.write(u"    " + cpp_identifier(field_name) + ",\n")
    else:
        file.write(u"\t\t" + field_name + ",\n")

def write_entity_enum_end(file,name,cpp_or_cs=True):
    if cpp_or_cs:
        name = cpp_identifier(name)
        file.write(u"};\n\n")
        file.write(u"using " + name + "_rep_type = std::underlying_type<"+ name +">::type;\n\n")
    else:
        file.write(u"\t}\n\n")

def write_entity_enum_all_member(file, name, entity_dict,cpp_or_cs=True):
    if cpp_or_cs:
        member_list = entity_dict[name]
        for k in range(0, len(member_list), 3):
            field_name = member_list[k]
            type_name = member_list[k+1]
            if(type_name != "class"):
                write_entity_enum_member(file, field_name,True)
    else:
        member_list = entity_dict[name]
        for k in range(0, len(member_list), 3):
            field_name = member_list[k]
            type_name = member_list[k+1]
            if(type_name != "class"):
                write_entity_enum_member(file, field_name,False)

def write_entity_cpp_head(file, name):
    identifier = cpp_identifier(name)
    file.write(u"class entity_meta_" + identifier)
    file.write(u"\n{\npublic:\n")
    file.write(u"    entity_meta_" + identifier + "() = default;\n\n")
    file.write(u'''    static const std::string& self_name() {
        static const std::string meta_%s = "%s";
        return meta_%s;
    }

''' % (identifier, name, identifier))


def write_entity_cs_head(file, name):
    file.write(u"\tpublic class AFEntityMeta" + name + "\n\t{\n")
    file.write(
        u'''\t\tpublic static readonly String self_name = "%s";\n\n''' % name)

def write_entity_cpp_member(file, name,field_name, type_name, sub_class_name=""):
    if type_name == "class":
        return
    else:
        identifier = cpp_identifier(field_name)
        enum_name = cpp_identifier(name)
        file.write(u'''    static const std::string& %s() {
        static const std::string %s_ = "%s";
        return %s_;
    } // %s

''' % (identifier, identifier, field_name, identifier, type_name))
        file.write(u'''    static std::uint32_t %s_index() {
        static const auto %s_index_ = static_cast<%s_rep_type>(%s::%s);
        return %s_index_;
    } // %s

''' % (identifier, identifier, enum_name, enum_name, identifier, identifier, field_name))


def write_entity_cs_member(file, name,field_name, type_name,sub_class_name=""):
    if type_name == "class":
        return
        #file.write(u'''\t\tpublic static readonly String %s = %s; // %s - AFEntityMeta%s\n''' %
                   #(field_name, field_name, type_name, sub_class_name))
        #file.write(u'''\t\tpublic static readonly Int %s_index = %d; // %s - AFEntityMeta%s\n''' %
                   #(field_name, int(field_index), type_name, sub_class_name))
    else:
        file.write(u'''\t\tpublic static readonly String %s = %s; // %s\n''' %
                   (field_name, field_name, type_name))
        file.write(u'''\t\tpublic static UInt32 %s_index = (UInt32)%s.%s; // %s\n''' %
                   (field_name, name,field_name, field_name))


def write_entity_cpp_end(file):
    file.write(u"};\n\n")


def write_entity_cs_end(file):
    file.write(u"\t};\n\n")


def write_entity_all_member(file,enumclass_name, name, entity_dict, cpp_or_cs=True):
    if cpp_or_cs:
        # get fields
        member_list = entity_dict[name]
        for k in range(0, len(member_list), 3):
            field_name = member_list[k]
            type_name = member_list[k + 1]
            sub_class_name = member_list[k + 2]
            write_entity_cpp_member(
                file, enumclass_name,field_name, type_name, sub_class_name)
    else:
        # get fields
        member_list = entity_dict[name]
        for k in range(0, len(member_list), 3):
            field_name = member_list[k]
            type_name = member_list[k + 1]
            sub_class_name = member_list[k + 2]
            write_entity_cs_member(
                file, enumclass_name,field_name, type_name, sub_class_name)

def write_entity(file, name, entity_dict, cpp_or_cs=True):
    if cpp_or_cs:
        write_entity_enum_head(file,name,True)
        write_entity_enum_all_member(file,name,entity_dict,True)
        write_entity_enum_end(file,name,True)

        write_entity_cpp_head(file, name)
        # get fields
        member_list = entity_dict[name]
        for k in range(0, len(member_list), 3):
            field_name = member_list[k]
            type_name = member_list[k + 1]
            sub_class_name = member_list[k + 2]
            write_entity_cpp_member(
                file, name,field_name, type_name, sub_class_name)
        write_entity_cpp_end(file)
    else:
        write_entity_enum_head(file,name,False)
        write_entity_enum_all_member(file,name,entity_dict,False)
        write_entity_enum_end(file,name,False)
        write_entity_cs_head(file, name)
        # get parent fields
        member_list = entity_dict[name]
        for k in range(0, len(member_list), 3):
            field_name = member_list[k]
            type_name = member_list[k + 1]
            sub_class_name = member_list[k + 2]
            write_entity_cs_member(
                file, name,field_name, type_name, sub_class_name)
        write_entity_cs_end(file)

# generate entity_class meta and config
def generate_entity_meta(res_path):
    entity_filepath = os.path.join(
        res_path, config_param.excel_path, config_param.special_file)
    print("start to generate [%s]" % entity_filepath)
    excel = config_excel.my_excel(entity_filepath)
    # entity parent class
    entity_parent_dict = {}

    config_doc = Document()
    config_root_node = config_doc.createElement('configs')
    config_doc.appendChild(config_root_node)

    for sheet_name in excel.get_sheet_names():
        config_node = config_doc.createElement('config')
        config_node.setAttribute("id", sheet_name)
        config_node.setAttribute("meta", os.path.join(
            config_param.meta_path, sheet_name + config_param.meta_ext))
        config_root_node.appendChild(config_node)

        excel.set_sheet_by_name(sheet_name)  # set sheet
        min_row_no = excel.get_min_row_no()
        max_row_no = excel.get_max_row_no()+1
        min_col_no = excel.get_min_col_no()
        max_col_no = excel.get_max_col_no()+1

        for row in range(min_row_no + config_param.entity_form_head_row, max_row_no):
            meta_class = excel.get_cell_content(row, min_col_no)
            if meta_class == None:
                continue
            meta_class_name = str(meta_class)
            if not entity_dict.__contains__(meta_class_name):
                entity_dict[meta_class_name] = []

            # field type sub_class
            for col in [config_param.entity_field_name_col, config_param.entity_field_type_col, config_param.entity_field_sub_class_col]:
                cell_content = excel.get_cell_content(row, col)
                if cell_content == None:
                    entity_dict[meta_class_name].append("")
                else:
                    entity_dict[meta_class_name].append(str(cell_content))
            # field type parent_class
            if(excel.get_cell_content(row, config_param.entity_field_type_col)=="class"):
                parent_class_cell = excel.get_cell_content(
                   row, config_param.entity_field_sub_class_col)

                if (parent_class_cell != None):
                    if (not entity_parent_dict.__contains__(meta_class_name)):
                       entity_parent_dict[meta_class_name] = []
                    parent_class_name = str(parent_class_cell)
                    if parent_class_name not in entity_parent_dict[meta_class_name]:
                       entity_parent_dict[meta_class_name].append(parent_class_name)


        meta_doc = Document()
        meta_root_node = meta_doc.createElement('metas')
        meta_doc.appendChild(meta_root_node)
        for row in range(min_row_no + config_param.entity_form_head_row, max_row_no):
            meta_class = excel.get_cell_content(row, min_col_no)
            if meta_class == None:
                continue
            meta_class_name = str(meta_class)
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

        with open(os.path.join(res_path, config_param.meta_path, sheet_name + config_param.meta_ext), 'w', encoding='utf-8') as f:
            meta_doc.writexml(f, indent="\n",
                              addindent="\t", encoding='utf-8')

        # open meta define file
    cpp_file = open(os.path.join(res_path, config_param.cpp_meta_file), 'a', encoding='utf-8')
    cpp_file.write(u"//////////////////////////////////\n// Entity metadata\n")
    cs_file = open(os.path.join(res_path, config_param.cs_meta_file), 'a', encoding='utf-8')
    cs_file.write(u"\t//////////////////////////////////\n\t//Entity meta\n")

    #generate config
    with open(os.path.join(res_path, config_param.entity_class_file), 'w', encoding='utf-8') as f:
              config_doc.writexml(f, indent="\n", addindent="\t", encoding='utf-8')

    # print entity_dict
    # print entity_parent_dict
    # first of all, add parent entity classes
    for i in entity_parent_dict:
        parent_list = entity_parent_dict.get(i)
        if parent_list == None:
            continue
        for j in range(0, len(parent_list)):
            parent = parent_list[j]
            if not entity_dict.__contains__(parent):
                continue

            write_entity(cpp_file, parent, entity_dict, True)
            write_entity(cs_file, parent, entity_dict, False)

    # then other entity classes
    for k in entity_dict:
        # jump over parent meta classes
        find = False
        for p in entity_parent_dict:
            parent_list = entity_parent_dict.get(p)
            if parent_list == None:
                continue
            if k in parent_list:
                find = True
                break
        if find == True:
            continue
        #enum class
        write_entity_enum_head(cpp_file,k,True)
        write_entity_enum_head(cs_file,k,False)
        parent_list = entity_parent_dict.get(k)
        if parent_list != None:
            cpp_file.write("    // Parent entity fields.\n")
            for parent in parent_list:
                write_entity_enum_all_member(cpp_file, parent, entity_dict,True)
                write_entity_enum_all_member(cs_file, parent, entity_dict,False)

        cpp_file.write("\n    // Entity fields.\n")
        write_entity_enum_all_member(cpp_file, k, entity_dict,True)
        write_entity_enum_all_member(cs_file, k, entity_dict,False)
        write_entity_enum_end(cpp_file, k,True)
        write_entity_enum_end(cs_file, k,False)


        write_entity_cpp_head(cpp_file, k)
        write_entity_cs_head(cs_file, k)
        # parent class members
        parent_list = entity_parent_dict.get(k)
        if parent_list != None:
            cpp_file.write("    // Parent entity fields.\n")
            for parent in parent_list:
                write_entity_all_member(cpp_file, k,parent, entity_dict, True)
                write_entity_all_member(cs_file,k, parent, entity_dict, False)

        cpp_file.write("\n    // Entity fields.\n")
        write_entity_all_member(cpp_file,k, k, entity_dict, True)
        write_entity_all_member(cs_file,k, k, entity_dict, False)
        write_entity_cpp_end(cpp_file)
        write_entity_cs_end(cs_file)

    # sperate line
    cpp_file.write(u"//////////////////////////////////\n// Configuration metadata\n")
    cs_file.write(u"\t//////////////////////////////////\n\t//Config meta\n")
    # close meta define files
    cpp_file.close()
    cs_file.close()
    print("generate [%s] finished" % entity_filepath)


# generate single config
def genrate_single_config(res_path, excel_list, classname):
    path = excel_list[0]
    excel = config_excel.my_excel(path)
    excel.set_sheet_by_index(0)  # set default sheet
    min_row_no = excel.get_min_row_no()
    max_row_no = excel.get_max_row_no()+1
    min_col_no = excel.get_min_col_no()
    max_col_no = excel.get_max_col_no()+1

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
        # data_node.setAttribute(config_param.field_index,filename+"."+excel.get_cell_content(min_row_no, col))
        if data_node.hasAttributes():
            meta_root_node.appendChild(data_node)
    with open(os.path.join(res_path, config_param.meta_path, classname + config_param.meta_ext), 'w', encoding='utf-8') as f:
        meta_doc.writexml(f, indent="\n", addindent="\t", encoding='utf-8')

    '''config data xml'''
    config_doc = Document()
    config_root_node = config_doc.createElement('data')
    config_doc.appendChild(config_root_node)
    for filepath in excel_list:
        excel = config_excel.my_excel(filepath)
        excel.set_sheet_by_index(0)  # set default sheet
        min_row_no = excel.get_min_row_no()
        max_row_no = excel.get_max_row_no()+1
        min_col_no = excel.get_min_col_no()
        max_col_no = excel.get_max_col_no()+1
        for row in range(min_row_no + config_param.config_form_head_row, max_row_no):
            data_node = config_doc.createElement('data')
            config_root_node.appendChild(data_node)
            for col in range(min_col_no, max_col_no):
                # ignore empty cell in first row
                if excel.get_cell_content(min_row_no, col) == None:
                    continue
                data_node.setAttribute(str(excel.get_cell_content(min_row_no, col)),
                                   str(excel.get_cell_content(row, col)))

        with open(os.path.join(res_path, config_param.server_res_path, classname + config_param.config_ext), 'w', encoding='utf-8') as f:
            config_doc.writexml(
                f, indent="\n", addindent="\t", encoding='utf-8')

    # open meta define file
    cpp_file = open(os.path.join(res_path, config_param.cpp_meta_file), 'a', encoding='utf-8')
    cs_file = open(os.path.join(res_path, config_param.cs_meta_file), 'a', encoding='utf-8')
    cpp_classname = cpp_identifier(classname)
    cpp_file.write(u"enum class %s : std::uint32_t\n{\n    meta_empty,\n" % cpp_classname)
    cs_file.write(u"\tenum %s\n\t{\n\t\tmeta_empty,\n" % (classname))
    excel = config_excel.my_excel(excel_list[0])
    excel.set_sheet_by_index(0)  # set default sheet
    min_row_no = excel.get_min_row_no()
    max_row_no = excel.get_max_row_no()+1
    min_col_no = excel.get_min_col_no()
    max_col_no = excel.get_max_col_no()+1
    for col in range(min_col_no, max_col_no):
        field_name = str(excel.get_cell_content(min_row_no, col))
        cpp_file.write(u"    %s,\n" % cpp_identifier(field_name))
        cs_file.write(u"\t\t%s,\n" % (field_name))
    cpp_file.write(u"};\n\n")
    cpp_file.write(u"using " + cpp_classname + "_rep_type = std::underlying_type<"+ cpp_classname +">::type;\n\n")
    cs_file.write(u"\t}\n")

    cpp_file.write(u"class config_meta_" + cpp_classname + "\n{\npublic:\n")
    cpp_file.write(
        u'''    static const std::string& self_name() {
        static const std::string meta_%s = "%s";
        return meta_%s;
    }

''' % (cpp_classname, classname, cpp_classname))

    cs_file.write(u"\tpublic class AFConfigMeta" +
                  classname.capitalize() + "\n\t{\n")
    cs_file.write(
        u'''\t\tpublic static readonly String self_name = "%s";\n\n''' % classname)

    if(not entity_dict.__contains__(classname)):
        entity_dict[classname]=[]
    for col in range(min_col_no, max_col_no):
        field_name = str(excel.get_cell_content(min_row_no, col))
        type_name = str(excel.get_cell_content(min_row_no + 1, col))
        entity_dict[classname].append(field_name)
        entity_dict[classname].append(type_name)
        entity_dict[classname].append("")
        write_entity_cpp_member(cpp_file, classname, field_name, type_name)

        cs_file.write(u'''\t\tpublic static readonly String %s = "%s"; // %s\n''' %
                      (field_name, field_name, type_name))
        cs_file.write(u'''\t\tpublic static UInt32 %s_index = (UInt32)%s.%s; // %s\n''' %
                      (field_name, classname,field_name, field_name))

    cpp_file.write(u"};\n\n")
    cs_file.write(u"\t}\n\n")
    # close meta define files
    cpp_file.close()
    cs_file.close()
    return True

# generate excel to config
def generate_config(res_path):
    print("Start to generate config...")
    print("---------------------------------------")
    meta_doc = Document()
    meta_root_node = meta_doc.createElement('configs')
    meta_doc.appendChild(meta_root_node)
    file_list = os.listdir(os.path.join(res_path, config_param.excel_path))

    filename_dict={}
    for i in range(0, len(file_list)):
        file_path = os.path.join(
            res_path, config_param.excel_path, file_list[i])
        if not os.path.isfile(file_path):
            continue
        (filename, ext) = os.path.splitext(file_list[i])
        # file is entity_class or is not excel file
        if filename == config_param.special_file_name or (ext != config_param.excel_ext and ext != config_param.excel_new_ext):
            continue
        if filename.startswith("~$"):
            continue
        else:
            classname = os.path.splitext(filename)[0]
            if not filename_dict.__contains__(classname):
                filename_dict[classname] = []
            filename_dict[classname].append(file_path)

    for classname in filename_dict:
        ret = genrate_single_config(res_path, filename_dict[classname], classname)
        if ret == True:
            # insert into config_class.config
            config_node = meta_doc.createElement('config')
            config_node.setAttribute("id", classname)
            config_node.setAttribute("meta", os.path.join(
                config_param.meta_path, classname + config_param.meta_ext))
            config_node.setAttribute("res", os.path.join(
                config_param.server_res_path, classname + config_param.config_ext))
            meta_root_node.appendChild(config_node)
            print("generate file = [%s] success" % file_path)
        else:
            raise Exception("generate file = [%s] failed" % file_path)

    with open(os.path.join(res_path, config_param.config_class_file), 'w', encoding='utf-8') as f:
              meta_doc.writexml(f, indent="\n", addindent="\t", encoding='utf-8')
    print("---------------------------------------")
    print("generate config finished")

def field_name_map_index(file,enum_class_name,field_name,cpp_or_cs=True):
    if cpp_or_cs:
       enum_identifier = cpp_identifier(enum_class_name)
       file.write("                        {\"%s\", static_cast<%s_rep_type>(%s::%s)},\n" %
                  (field_name, enum_identifier, enum_identifier, cpp_identifier(field_name)))
    else:
       file.write("\t\t\t\t\t{\"%s\",(UInt32)%s.%s},\n" % (field_name,enum_class_name,field_name))

def field_name_map_index_all(file,enum_class_name,class_name,entity_dict,cpp_or_cs=True):
    if cpp_or_cs:
        member_list=entity_dict[class_name]
        for k in range(0,len(member_list),3):
            field_name = member_list[k]
            type_name = member_list[k+1]
            sub_class_name = member_list[k+2]
            if type_name == "class":
                field_name_map_index_all(file,enum_class_name,sub_class_name,entity_dict,cpp_or_cs=True)
            else:
                field_name_map_index(file,enum_class_name,field_name,cpp_or_cs=True)
    else:
        member_list=entity_dict[class_name]
        for k in range(0,len(member_list),3):
            field_name = member_list[k]
            type_name = member_list[k+1]
            sub_class_name = member_list[k+2]
            if type_name == "class":
                field_name_map_index_all(file,enum_class_name,sub_class_name,entity_dict,cpp_or_cs=False)
            else:
                field_name_map_index(file,enum_class_name,field_name,cpp_or_cs=False)

def field_name_map_index_head_and_end(res_path,entity_dict,cpp_or_cs=True):
    if cpp_or_cs:
        cpp_file = open(os.path.join(res_path, config_param.cpp_meta_file), 'a', encoding='utf-8')
        cpp_file.write(u"class meta_name_index\n{\n")
        cpp_file.write(u"public:\n")
        cpp_file.write(u"    static std::uint32_t get_index(const std::string& class_name, const std::string& field_name) {\n")
        cpp_file.write(u"        static const std::unordered_map<std::string, std::unordered_map<std::string, std::uint32_t>> class_name_index =\n        {\n")
        for class_name in entity_dict:
            cpp_file.write(u"            {\"%s\",\n                {\n" % (class_name))
            field_name_map_index_all(cpp_file,class_name,class_name,entity_dict,True)
            cpp_file.write("                }\n            },\n")
        cpp_file.write(u"        };\n")
        cpp_file.write(u"        auto iter_class = class_name_index.find(class_name);\n")
        cpp_file.write(u"        if (iter_class == class_name_index.end())\n        {\n            return 0;\n        }\n")
        cpp_file.write(u"        auto iter_field = iter_class->second.find(field_name);\n")
        cpp_file.write(u"        if (iter_field == iter_class->second.end())\n        {\n            return 0;\n        }\n")
        cpp_file.write(u"        return iter_field->second;\n")
        cpp_file.write(u"    }\n")
        cpp_file.write(u"};\n")
        cpp_file.close()
    else:
        cs_file = open(os.path.join(res_path, config_param.cs_meta_file), 'a', encoding='utf-8')
        cs_file.write(u"\tclass AFMetaNameIndex{\n")
        cs_file.write(u"\t\tDictionary<string, Dictionary<string,UInt32>> class_name_index = new Dictionary<string, Dictionary<string,UInt32>>{\n")
        for class_name in entity_dict:
            cs_file.write(u"\t\t\t{\"%s\",\n\t\t\t\tnew Dictionary<string,UInt32>{\n" % (class_name))
            field_name_map_index_all(cs_file,class_name,class_name,entity_dict,False)
            cs_file.write("\t\t\t\t}\n\t\t\t},\n")
        cs_file.write(u"\t\t};\n")
        cs_file.write(u"\t}\n")
        cs_file.close()


# write meta define files for cpp and c#
def write_meta_define_begin(res_path):
    with open(os.path.join(res_path, config_param.cpp_meta_file), 'w', encoding='utf-8') as cpp_file:
        cpp_file.write(u'''/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2020 ArkNX authors.
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

#include <cstdint>
#include <map>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace ark {

''')

    with open(os.path.join(res_path, config_param.cs_meta_file), 'w', encoding='utf-8') as cs_file:
        cs_file.write(u'''/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2020 ArkNX authors.
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
    with open(os.path.join(res_path, config_param.cpp_meta_file), 'a', encoding='utf-8') as cpp_file:
        cpp_file.write(u"\n} // namespace ark\n")

    with open(os.path.join(res_path, config_param.cs_meta_file), 'a', encoding='utf-8') as cs_file:
        cs_file.write(u"}")


    # main
if __name__ == "__main__":
    args = parse_args()
    res_path = args['path']
    print(u"res_path = " + res_path)
    # gen meta define files
    write_meta_define_begin(res_path)
    print("+++++++++++++++++++++++++++++++++++++++++")
    # {{class:field_name,type_name,sub_class_name}}
    entity_dict = {}
    # gen entity meta
    generate_entity_meta(res_path)
    # gen config meta
    generate_config(res_path)
    # name_index
    field_name_map_index_head_and_end(res_path,entity_dict,True)
    field_name_map_index_head_and_end(res_path,entity_dict,False)
    write_meta_define_end(res_path)

