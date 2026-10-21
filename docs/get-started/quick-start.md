# Quick start

Run Sample1 to check that the application can load plugins and enter the module lifecycle. It uses `ark_utility_plugin` and `sample1_plugin`; it does not require Consul, a database service or Excel configuration generation.

## Build the application and plugins

Complete [Install and build](installation.md) first. The commands below use its Release output directory, `app-build/bin/`, from the repository root. Keep the executable and plugin files from the same build together.

## Prepare a runtime directory

Create a separate runtime directory with the binaries, application manifest and logger settings. This keeps runtime files separate from the source configuration.

On macOS/Linux:

```sh
mkdir -p run/sample1/bin run/sample1/conf
cp -R app-build/bin/. run/sample1/bin/
cp config/sample1.app.conf config/log.conf run/sample1/conf/
```

On Windows, in PowerShell:

```powershell
New-Item -ItemType Directory -Path run/sample1/bin, run/sample1/conf -Force | Out-Null
Copy-Item app-build/bin/* run/sample1/bin/ -Recurse -Force
Copy-Item config/sample1.app.conf, config/log.conf run/sample1/conf/
```

The required runtime files are:

```text
run/sample1/
├── bin/       app + ark_utility_plugin + sample1_plugin from the same build
└── conf/
    ├── sample1.app.conf
    └── log.conf
```

The copy commands also stage the build's other artifacts. Only the two libraries named by `sample1.app.conf` are selected by this sample. No separate `sample1.conf` or `resource/` directory is required for it.

## Start Sample1

Run from `run/sample1/`: the manifest's `bin/` and `conf/` paths are relative to the working directory.

On macOS/Linux:

```sh
cd run/sample1
./bin/app --busid=1.0.1.1 --name=sample1 \
  --app_conf=conf/sample1.app.conf --log_conf=conf/log.conf
```

On Windows:

```powershell
cd run/sample1
.\bin\app.exe --busid=1.0.1.1 --name=sample1 `
  --app_conf=conf/sample1.app.conf --log_conf=conf/log.conf
```

Debug builds use `app_d` or `app_d.exe` and plugin filenames with `_d` before the platform suffix. Use the complete Debug output in its own runtime directory; the XML plugin names stay unchanged.

## Check the result

The console should show a `sample1_module` log with `PostInit`. The sample registers a timer that emits `timer log` at ten-second intervals, five times. The supplied logger settings also write beneath `binlog/` in the runtime directory.

Stop with Ctrl+C on macOS/Linux, or enter `exit` in the Windows application console. Sample1 should log `PreShut`, then `Shut`.

<Aside type="caution" title="Handle startup failures">

A failed plugin load or module startup callback stops later startup stages and updates. The launcher reports the failing stage and exits with code `1`. Use [Application launcher](../guides/applications.md#troubleshoot-startup) to diagnose configuration and library errors.

</Aside>

## Continue with your application

- [Create a module](../guides/modules.md) and [Create a plugin](../guides/plugins.md) explain how to register behavior.
- [Application launcher](../guides/applications.md) explains Bus identity, plugin selection and configuration paths. Applications that load Kernel data also need their generated `resource/` files.
- [Configuration tools](../guides/tools.md) shows the separate Excel-to-resource workflow. Its Python requirements do not apply to this sample or to the C++ network tests.
- [Network regression tests](../networking.md) verify network behavior independently. Applications that select Consul or database plugins need those services and their own settings.

## Read the typed sample construction

`examples/sample1` and `examples/sample2` use
[`module_base<interface_type>`](../guides/modules.md#use-a-base-class-with-a-private-implementation)
and [`plugin_base<derived_type>`](../guides/plugins.md#use-typed-registration-and-explicit-entries).
The sample classes remain in their plugin's private, adjacent headers. They do
not introduce a separate service interface: each inherits `module_base<module>`
and retains its existing `sample1_module` or `sample2_module` lookup identity.

Each `sampleN_plugin.hpp` declares a factory returning
`std::unique_ptr<sampleN_module>`; `sampleN_module.cpp` defines it. The plugin
registers that owned result and explicitly exports `DllEntryPlugin` /
`DllExitPlugin`, including the existing logger initialization. Successful
registration still requires explicit unregister/delete during `uninstall()`.

| Sample | Update registration | Runtime dependencies |
| --- | --- | --- |
| Sample1 | `module_update::enabled`, preserving its `update()` override | Utility plugin; no external business metadata |
| Sample2 | `module_update::disabled`, preserving its inherited default `update()` | Kernel and Utility plugins; business metadata under `resource/meta/` |

The lifecycle/business method bodies and manifest library names are unchanged.
Sample2's metadata is external business data; this construction change does not
generate or convert it. When applications, examples, configuration and core tests
are enabled with the required Kernel/Utility plugins, run
`cmake --build app-build --target check-typed-examples -j2` to load the actual
Sample1/Sample2 DSOs, execute five updates on the manager thread and verify ordered startup and
shutdown. This test supplies a small test dataset for Sample2; it does not
validate an application's exported business dataset.
