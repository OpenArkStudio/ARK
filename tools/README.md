# Offline tools

Usage and dependencies: [Configuration tools](../docs/guides/tools.md) · [配置工具](../docs/guides/tools_CN.md).

`config_tool/` and `gen-config.*` export the `resources/` workbooks. The exporter
requires Python 3 and openpyxl; the wrapper also needs clang-format and replaces
`include/ark/meta_define.hpp`.

`plugin_and_module_tool/` retains historical scaffolding sources and an x86-64
macOS binary. Its templates target the old SDK layout and are not supported for
current plugin generation. `gcm/conf/` contains configuration examples only; no
runnable environment manager is supplied. Moving these files does not validate
the old binaries or restore those integrations.
