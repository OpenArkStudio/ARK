# Application plugin manifests

An application manifest selects plugin libraries and their configuration files. Pass its path through the existing `-a` / `--app_conf` argument. A `.toml` suffix selects TOML; other paths retain legacy XML parsing. A TOML error never falls back to XML.

## TOML format

The shipped [sample1.app.toml](../build/conf/sample1.app.toml) is equivalent to [sample1.app.conf](../build/conf/sample1.app.conf):

```toml
[application]
plugin_library_dir = "bin/"
plugin_config_dir = "conf/"

[[plugins]]
name = "AFUtilityPlugin"
config = ""

[[plugins]]
name = "Sample1Plugin"
```

| Field | Type and default | Meaning |
| --- | --- | --- |
| `application` | Required table | Application manifest settings. |
| `application.plugin_library_dir` | String, `""` | Prefix for plugin library names. |
| `application.plugin_config_dir` | String, `""` | Prefix for plugin configuration names. |
| `plugins` | Array of tables, empty | Omission and a top-level `plugins = []` both select no plugins. |
| `plugins[].name` | Required nonempty string | Unique library stem, such as `Sample1Plugin`; platform extensions and the Debug `_d` suffix remain automatic. |
| `plugins[].config` | String, `""` | Configuration filename; omission and `""` retain empty-configuration behavior. |

Unknown fields, incorrect types, duplicate or empty plugin names, invalid TOML/UTF-8 and embedded NUL characters are rejected before plugin startup. Diagnostics contain the path and available line/column, without printing configuration values.

Directory strings are preserved exactly. Relative paths use the process working directory. The manager appends filenames by string concatenation, so directory prefixes normally need a trailing `/` or platform-appropriate separator. The loader does not add separators or require configuration files for empty `config` values.

The order of `[[plugins]]` entries does **not** define module initialization order: the manager still consumes an unordered map. This format introduces no dependency ordering or live reload mechanism.

## Run sample1

Follow the [application build instructions](networking.md) to build `app`, `AFUtilityPlugin` and `Sample1Plugin`. For a build directory named `out`, run these commands from the repository root:

```sh
cp -R build/conf out/conf
cd out
./bin/app --busid=1.0.0.1 --name=sample1 -a conf/sample1.app.toml
```

Debug builds use `app_d`. Logging still reads `conf/log.conf`; bus, logging, database and business configuration formats are unchanged.

## XML compatibility

The existing wrapper and attributes remain valid:

```xml
<xml>
    <plugins plugin_lib_dir="bin/" plugin_conf_dir="conf/">
        <plugin name="AFUtilityPlugin" conf=""/>
        <plugin name="Sample1Plugin"/>
    </plugins>
</xml>
```

The application loader requires a root containing a `plugins` child; a direct `<plugins>` root is not an application manifest. Missing directory attributes and `conf` remain empty strings, and unknown XML attributes remain ignored. XML duplicate/empty names and embedded NUL are now rejected during manifest loading instead of reaching plugin loading with ambiguous or truncated data. A manifest error exits the application with a nonzero status before `plugin_manager::start()`.

## C++ and build integration

`ark::config` is a static CMake target with a standard-library-only public header:

```cpp
#include <ark/application_manifest.hpp>

auto manifest = ark::load_application_manifest("conf/sample1.app.toml");
```

```cmake
target_link_libraries(my_loader PRIVATE ark::config)
```

`ARK_BUILD_CONFIG` defaults to `ON` for application or core-test builds, otherwise `OFF`. Applications require it. The target privately uses [toml++](https://github.com/marzer/tomlplusplus) (MIT): CMake prefers an installed `tomlplusplus` 3.4+ package, otherwise fetches v3.4.0 at `30172438cee64926dc41fdd9c11fb3ba5b2ba9de`. The target does not propagate parser headers or macros to `ark::core` / `ark::ark` consumers. RapidXML remains the XML dependency.

Run `cmake --build out --target check-application-manifest` for native regressions. `check-core` includes them when `ARK_BUILD_CONFIG=ON`.
