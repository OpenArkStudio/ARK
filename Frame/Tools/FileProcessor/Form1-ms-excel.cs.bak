using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using CCWin;
using CCWin.SkinControl;
using System.Threading;
using System.Xml;
using System.IO;
using Microsoft.Office.Core;
using Microsoft.Office.Interop.Excel;
using System.Reflection;

namespace FileProcess
{
    public partial class Form1 : Skin_Metro
    {
        string strBasePath = "../../NFDataCfg/";
        string strToolBasePath = "../";
        string strExcelStructPath = "Excel_Struct/";
        string strXMLStructPath = "Struct/Class/";

        string strExcelIniPath = "Excel_Ini/";
        string strXMLIniPath = "Ini/NPC/";

        string strLogicClassFile = "../Struct/LogicClass.xml";
        string strMySQLFile = "../mysql/NFrame.sql";

        StreamWriter mysqlWriter;
        volatile Int32 nStop = 0;
        public Form1()
        {
            InitializeComponent();
        }

        private void OnCreateMysqlFile(object sender, EventArgs e)
        {
            this.skinButton1.Text = "正在生成文件，请稍等...";
            this.skinButton1.Enabled = false;

            CreateMySQLFile();

            if (!LoadLogicClass(strLogicClassFile))
            {
                MessageBox.Show("生成MySQL文件失败，请检查文件!");
                this.skinButton1.Text = "生成MySQL文件失败";
            }
            else
            {
                this.skinButton1.Text = "生成MySQL文件成功";
            }

            this.skinButton1.Enabled = true;

            mysqlWriter.Close();
        }

        void CreateMySQLFile()
        {
            if (File.Exists(strMySQLFile))
            {
                File.Delete(strMySQLFile);
            }

            mysqlWriter = new StreamWriter(strMySQLFile);
        }

        bool LoadLogicClass(string strFile)
        {
            skinProgressBar1.Visible = false;
            skinProgressBar2.Value = 0;
            ////////////////////////////////////////////////////
            XmlDocument doc = new XmlDocument();
            doc.Load(strFile);

            XmlElement root = doc.DocumentElement;
            XmlNode classNode = root.SelectSingleNode("Class");

            XmlElement classElement = (XmlElement)classNode;
            string strIObjectPath = classElement.GetAttribute("Path");

            XmlNodeList classNodeList = classNode.ChildNodes;
            skinProgressBar2.Maximum = classNodeList.Count;
            foreach (XmlNode node in classNodeList)
            {
                XmlElement nodeElement = (XmlElement)node;
                string strID = nodeElement.GetAttribute("Id");
                mysqlWriter.WriteLine("CREATE TABLE `" + strID + "` (");
                mysqlWriter.WriteLine("\t`ID` varchar(128) NOT NULL,");
                mysqlWriter.WriteLine("\tPRIMARY KEY (`ID`)");
                mysqlWriter.WriteLine(") ENGINE=InnoDB DEFAULT CHARSET=utf8;");
                mysqlWriter.WriteLine();

                // 读取父节点内容
                if (!LoadClass(strIObjectPath, strID))
                {
                    return false;
                }

                // 读取自己节点内容
                string strPath = nodeElement.GetAttribute("Path");
                if (!LoadClass(strPath, strID))
                {
                    return false;
                }

                mysqlWriter.WriteLine();
                mysqlWriter.WriteLine();

                skinProgressBar2.Value += 1;
            }

            return true;
        }

