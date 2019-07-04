#include "AFServiceDiscoveryPlugin.h"
#include "AFCServiceDiscoveryModule.h"

namespace ark
{
	//Dll entrance and exit function
	ARK_DLL_PLUGIN_ENTRY(AFServiceDiscoveryPlugin)
	ARK_DLL_PLUGIN_EXIT(AFServiceDiscoveryPlugin)

	//For now, we don't check version number
	int AFServiceDiscoveryPlugin::GetPluginVersion()
	{
		return 0;
	}

	const std::string AFServiceDiscoveryPlugin::GetPluginName()
	{
		return GET_CLASS_NAME(AFServiceDiscoveryPlugin);
	}

	void AFServiceDiscoveryPlugin::Install()
	{
		RegisterModule<AFIServiceDiscoveryModule, AFCServiceDiscoveryModule>();
	}

	void AFServiceDiscoveryPlugin::Uninstall()
	{
		DeregisterModule<AFIServiceDiscoveryModule, AFCServiceDiscoveryModule>();
	}

}