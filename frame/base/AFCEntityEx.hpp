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

#include "interface/AFIEntityEx.h"
#include "interface/AFIDataEx.hpp"

namespace ark
{

    class AFCEntityEx : public AFIEntityEx
    {
    public:
        AFCEntityEx() = delete;
        explicit AFCEntityEx(const AFGUID& self) :
            self_(self)
        {
            //TODO
        }

        ~AFCEntityEx() override
        {
            //TODO
        }

        void SetSelf(const AFGUID& id)
        {
            self_ = id;
        }

        const AFGUID& Self()
        {
            return self_;
        }

        void InitData()
        {
            //TODO
        }

        bool Inited() override
        {
            return inited_;
        }

        void SetInited() override
        {
            inited_ = true;
        }

        bool NeedSave() override
        {
            return need_save_;
        }

        void SetNeedSave() override
        {
            need_save_ = true;
        }

        //////////////////////////////////////////////////////////////////////////
        //data
        AFIDataEx* CreateData(const std::string& data_name) override
        {
            //TODO
        }

        AFIDataEx* CreateData(const std::string& data_name, uint64_t key) override
        {
            //TODO
        }

        bool AddData(AFIDataEx* parent, AFIDataEx* data) override
        {
            //TODO
        }

        bool AddData(AFIDataEx* parent, uint64_t key) override
        {
            //TODO
        }

        bool AddData(const std::string& parent_name, uint64_t key, AFIDataEx* data) override
        {
            //TODO
        }

        bool RemoveData(const std::string& parent_name, uint64_t key) override
        {
            //TODO
        }

        bool RemoveData(AFIDataEx* parent, uint64_t key) override
        {
            //TODO
        }

        bool RemoveData(const std::string& parent_name) override
        {
            //TODO
        }

        bool RemoveData(const std::string& parent_name, const std::string& data_name)
        {
            //TODO
        }

        //UpdateData -> value = string
        void UpdateData(const std::string& data_name, const std::string& value) override
        {
            //TODO
        }

        void UpdateData(const std::string& parent_name, const std::string& data_name, const std::string& value) override
        {
            //TODO
        }

        void UpdateData(const std::string& parent_name, uint64_t key, const std::string& data_name, const std::string& value) override
        {
            //TODO
        }

        void UpdateData(AFIDataEx* data, const std::string& value) override
        {
            //TODO
        }

        void UpdateData(AFIDataEx* parent, const std::string& data_name, const std::string& value) override
        {
            //TODO
        }

        //UpdateData with operate -> value = uint64_t
        uint64_t UpdateData(const std::string& data_name, ArkDataOp op, uint64_t value) override
        {
            //todo
        }

        uint64_t UpdateData(const std::string& parent_name, const std::string& data_name, ArkDataOp op, uint64_t value) override
        {
            //todo
        }

        uint64_t UpdateData(const std::string& parent_name, uint64_t key, const std::string& data_name, ArkDataOp op, uint64_t value) override
        {
            //todo
        }

        uint64_t UpdateData(AFIDataEx* data, ArkDataOp op, uint64_t value) override
        {
            //todo
        }

        uint64_t UpdateData(uint64_t key, AFIDataEx* data, ArkDataOp op, uint64_t value) override
        {
            //todo
        }

        uint64_t UpdateData(uint64_t key, AFIDataEx* data, uint64_t index, ArkDataOp op, uint64_t value) override
        {
            //todo
        }


        uint64_t UpdateData(AFIDataEx* parent, const std::string& data_name, ArkDataOp op, uint64_t value) override
        {
            //todo
        }

        uint64_t UpdateData(AFIDataEx* parent, uint64_t key, const std::string& data_name, ArkDataOp op, uint64_t value) override
        {
            //todo
        }

    protected:
        void SyncAddData(AFIDataEx* data, uint64_t key)
        {
            //todo
        }

        void SyncRemoveData(AFIDataEx* data, uint64_t key)
        {
            //todo
        }

        void SyncUpdateData(AFIDataEx* data, uint64_t key)
        {
            //todo
        }

        void SyncEntityToClient()
        {
            //todo
        }

        void SyncAddDataToClient()
        {
            //todo
        }

        void SyncRemoveDataToClient()
        {
            //todo
        }

        void SyncUpdateDataToClient()
        {
            //todo
        }

        void AddSyncUpdateDataToPBData(AFIDataEx* data, AFMsg::PBDataEx* pb_data)
        {
            //todo
        }

    private:
        AFGUID self_{ NULL_GUID };
        AFIDataEx* entity_data_{ nullptr };
        std::string name_{ "" };
        bool inited_{ false };
        bool need_save_{ false };

        //data added
        bool have_add_pb_data_{ false };
        AFMsg::PBDataEx add_pb_datas_;

        //data removed
        bool have_remove_pb_data_{ false };
        AFMsg::PBDataEx remove_pb_datas;

        //data updated
        bool have_update_pb_data_{ false };
        AFMsg::PBDataEx update_pb_datas_;
    };

}