#include <ark/application_manifest.hpp>
#include <ark/container/queue.hpp>
#include <ark/plugin_manager.hpp>

int main() {
    ark::application_manifest manifest;
    return manifest.plugins.empty() ? 0 : 1;
}
