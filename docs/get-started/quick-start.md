# Quick start

Run Sample1 to check that the application can load plugins and enter the module lifecycle. It uses `AFUtilityPlugin` and `Sample1Plugin`; it does not require Consul, a database service or Excel configuration generation.

## Build the application and plugins

Complete [Install and build](installation.md) first. The commands below use its Release output directory, `app-build/bin/`, from the repository root. Keep the executable and plugin files from the same build together.

## Prepare a runtime directory

Create a separate runtime directory with the binaries, application manifest and logger settings. This keeps runtime files separate from the source configuration.

On macOS/Linux:

```sh
mkdir -p run/sample1/bin run/sample1/conf
cp -R app-build/bin/. run/sample1/bin/
cp build/conf/sample1.app.conf build/conf/log.conf run/sample1/conf/
```

On Windows, in PowerShell:

```powershell
New-Item -ItemType Directory -Path run/sample1/bin, run/sample1/conf -Force | Out-Null
Copy-Item app-build/bin/* run/sample1/bin/ -Recurse -Force
Copy-Item build/conf/sample1.app.conf, build/conf/log.conf run/sample1/conf/
```

The required runtime files are:

```text
run/sample1/
├── bin/       app + AFUtilityPlugin + Sample1Plugin from the same build
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

<Aside type="caution" title="Check initialization, not only the exit status">

The current launcher does not provide an aggregate startup-success exit code. If the expected module log is missing, inspect the configuration and library errors using [Application launcher](../guides/applications.md#troubleshoot-startup).

</Aside>

## Continue with your application

- [Create a module](../guides/modules.md) and [Create a plugin](../guides/plugins.md) explain how to register behavior.
- [Application launcher](../guides/applications.md) explains Bus identity, plugin selection and configuration paths. Applications that load Kernel data also need their generated `resource/` files.
- [Configuration tools](../guides/tools.md) shows the separate Excel-to-resource workflow. Its Python requirements do not apply to this sample or to the C++ network tests.
- [Network regression tests](../networking.md) verify network behavior independently. Applications that select Consul or database plugins need those services and their own settings.