        bool LoadClass(string strFile, string strTable)
        {
            XmlDocument xDoc = new XmlDocument();
            try
            {
                xDoc.Load(strFile);
            }
            catch (Exception err)
            {
                MessageBox.Show("Load " + strFile + " error");
                return false;
            }

            XmlElement root = xDoc.DocumentElement;
            XmlNode xPropertysNode = root.SelectSingleNode("Propertys");
            XmlNodeList xPropertyNodeList = xPropertysNode.ChildNodes;
            foreach (XmlNode node in xPropertyNodeList)
            {
                XmlElement nodeElement = (XmlElement)node;
                if (nodeElement.GetAttribute("Save") != "1")
                {
                    continue;
                }

                string strID = nodeElement.GetAttribute("Id");
                string strDesc = nodeElement.GetAttribute("Desc");
                string strType = nodeElement.GetAttribute("Type");

                switch (strType)
                {
                    case "string":
                        mysqlWriter.WriteLine("ALTER TABLE `" + strTable + "` ADD `" + strID + "` varchar(128) DEFAULT '' COMMENT '" + strDesc + "';");
                        break;
                    case "int":
                        mysqlWriter.WriteLine("ALTER TABLE `" + strTable + "` ADD `" + strID + "` bigint(11) DEFAULT '0' COMMENT '" + strDesc + "';");
                        break;
                    case "object":
                        mysqlWriter.WriteLine("ALTER TABLE `" + strTable + "` ADD `" + strID + "` varchar(128) DEFAULT '' COMMENT '" + strDesc + "';");
                        break;
                    case "float":
                        mysqlWriter.WriteLine("ALTER TABLE `" + strTable + "` ADD `" + strID + "` float(11,3) DEFAULT '0' COMMENT '" + strDesc + "';");
                        break;
                    default:
                        mysqlWriter.WriteLine("ALTER TABLE `" + strTable + "` ADD `" + strID + "` varchar(128) DEFAULT '' COMMENT '" + strDesc + "';");
                        break;
                }
            }

            XmlNode xRecordsNode = root.SelectSingleNode("Records");
            XmlNodeList xRecordNodeList = xRecordsNode.ChildNodes;
            foreach (XmlNode node in xRecordNodeList)
            {
                XmlElement nodeElement = (XmlElement)node;
                if (nodeElement.GetAttribute("Save") != "1")
                {
                    continue;
                }

                string strID = nodeElement.GetAttribute("Id");
                string strDesc = nodeElement.GetAttribute("Desc");
                mysqlWriter.WriteLine("ALTER TABLE `" + strTable + "` ADD `" + strID + "` BLOB COMMENT '" + strDesc + "';");
            }

            return true;
        }

        /////////////////////////////////////////////////////////////////////////

        private void OnCreateXMLFile(object sender, EventArgs e)
        {
            this.skinButton2.Text = "正在生成文件，请稍等...";
            this.skinButton2.Enabled = false;

            //////////////////////////////////////////////////////////////////////

            //Thread xStructThread = new Thread(CreateStructThreadFunc);
            //Thread xIniThread = new Thread(CreateIniThreadFunc);

            //xStructThread.Start();
            //xIniThread.Start();
            System.Action xStructAction = new System.Action(CreateStructThreadFunc);
            System.Action xIniAction = new System.Action(CreateIniThreadFunc);

            xStructAction.BeginInvoke(new AsyncCallback(CreateXMLCallBack), null);
            xIniAction.BeginInvoke(new AsyncCallback(CreateXMLCallBack), null);

            ////////////////////////////////////////////////////////
        }

        void CreateXMLCallBack(IAsyncResult ar)
        {
            if (nStop >= 2)
            {
                this.skinButton2.Text = "生成XML文件成功";
                this.skinButton2.Enabled = true;
            }
        }

        void CreateStructThreadFunc()
        {
            skinProgressBar1.Visible = true;

            XmlDocument xmlDoc = new XmlDocument();
            XmlDeclaration xmlDecl;
            xmlDecl = xmlDoc.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            xmlDoc.AppendChild(xmlDecl);

            XmlElement root = xmlDoc.CreateElement("", "XML", "");
            xmlDoc.AppendChild(root);

            XmlElement classElement = xmlDoc.CreateElement("", "Class", "");
            root.AppendChild(classElement);
            //////////////////////////////////////////////////////////////////////
            classElement.SetAttribute("Id", "IObject");
            classElement.SetAttribute("Type", "TYPE_IOBJECT");
            classElement.SetAttribute("Path", "../../NFDataCfg/Struct/Class/IObject.xml");
            classElement.SetAttribute("InstancePath", "../../NFDataCfg/Ini/NPC/IObject.xml");
            classElement.SetAttribute("Public", "0");
            classElement.SetAttribute("Desc", "IObject");
            //////////////////////////////////////////////////////////////////////
            // 遍历Struct文件夹下的excel文件
            String[] xStructXLSXList = Directory.GetFiles(strToolBasePath + strExcelStructPath, "*", SearchOption.AllDirectories);
            skinProgressBar1.Maximum = xStructXLSXList.Length - 1; // 去掉IObject
            foreach (string file in xStructXLSXList)
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

                // 单个excel文件转为xml
                if (!CreateStructXML(file))
                {
                    MessageBox.Show("Create " + file + " failed!");
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

                // ProgressBar
                skinProgressBar1.Value += 1;
            }

            xmlDoc.Save(strLogicClassFile);
            nStop += 1;
        }

