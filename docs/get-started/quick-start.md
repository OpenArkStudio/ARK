# Quick start

Build the C++20 application and sample plugins using [Install and build](installation.md). The checkout created by `git clone https://github.com/OpenArkStudio/ark.git` is named `ark`.

## Run the sample application

From the repository root, copy the chosen build's binaries into the runtime directory, then run from `build/` so relative configuration paths resolve correctly. For the Release build above on macOS or Linux:

```sh
mkdir -p build/bin
cp -R app-build/bin/. build/bin/
cd build
./bin/app --busid=1.0.1.1 --name=sample1 --app_conf=conf/sample1.app.conf
```

On Windows:

```powershell
New-Item -ItemType Directory -Path build/bin -Force | Out-Null
Copy-Item app-build/bin/* build/bin/ -Recurse -Force
cd build
.\bin\app.exe --busid=1.0.1.1 --name=sample1 --app_conf=conf/sample1.app.conf
```

Debug builds use `app_d` or `app_d.exe`. The checked-in [sample configuration](https://github.com/OpenArkStudio/ark/blob/main/build/conf/sample1.app.conf) selects the plugins to load; their binaries must be staged under `build/bin/` from the same build as the application. See [Application launcher](../guides/applications.md) for command-line parameters and [Create a plugin](../guides/plugins.md) for the sample implementation.

## Configuration and service discovery

- Checked-in configurations are under `build/conf/`, with data resources under `build/resource/`.
- Excel-based configuration generation is a separate tool. Its Python/openpyxl requirements and generated files are described in [Configuration tools](../guides/tools.md).
- Deployments that select the Consul plugin must provide their own Consul service and matching application configuration. Consul is not required for every plugin combination.

<Aside type="caution" title="Check deployment scripts before use">

Some existing multi-process launch scripts use older directory layouts and service settings. Check the executable names, plugin paths and ports against the selected configuration before running them.

</Aside>

## Verify networking separately

The [network regression tests](../networking.md) compile and run as C++ programs. They do not require Python or the Excel configuration tool.
