# Configuration tools

The [build/tools directory](https://github.com/OpenArkStudio/ark/tree/main/build/tools) contains the Excel configuration generator and runtime-environment tools.

| Tool | Purpose |
| --- | --- |
| `config_tool` | Generate configuration from Excel |
| `gcm` | Prepare a runtime environment |

The existing Excel generator uses Python and openpyxl; PyInstaller is used when packaging it as an executable. These requirements apply to that tool, not to the C++ build or network-test entry points. See its [README](https://github.com/OpenArkStudio/ark/blob/main/build/tools/config_tool/README.md).

TODO: complete parameter reference and a reproducible Excel-to-runtime example.

## Generated metadata names

Run `build/tools/gen-config.sh` or `gen-config.bat` with the tool's existing Python dependencies. The C++ output is `build/resource/code/meta_define.hpp`; the script formats it and copies it to `include/ark/meta_define.hpp`. It emits names such as `entity_meta_player`, `meta_name_index` and `get_index()` using the same snake_case policy as hand-written C++.

Excel class names, configuration fields and reflection strings are data contracts. They are not renamed by the C++ style migration, and the C# output keeps its existing names. Do not infer a resource-schema migration from a generated C++ filename change.
