#include "SDK/Interface/NFIPlugin.h"
#include "SDK/Interface/NFIPluginManager.h"

#ifdef NFLOGICCLASSPLUGIN_EXPORTS
#define NFLOGICCLASSCPLUGIN_API __declspec(dllexport)
#else
#define NFLOGICCLASSPLUGIN_API __declspec(dllimport)
#endif

class NFConfigPlugin : public NFIPlugin
{
public:
    NFConfigPlugin(NFIPluginManager* p);

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
