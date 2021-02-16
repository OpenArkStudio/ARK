#!/usr/bin/python
# encoding=utf-8

# author: NickYang
# date: 2019/04/02

import xml.etree.ElementTree as et


def read_xml(in_path):
    '''
    read and parse xml file
    in_path: xml filepath
    return: ElementTree
    '''
    #print u'reading xml file: ' + in_path
    tree = et.parse(in_path)
    return tree


def create_xml_tree(root):
    tree = et.ElementTree(element=root)
    return tree


def write_xml(tree, out_path):
    '''
    write the xml data to file
    tree: ElementTree
    out_path: output filepath
    '''
    tree.write(out_path, encoding='UTF-8', xml_declaration=True)


def if_match(node, kv_map):
    '''
    check the node is in kv_map 
    node: the node
    kv_map: the k-v map
    '''
    for key in kv_map:
        if node.get(key) != kv_map.get(key):
            return False
    return True


def find_nodes(tree, path):
    '''
    find all nodes in a tree path
    tree: xml ElementTree
    path: node path
    '''
    return tree.findall(path)


def get_node_by_keyvalue(nodelist, kv_map):
    '''
    find all nodes that match the key-value
    nodelist: node list
    kv_map: property map
    return: all matched nodes
    '''
    result_nodes = []
    for node in nodelist:
        if if_match(node, kv_map):
            result_nodes.append(node)
    return result_nodes


def change_node_properties(nodelist, kv_map, delete=False):
    '''
    change/add/delete a node's property
    nodelist: node list
    kv_map: property map
    '''
    for node in nodelist:
        for key in kv_map:
            if delete:
                if key in node.attrib:
                    del node.attrib[key]
            else:
                node.set(key, kv_map.get(key))


def change_node_text(nodelist, text, is_add=False, is_delete=False):
    '''
    change/add/delete a node text
    nodelist: node list
    text: the text after updated
    '''
    for node in nodelist:
        if is_add:
            node.text += text
        elif is_delete:
            node.text = ""
        else:
            node.text = text


def create_node(tag, property_map, content=""):
    '''
    create a node
    tag: node tag 
    property_map: property map 
    content: node content
    return: the new node
    '''
    element = et.Element(tag, property_map)
    element.text = content
    return element


def add_child_node(nodelist, element):
    '''
    add a sub-node in a node list
    nodelist: node list 
    element: sub node
    '''
    for node in nodelist:
        node.append(element)


def del_node_by_tagkeyvalue(nodelist, tag, kv_map):
    '''
    delete all node if the tag all key-value matched.
    nodelist: parent node list
    tag: sub node tag name
    kv_map: key-value map
    '''
    for parent_node in nodelist:
        children = parent_node.getchildren()
        for child in children:
            if child.tag == tag and if_match(child, kv_map):
                parent_node.remove(child)
