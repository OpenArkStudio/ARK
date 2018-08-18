using NPOI.HSSF.UserModel;
using NPOI.SS.UserModel;
using NPOI.XSSF.UserModel;
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
        static string strBasePath = "resource/";
        static string strExcelPath = "excel/";
        static string strSchemaPath = "schema/class/";
        static string strResPath = "res/";
        static string strClientResPath = "client/";
        static string strToolBasePath = "../";

        static string strLogiClassFile = "../schema/LogicClass.xml";

        static string strCPPFile = "../proto/ARKDataDefine.hpp";
        static string strCSFile = "../proto/ARKDataDefine.cs";

        static StreamWriter cppWriter;
        static StreamWriter csWriter;

        static string strCPPIObjectInfo;
        static string strCSIObjectInfo;

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
* Copyright (c) 2013-2018 ArkGame authors.
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

namespace ARK
{";
            cppWriter.WriteLine(strCPPHead);

            csWriter = new StreamWriter(strCSFile);
            string strCSHead = @"/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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
            csWriter.WriteLine(strCSHead);
        }

        static void CreateSchemaFile()
        {
            Console.WriteLine("Start to generate schema files");

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
            classElement.SetAttribute("Path", strBasePath + strSchemaPath + "IObject.xml");
            classElement.SetAttribute("ResPath", strBasePath + strResPath + "IObject.xml");
            classElement.SetAttribute("Public", "0");
            classElement.SetAttribute("Desc", "IObject");

            // 提前把IObject跑一边
            CreateSchemaXML(strToolBasePath + strExcelPath + "IObject.xlsx", "IObject");

            String[] xSchemaXLSXList = Directory.GetFiles(strToolBasePath + strExcelPath, "*", SearchOption.AllDirectories);
            foreach (string file in xSchemaXLSXList)
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

                // 是IObject.xlsx跳过
                if (strFileName == "IObject")
                {
                    continue;
                }

                // 单个excel文件转为xml
                if (!CreateSchemaXML(file, strFileName))
                {
                    Console.WriteLine("Create " + file + " failed, please check it!!!");
                    return;
                }

                // LogicClass.xml中的索引配置
                XmlElement subClassElement = xmlDoc.CreateElement("", "Class", "");
                classElement.AppendChild(subClassElement);

                subClassElement.SetAttribute("Id", strFileName);
                subClassElement.SetAttribute("Type", "TYPE_" + strFileName.ToUpper());
                subClassElement.SetAttribute("Path", strBasePath + strSchemaPath + strFileName + ".xml");
                subClassElement.SetAttribute("ResPath", strBasePath + strResPath + strFileName + ".xml");
                subClassElement.SetAttribute("Public", "0");
                subClassElement.SetAttribute("Desc", strFileName);
            }

            cppWriter.WriteLine("}");
            cppWriter.Close();

            csWriter.WriteLine("}");
            csWriter.Close();

            xmlDoc.Save(strLogiClassFile);
        }

        static bool CreateSchemaXML(string file, string strFileName)
        {
            Console.WriteLine("Processing [" + file + "]");

            string curDir = Directory.GetCurrentDirectory();
            XSSFWorkbook workBook;
            using (FileStream fs = new FileStream(file, FileMode.Open, FileAccess.Read))
            {
                workBook = new XSSFWorkbook(fs);
            }

            if (workBook == null)
            {
                return false;
            }

            // 开始创建xml
            XmlDocument schemaDoc = new XmlDocument();
            XmlDeclaration xmlDecl;
            xmlDecl = schemaDoc.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            schemaDoc.AppendChild(xmlDecl);

            // 写入XML root标签
            XmlElement root = schemaDoc.CreateElement("", "XML", "");
            schemaDoc.AppendChild(root);

            // 写入Propertys标签
            XmlElement dataNodes = schemaDoc.CreateElement("", "DataNodes", "");
            root.AppendChild(dataNodes);

            // 写入Records标签
            XmlElement dataTables = schemaDoc.CreateElement("", "DataTables", "");
            root.AppendChild(dataTables);

            //cpp
            string strCPPInfo = "";
            string strCPPTableInfo = "";
            string strCPPEnumInfo = "";
            strCPPInfo = strCPPInfo + "class " + strFileName + "\n{\npublic:\n";
            strCPPInfo = strCPPInfo + "\t//Class name\n\tstatic const std::string& ThisName(){ static std::string x" + strFileName + " = \"" + strFileName + "\";" + " return x" + strFileName + "; }\n" + "\t//IObject\n" + strCPPIObjectInfo + "\t//DataNodes\n";

            //C#
            string strCSInfo = "";
            string strCSTableInfo = "";
            string strCSEnumInfo = "";

            strCSInfo = strCSInfo + "public class " + strFileName + "\n{\n";
            strCSInfo = strCSInfo + "\t//Class name\n\tpublic static readonly string ThisName = \"" + strFileName + "\";\n\t//IObject\n" + strCSIObjectInfo + "\t//DataNodes\n";

            for (int i = 0; i < workBook.NumberOfSheets; ++i)
            {
                ISheet sheet = workBook.GetSheetAt(i);
                string sheetSubName = sheet.SheetName;
                int dash_pos = sheet.SheetName.IndexOf('_');
                if (dash_pos != -1)
                {
                    sheetSubName = sheet.SheetName.Substring(0, dash_pos);
                }

                sheetSubName = sheetSubName.ToLower();
                switch (sheetSubName)
                {
                    case "datanode":
                        //Data nodes
                        {
                            IRow headerRow = sheet.GetRow(sheet.FirstRowNum);

                            List<string> colNames = new List<string>();
                            for (int row = sheet.FirstRowNum; row <= sheet.FirstRowNum + 7; ++row)
                            {
                                ICell cellData = sheet.GetRow(row).GetCell(headerRow.FirstCellNum);
                                colNames.Add(cellData.StringCellValue);
                            }

                            for (int col = headerRow.FirstCellNum + 1; col < headerRow.LastCellNum; ++col)
                            {
                                var dataNode = schemaDoc.CreateElement("", "DataNode", "");
                                dataNodes.AppendChild(dataNode);

                                string strDataNodeName = headerRow.GetCell(col).StringCellValue;
                                if (strDataNodeName == "")
                                {
                                    continue;
                                }

                                string strNodeType = "";
                                for (int row = sheet.FirstRowNum; row < sheet.FirstRowNum + 7; ++row)
                                {
                                    var name = colNames.ElementAt(row);

                                    IRow rowData = sheet.GetRow(row);
                                    ICell cellData = rowData.GetCell(col);
                                    if (cellData == null)
                                    {
                                        continue;
                                    }

                                    string strValue = "";
                                    switch (cellData.CellType)
                                    {
                                        case CellType.Boolean:
                                            {
                                                strValue = cellData.BooleanCellValue.ToString();
                                                if (strValue == "TRUE" || strValue == "true" || strValue == "True")
                                                {
                                                    strValue = "1";
                                                }
                                                else
                                                {
                                                    strValue = "0";
                                                }
                                            }
                                            break;
                                        case CellType.Numeric:
                                            strValue = cellData.NumericCellValue.ToString();
                                            break;
                                        case CellType.String:
                                            {
                                                strValue = cellData.StringCellValue;
                                                if (name == "Type")
                                                {
                                                    strNodeType = strValue;
                                                }
                                            }
                                            break;
                                        default:
                                            break;
                                    }

                                    dataNode.SetAttribute(name, strValue);
                                }

                                //hpp & cs
                                strCPPInfo += "\t" + "static const std::string& " + strDataNodeName + "() { static std::string x" + strDataNodeName + " = \"" + strDataNodeName + "\"; return x" + strDataNodeName + "; } //" + strNodeType + "\n";
                                strCSInfo += "\t" + "public static readonly String " + strDataNodeName + " = \"" + strDataNodeName + "\"; //" + strNodeType + "\n";

                                if (strFileName == "IObject")
                                {
                                    strCPPIObjectInfo += "\t" + "static const std::string& " + strDataNodeName + "(){ static std::string x" + strDataNodeName + " = \"" + strDataNodeName + "\";" + " return x" + strDataNodeName + "; } //" + strNodeType + "\n";
                                    strCSIObjectInfo += "\t" + "public static readonly String " + strDataNodeName + " = \"" + strDataNodeName + "\"; //" + strNodeType + "\n";
                                }
                            }
                        }
                        break;
                    case "component":
                        //Do nothing
                        break;
                    default:
                        //Data tables
                        {
                            int nRowsCount = sheet.LastRowNum - sheet.FirstRowNum + 1;
                            int nTableCount = nRowsCount / 10;

                            if (nRowsCount != nTableCount * 10)
                            {
                                Console.WriteLine("This Excel[{0}]'s DataTable is something wrong, Sheet[{1}] Total Rows is {2}, Not 10*N", file, sheet.SheetName, nRowsCount);
                                return false;
                            }

                            int infoCell = 1;
                            for (int nTable = 0; nTable < nTableCount; ++nTable)
                            {
                                string strTableName = sheet.GetRow(nTable * 9 + nTable).GetCell(infoCell).StringCellValue;
                                string strRow = sheet.GetRow(nTable * 9 + 1 + nTable).GetCell(infoCell).NumericCellValue.ToString();
                                string strCol = sheet.GetRow(nTable * 9 + 2 + nTable).GetCell(infoCell).NumericCellValue.ToString();
                                string strPublic = sheet.GetRow(nTable * 9 + 3 + nTable).GetCell(infoCell).BooleanCellValue.ToString();
                                if (strPublic == "TRUE" || strPublic == "True" || strPublic == "true")
                                {
                                    strPublic = "1";
                                }
                                else
                                {
                                    strPublic = "0";
                                }

                                string strPrivate = sheet.GetRow(nTable * 9 + 4 + nTable).GetCell(infoCell).BooleanCellValue.ToString();
                                if (strPrivate == "TRUE" || strPrivate == "True" || strPrivate == "true")
                                {
                                    strPrivate = "1";
                                }
                                else
                                {
                                    strPrivate = "0";
                                }

                                string strSave = sheet.GetRow(nTable * 9 + 5 + nTable).GetCell(infoCell).BooleanCellValue.ToString();
                                if (strSave == "TRUE" || strSave == "True" || strSave == "true")
                                {
                                    strSave = "1";
                                }
                                else
                                {
                                    strSave = "0";
                                }

                                string strCache = sheet.GetRow(nTable * 9 + 6 + nTable).GetCell(infoCell).BooleanCellValue.ToString();
                                if (strCache == "TRUE" || strCache == "True" || strCache == "true")
                                {
                                    strCache = "1";
                                }
                                else
                                {
                                    strCache = "0";
                                }

                                string strDesc = "";
                                ICell descCell = sheet.GetRow(nTable * 9 + 9 + nTable).GetCell(infoCell);
                                if (descCell != null)
                                {
                                    strDesc = descCell.StringCellValue;
                                }

                                int nExcelCols = Int32.Parse(strCol);
                                int nRealCols = 0;

                                XmlElement tableNode = schemaDoc.CreateElement("DataTable");
                                dataTables.AppendChild(tableNode);

                                tableNode.SetAttribute("Id", strTableName);
                                tableNode.SetAttribute("Row", strRow);
                                tableNode.SetAttribute("Col", strCol);
                                tableNode.SetAttribute("Public", strPublic);
                                tableNode.SetAttribute("Private", strPublic);
                                tableNode.SetAttribute("Save", strSave);
                                tableNode.SetAttribute("Cache", strCache);
                                tableNode.SetAttribute("Desc", strDesc);

                                strCPPTableInfo = strCPPTableInfo + "\tstatic const std::string& R_" + strTableName + "(){ static std::string x" + strTableName + " = \"" + strTableName + "\";" + " return x" + strTableName + ";}\n";
                                strCPPEnumInfo = strCPPEnumInfo + "\n\tenum " + strTableName + "\n\t{\n";

                                strCSTableInfo = strCSTableInfo + "\tpublic static readonly String R_" + strTableName + " = \"" + strTableName + "\";\n";
                                strCSEnumInfo = strCSEnumInfo + "\n\tpublic enum " + strTableName + "\n\t{\n";

                                var tableFieldRow = sheet.GetRow(nTable * 9 + 7 + nTable);
                                var tableFieldTypeRow = sheet.GetRow(nTable * 9 + 8 + nTable);
                                for (int nTableCol = tableFieldRow.FirstCellNum; nTableCol < tableFieldRow.LastCellNum; nTableCol++)
                                {
                                    string strField = tableFieldRow.GetCell(nTableCol).StringCellValue;
                                    string strType = tableFieldTypeRow.GetCell(nTableCol).StringCellValue;

                                    XmlElement colNode = schemaDoc.CreateElement("Col");
                                    tableNode.AppendChild(colNode);

                                    colNode.SetAttribute("Name", strField);
                                    colNode.SetAttribute("Type", strType);

                                    strCPPEnumInfo += "\t\t" + strTableName + "_" + strField + "\t\t= " + string.Format("{0}", nTableCol) + ", //" + strField + " -> " + strType + "\n";
                                    strCSEnumInfo += "\t\t" + strField + "\t\t= " + string.Format("{0}", nTableCol) + ", //" + strField + " -> " + strType + "\n";

                                    nRealCols++;
                                }

                                if (nExcelCols != nRealCols)
                                {
                                    Console.WriteLine("This Excel[{0}]'s format is something wrong, DataTable[{1}] field \"col\"== {2} DO NOT equal the real cols == {3}!", file, strTableName, nExcelCols, nRealCols);
                                    return false;
                                }

                                strCPPEnumInfo += "\n\t};\n";
                                strCSEnumInfo += "\n\t};\n";
                            }
                            break;
                        }
                }
            }

            // cpp
            strCPPInfo += "\t//DataTables\n" + strCPPTableInfo + strCPPEnumInfo + "\n};\n";
            cppWriter.WriteLine(strCPPInfo);

            // C#
            strCSInfo += "\t//DataTables\n" + strCSTableInfo + strCSEnumInfo + "\n}\n";
            csWriter.WriteLine(strCSInfo);

            string strXMLFile = strToolBasePath + strSchemaPath + strFileName + ".xml";
            schemaDoc.Save(strXMLFile);

            return true;
        }

        static void CreateResFile()
        {
            Console.WriteLine("Start to generate resource files");
            
            String[] xIniXLSXList = Directory.GetFiles(strToolBasePath + strExcelPath, "*", SearchOption.AllDirectories);
            foreach (string file in xIniXLSXList)
            {
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

                if (!CreateResXML(file))
                {
                    Console.WriteLine("Create " + file + " failed, please check it!!!");
                    return;
                }
            }
        }

        static bool CreateResXML(string file)
        {
            Console.WriteLine("Processing [" + file + "]");

            int nLastPoint = file.LastIndexOf(".") + 1;
            int nLastSlash = file.LastIndexOf("/") + 1;
            string strFileName = file.Substring(nLastSlash, nLastPoint - nLastSlash - 1);
            string strFileExt = file.Substring(nLastPoint, file.Length - nLastPoint);

            string curDir = Directory.GetCurrentDirectory();
            XSSFWorkbook workBook;
            using (FileStream fs = new FileStream(file, FileMode.Open, FileAccess.Read))
            {
                workBook = new XSSFWorkbook(fs);
            }

            if (workBook == null)
            {
                return false;
            }

            XmlDocument resDoc = new XmlDocument();
            XmlDeclaration xmlDecl;
            xmlDecl = resDoc.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            resDoc.AppendChild(xmlDecl);

            XmlElement root = resDoc.CreateElement("", "XML", "");
            resDoc.AppendChild(root);

            StreamWriter csvWriter = new StreamWriter(strToolBasePath + strClientResPath + strFileName + ".csv");
            string csvHeader = "";
            string csvContent = "";

            for (int i = 0; i < workBook.NumberOfSheets; ++i)
            {
                ISheet sheet = workBook.GetSheetAt(i);
                string sheetSubName = sheet.SheetName;
                int dash_pos = sheet.SheetName.IndexOf('_');
                if (dash_pos != -1)
                {
                    sheetSubName = sheet.SheetName.Substring(0, dash_pos);
                }

                sheetSubName = sheetSubName.ToLower();
                if (sheetSubName != "datanode")
                {
                    continue;
                }

                IRow headerRow = sheet.GetRow(sheet.FirstRowNum);
                List<string> colNames = new List<string>();
                for (int col = headerRow.FirstCellNum; col < headerRow.LastCellNum; ++col)
                {
                    ICell cellData = headerRow.GetCell(col);
                    colNames.Add(cellData.StringCellValue);
                    csvHeader += cellData.StringCellValue + ",";
                }

                for (int row = sheet.FirstRowNum + 8; row <= sheet.LastRowNum; ++row)
                {
                    IRow rowData = sheet.GetRow(row);
                    ICell firstCell = rowData.GetCell(headerRow.FirstCellNum);
                    if (firstCell == null)
                    {
                        continue;
                    }

                    string strID = "";
                    if(firstCell.CellType == CellType.Numeric)
                    {
                        strID = firstCell.NumericCellValue.ToString();
                    }
                    else if(firstCell.CellType == CellType.String)
                    {
                        if(string.IsNullOrEmpty(firstCell.StringCellValue))
                        {
                            continue;
                        }

                        strID = firstCell.StringCellValue;
                    }
                    else
                    {
                        continue;
                    }

                    XmlElement entry = resDoc.CreateElement("Entry");
                    root.AppendChild(entry);

                    for (int col = headerRow.FirstCellNum; col < headerRow.LastCellNum; ++col)
                    {
                        string name = colNames.ElementAt(col);

                        ICell cellData = rowData.GetCell(col);
                        if (cellData == null)
                        {
                            continue;
                        }

                        string strValue = "";
                        switch (cellData.CellType)
                        {
                            case CellType.Boolean:
                                {
                                    strValue = cellData.BooleanCellValue.ToString();
                                    if (strValue == "TRUE" || strValue == "true" || strValue == "True")
                                    {
                                        strValue = "1";
                                    }
                                    else
                                    {
                                        strValue = "0";
                                    }
                                }
                                break;
                            case CellType.Numeric:
                                strValue = cellData.NumericCellValue.ToString();
                                break;
                            case CellType.String:
                                {
                                    strValue = cellData.StringCellValue;
                                }
                                break;
                            default:
                                break;
                        }

                        entry.SetAttribute(name, strValue);

                        if (cellData.CellType == CellType.String)
                        {
                            strValue = "\"" + strValue + "\"";
                        }

                        csvContent += strValue + ",";
                    }

                    csvContent = csvContent.TrimEnd(',');
                    csvContent += "\n";
                }
            }

            csvContent = csvContent.TrimEnd('\n');
            resDoc.Save(strToolBasePath + strResPath + strFileName + ".xml");
            csvWriter.WriteLine(csvHeader.TrimEnd(','));
            csvWriter.Write(csvContent);
            csvWriter.Close();

            return true;
        }
        
        static void Main(string[] args)
        {
            var now = DateTime.Now;
            CreateSchemaFile();
            CreateResFile();

            var dis = DateTime.Now - now;
            Console.WriteLine("Generate all files successful, use time = {0:3} s", string.Format("{0:N3}", dis.TotalSeconds));
            Console.Write("Press any key to quit...");
            Console.ReadKey();
        }
    }
}
