// -------------------------------------------------------------------------
//    @FileName			:    AFIPluginManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFIPluginManager
//
// -------------------------------------------------------------------------

#pragma once

#include "AFIModule.h"

#define FIND_MODULE(classBaseName, className)  \
	assert((TIsDerived<classBaseName, AFIModule>::Result));

class AFIPlugin;

class AFIPluginManager : public AFIModule
{
public:
    AFIPluginManager()
    {

    }

	template <typename T>
	T* FindModule()
	{
		AFIModule* pLogicModule = FindModule(typeid(T).name());
		if (pLogicModule)
		{
			if (!TIsDerived<T, AFIModule>::Result)
			{
				return NULL;
			}

			T* pT = dynamic_cast<T*>(pLogicModule);
			assert(NULL != pT);

			return pT;
		}
		assert(NULL);
		return NULL;
	}

    virtual void Registered(AFIPlugin* plugin) = 0;

    virtual void UnRegistered(AFIPlugin* plugin) = 0;

    virtual AFIPlugin* FindPlugin(const std::string& strPluginName) = 0;

    virtual void AddModule(const std::string& strModuleName, AFIModule* pModule) = 0;

    virtual void RemoveModule(const std::string& strModuleName) = 0;

    virtual AFIModule* FindModule(const std::string& strModuleName) = 0;

    virtual int AppID() const = 0;
    virtual AFINT64 GetInitTime() const = 0;
    virtual AFINT64 GetNowTime() const = 0;
	virtual const std::string& GetConfigPath() const = 0;
	virtual void SetConfigName(const std::string& strFileName) = 0;
};
