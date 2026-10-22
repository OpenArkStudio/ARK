#include <ark/application_manifest.hpp>

int main() {
    ark::application_manifest manifest;
    return manifest.plugins.empty() ? 0 : 1;
}
