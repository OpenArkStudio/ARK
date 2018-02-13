using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace FileProcessor
{
    class Program
    {
        static string strBasePath = "DataConfig/";
        static string strExcelPath = "Excel/";
        static string strStructPath = "Struct/";
        static string strResPath = "Resource/";
        static string strLogiClassFile = "../Struct/LogicClass.xml";

        static string strToolBasePath = "../";
        static string strCPPFile = "../proto/ARKDataDefine.hpp";
        static string strCSFile = "../proto/ARKDataDefine.cs";

        static StreamWriter cppWriter;
        static StreamWriter csWriter;

        static void CreateProtoFile()
        {
            if (File.Exists(strCPPFile))
            {
                File.Delete(strCPPFile);
            }

            if (File.Exists(strCSFile))
            {
                File.Delete(strCSFile);
            }

            cppWriter = new StreamWriter(strCPPFile);

            string strCPPHead = @"/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

namespace ARK
{";
            cppWriter.Write(strCPPHead);

            csWriter = new StreamWriter(strCSFile);
            string strCSHead = @"/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
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

namespace ARK
{";
            csWriter.Write(strCSHead);

            //TODO:
            cppWriter.Close();
            csWriter.Close();
        }

        static void CreateStructFile()
        {
            Console.WriteLine("Start to generate struct files");

            //generate proto file for C++ & C#
            CreateProtoFile();

            //load all excel
            XmlDocument xmlDoc = new XmlDocument();
            XmlDeclaration xmlDecl;
            xmlDecl = xmlDoc.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            xmlDoc.AppendChild(xmlDecl);

            XmlElement root = xmlDoc.CreateElement("", "XML", "");
            xmlDoc.AppendChild(root);

            XmlElement classElement = xmlDoc.CreateElement("", "Class", "");
            root.AppendChild(classElement);
            classElement.SetAttribute("Id", "IObject");
            classElement.SetAttribute("Type", "TYPE_IOBJECT");
            classElement.SetAttribute("Path", "../../DataConfig/Struct/Class/IObject.xml");
            classElement.SetAttribute("InstancePath", "../../DataConfig/Resource/IObject.xml");
            classElement.SetAttribute("Public", "0");
            classElement.SetAttribute("Desc", "IObject");

            String[] xStructXLSXList = Directory.GetFiles(strToolBasePath + strExcelPath, "*", SearchOption.AllDirectories);
            foreach (string file in xStructXLSXList)
            {
                //TODO:此处要考虑多个同名文件，但是归属不同人的表格
                int nLastPoint = file.LastIndexOf(".") + 1;
                int nLastSlash = file.LastIndexOf("/") + 1;
                string strFileName = file.Substring(nLastSlash, nLastPoint - nLastSlash - 1);
                string strFileExt = file.Substring(nLastPoint, file.Length - nLastPoint);

                int nTempExcelPos = file.IndexOf("$");
                if (nTempExcelPos >= 0)
                {
                    // 打开excel之后生成的临时文件，略过
                    continue;
                }

                // 不是excel文件，默认跳过
                if (strFileExt != "xls" && strFileExt != "xlsx")
                {
                    continue;
                }

                // 单个excel文件转为xml
                if (!CreateStructXML(file))
                {
                    Console.WriteLine("Create " + file + " failed, please check it!!!");
                    return;
                }

                // 是IObject.xlsx跳过
                if (strFileName == "IObject")
                {
                    continue;
                }

                // LogicClass.xml中的索引配置
                XmlElement subClassElement = xmlDoc.CreateElement("", "Class", "");
                classElement.AppendChild(subClassElement);

                subClassElement.SetAttribute("Id", strFileName);
                subClassElement.SetAttribute("Type", "TYPE_" + strFileName.ToUpper());
                subClassElement.SetAttribute("Path", strBasePath + strXMLStructPath + strFileName + ".xml");
                subClassElement.SetAttribute("InstancePath", strBasePath + strXMLIniPath + strFileName + ".xml");
                subClassElement.SetAttribute("Public", "0");
                subClassElement.SetAttribute("Desc", strFileName);

                //TODO:
            }
        }

        static void CreateResFile()
        {

        }

        static void Main(string[] args)
        {
            CreateStructFile();
            CreateResFile();

            Console.WriteLine("Generate all files successful.");
            Console.Write("Press any key to quit...");
            Console.ReadKey();
        }
    }
}
