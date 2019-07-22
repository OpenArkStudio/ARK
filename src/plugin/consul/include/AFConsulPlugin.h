#include "interface/AFIPlugin.h"
#include "interface/AFIPluginManager.h"

namespace ark {

class AFConsulPlugin : public AFIPlugin
{
public:
    explicit AFConsulPlugin() = default;

    int GetPluginVersion() override;
    const std::string GetPluginName() override;

    void Install() override;
    void Uninstall() override;
};

} // namespace ark
