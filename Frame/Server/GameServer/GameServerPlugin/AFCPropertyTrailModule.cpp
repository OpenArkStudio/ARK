// -------------------------------------------------------------------------
//    @FileName         :    AFCPropertyTrailModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-09-30
//    @Module           :    AFCPropertyTrailModule
//
// -------------------------------------------------------------------------

#include "AFCPropertyTrailModule.h"
#include "SDK/Interface/AFIPluginManager.h"
#include "SDK/Base/AFCDataList.h"

bool AFCPropertyTrailModule::Init()
{

    return true;
}


bool AFCPropertyTrailModule::Shut()
{
    return true;
}

bool AFCPropertyTrailModule::Execute()
{
    return true;
}

bool AFCPropertyTrailModule::AfterInit()
{
    m_pKernelModule = pPluginManager->FindModule<AFIKernelModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pLogModule = pPluginManager->FindModule<AFILogModule>();

    return true;
}

void AFCPropertyTrailModule::StartTrail(const AFGUID self)
{
    LogObjectData(self);


}

void AFCPropertyTrailModule::EndTrail(const AFGUID self)
{

}

int AFCPropertyTrailModule::LogObjectData(const AFGUID& self)
{
    /* ARK_SHARE_PTR<AFIObject> xObject = m_pKernelModule->GetObject(self);
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

int AFCPropertyTrailModule::OnObjectPropertyEvent(const AFGUID& self, const std::string& strPropertyName, const AFIData& oldVar, const AFIData& newVar)
{
    std::ostringstream stream;

    stream << " Trailing ";
    stream << " [Old] ";
    stream << oldVar.GetString();
    stream << " [New] ";
    stream << newVar.GetString();

    m_pLogModule->LogInfo(self, strPropertyName, stream.str(),  __FUNCTION__, __LINE__);

    return 0;
}

int AFCPropertyTrailModule::OnObjectRecordEvent(const AFGUID& self, const RECORD_EVENT_DATA& xEventData, const AFIData& oldVar, const AFIData& newVar)
{
    std::ostringstream stream;
    ARK_SHARE_PTR<AFIRecord> xRecord = m_pKernelModule->FindRecord(self, xEventData.strRecordName);
    if(nullptr == xRecord)
    {
        return 0;
    }

    switch(xEventData.nOpType)
    {
    case AFIRecord::RecordOptype::Add:
        {
            AFCDataList xDataList;
            bool bRet = xRecord->QueryRow(xEventData.nRow, xDataList);
            if(bRet)
            {
                stream << " Trail Add Row[" << xEventData.nRow << "]";

                for(int j = 0; j < xDataList.GetCount(); ++j)
                {
                    stream << " [" << j << "] " << xDataList.ToString(j);
                }

                m_pLogModule->LogInfo(self, xRecord->GetName(), stream.str(),  __FUNCTION__, __LINE__);
            }
        }
        break;
    case AFIRecord::RecordOptype::Del:
        {
            stream << " Trail Del Row[" << xEventData.nRow << "]";
            m_pLogModule->LogInfo(self, xRecord->GetName(), stream.str(),  __FUNCTION__, __LINE__);
        }
        break;
    case AFIRecord::RecordOptype::Swap:
        {
            stream << " Trail Swap Row[" << xEventData.nRow << "] Row[" << xEventData.nCol << "]";
            m_pLogModule->LogInfo(self, xRecord->GetName(), stream.str(),  __FUNCTION__, __LINE__);
        }
        break;
    case AFIRecord::RecordOptype::Create:
        break;
    case AFIRecord::RecordOptype::Update:
        {
            stream << " Trail UpData Row[" << xEventData.nRow << "] Col[" << xEventData.nCol << "]";
            //stream << " [Old] " << oldVar.ToString();
            //stream << " [New] " << newVar.ToString();
            m_pLogModule->LogInfo(self, xRecord->GetName(), stream.str(),  __FUNCTION__, __LINE__);
        }
        break;
    case AFIRecord::RecordOptype::Cleared:
        break;
    case AFIRecord::RecordOptype::Sort:
        break;
    default:
        break;
    }

    return 0;
}

int AFCPropertyTrailModule::TrailObjectData(const AFGUID& self)
{
    ARK_SHARE_PTR<AFIObject> xObject = m_pKernelModule->GetObject(self);
    if(nullptr == xObject)
    {
        return -1;
    }

    /* ARK_SHARE_PTR<AFIPropertyManager> xPropertyManager = xObject->GetPropertyManager();
     if(nullptr != xPropertyManager)
     {
         ARK_SHARE_PTR<AFIProperty> xProperty = xPropertyManager->First();
         while(nullptr != xProperty)
         {
             m_pKernelModule->AddPropertyCallBack(self, xProperty->GetKey(), this, &AFCPropertyTrailModule::OnObjectPropertyEvent);

             xProperty = xPropertyManager->Next();
         }
     }
    */
    ARK_SHARE_PTR<AFIRecordManager> xRecordManager = xObject->GetRecordManager();
    if(nullptr != xRecordManager)
    {
        ARK_SHARE_PTR<AFIRecord> xRecord = xRecordManager->First();
        while(nullptr != xRecord)
        {
            m_pKernelModule->AddRecordCallBack(self, xRecord->GetName(), this, &AFCPropertyTrailModule::OnObjectRecordEvent);


            xRecord = xRecordManager->Next();
        }
    }

    return 0;
}