        void CreateIniThreadFunc()
        {
            // 遍历Ini文件夹下的excel文件
            String[] xIniXLSXList = Directory.GetFiles(strToolBasePath + strExcelIniPath, "*", SearchOption.AllDirectories);
            skinProgressBar2.Maximum = xIniXLSXList.Length;
            skinProgressBar2.Value = 0;
            foreach (string file in xIniXLSXList)
            {
                if (!CreateIniXML(file))
                {
                    MessageBox.Show("Create " + file + " failed!");
                    return;
                }

                skinProgressBar2.Value += 1;
            }

            nStop += 1;
        }

        bool CreateStructXML(string strFile)
        {
            string strCurPath = Directory.GetCurrentDirectory();

            // 打开excel
            Microsoft.Office.Interop.Excel.Application excel = new Microsoft.Office.Interop.Excel.Application();
            Workbook book = excel.Workbooks.Add(strCurPath + "/" + strFile);
            //excel.Visible = true;

            // 开始创建xml
            XmlDocument structDoc = new XmlDocument();
            XmlDeclaration xmlDecl;
            xmlDecl = structDoc.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            structDoc.AppendChild(xmlDecl);

            // 写入XML root标签
            XmlElement root = structDoc.CreateElement("", "XML", "");
            structDoc.AppendChild(root);

            // 写入Propertys标签
            XmlElement propertyNodes = structDoc.CreateElement("", "Propertys", "");
            root.AppendChild(propertyNodes);

            // 写入Records标签
            XmlElement recordNodes = structDoc.CreateElement("", "Records", "");
            root.AppendChild(recordNodes);

            // 写入components的处理
            XmlElement componentNodes = structDoc.CreateElement("", "Components", "");
            root.AppendChild(componentNodes);

            // 读取excel中每一个sheet
            foreach (Worksheet sheet in book.Sheets)
            {
                string strSheetName = sheet.Name;
                int nRowCount = sheet.UsedRange.Rows.Count;
                int nColCount = sheet.UsedRange.Columns.Count;

                switch (strSheetName.ToLower())
                {
                    case "property":
                        {
                            //string data = (string)(sheet.Cells[1, 1] as Microsoft.Office.Interop.Excel.Range).Value;
                            for (Int32 row = 2; row <= nRowCount; ++row)
                            {
                                var testValue = (sheet.Cells[row, 1] as Microsoft.Office.Interop.Excel.Range).Value;
                                if (testValue == null)
                                {
                                    continue;
                                }

                                var propertyNode = structDoc.CreateElement("", "Property", "");
                                propertyNodes.AppendChild(propertyNode);

                                for (Int32 col = 1; col <= nColCount; ++col)
                                {
                                    try
                                    {
                                        string name = (string)(sheet.Cells[1, col] as Microsoft.Office.Interop.Excel.Range).Value;
                                        var value = (sheet.Cells[row, col] as Microsoft.Office.Interop.Excel.Range).Value;

                                        if (value == null)
                                        {
                                            value = "";
                                        }

                                        if (value.ToString() == "False")
                                        {
                                            value = "0";
                                        }
                                        else if (value.ToString() == "True")
                                        {
                                            value = "1";
                                        }

                                        if (value is double)
                                        {
                                            float fValue = (float)value;
                                            value = ((Int32)fValue).ToString();
                                        }

                                        propertyNode.SetAttribute(name, value.ToString());
                                    }
                                    catch (Exception err)
                                    {
                                        excel.Workbooks.Close();
                                        MessageBox.Show(strFile + "\n" + "Sheet: " + strSheetName + "\n" + "Cell: " + row.ToString() + ", " + col.ToString() + "\n" + "This cell is empty or error");
                                        return false;
                                    }
                                }
                            }
                        }
                        break;
                    case "component":
                        {
                            for (Int32 row = 2; row < nRowCount; ++row)
                            {
                                var testValue = (sheet.Cells[row, 1] as Microsoft.Office.Interop.Excel.Range).Value;
                                if (testValue == null)
                                {
                                    continue;
                                }

                                var componentNode = structDoc.CreateElement("", "Component", "");
                                componentNodes.AppendChild(componentNode);

                                for (Int32 col = 1; col < nColCount; ++col)
                                {
                                    try
                                    {
                                        string name = (string)(sheet.Cells[1, col] as Microsoft.Office.Interop.Excel.Range).Value;
                                        var value = (sheet.Cells[row, col] as Microsoft.Office.Interop.Excel.Range).Value;
                                        if (value == null)
                                        {
                                            value = "";
                                        }

                                        if (value.ToString() == "False")
                                        {
                                            value = "0";
                                        }
                                        else if (value.ToString() == "True")
                                        {
                                            value = "1";
                                        }

                                        if (value is double)
                                        {
                                            float fValue = (float)value;
                                            value = ((Int32)fValue).ToString();
                                        }

                                        componentNode.SetAttribute(name, value.ToString());
                                    }
                                    catch (Exception err)
                                    {
                                        excel.Workbooks.Close();
                                        MessageBox.Show(strFile + "\n" + "Sheet: " + strSheetName + "\n" + "Cell: " + row.ToString() + ", " + col.ToString() + "\n" + "This cell is empty or error");
                                        return false;
                                    }
                                }
                            }
                        }
                        break;
                    default:
                        {
                            var recordNode = structDoc.CreateElement("", "Record", "");
                            recordNodes.AppendChild(recordNode);

                            // 从12个开始就是Record中具体的Col了
                            Int32 nSetColNum = 0;
                            for (Int32 col = 1; col < 12; ++col)
                            {
                                try
                                {
                                    string name = (string)(sheet.Cells[1, col] as Microsoft.Office.Interop.Excel.Range).Value;
                                    var value = (sheet.Cells[2, col] as Microsoft.Office.Interop.Excel.Range).Value;
                                    if (value == null)
                                    {
                                        value = "";
                                    }

                                    if (value.ToString() == "False")
                                    {
                                        value = "0";
                                    }
                                    else if (value.ToString() == "True")
                                    {
                                        value = "1";
                                    }

                                    if (value is double)
                                    {
                                        float fValue = (float)value;
                                        value = ((Int32)fValue).ToString();
                                    }

                                    recordNode.SetAttribute(name, value.ToString());
                                    if (name == "Col")
                                    {
                                        nSetColNum = Int32.Parse(value);
                                    }
                                }
                                catch (Exception err)
                                {
                                    excel.Workbooks.Close();
                                    MessageBox.Show(strFile + "\n" + "Sheet: " + strSheetName + "\n" + "Cell: 1/2 " + col.ToString() + "\n" + "This cell is empty or error");
                                    return false;
                                }
                            }

                            for (Int32 col = 12; col < 12 + nSetColNum; ++col)
                            {
                                try
                                {
                                    string name = (string)(sheet.Cells[1, col] as Microsoft.Office.Interop.Excel.Range).Value;
                                    var value = (sheet.Cells[2, col] as Microsoft.Office.Interop.Excel.Range).Value;
                                    if (value == null)
                                    {
                                        value = "";
                                    }

                                    if (value.ToString() == "False")
                                    {
                                        value = "0";
                                    }
                                    else if (value.ToString() == "True")
                                    {
                                        value = "1";
                                    }

                                    var colNode = structDoc.CreateElement("Col");
                                    recordNode.AppendChild(colNode);

                                    if (value is double)
                                    {
                                        float fValue = (float)value;
                                        value = ((Int32)fValue).ToString();
                                    }

                                    colNode.SetAttribute("Type", value);
                                    colNode.SetAttribute("Tag", name);
                                }
                                catch (Exception err)
                                {
                                    excel.Workbooks.Close();
                                    MessageBox.Show(strFile + "\n" + "Sheet: " + strSheetName + "\n" + "Cell: 1/2 " + col.ToString() + "\n" + "This cell is empty or error");
                                    return false;
                                }
                            }
                        }
                        break;
                }
            }

            excel.Workbooks.Close();
            ////////////////////////////////////////////////////////////////////////////
            // 保存文件
            int nLastPoint = strFile.LastIndexOf(".") + 1;
            int nLastSlash = strFile.LastIndexOf("/") + 1;
            string strFileName = strFile.Substring(nLastSlash, nLastPoint - nLastSlash - 1);
            string strFileExt = strFile.Substring(nLastPoint, strFile.Length - nLastPoint);

            string strXMLFile = strToolBasePath + strXMLStructPath + strFileName + ".xml";
            structDoc.Save(strXMLFile);
            return true;
        }

