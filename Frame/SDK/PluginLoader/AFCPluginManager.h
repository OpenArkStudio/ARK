// -------------------------------------------------------------------------
//    @FileName			:    AFCPluginManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    AFCPluginManager
//
// -------------------------------------------------------------------------

#ifndef AFC_PLUGIN_MANAGER_H
#define AFC_PLUGIN_MANAGER_H

#include <map>
#include <string>
#include <time.h>
#include "AFCDynLib.h"
#include "SDK/Core/AFSingleton.h"
#include "SDK/Interface/AFIPluginManager.h"

class AFCPluginManager
    : public AFIPluginManager,
	public NFSingleton<AFCPluginManager>
{
public:
    AFCPluginManager();
    virtual ~AFCPluginManager();

	virtual bool Init();

    virtual bool AfterInit();

    virtual bool CheckConfig();

    virtual bool BeforeShut();

    virtual bool Shut();

    //////////////////////////////////////////////////////////////////////////

    virtual void Registered(AFIPlugin* pPlugin);

    virtual void UnRegistered(AFIPlugin* pPlugin);

    virtual bool StartReLoadState();

    virtual bool EndReLoadState();
    //////////////////////////////////////////////////////////////////////////

    virtual AFIPlugin* FindPlugin(const std::string& strPluginName);

    virtual void AddModule(const std::string& strModuleName, AFIModule* pModule);

    virtual void RemoveModule(const std::string& strModuleName);

    virtual AFIModule* FindModule(const std::string& strModuleName);

    virtual bool Execute();

	virtual int AppID() const;

	virtual int64_t GetInitTime() const;

	virtual int64_t GetNowTime() const;

	virtual const std::string& GetConfigPath() const;

	virtual void SetConfigName(const std::string& strFileName);
protected:
	bool LoadPluginConfig();

	bool LoadStaticPlugin(const std::string& strPluginDLLName);
    bool LoadPluginLibrary(const std::string& strPluginDLLName);
    bool UnLoadPluginLibrary(const std::string& strPluginDLLName);
	bool UnLoadStaticPlugin(const std::string& strPluginDLLName);

private:
    int mnAppID;
    int64_t mnInitTime;
    int64_t mnNowTime;
    std::string mstrConfigPath;
	std::string mstrConfigName;

    typedef std::map<std::string, bool> PluginNameMap;
    typedef std::map<std::string, AFCDynLib*> PluginLibMap;
    typedef std::map<std::string, AFIPlugin*> PluginInstanceMap;
    typedef std::map<std::string, AFIModule*> ModuleInstanceMap;

    typedef void(* DLL_START_PLUGIN_FUNC)(AFIPluginManager* pm);
    typedef void(* DLL_STOP_PLUGIN_FUNC)(AFIPluginManager* pm);

    PluginNameMap mPluginNameMap;
    PluginLibMap mPluginLibMap;
    PluginInstanceMap mPluginInstanceMap;
    ModuleInstanceMap mModuleInstanceMap;
};

#endif
