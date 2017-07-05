#include "SDK/Interface/AFIPlugin.h"
#include "SDK/Interface/AFIPluginManager.h"

#ifdef NFLOGICCLASSPLUGIN_EXPORTS
#define NFLOGICCLASSCPLUGIN_API __declspec(dllexport)
#else
#define NFLOGICCLASSPLUGIN_API __declspec(dllimport)
#endif

class AFConfigPlugin : public AFIPlugin
{
public:
    AFConfigPlugin(AFIPluginManager* p);

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
