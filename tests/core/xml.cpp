// Copyright (c) 2026 OpenArkStudio. Licensed under Apache-2.0. See LICENSE.
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "../network/doctest.h"

#include <ark/xml.hpp>

namespace {

class xml_files
{
public:
    xml_files()
        : directory_(std::filesystem::temp_directory_path() /
                     ("ark-xml-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()))) {
        if (!std::filesystem::create_directory(directory_))
        {
            throw std::runtime_error("could not create temporary XML directory");
        }
    }

    ~xml_files() {
        std::error_code ignored;
        std::filesystem::remove_all(directory_, ignored);
    }

    std::string write(std::string_view contents) const {
        const auto path = directory_ / "document.xml";
        std::ofstream output(path, std::ios::binary);
        output.exceptions(std::ios::badbit | std::ios::failbit);
        output << contents;
        output.close();
        return path.string();
    }

    std::string missing_path() const {
        return (directory_ / "missing.xml").string();
    }

private:
    std::filesystem::path directory_;
};

} // namespace

DOCTEST_TEST_CASE("XML accepts matching nested tags and preserves attribute access") {
    xml_files files;
    const std::string contents = R"(<?xml version="1.0"?>
<!-- project configuration -->
<plugins enabled="true" count="2">
    <plugin name="network"><options interval="1.5"/></plugin>
    <plugin name="database"/>
</plugins>)";
    const auto path = files.write(contents);
    ark::xml document(path);
    DOCTEST_CHECK(std::string(document.get_file()) == path);
    DOCTEST_CHECK(document.get_size() == contents.size() + 1);
    auto root = document.get_root_node();
    DOCTEST_REQUIRE(root.is_valid());
    DOCTEST_CHECK(root.get_bool("enabled"));
    DOCTEST_CHECK(root.get_uint32("count") == 2);
    auto child = root.find_node("plugin");
    DOCTEST_REQUIRE(child.is_valid());
    DOCTEST_CHECK(child.get_string("name") == "network");
    auto options = child.find_node("options");
    DOCTEST_REQUIRE(options.is_valid());
    DOCTEST_CHECK(options.get_float("interval") == doctest::Approx(1.5));
    child.next_node("plugin");
    DOCTEST_REQUIRE(child.is_valid());
    DOCTEST_CHECK(child.get_string("name") == "database");
    child.next_node("plugin");
    DOCTEST_CHECK_FALSE(child.is_valid());
    DOCTEST_CHECK(document.find_node("plugins").is_valid());
    DOCTEST_CHECK_FALSE(document.find_node("Plugins").is_valid());
}

DOCTEST_TEST_CASE("XML rejects mismatched root closing tags") {
    xml_files files;
    const std::array<std::string_view, 2> documents = {"<plugins></Plugins>", "<plugins></services>"};
    for (const auto contents : documents)
    {
        DOCTEST_CAPTURE(contents);
        const auto path = files.write(contents);
        DOCTEST_CHECK_THROWS_AS(ark::xml{path}, rapidxml::parse_error);
    }
}

DOCTEST_TEST_CASE("XML rejects mismatched nested closing tags") {
    xml_files files;
    const std::array<std::string_view, 3> documents = {"<plugins><plugin></Plugin></plugins>",
        "<plugins><plugin></options></plugins>", "<plugins><plugin></plugins></plugin>"};
    for (const auto contents : documents)
    {
        DOCTEST_CAPTURE(contents);
        const auto path = files.write(contents);
        DOCTEST_CHECK_THROWS_AS(ark::xml{path}, rapidxml::parse_error);
    }
}

DOCTEST_TEST_CASE("XML retains parse errors for malformed attributes and incomplete elements") {
    xml_files files;
    const std::array<std::string_view, 3> documents = {
        "<plugins enabled=true/>", "<plugins><plugin/>", "<plugins><plugin name=\"network></plugin></plugins>"};
    for (const auto contents : documents)
    {
        DOCTEST_CAPTURE(contents);
        const auto path = files.write(contents);
        DOCTEST_CHECK_THROWS_AS(ark::xml{path}, rapidxml::parse_error);
    }
}

DOCTEST_TEST_CASE("XML retains file errors for missing input") {
    xml_files files;
    const auto path = files.missing_path();
    DOCTEST_CHECK_THROWS_AS(ark::xml{path}, std::runtime_error);
}

DOCTEST_TEST_CASE("XML retains empty documents without a root node") {
    xml_files files;
    const auto path = files.write("");
    ark::xml document(path);
    DOCTEST_CHECK(document.get_size() == 1);
    DOCTEST_CHECK(document.get_data()[0] == '\0');
    DOCTEST_CHECK_FALSE(document.get_root_node().is_valid());
    DOCTEST_CHECK_FALSE(document.find_node("plugins").is_valid());
}

DOCTEST_TEST_CASE("XML parses the shipped freebird configuration") {
    const auto path = std::filesystem::path(ARK_TEST_SOURCE_DIR) / "build" / "conf" / "freebird.app.conf";
    ark::xml document(path.string());
    auto root = document.find_node("plugins");
    DOCTEST_REQUIRE(root.is_valid());
    DOCTEST_CHECK(root.get_string("plugin_lib_dir") == "bin/");
    DOCTEST_CHECK(root.get_string("plugin_conf_dir") == "conf/");
}
