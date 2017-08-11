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

#include "AFIModule.h"
#include "SDK/Base/AFCDataList.h"
#include "SDK/Core/AFIPropertyMgr.h"
#include "SDK/Core/AFIRecordManager.h"

class AFIElementModule
    : public AFIModule
{
public:
    virtual bool Load() = 0;
    virtual bool Save() = 0;
    virtual bool Clear() = 0;
    virtual bool ExistElement(const std::string& strConfigName) = 0;
    virtual bool ExistElement(const std::string& strClassName, const std::string& strConfigName) = 0;

    virtual std::shared_ptr<AFIPropertyMgr> GetPropertyManager(const std::string& strConfigName) = 0;

    virtual std::shared_ptr<AFIRecordManager> GetRecordManager(const std::string& strConfigName) = 0;

    virtual bool GetPropertyBool(const std::string& strConfigName, const std::string& strPropertyName) = 0;
    virtual int32_t GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName) = 0;
    virtual int64_t GetPropertyInt64(const std::string& strConfigName, const std::string& strPropertyName) = 0;
    virtual float GetPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName) = 0;
    virtual double GetPropertyDouble(const std::string& strConfigName, const std::string& strPropertyName) = 0;
    virtual const std::string& GetPropertyString(const std::string& strConfigName, const std::string& strPropertyName) = 0;
};