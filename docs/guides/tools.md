# Configuration tools

The Excel generator converts the resource workbooks into XML metadata, configuration data and C++/C# definitions. It is an offline authoring step; launching Sample1 and running the C++ network tests do not require it.

## Requirements and command

Run the existing generator with Python 3 and `openpyxl` installed in the selected Python environment. PyInstaller is needed only when packaging a standalone executable.

```sh
python3 build/tools/config_tool/config_tool.py --help
python3 build/tools/config_tool/config_tool.py --path /path/to/resource
```

| Argument | Required | Meaning |
| --- | --- | --- |
| `--path` / `-p` | Yes | Resource root containing `excel/`, `meta/`, `server/` and `code/` |
| `--help` / `-h` | No | Print usage |

Although the argument help says “excel file path”, pass the **resource root**, not an `.xlsx` file or its `excel/` subdirectory. The generator writes beneath that root and expects its output directories to exist.

## Export a disposable resource copy

From the repository root, copy the supplied resource tree before changing workbooks. On macOS/Linux:

```sh
resource_demo=$(mktemp -d)
cp -R build/resource/. "$resource_demo/"
python3 build/tools/config_tool/config_tool.py --path "$resource_demo"
printf '%s\n' "$resource_demo"
```

On Windows, in PowerShell with Python and openpyxl available:

```powershell
$resourceDemo = Join-Path ([System.IO.Path]::GetTempPath()) ("ark-resource-" + [guid]::NewGuid())
New-Item -ItemType Directory -Path $resourceDemo | Out-Null
Copy-Item build/resource/* $resourceDemo -Recurse -Force
python build/tools/config_tool/config_tool.py --path $resourceDemo
Write-Output $resourceDemo
```

The supplied workbooks produce these files inside the copy:

| Output | Used for |
| --- | --- |
| `meta/entity_class.config`, `meta/common.meta`, `meta/player.meta` | Entity metadata index and definitions |
| `meta/config_class.config`, `meta/item.meta` and other configuration `.meta` files | Configuration metadata index and field definitions |
| `server/item.config`, `server/attribute.config`, `server/language.config`, `server/map.config` | Configuration rows |
| `code/meta_define.hpp` | Generated C++ names and field indexes |
| `code/AFMetaDefine.cs` | Generated C# definitions |

Inspect the generated XML and definitions before staging them for an application. The current generator does not produce a separate client export, migrate a database schema or reload a running process. To load the generated data with Kernel, copy the required `meta/` and `server/` outputs under the runtime's `resource/` directory; see [Application launcher](applications.md#resolve-configuration-paths).

## Edit an ordinary configuration workbook

Use `.xlsx`. For ordinary workbooks such as `item.xlsx`, the generator reads the first worksheet, with one field per column:

| Row | Meaning | First column in `item.xlsx` |
| --- | --- | --- |
| 1 | Field name | `id` |
| 2 | Field type | `uint64` |
| 3 | Target label | `a` |
| 4 | Reference | Empty |
| 5 | Description | `id` |
| 6 onward | Data values | `100001`, `100002`, … |

`entity_class.xlsx` uses a different layout for entity classes and fields; use the supplied workbook as its template. Keep class names, field names and reflection strings stable when changing a C++ identifier: they are resource contracts.

<Aside type="caution" title="Review the generated resource set">

The existing exporter is not a complete workbook validator. Do not rely on its `target` labels to separate server-only and client-only fields, or on empty cells becoming application defaults. Check the emitted XML, field types and intended distribution before deployment.

</Aside>

## Update the checked-in generated header

After reviewing a resource change, run the wrapper on a working branch. It exports the repository's `build/resource/` tree, formats `code/meta_define.hpp` with the project's clang-format rules and copies it to `include/ark/meta_define.hpp`. It overwrites those generated files.

On macOS/Linux, select the Python environment and the project's supported clang-format version:

```sh
PYTHON=python3 CLANG_FORMAT=clang-format sh build/tools/gen-config.sh
```

On Windows, set `PYTHON` and `CLANG_FORMAT` as needed, then run `build/tools/gen-config.bat`. Running `config_tool.py` directly does **not** perform the final header copy.

The generated header exposes snake_case C++ names while retaining resource strings such as `Player`:

```cpp
#include "ark/meta_define.hpp"

const auto& class_name = ark::entity_meta_player::self_name();
const auto field_index = ark::meta_name_index::get_index(class_name, "map_id");
```

An unknown class or field returns index `0`. C# output retains its existing naming conventions. Resource naming and format changes need separate compatibility decisions; generated C++ naming alone does not migrate the resource schema.

## Other files in the tools directory

| Directory | Current scope |
| --- | --- |
| `config_tool` | Excel exporter described above |
| `plugin_and_module_tool` | Legacy scaffolding templates using older interfaces; do not use them as current SDK examples. Follow [Create a module](modules.md) and [Create a plugin](plugins.md) instead |
| `gcm` | Configuration examples only; the directory does not currently provide a runnable environment-management tool |

The tool sources are under [build/tools](https://github.com/OpenArkStudio/ark/tree/main/build/tools). The application's XML plugin list, logger settings and generated Kernel resources are separate configuration inputs.
