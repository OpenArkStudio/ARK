/*
 * This source file is part of ArkNX
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
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

#include "AFIContainer.hpp"
#include "base/container/AFMap.hpp"
#include "base/container/AFList.hpp"
#include "AFITable.hpp"
#include "AFIData.hpp"

namespace ark {

class AFIContainer;
class AFIEntity
{
public:
    virtual ~AFIEntity() = default;

    // get object unique id
    virtual const guid_t& GetID() const = 0;

    // get object parent unique id
    virtual std::shared_ptr<AFIContainer> GetParentContainer() const = 0;
    virtual bool SetParentContainer(std::shared_ptr<AFIContainer> pContainer) = 0;

    virtual void Update() = 0;

    // get base data
    virtual bool IsPublic(const std::string& name) const = 0;
    virtual bool IsPublic(const uint32_t index) const = 0;
    virtual bool IsPrivate(const std::string& name) const = 0;
    virtual bool IsPrivate(const uint32_t index) const = 0;
    virtual bool IsSave(const std::string& name) const = 0;
    virtual bool IsSave(const uint32_t index) const = 0;
    virtual bool IsRealTime(const std::string& name) const = 0;
    virtual bool IsRealTime(const uint32_t index) const = 0;
    virtual bool HaveMask(const std::string& name, ArkDataMask feature) const = 0;
    virtual bool HaveMask(const uint32_t index, ArkDataMask feature) const = 0;

    virtual ArkMaskType GetMask(const uint32_t index) const = 0;

    virtual const std::string& GetClassName() const = 0;

    virtual ArkDataType GetNodeType(const std::string& name) const = 0;
    virtual ArkDataType GetNodeType(const uint32_t value) const = 0;

    virtual uint32_t GetIndex(const std::string& name) const = 0;

    virtual ID_TYPE GetConfigID() const = 0;
    virtual int32_t GetMapID() const = 0;
    virtual int32_t GetMapEntityID() const = 0;
    virtual bool SwitchScene(const int32_t map_id, const int32_t map_inst_id, const AFVector3D& pos) = 0;

    virtual const AFVector3D& GetPosition() const = 0;
    virtual float GetPositionX() const = 0;
    virtual float GetPositionY() const = 0;
    virtual float GetPositionZ() const = 0;
    virtual float GetOrient() const = 0;

    virtual bool SetPosition(const AFVector3D& postion, const float orient) = 0;
    virtual bool SetPosition(const float x, const float y, const float z, const float orient) = 0;

    // set data
    virtual bool SetBool(const std::string& name, bool value) = 0;
    virtual bool SetInt32(const std::string& name, const int32_t value) = 0;
    virtual bool SetUInt32(const std::string& name, const uint32_t value) = 0;
    virtual bool SetInt64(const std::string& name, const int64_t value) = 0;
    virtual bool SetUInt64(const std::string& name, const uint64_t value) = 0;
    virtual bool SetFloat(const std::string& name, const float value) = 0;
    virtual bool SetDouble(const std::string& name, const double value) = 0;
    virtual bool SetString(const std::string& name, const std::string& value) = 0;
    virtual bool SetWString(const std::string& name, const std::wstring& value) = 0;
    virtual bool SetGUID(const std::string& name, const guid_t& value) = 0;

    virtual bool SetBool(const uint32_t index, bool value) = 0;
    virtual bool SetInt32(const uint32_t index, const int32_t value) = 0;
    virtual bool SetUInt32(const uint32_t index, const uint32_t value) = 0;
    virtual bool SetInt64(const uint32_t index, const int64_t value) = 0;
    virtual bool SetUInt64(const uint32_t index, const uint64_t value) = 0;
    virtual bool SetFloat(const uint32_t index, const float value) = 0;
    virtual bool SetDouble(const uint32_t index, const double value) = 0;
    virtual bool SetString(const uint32_t index, const std::string& value) = 0;
    virtual bool SetWString(const uint32_t index, const std::wstring& value) = 0;
    virtual bool SetGUID(const uint32_t index, const guid_t& value) = 0;

    // Get data by field name
    virtual bool GetNode(const std::string& name, AFIData& data) const = 0;
    virtual bool GetBool(const std::string& name) const = 0;
    virtual int32_t GetInt32(const std::string& name) const = 0;
    virtual uint32_t GetUInt32(const std::string& name) const = 0;
    virtual int64_t GetInt64(const std::string& name) const = 0;
    virtual uint64_t GetUInt64(const std::string& name) const = 0;
    virtual float GetFloat(const std::string& name) const = 0;
    virtual double GetDouble(const std::string& name) const = 0;
    virtual const std::string& GetString(const std::string& name) const = 0;
    virtual const std::wstring& GetWString(const std::string& name) const = 0;
    virtual const guid_t& GetGUID(const std::string& name) const = 0;

    // Get data by field index
    virtual bool GetNode(const uint32_t index, AFIData& data) const = 0;
    virtual bool GetBool(const uint32_t index) const = 0;
    virtual int32_t GetInt32(const uint32_t index) const = 0;
    virtual uint32_t GetUInt32(const uint32_t index) const = 0;
    virtual int64_t GetInt64(const uint32_t index) const = 0;
    virtual uint64_t GetUInt64(const uint32_t index) const = 0;
    virtual float GetFloat(const uint32_t index) const = 0;
    virtual double GetDouble(const uint32_t index) const = 0;
    virtual const std::string& GetString(const uint32_t index) const = 0;
    virtual const std::wstring& GetWString(const uint32_t index) const = 0;
    virtual const guid_t& GetGUID(const uint32_t index) const = 0;

    // table data
    virtual AFITable* FindTable(const std::string& name) = 0;
    virtual AFITable* FindTable(const uint32_t index) = 0;

    virtual std::shared_ptr<AFIContainer> FindContainer(const std::string& name) = 0;
    virtual std::shared_ptr<AFIContainer> FindContainer(const uint32_t index) = 0;

    // custom data
    virtual bool AddCustomBool(const std::string& name, bool value) = 0;
    virtual bool AddCustomInt32(const std::string& name, const int32_t value) = 0;
    virtual bool AddCustomUInt32(const std::string& name, const uint32_t value) = 0;
    virtual bool AddCustomInt64(const std::string& name, const int64_t value) = 0;
    virtual bool AddCustomFloat(const std::string& name, const float value) = 0;
    virtual bool AddCustomDouble(const std::string& name, const double value) = 0;
    virtual bool AddCustomString(const std::string& name, const std::string& value) = 0;
    virtual bool AddCustomWString(const std::string& name, const std::wstring& value) = 0;
    virtual bool AddCustomGUID(const std::string& name, const guid_t& value) = 0;

    virtual bool SetCustomBool(const std::string& name, bool value) = 0;
    virtual bool SetCustomInt32(const std::string& name, const int32_t value) = 0;
    virtual bool SetCustomUInt32(const std::string& name, const uint32_t value) = 0;
    virtual bool SetCustomInt64(const std::string& name, const int64_t value) = 0;
    virtual bool SetCustomFloat(const std::string& name, const float value) = 0;
    virtual bool SetCustomDouble(const std::string& name, const double value) = 0;
    virtual bool SetCustomString(const std::string& name, const std::string& value) = 0;
    virtual bool SetCustomWString(const std::string& name, const std::wstring& value) = 0;
    virtual bool SetCustomGUID(const std::string& name, const guid_t& value) = 0;

    virtual bool GetCustomBool(const std::string& name) const = 0;
    virtual int32_t GetCustomInt32(const std::string& name) const = 0;
    virtual uint32_t GetCustomUInt32(const std::string& name) const = 0;
    virtual int64_t GetCustomInt64(const std::string& name) const = 0;
    virtual float GetCustomFloat(const std::string& name) const = 0;
    virtual double GetCustomDouble(const std::string& name) const = 0;
    virtual const char* GetCustomString(const std::string& name) const = 0;
    virtual const std::wstring& GetCustomWString(const std::string& name) const = 0;
    virtual const guid_t& GetCustomGUID(const std::string& name) const = 0;

    virtual bool FindCustomData(const std::string& name) const = 0;
    virtual bool RemoveCustomData(const std::string& name) = 0;

    virtual bool IsSent() const = 0;
    virtual void UpdateSent() = 0;
};

} // namespace ark
