/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
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

#include "AFPlatform.hpp"

#ifdef ARK_PLATFORM == PLATFORM_WIN
#include <direct.h>
#endif

namespace ark
{

    class AFFileUtil
    {
    public:
        static bool IsValidDirName(const char* name)
        {
            return !(name[0] == ('0') || (name[1] != 0 && (name[1] == '.' && name[2] == 0)))
        }

        static bool IsAbsolutePath(const std::string& path)
        {
            if (path.size() == 0)
            {
                return false;
            }

#ifdef ARK_PLATFORM == PLATFORM_WIN
            return (isalpha(static_cast<unsigned char>(path[0])) && path[1] == ':');
#else
            return path[0] == '/';
#endif
        }
        static std::string GetAbsolutePathName(const std::string& path)
        {
            std::string tmp = path;
            if (IsAbsolutePath(path))
            {
                char dir[_MAX_PATH] = { 0 };
#ifdef ARK_PLATFORM == PLATFORM_WIN
                if (_getcwd(dir, _MAX_PATH))
                {
                    tmp = std::string(dir) + "/" + tmp;
                }
#else
                getcwd(dir, _MAX_PATH);
                tmp = std::string(dir) + "/" + tmp;
#endif
            }

            NormalizePath(tmp);
            return tmp;
        }

        static std::string Clean(const std::string& path)
        {

        }

        static void SplitFileName(const std::string& file_path, std::string& file_name, std::string& base_dir, bool with_trailing_slash = false)
        {

        }

        static std::string& Join(const std::string& prefix, const std::string& postfix)
        {

        }

        static std::string& NormalizePath(const std::string& file_path, bool with_trailing_slash = true)
        {

        }

        static std::string GetParentDir(const std::string& file_path, bool with_trailing_slash = true)
        {

        }

        static std::string GetFileName(const std::string& file_path)
        {
            std::string::size_type pos = file_path.find_last_of(std::string::value_type('/'));
            if (pos == std::string::npos)
            {
                return file_path;
            }
            else
            {
                return file_path.substr(pos + 1);
            }
        }

        static std::string GetFileNameWithoutExt(const string& file_path)
        {

        }

        static std::string GetFileNameExtension(const string& file_path)
        {

        }

        static std::string& GetFileDirName(const std::string& file_path)
        {

        }

        static bool Unlink(const std::string& file_path)
        {

        }

        static bool Remove(const std::string& file_path)
        {

        }

        static bool RemoveDirIfEmpty(const std::string& dir, bool recursively_delete_empty_parent_dir = true)
        {

        }

        static bool MakeDir(const std::string& dir)
        {

        }

        static bool RemoveDir(const std::string& dir)
        {

        }

        static bool RemoveDir(const char* dir)
        {

        }

        static bool IsFileExist(const std::string& file_path)
        {

        }

        static bool IsReadable(const std::string& file_path)
        {

        }

        static bool IsWriteable(const std::string& file_path)
        {

        }

        static bool IsExecutable(const std::string& file_path)
        {

        }

        static void GetFiles(const std::string& dir, std::list<std::string>& files, bool recursively = true, const std::string& filter = "*")
        {

        }

        static void GetFiles(const std::string& dir, std::list<std::string>& files, int depth, const std::string& filter = "*")
        {

        }

        static bool ReadFiles(const char* file_path, std::list<std::string>& lines)
        {

        }

        static bool WriteFile(const char* file_path, const void* content, const size_t len)
        {

        }

        static bool WriteFile(const std::string& file_path, const void* content, const size_t len)
        {

        }
    };

}