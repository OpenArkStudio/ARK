using System;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using NPOI.HSSF.UserModel;
using NPOI.SS.UserModel;
using NPOI.XSSF.UserModel;


namespace config_tool
{
    class Program
    {

        static string base_path = "";
        static string excel_path = "excel/";
        static string meta_path = "meta/";
        static string server_res_path = "server/";
        static string client_res_path = "client/";
        static string config_class_file = "meta/config_class.config";
        static string cpp_meta_file = "proto/AFMetaDefine.hpp";
        static string cs_meta_file = "proto/AFMetaDefine.cs";
        static string meta_ext = ".meta";
        static string config_ext = ".config";
        static string csv_ext = ".csv";
        static int form_head_row = 5;

        static string ignore_file = "entity_class";

        static StreamWriter cpp_writer;
        static StreamWriter cs_writer;

        static string target_all = "A";     //for all
        static string target_server = "S";  //for server side
        static string target_client = "C";  //for client
        static string target_design = "D";  //for design

        static void create_proto_file()
        {
            //cpp
            if (File.Exists(base_path + cpp_meta_file))
            {
                File.Delete(base_path + cpp_meta_file);
            }
            
            cpp_writer = new StreamWriter(base_path + cpp_meta_file);

            string cpp_header = @"/*
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
{
";
            cpp_writer.WriteLine(cpp_header);

/////////////////////////////////////////////////////////////
            //c#
            if (File.Exists(base_path + cs_meta_file))
            {
                File.Delete(base_path + cs_meta_file);
            }

            cs_writer = new StreamWriter(base_path + cs_meta_file);
            string cs_header = @"/*
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
{
";
            cs_writer.WriteLine(cs_header);
        }

        static bool create_single_config(string file, string file_name)
        {
            Console.WriteLine("Processing [" + file + "]");

            XSSFWorkbook work_book;
            using (FileStream fs = new FileStream(file, FileMode.Open, FileAccess.Read))
            {
                work_book = new XSSFWorkbook(fs);
            }

            if (work_book == null)
            {
                return false;
            }
            
            //XmlDocument xml_doc = new XmlDocument();
            //XmlDeclaration xml_decl;
            //xml_decl = xml_doc.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            //xml_doc.AppendChild(xml_decl);

            //XmlElement root = xml_doc.CreateElement("", "data", "");
            //xml_doc.AppendChild(root);

            //StreamWriter csv_writer = new StreamWriter(base_path + client_res_path + file_name + csv_ext);
            //string csv_header = "";
            //string csv_content = "";

            
            var server_config_meta_xml = new XmlDocument();
            XmlDeclaration serever_config_meta_decl;
            serever_config_meta_decl = server_config_meta_xml.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            server_config_meta_xml.AppendChild(serever_config_meta_decl);
            var server_config_meta_root = server_config_meta_xml.CreateElement("", "data", "");
            server_config_meta_xml.AppendChild(server_config_meta_root);
            ///////////////////////////////////////////////
            var server_config_xml = new XmlDocument();
            XmlDeclaration server_config_decl;
            server_config_decl = server_config_xml.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            server_config_xml.AppendChild(server_config_decl);
            var server_config_root = server_config_xml.CreateElement("", "data", "");
            server_config_xml.AppendChild(server_config_root);

            for (int i = 0; i < work_book.NumberOfSheets; ++i)
            {
                var sheet = work_book.GetSheetAt(i);

                var field_row = sheet.GetRow(sheet.FirstRowNum);
                var type_row = sheet.GetRow(sheet.FirstRowNum + 1);
                //////////////////////////////////////////////////////
                {
                    //form header
                    var target_row = sheet.GetRow(sheet.FirstRowNum + 2);
                    var refer_row = sheet.GetRow(sheet.FirstRowNum + 3);

                    for (int col = field_row.FirstCellNum; col < field_row.LastCellNum; ++col)
                    {
                        var target = target_row.GetCell(col).StringCellValue.ToUpper();
                        if (target == target_all || target == target_server)
                        {
                            XmlElement node = server_config_meta_xml.CreateElement("", "meta", "");
                            server_config_meta_root.AppendChild(node);
                            node.SetAttribute("field", field_row.GetCell(col).StringCellValue);
                            node.SetAttribute("type", type_row.GetCell(col).StringCellValue);
                            node.SetAttribute("refer", refer_row.GetCell(col).StringCellValue);
                        }
                    }
                }

                //////////////////////////////////////////////////////
                {
                    //form content
                    for (int row = sheet.FirstRowNum + form_head_row; row < sheet.LastRowNum; ++row)
                    {
                        var config_row = sheet.GetRow(row);
                        XmlElement node = server_config_xml.CreateElement("", "config", "");
                        server_config_root.AppendChild(node);
                        for (int col = field_row.FirstCellNum; col < field_row.LastCellNum; ++col)
                        {
                            var config_cell = config_row.GetCell(col);
                            if (config_cell == null)
                            {
                                var type = type_row.GetCell(col).StringCellValue;
                                switch (type)
                                {
                                    case "string":
                                        node.SetAttribute(field_row.GetCell(col).StringCellValue, "");
                                        break;
                                    case "uint32":
                                    case "int32":
                                    case "uint64":
                                    case "int64":
                                        node.SetAttribute(field_row.GetCell(col).StringCellValue, "0");
                                        break;
                                    default:
                                        node.SetAttribute(field_row.GetCell(col).StringCellValue, "");
                                        break;
                                }

                                continue;
                            }

                            string config_value = "";
                            switch (config_cell.CellType)
                            {
                                case CellType.Boolean:
                                    config_value = config_cell.BooleanCellValue.ToString();
                                    break;
                                case CellType.String:
                                    config_value = config_cell.StringCellValue;
                                    break;
                                case CellType.Numeric:
                                    config_value = config_cell.NumericCellValue.ToString();
                                    break;
                                default:
                                    break;
                            }
                            node.SetAttribute(field_row.GetCell(col).StringCellValue, config_value);
                        }
                    }
                }
            }
            
            StreamWriter server_config_meta_steam = new StreamWriter(base_path + meta_path + file_name + meta_ext, false, new UTF8Encoding(false));
            server_config_meta_xml.Save(server_config_meta_steam);
            server_config_meta_steam.Close();

            StreamWriter server_config_steam = new StreamWriter(base_path + server_res_path + file_name + config_ext, false, new UTF8Encoding(false));
            server_config_xml.Save(server_config_steam);
            server_config_steam.Close();

            return true;
        }

        static void create_config_file()
        {
            string cur_dir = Directory.GetCurrentDirectory();

            //gen entity_class.meta
            XmlDocument xml_doc = new XmlDocument();
            XmlDeclaration xml_decl = xml_doc.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            xml_doc.AppendChild(xml_decl);

            XmlElement root = xml_doc.CreateElement("", "data", "");
            xml_doc.AppendChild(root);

            String[] all_excel = Directory.GetFiles(base_path + excel_path, "*", SearchOption.AllDirectories);
            foreach (string file in all_excel)
            {
                //TODO:此处要考虑多个同名文件，但是归属不同人的表格
                int last_point = file.LastIndexOf(".") + 1;
                int last_slash = file.LastIndexOf("/") + 1;

                string file_name = file.Substring(last_slash, last_point - last_slash - 1);
                string file_ext = file.Substring(last_point, file.Length - last_point);

                int tmp_excel_pos = file.IndexOf("$");
                if (tmp_excel_pos >= 0)
                {
                    //ignore temp file named like $xxx.xlsx
                    continue;
                }

                if (file_ext != "xls" && file_ext != "xlsx")
                {
                    continue;
                }

                if (file_name == ignore_file)
                {
                    continue;
                }

                if (!create_single_config(file, file_name))
                {
                    Console.WriteLine("Create " + file + " failed, please check it!!!");
                    return;
                }

                XmlElement node = xml_doc.CreateElement("", "config", "");
                root.AppendChild(node);
                node.SetAttribute("id", file_name);
                node.SetAttribute("meta", meta_path + file_name + meta_ext);
                node.SetAttribute("res", server_res_path + file_name + config_ext);
            }

            StreamWriter sw = new StreamWriter(base_path + config_class_file, false, new UTF8Encoding(false));
            xml_doc.Save(sw);
            sw.Close();
        }

        static void create_entity_meta()
        {
            Console.WriteLine("Start to generate entity meta files");

            //gen proto file for cpp & c#
            create_proto_file();

            //gen entity_class.meta
            XmlDocument xml_doc = new XmlDocument();
            XmlDeclaration xml_decl = xml_doc.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            xml_doc.AppendChild(xml_decl);

            XmlElement root = xml_doc.CreateElement("", "data", "");
            xml_doc.AppendChild(root);

            //TODO:
            
            cpp_writer.Close();
            cs_writer.Close();
        }

        static void Main(string[] args)
        {
            if (args.Length != 1)
            {
                Console.WriteLine("Usage: config_tool.exe resource_path, expected the path of resource folder");
                Console.WriteLine("Press any key to quit");
                Console.ReadKey();
                return;
            }

            base_path = args[0];

            var now = DateTime.Now;

            create_config_file();
            create_entity_meta();
            
            var time_dis = DateTime.Now - now;
            Console.WriteLine("Generate all files successful, use time = {0:3}s", string.Format("{0:N3}", time_dis.TotalSeconds));
            Console.Write("Press any key to quit...");
            Console.ReadKey();
        }
    }
}
