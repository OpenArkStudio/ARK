/*
* This source file is part of ArkNX
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


namespace ark
{
    //game entity in game world
    class AFIEntityEx
    {
    public:
        explicit AFIEntityEx(const AFGUID& self) {}
        virtual ~AFIEntityEx() = default;

        virtual const AFGUID& Self() = 0;
        virtual void SetSelf(const AFGUID& id) = 0;

        virtual bool Inited() = 0;
        virtual void SetInited() = 0;

        virtual bool NeedSave() = 0;
        virtual void SetNeedSave() = 0;

        //////////////////////////////////////////////////////////////////////////
        //Create data
        virtual AFIDataEx* CreateData(const std::string& data_name) = 0;
        virtual AFIDataEx* CreateData(const std::string& data_name, uint64_t key) = 0;

        //Add data
        virtual bool AddData(AFIDataEx* parent, AFIDataEx* data) = 0;
        virtual bool AddData(AFIDataEx* parent, uint64_t key) = 0;
        virtual bool AddData(const std::string& parent_name, uint64_t key, AFIDataEx* data) = 0;

        //Remove data
        virtual bool RemoveData(const std::string& parent_name, uint64_t key) = 0;
        virtual bool RemoveData(AFIDataEx* parent, uint64_t key) = 0;
        virtual bool RemoveData(const std::string& parent_name) = 0;
        virtual bool RemoveData(const std::string& parent_name, const std::string& data_name) = 0;

        //Update data
        //Update 1 level data(Normal data), e.g. Player -> Level
        virtual void UpdateData(const std::string& data_name, const std::string& value) = 0;
        virtual uint64_t UpdateData(const std::string& data_name, ArkDataOp op, uint64_t value) = 0;

        //Update 2 level data(SubStruct), e.g. Player -> SubAttr -> HP
        virtual void UpdateData(const std::string& parent_name, const std::string& data_name, const std::string& value) = 0;
        virtual uint64_t UpdateData(const std::string& parent_name, const std::string& data_name, ArkDataOp op, uint64_t value) = 0;

        //Update 3 level data(Table), e.g. Player -> ItemBag -> id=12345 -> count
        virtual void UpdateData(const std::string& parent_name, uint64_t key, const std::string& data_name, const std::string& value) = 0;
        virtual uint64_t UpdateData(const std::string& parent_name, uint64_t key, const std::string& data_name, ArkDataOp op, uint64_t value) = 0;
        virtual uint64_t UpdateData(AFIDataEx* parent, uint64_t key, const std::string& data_name, ArkDataOp op, uint64_t value) = 0;

        //Update a data-node's value
        virtual void UpdateData(AFIDataEx* data, const std::string& value) = 0;
        virtual uint64_t UpdateData(AFIDataEx* data, ArkDataOp op, uint64_t value) = 0;

        //Update a data-node's value which is in data set, e.g.
        virtual uint64_t UpdateData(uint64_t key, AFIDataEx* data, ArkDataOp op, uint64_t value) = 0;

        virtual void UpdateData(AFIDataEx* parent, const std::string& data_name, const std::string& value) = 0;
        virtual uint64_t UpdateData(AFIDataEx* parent, const std::string& data_name, ArkDataOp op, uint64_t value) = 0;

        //Update the value of index slot in array
        virtual uint64_t UpdateData(uint64_t key, AFIDataEx* data, uint64_t index, ArkDataOp op, uint64_t value) = 0;

    };

}