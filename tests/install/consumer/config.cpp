#include <fstream>

#include <ark/application_manifest.hpp>

int main() {
    const char* path = "installed-consumer.app.toml";
    {
        std::ofstream stream(path);
        stream << "[application]\nplugin_library_dir = 'bin/'\nplugin_config_dir = 'conf/'\n";
    }
    const auto manifest = ark::load_application_manifest(path);
    return manifest.plugin_library_dir == "bin/" && manifest.plugins.empty() ? 0 : 1;
}
