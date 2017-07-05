// -------------------------------------------------------------------------
//    @FileName			:    AFCPropertyConfigModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-09-30
//    @Module           :    AFCPropertyConfigModule
//
// -------------------------------------------------------------------------

#include "AFCPropertyConfigModule.h"
#include "SDK/Interface/AFIPluginManager.h"

bool AFCPropertyConfigModule::Init()
{
    return true;
}

bool AFCPropertyConfigModule::Shut()
{
    return true;
}

bool AFCPropertyConfigModule::Execute()
{
    return true;
}

bool AFCPropertyConfigModule::AfterInit()
{
    m_pClassModule = pPluginManager->FindModule<AFIClassModule>();
    m_pElementModule = pPluginManager->FindModule<AFIElementModule>();

    Load();

    return true;
}

int AFCPropertyConfigModule::CalculateBaseValue(const int nJob, const int nLevel, const std::string& strProperty)
{
	NF_SHARE_PTR <AFMapEx<int, std::string> > xPropertyMap = mhtCoefficienData.GetElement(nJob);
	if (xPropertyMap)
	{
		NF_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);
		if (xRefPropertyIDName)
		{
			return m_pElementModule->GetPropertyInt(*xRefPropertyIDName, strProperty);
		}
    }

    return 0;
}

void AFCPropertyConfigModule::Load()
{
    NF_SHARE_PTR<AFIClass> pLogicClass = m_pClassModule->GetElement(NFrame::InitProperty::ThisName());
    if (nullptr != pLogicClass)
    {
        NFList<std::string>& xList = pLogicClass->GetConfigNameList();
        std::string strData;
        bool bRet = xList.First(strData);
        while (bRet)
        {
            NF_SHARE_PTR<AFIPropertyManager> pPropertyManager = m_pElementModule->GetPropertyManager(strData);
            if (nullptr != pPropertyManager)
            {
                int nJob = m_pElementModule->GetPropertyInt(strData, NFrame::InitProperty::Job());
                int nLevel = m_pElementModule->GetPropertyInt(strData, NFrame::InitProperty::Level());
                std::string strEffectData = m_pElementModule->GetPropertyString(strData, NFrame::InitProperty::EffectData());

				NF_SHARE_PTR <AFMapEx<int, std::string> > xPropertyMap = mhtCoefficienData.GetElement(nJob);
				if (!xPropertyMap)
				{
					xPropertyMap = NF_SHARE_PTR<AFMapEx<int, std::string>>(NF_NEW AFMapEx<int, std::string>());
					mhtCoefficienData.AddElement(nJob, xPropertyMap);

					NF_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);
					if (!xRefPropertyIDName)
					{
						xRefPropertyIDName = NF_SHARE_PTR<std::string>(NF_NEW std::string(strEffectData));
						xPropertyMap->AddElement(nLevel, xRefPropertyIDName);
					}
				}
            }

            bRet = xList.Next(strData);
        }
    }
}

bool AFCPropertyConfigModule::LegalLevel(const int nJob, const int nLevel)
{
	NF_SHARE_PTR <AFMapEx<int, std::string> > xPropertyMap = mhtCoefficienData.GetElement(nJob);
	if (xPropertyMap)
	{
		NF_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);
		if (xRefPropertyIDName)
		{
			return true;
		}
	}


    return false;
}
