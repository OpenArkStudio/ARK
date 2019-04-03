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
#from xml.dom import minidom


import config_param
import config_xml
import config_excel

reload(sys)
sys.setdefaultencoding("utf-8")

'''
class _MinidomHooker(object):
    def __enter__(self):
        minidom.NamedNodeMap.keys_orig = minidom.NamedNodeMap.keys
        minidom.NamedNodeMap.keys = self._NamedNodeMap_keys_hook
        return self

    def __exit__(self, *args):
        minidom.NamedNodeMap.keys = minidom.NamedNodeMap.keys_orig
        del minidom.NamedNodeMap.keys_orig

    @staticmethod
    def _NamedNodeMap_keys_hook(node_map):
        class OrderPreservingList(list):
            def sort(self):
                pass
        return OrderPreservingList(node_map.keys_orig())
'''


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
    print "---------------------------------------"
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
            if excel.get_cell_content(row, col) == None:
                data_node.setAttribute(
                    str(excel.get_cell_content(min_row_no, col)), "")
            else:
                data_node.setAttribute(str(excel.get_cell_content(min_row_no, col)),
                                       str(excel.get_cell_content(row, col)))
    with open(os.path.join(res_path, config_param.meta_path, config_param.special_file_name + config_param.meta_ext), 'w') as f:
        # with _MinidomHooker():
        meta_doc.writexml(f, indent="\n", addindent="\t", encoding='utf-8')
    print "---------------------------------------"
    print "finish to generate [%s]" % entity_filepath


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
        # with _MinidomHooker():
        meta_doc.writexml(f, indent="\n", addindent="\t", encoding='utf-8')

    '''config data xml'''
    config_doc = Document()
    config_root_node = config_doc.createElement('data')
    config_doc.appendChild(config_root_node)
    for row in range(min_row_no + config_param.config_form_head_row, max_row_no):
        data_node = config_doc.createElement('data')
        config_root_node.appendChild(data_node)
        for col in range(min_col_no, max_col_no):
            data_node.setAttribute(str(excel.get_cell_content(min_row_no, col)),
                                   str(excel.get_cell_content(row, col)))

    with open(os.path.join(res_path, config_param.server_res_path, filename + config_param.config_ext), 'w') as f:
        # with _MinidomHooker():
        config_doc.writexml(f, indent="\n", addindent="\t", encoding='utf-8')

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
        # with _MinidomHooker():
        meta_doc.writexml(f, indent="\n", addindent="\t", encoding='utf-8')
    print "---------------------------------------"
    print "generate config finished"


# main
if __name__ == "__main__":
    args = parse_args()
    res_path = args['path']
    print u"res_path = " + res_path
    generate_entity_meta(res_path)
    print "+++++++++++++++++++++++++++++++++++++++++"
    generate_config(res_path)
