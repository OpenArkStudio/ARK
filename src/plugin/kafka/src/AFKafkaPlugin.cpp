#include "kafka/include/AFKafkaPlugin.hpp"
#include "kafka/include/AFCKafkaModule.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(AFKafkaPlugin)

void AFKafkaPlugin::Install()
{
    ARK_REGISTER_MODULE(AFIKafkaModule, AFCKafkaModule);
}

void AFKafkaPlugin::Uninstall()
{
    ARK_UNREGISTER_MODULE(AFIKafkaModule, AFCKafkaModule);
}

} // namespace ark
