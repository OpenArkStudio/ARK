#!/usr/bin/python
# encoding=utf-8

# resource path
resource_path = "../../resource"

# special file
special_file_name = "entity_class"
special_file = "entity_class.xlsx"

# some extensions
excel_ext = ".xls"
excel_new_ext = ".xlsx"

meta_ext = ".meta"
config_ext = ".config"
csv_ext = ".csv"

# target for different staff
target_all = "A"  # for all
target_server = "S"  # for server side
target_client = "C"  # for client
target_design = "D"  # for design

# some path
excel_path = "excel/"
meta_path = "meta/"
server_res_path = "server/"
client_res_path = "client/"
config_class_file = "meta/config_class.config"
entity_class_file = "meta/entity_class.config"
cpp_meta_file = "code/AFMetaDefine.hpp"
cs_meta_file = "code/AFMetaDefine.cs"

# excel form header, after this row, the content is config data
config_form_head_list = ['', 'field', 'type', 'target', 'refer', 'desc']
config_form_head_row = 5

#field_index
field_index="field_index"

# entity class form header
entity_form_head_row = 3
entity_unused_flag = 'unused'

entity_field_name_col = 2
entity_field_type_col = 3
entity_field_sub_class_col = 4

nonentity_form_head_row = 5
 