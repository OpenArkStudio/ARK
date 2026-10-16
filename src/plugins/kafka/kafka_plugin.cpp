#include "ark/kafka/kafka_module_impl.hpp"
#include "ark/kafka/kafka_plugin.hpp"

namespace ark {

ARK_DECLARE_PLUGIN_DLL_FUNCTION(kafka_plugin)

void kafka_plugin::install() {
    ARK_REGISTER_MODULE(kafka_module, kafka_module_impl);
}

void kafka_plugin::uninstall() {
    ARK_UNREGISTER_MODULE(kafka_module, kafka_module_impl);
}

} // namespace ark
