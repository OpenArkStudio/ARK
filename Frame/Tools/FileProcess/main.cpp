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

#include "FileProcess.h"

int main(int argc, const char *argv[])
{
	int arg = 0;
	if (argc > 1)
	{
		arg = atoi(argv[1]);
	}
	for (int i = 0;i < 1;i++)
	{
		auto t1 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		FileProcess* fp = new FileProcess();
		fp->bConvertIntoUTF8 = false;//set it true to convert UTF8 to GBK for supporting chinese in NF to show. 
		fp->OnCreateXMLFile();
		fp->OnCreateMysqlFile();
		auto t2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		std::cout << "Generate All Done for Good!!!" << std::endl;
		std::cout << "Timespan: " << t2 - t1 << " us (" << (t2 - t1) / 1000000.0f << "s)" << std::endl;
		delete fp;
	}
#if ARK_PLATFORM == PLATFORM_WIN
	if (arg != 1)
	{
        system("pause");
	}
#endif
	return 0;
}