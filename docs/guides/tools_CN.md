# 配置工具

[build/tools](https://github.com/OpenArkStudio/ark/tree/main/build/tools) 包含 Excel 配置生成器与运行环境工具。

| 工具 | 用途 |
| --- | --- |
| `config_tool` | 从 Excel 生成配置 |
| `gcm` | 准备运行环境 |

现有 Excel 生成器使用 Python 与 openpyxl，打包为可执行文件时使用 PyInstaller。这些依赖只属于该工具，C++ 构建和网络测试入口不需要它们。详见[工具 README](https://github.com/OpenArkStudio/ark/blob/main/build/tools/config_tool/README.md)。

TODO：补齐参数参考与从 Excel 到运行时的完整示例。

## 生成的元数据名称

使用原有 Python 依赖运行 `build/tools/gen-config.sh` 或 `gen-config.bat`。C++ 输出为 `build/resource/code/meta_define.hpp`，脚本格式化后复制到 `include/ark/meta_define.hpp`。生成名称如 `entity_meta_player`、`meta_name_index`、`get_index()`，与手写 C++ 采用相同的 snake_case 规范。

Excel 类名、配置字段和反射字符串属于数据约定，不随 C++ 风格迁移改名；C# 输出也保留原有名称。生成的 C++ 文件改名不代表资源 schema 已迁移。
