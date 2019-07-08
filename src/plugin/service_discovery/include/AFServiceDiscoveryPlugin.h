#include "interface/AFIPlugin.h"
#include "interface/AFIPluginManager.h"

namespace ark {

class AFServiceDiscoveryPlugin : public AFIPlugin
{
public:
    explicit AFServiceDiscoveryPlugin() = default;

    int GetPluginVersion() override;
    const std::string GetPluginName() override;

    void Install() override;
    void Uninstall() override;
};

} // namespace ark