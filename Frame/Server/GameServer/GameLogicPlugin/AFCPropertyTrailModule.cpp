/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2018 ArkGame authors.
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

#include "AFCPropertyTrailModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Core/Base/AFCDataList.h"

bool AFCPropertyTrailModule::Init()
{
    return true;
}

bool AFCPropertyTrailModule::Shut()
{
    return true;
}

bool AFCPropertyTrailModule::Update()
{
    return true;
}

bool AFCPropertyTrailModule::PostInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();

    return true;
}

void AFCPropertyTrailModule::StartTrail(const AFGUID self)
{
    int nRet = LogObjectData(self);
}

void AFCPropertyTrailModule::EndTrail(const AFGUID self)
{
    //Will complete this
}

int AFCPropertyTrailModule::LogObjectData(const AFGUID& self)
{
    /* ARK_SHARE_PTR<AFIEntity> xObject = m_pKernelModule->GetObject(self);
     if(nullptr == xObject)
     {
         return -1;
     }

     ARK_SHARE_PTR<AFIPropertyMgr> xPropertyManager = xObject->GetPropertyManager();
     if(nullptr != xPropertyManager)
     {
         ARK_SHARE_PTR<AFIProperty> xProperty = xPropertyManager->First();
         while(nullptr != xProperty)
         {
             std::ostringstream stream;

             stream << " Start trail ";
             stream << xProperty->ToString();

             m_pLogModule->LogInfo(self, xProperty->GetKey(), stream.str(),  __FUNCTION__, __LINE__);

             xProperty = xPropertyManager->Next();
         }
     }

     ARK_SHARE_PTR<AFIRecordManager> xRecordManager = xObject->GetRecordManager();
     if(nullptr != xRecordManager)
     {
         ARK_SHARE_PTR<AFIRecord> xRecord = xRecordManager->First();
         while(nullptr != xRecord)
         {
             for(int i = 0; i < xRecord->GetRows(); ++i)
             {
                 AFCDataList xDataList;
                 bool bRet = xRecord->QueryRow(i, xDataList);
                 if(bRet)
                 {
                     std::ostringstream stream;
                     stream << " Start trail Row[" << i << "]";

                     for(int j = 0; j < xDataList.GetCount(); ++j)
                     {
                         stream << " [" << j << "] " << xDataList.ToString(j);
                     }

                     m_pLogModule->LogInfo(self, xRecord->GetName(), stream.str(),  __FUNCTION__, __LINE__);
                 }
             }

             xRecord = xRecordManager->Next();
         }
     }*/

    return 0;
}

int AFCPropertyTrailModule::OnObjectPropertyEvent(const AFGUID& self, const std::string& nodeName, const AFIData& oldVar, const AFIData& newVar)
{
    ARK_LOG_INFO("Trace id[{}] Name[{}] Old[{}] New[{}]", self.ToString().c_str(), nodeName.c_str(), oldVar.GetString(), newVar.GetString());
    return 0;
}

int AFCPropertyTrailModule::OnEntityTableEvent(const AFGUID& self, const DATA_TABLE_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar)
{
    //std::ostringstream stream;
    //ARK_SHARE_PTR<AFIRecord> xRecord = m_pKernelModule->FindRecord(self, xEventData.strRecordName);
    //if(nullptr == xRecord)
    //{
    //    return 0;
    //}

    //switch(xEventData.nOpType)
    //{
    //case AFIRecord::RecordOptype::Add:
    //    {
    //        AFCDataList xDataList;
    //        bool bRet = xRecord->QueryRow(xEventData.nRow, xDataList);
    //        if(bRet)
    //        {
    //            stream << " Trail Add Row[" << xEventData.nRow << "]";

    //            for(int j = 0; j < xDataList.GetCount(); ++j)
    //            {
    //                stream << " [" << j << "] " << xDataList.ToString(j);
    //            }

    //            m_pLogModule->LogInfo(self, xRecord->GetName(), stream.str(),  __FUNCTION__, __LINE__);
    //        }
    //    }
    //    break;
    //case AFIRecord::RecordOptype::Del:
    //    {
    //        stream << " Trail Del Row[" << xEventData.nRow << "]";
    //        m_pLogModule->LogInfo(self, xRecord->GetName(), stream.str(),  __FUNCTION__, __LINE__);
    //    }
    //    break;
    //case AFIRecord::RecordOptype::Swap:
    //    {
    //        stream << " Trail Swap Row[" << xEventData.nRow << "] Row[" << xEventData.nCol << "]";
    //        m_pLogModule->LogInfo(self, xRecord->GetName(), stream.str(),  __FUNCTION__, __LINE__);
    //    }
    //    break;
    //case AFIRecord::RecordOptype::Create:
    //    break;
    //case AFIRecord::RecordOptype::Update:
    //    {
    //        stream << " Trail UpData Row[" << xEventData.nRow << "] Col[" << xEventData.nCol << "]";
    //        //stream << " [Old] " << oldVar.ToString();
    //        //stream << " [New] " << newVar.ToString();
    //        m_pLogModule->LogInfo(self, xRecord->GetName(), stream.str(),  __FUNCTION__, __LINE__);
    //    }
    //    break;
    //case AFIRecord::RecordOptype::Cleared:
    //    break;
    //case AFIRecord::RecordOptype::Sort:
    //    break;
    //default:
    //    break;
    //}

    return 0;
}

int AFCPropertyTrailModule::TrailObjectData(const AFGUID& self)
{
    //ARK_SHARE_PTR<AFIEntity> xObject = m_pKernelModule->GetObject(self);
    //if(nullptr == xObject)
    //{
    //    return -1;
    //}

    //ARK_SHARE_PTR<AFIPropertyManager> xPropertyManager = xObject->GetPropertyManager();
    //if(nullptr != xPropertyManager)
    //{
    //    ARK_SHARE_PTR<AFIProperty> xProperty = xPropertyManager->First();
    //    while(nullptr != xProperty)
    //    {
    //        m_pKernelModule->AddPropertyCallBack(self, xProperty->GetKey(), this, &AFCPropertyTrailModule::OnObjectPropertyEvent);

    //        xProperty = xPropertyManager->Next();
    //    }
    //}
    //
    //ARK_SHARE_PTR<AFIRecordManager> xRecordManager = xObject->GetRecordManager();
    //if(nullptr != xRecordManager)
    //{
    //    ARK_SHARE_PTR<AFIRecord> xRecord = xRecordManager->First();
    //    while(nullptr != xRecord)
    //    {
    //        m_pKernelModule->AddRecordCallBack(self, xRecord->GetName(), this, &AFCPropertyTrailModule::OnObjectRecordEvent);


    //        xRecord = xRecordManager->Next();
    //    }
    //}

    return 0;
}