        bool CreateIniXML(string strFile)
        {
            string strCurPath = Directory.GetCurrentDirectory();
            // 打开excel
            Microsoft.Office.Interop.Excel.Application excel = new Microsoft.Office.Interop.Excel.Application();
            Workbook book = excel.Workbooks.Add(strCurPath + "/" + strFile);
            ////////////////////////////////////////////////////////////////////////////
            XmlDocument iniDoc = new XmlDocument();
            XmlDeclaration xmlDecl;
            xmlDecl = iniDoc.CreateXmlDeclaration("1.0", "UTF-8", "yes");
            iniDoc.AppendChild(xmlDecl);

            XmlElement root = iniDoc.CreateElement("", "XML", "");
            iniDoc.AppendChild(root);

            // 读取excel中每一个sheet
            foreach (Worksheet sheet in book.Sheets)
            {
                string strSheetName = sheet.Name;
                int nRowCount = sheet.UsedRange.Rows.Count;
                int nColCount = sheet.UsedRange.Columns.Count;

                for (Int32 row = 2; row <= nRowCount; ++row)
                {
                    var testValue = (sheet.Cells[row, 1] as Microsoft.Office.Interop.Excel.Range).Value;
                    if (testValue == null)
                    {
                        continue;
                    }

                    XmlElement objectNode = iniDoc.CreateElement("", "Object", "");
                    root.AppendChild(objectNode);

                    for (Int32 col = 1; col <= nColCount; ++col)
                    {
                        try
                        {
                            string name = (string)(sheet.Cells[1, col] as Microsoft.Office.Interop.Excel.Range).Value;
                            var value = (sheet.Cells[row, col] as Microsoft.Office.Interop.Excel.Range).Value;

                            if (value == null)
                            {
                                value = "";
                            }

                            if (value.ToString() == "False")
                            {
                                value = "0";
                            }
                            else if (value.ToString() == "True")
                            {
                                value = "1";
                            }

                            if (value is double)
                            {
                                float fValue = (float)value;
                                value = ((Int32)fValue).ToString();
                            }

                            objectNode.SetAttribute(name, value);
                        }
                        catch (Exception err)
                        {
                            excel.Workbooks.Close();
                            MessageBox.Show(strFile + "\n" + "Sheet: " + strSheetName + "\n" + "Cell: " + row.ToString() + ", " + col.ToString() + "\n" + "This cell is empty or error");
                            return false;
                        }
                    }
                }
            }

            excel.Workbooks.Close();
            ////////////////////////////////////////////////////////////////////////////
            // 保存文件
            int nLastPoint = strFile.LastIndexOf(".") + 1;
            int nLastSlash = strFile.LastIndexOf("/") + 1;
            string strFileName = strFile.Substring(nLastSlash, nLastPoint - nLastSlash - 1);
            string strFileExt = strFile.Substring(nLastPoint, strFile.Length - nLastPoint);

            string strXMLFile = strToolBasePath + strXMLIniPath + strFileName + ".xml";
            iniDoc.Save(strXMLFile);
            return true;
        }
    }
}
