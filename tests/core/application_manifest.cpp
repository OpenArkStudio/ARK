// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

#include "../network/doctest.h"

#include <ark/application_manifest.hpp>

namespace {

std::string utf8_path(const std::filesystem::path& path) {
    const auto encoded = path.u8string();
    return {encoded.begin(), encoded.end()};
}

std::filesystem::path native_path(std::string_view path) {
    return std::filesystem::path(std::u8string(path.begin(), path.end()));
}

class manifest_files
{
public:
    manifest_files()
        : directory_(std::filesystem::temp_directory_path() /
                     ("ark-manifest-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()))) {
        if (!std::filesystem::create_directory(directory_))
        {
            throw std::runtime_error("could not create temporary manifest directory");
        }
    }

    ~manifest_files() {
        std::error_code ignored;
        std::filesystem::remove_all(directory_, ignored);
    }

    std::string path(std::string_view name) const {
        return utf8_path(directory_ / native_path(name));
    }

    std::string write(std::string_view contents, std::string_view name = "sample.app.toml") const {
        const auto filename = path(name);
        std::ofstream output(native_path(filename), std::ios::binary);
        output.exceptions(std::ios::badbit | std::ios::failbit);
        output.write(contents.data(), static_cast<std::streamsize>(contents.size()));
        output.close();
        return filename;
    }

private:
    std::filesystem::path directory_;
};

void check_error(const std::string& path, bool has_position = true) {
    try
    {
        static_cast<void>(ark::load_application_manifest(path));
        DOCTEST_FAIL("invalid manifest was accepted");
    }
    catch (const ark::application_manifest_error& error)
    {
        DOCTEST_CHECK(error.path() == path);
        DOCTEST_CHECK(std::string(error.what()).find(path) != std::string::npos);
        DOCTEST_CHECK((error.line() > 0) == has_position);
        DOCTEST_CHECK((error.column() > 0) == has_position);
    }
}

} // namespace

DOCTEST_TEST_CASE("TOML and XML samples produce equivalent owning application manifests") {
    const auto base = std::filesystem::path(ARK_TEST_SOURCE_DIR) / "build" / "conf";
    const auto toml = ark::load_application_manifest(utf8_path(base / "sample1.app.toml"));
    const auto xml = ark::load_application_manifest(utf8_path(base / "sample1.app.conf"));
    DOCTEST_CHECK(toml.plugin_library_dir == "bin/");
    DOCTEST_CHECK(toml.plugin_config_dir == "conf/");
    DOCTEST_REQUIRE(toml.plugins.size() == 2);
    DOCTEST_CHECK(toml.plugins.at("ark_utility_plugin").empty());
    DOCTEST_CHECK(toml.plugins.at("sample1_plugin").empty());
    DOCTEST_CHECK(toml.plugin_library_dir == xml.plugin_library_dir);
    DOCTEST_CHECK(toml.plugin_config_dir == xml.plugin_config_dir);
    DOCTEST_CHECK(toml.plugins == xml.plugins);
}

DOCTEST_TEST_CASE("TOML preserves directory prefixes and optional empty plugin configurations") {
    ark::application_manifest manifest;
    {
        manifest_files files;
        manifest = ark::load_application_manifest(files.write(R"([application]
plugin_library_dir = 'relative.with.dots/bin'
plugin_config_dir = ''
[[plugins]]
name = 'FirstPlugin'
[[plugins]]
name = 'SecondPlugin'
config = ''
[[plugins]]
name = 'ThirdPlugin'
config = 'nested/service.xml'
)"));
    }
    DOCTEST_CHECK(manifest.plugin_library_dir == "relative.with.dots/bin");
    DOCTEST_CHECK(manifest.plugin_config_dir.empty());
    DOCTEST_REQUIRE(manifest.plugins.size() == 3);
    DOCTEST_CHECK(manifest.plugins.at("FirstPlugin").empty());
    DOCTEST_CHECK(manifest.plugins.at("SecondPlugin").empty());
    DOCTEST_CHECK(manifest.plugins.at("ThirdPlugin") == "nested/service.xml");
}

DOCTEST_TEST_CASE("Missing directories and omitted or empty plugin arrays retain empty manifest semantics") {
    manifest_files files;
    const std::array<std::string_view, 3> inputs = {
        "[application]\n", "plugins = []\n[application]\n", "<xml><plugins/></xml>"};
    for (std::size_t index = 0; index < inputs.size(); ++index)
    {
        const auto manifest = ark::load_application_manifest(
            files.write(inputs[index], index == 2 ? "empty.app.conf" : "empty.app.toml"));
        DOCTEST_CHECK(manifest.plugin_library_dir.empty());
        DOCTEST_CHECK(manifest.plugin_config_dir.empty());
        DOCTEST_CHECK(manifest.plugins.empty());
    }
}

DOCTEST_TEST_CASE("XML retains legacy attribute names optional conf and unknown attributes") {
    manifest_files files;
    const auto manifest = ark::load_application_manifest(files.write(
        R"(<xml><plugins plugin_lib_dir="bin/" plugin_conf_dir="../conf/" ignored="yes">
<plugin name="FirstPlugin"/><plugin name="SecondPlugin" conf="" extra="yes"/>
</plugins></xml>)",
        "legacy.app.conf"));
    DOCTEST_CHECK(manifest.plugin_library_dir == "bin/");
    DOCTEST_CHECK(manifest.plugin_config_dir == "../conf/");
    DOCTEST_REQUIRE(manifest.plugins.size() == 2);
    DOCTEST_CHECK(manifest.plugins.at("FirstPlugin").empty());
    DOCTEST_CHECK(manifest.plugins.at("SecondPlugin").empty());
}

DOCTEST_TEST_CASE("TOML validates required tables exact field types and plugin names") {
    manifest_files files;
    const std::array<std::string_view, 14> invalid = {"", "plugins = []\n", "application = 7\n", "application = []\n",
        "[application]\nplugin_library_dir = 1\n", "[application]\nplugin_config_dir = false\n",
        "plugins = 'bad'\n[application]\n", "plugins = [1]\n[application]\n", "[plugins]\n[application]\n",
        "[application]\n[[plugins]]\n", "[application]\n[[plugins]]\nname = ''\n",
        "[application]\n[[plugins]]\nname = 42\n", "[application]\n[[plugins]]\nname = 'Plugin'\nconfig = []\n",
        "[application]\n[[plugins]]\nname = 'Plugin'\nconfig = {}\n"};
    for (const auto input : invalid)
    {
        DOCTEST_CAPTURE(input);
        check_error(files.write(input));
    }
}

DOCTEST_TEST_CASE("TOML rejects unknown fields at every schema level") {
    manifest_files files;
    const std::array<std::string_view, 4> invalid = {"typo = 1\n[application]\n",
        "[application]\nplugin_lib_dir = 'bin/'\n", "[application]\n[[plugins]]\nname = 'Plugin'\nconf = ''\n",
        "[application]\n[extra]\n"};
    for (const auto input : invalid)
    {
        check_error(files.write(input));
    }
}

DOCTEST_TEST_CASE("Both manifest formats reject duplicate and empty plugin names before returning a result") {
    manifest_files files;
    const std::array<std::string_view, 4> invalid = {
        "[application]\n[[plugins]]\nname = 'Plugin'\n[[plugins]]\nname = 'Plugin'\n",
        "<xml><plugins><plugin name='Plugin'/><plugin name='Plugin'/></plugins></xml>",
        "<xml><plugins><plugin/></plugins></xml>", "<xml><plugins><plugin name=''/></plugins></xml>"};
    for (std::size_t index = 0; index < invalid.size(); ++index)
    {
        check_error(files.write(invalid[index], index == 0 ? "duplicate.toml" : "invalid.app.conf"));
    }
    auto manifest = ark::load_application_manifest(files.write("[application]\n[[plugins]]\nname = 'Original'\n"));
    DOCTEST_CHECK_THROWS_AS(
        manifest = ark::load_application_manifest(files.write(invalid[0])), ark::application_manifest_error);
    DOCTEST_REQUIRE(manifest.plugins.size() == 1);
    DOCTEST_CHECK(manifest.plugins.contains("Original"));
}

DOCTEST_TEST_CASE("TOML and XML syntax errors have source positions without disclosing configuration values") {
    manifest_files files;
    const std::array<std::string_view, 4> invalid = {"[application]\nplugin_library_dir = SECRET_SENTINEL\n",
        "[application]\n[[plugins]]\nname = 'Plugin'\nname = 'SECRET_SENTINEL'\n",
        "<xml>\n<plugins secret='SECRET_SENTINEL'></Plugins></xml>", "<xml><plugins secret=SECRET_SENTINEL/></xml>"};
    for (std::size_t index = 0; index < invalid.size(); ++index)
    {
        const auto path = files.write(invalid[index], index < 2 ? "syntax.toml" : "syntax.app.conf");
        check_error(path);
        try
        { static_cast<void>(ark::load_application_manifest(path)); }
        catch (const ark::application_manifest_error& error)
        { DOCTEST_CHECK(std::string(error.what()).find("SECRET_SENTINEL") == std::string::npos); }
    }
    const auto path = files.write("[application]\nplugin_library_dir = 7\n");
    try
    {
        static_cast<void>(ark::load_application_manifest(path));
        DOCTEST_FAIL("numeric directory accepted");
    }
    catch (const ark::application_manifest_error& error)
    {
        DOCTEST_CHECK(error.line() == 2);
        DOCTEST_CHECK(error.column() > 0);
    }
}

DOCTEST_TEST_CASE("Manifest suffix dispatch never falls back from TOML to XML") {
    manifest_files files;
    check_error(files.write("<xml><plugins/></xml>"));
    check_error(files.write("[application]\n", "wrong.app.conf"));
    check_error(files.write("[application]\n", "wrong.app.toml.extra"));
    check_error(files.write("<plugins/> unmatched", "empty.app.conf"));
    check_error(files.write("", "empty.app.conf"));
    check_error(files.write("<plugins/>", "direct-root.app.conf"));
}

DOCTEST_TEST_CASE("Manifest strings and input reject raw or decoded embedded NUL") {
    manifest_files files;
    const std::array<std::string_view, 5> decoded = {"[application]\nplugin_library_dir = \"x\\u0000y\"\n",
        "[application]\nplugin_config_dir = \"x\\u0000y\"\n", "[application]\n[[plugins]]\nname = \"x\\u0000y\"\n",
        "[application]\n[[plugins]]\nname = 'Plugin'\nconfig = \"x\\u0000y\"\n",
        "<xml><plugins><plugin name='Plugin' conf='x&#0;y'/></plugins></xml>"};
    for (std::size_t index = 0; index < decoded.size(); ++index)
    {
        check_error(files.write(decoded[index], index == 4 ? "nul.app.conf" : "nul.toml"));
    }
    for (const auto name : {"raw.toml", "raw.app.conf"})
    {
        std::string input = name == std::string_view("raw.toml") ? "[application]\n" : "<xml><plugins/></xml>";
        input += '\0';
        input += "hidden data";
        check_error(files.write(input, name));
    }
    const auto nul_path = files.path("missing") + std::string("\0.toml", 6);
    DOCTEST_CHECK_THROWS_AS(ark::load_application_manifest(nul_path), ark::application_manifest_error);
}

DOCTEST_TEST_CASE("TOML supports UTF8 values and native Unicode filenames and rejects invalid UTF8") {
    manifest_files files;
    const auto path =
        files.write("[application]\nplugin_config_dir = '配置/'\n[[plugins]]\nname = '插件'\n", "配置.app.toml");
    const auto manifest = ark::load_application_manifest(path);
    DOCTEST_CHECK(manifest.plugin_config_dir == "配置/");
    DOCTEST_CHECK(manifest.plugins.contains("插件"));
    std::string invalid = "[application]\nplugin_config_dir = '";
    invalid += static_cast<char>(0xff);
    invalid += "'\n";
    check_error(files.write(invalid));
}

DOCTEST_TEST_CASE("Missing files and unreadable directory inputs fail without partial results") {
    manifest_files files;
    check_error(files.path("missing.toml"), false);
    check_error(files.path("missing.app.conf"), false);
    const auto directory = files.path("directory.toml");
    DOCTEST_REQUIRE(std::filesystem::create_directory(native_path(directory)));
    check_error(directory, false);
}
