#include "SDK/Interface/NFIPlugin.h"
#include "SDK/Interface/NFIPluginManager.h"

#ifdef NFLOGPLUGIN_EXPORTS
#define NFLOGPLUGIN_API __declspec(dllexport)
#else
#define NFLOGPLUGIN_API __declspec(dllimport)
#endif


//////////////////////////////////////////////////////////////////////////
class UtilityPlugin : public NFIPlugin
{
public:
    UtilityPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
