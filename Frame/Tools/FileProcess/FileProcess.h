/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
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

#include "SDK/Base/AFPlatform.hpp"
#include "tinyxml2.h"
#include "MiniExcelReader.h"

class FileProcess
{
public:
	FileProcess();
	~FileProcess();

	void CreateProtoFile();
	void CreateNameFile();
	void OnCreateXMLFile();
	void CreateStructThreadFunc();
	void CreateIniThreadFunc();
	bool CreateStructXML(std::string strFile, std::string strFileName);
	bool CreateIniXML(std::string strFile);
	void OnCreateMysqlFile();
	bool LoadLogicClass(std::string strFile);
	bool LoadClass(std::string strFile, std::string strTable);

	bool bConvertIntoUTF8 = false;

private:
	int nCipher = 0;
	std::string strCipherCfg = "conf";

	std::string strExecutePath = "DataConfig/";
	std::string strToolBasePath = "../";
	std::string strRelativePath = "../../";
	//std::string strExcelStructPath = "Excel_Struct/";
	std::string strXMLStructPath = "Struct/Class/";

	std::string strExcelIniPath = "Excel/";
	std::string strXMLIniPath = "Ini/NPC/";

	std::string strLogicClassFile = "";
	std::string strMySQLFile = "../mysql/NFrame.sql";
	std::string strMySQLClassFile = "../mysql/AFClass.sql";

	std::string strProtoFile = "../proto/ArkRecordDefine.proto";

	std::string strHPPFile = "../proto/ArkProtocolDefine.hpp";
	std::string strJavaFile = "../proto/ArkProtocolDefine.java";
	std::string strCSFile = "../proto/ArkProtocolDefine.cs";

	FILE* mysqlWriter = nullptr;
	FILE* mysqlClassWriter = nullptr;
	FILE* protoWriter = nullptr;
    FILE* hppWriter = nullptr;
    FILE* javaWriter = nullptr;
	FILE* csWriter = nullptr;

	std::string strHppIObjectInfo;
	std::string strJavaIObjectInfo;
	std::string strCSIObjectInfo;

	int nRecordStart = 11;

	std::vector<std::string> GetFileListInFolder(std::string folderPath, int depth)
	{
		std::vector<std::string> result;
#if ARK_PLATFORM == PLATFORM_WIN
		_finddata_t FileInfo;
		std::string strfind = folderPath + "\\*";
		long long Handle = _findfirst(strfind.c_str(), &FileInfo);


		if (Handle == -1L)
		{
			std::cerr << "can not match the folder path" << std::endl;
			exit(-1);
		}
		do {
			//判断是否有子目录
			if (FileInfo.attrib & _A_SUBDIR)
			{
				//这个语句很重要
				if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0))
				{
					std::string newPath = folderPath + "\\" + FileInfo.name;
					//dfsFolder(newPath, depth);
				}
			}
			else
			{

				std::string filename = (folderPath + "\\" + FileInfo.name);
				result.push_back(filename);
			}
		} while (_findnext(Handle, &FileInfo) == 0);


		_findclose(Handle);
#else
	DIR *pDir;
	struct dirent *ent;
	char childpath[512];
	char absolutepath[512];
	pDir = opendir(folderPath.c_str());
	memset(childpath, 0, sizeof(childpath));
	while ((ent = readdir(pDir)) != NULL)
	{
		if (ent->d_type & DT_DIR)
		{
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;
			}
		}
		else
		{
			sprintf(absolutepath, "%s/%s", folderPath.c_str(), ent->d_name);
			result.push_back(absolutepath);
		}
	}

	sort(result.begin(), result.end());//排序
#endif
		return result;
	}

	void StringReplace(std::string &strBig, const std::string &strsrc, const std::string &strdst)
	{
		std::string::size_type pos = 0;
		std::string::size_type srclen = strsrc.size();
		std::string::size_type dstlen = strdst.size();

		while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
		{
			strBig.replace(pos, srclen, strdst);
			pos += dstlen;
		}
	}
};


