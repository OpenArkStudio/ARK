# 配置工具

Excel 生成器将资源工作簿转换为 XML 元数据、配置数据及 C++、C# 定义。这是离线制作资源的步骤，启动 Sample1 和运行 C++ 网络测试都不需要执行它。

## 运行条件与命令

使用 Python 3 运行现有生成器，并在所选 Python 环境中安装 `openpyxl`。只有打包独立可执行文件时才需要 PyInstaller。

```sh
python3 build/tools/config_tool/config_tool.py --help
python3 build/tools/config_tool/config_tool.py --path /path/to/resource
```

| 参数 | 必填 | 含义 |
| --- | --- | --- |
| `--path` / `-p` | 是 | 包含 `excel/`、`meta/`、`server/`、`code/` 的资源根目录 |
| `--help` / `-h` | 否 | 输出参数说明 |

参数帮助虽然写着「excel file path」，实际应传入**资源根目录**，不是 `.xlsx` 文件或 `excel/` 子目录。生成器在该根目录下写文件，并要求输出目录已经存在。

## 在临时资源副本中导出

先在仓库根目录复制附带的资源目录，再修改工作簿。macOS/Linux：

```sh
resource_demo=$(mktemp -d)
cp -R build/resource/. "$resource_demo/"
python3 build/tools/config_tool/config_tool.py --path "$resource_demo"
printf '%s\n' "$resource_demo"
```

Windows PowerShell，需先准备 Python 和 openpyxl：

```powershell
$resourceDemo = Join-Path ([System.IO.Path]::GetTempPath()) ("ark-resource-" + [guid]::NewGuid())
New-Item -ItemType Directory -Path $resourceDemo | Out-Null
Copy-Item build/resource/* $resourceDemo -Recurse -Force
python build/tools/config_tool/config_tool.py --path $resourceDemo
Write-Output $resourceDemo
```

附带的工作簿会在副本中生成以下文件：

| 输出 | 用途 |
| --- | --- |
| `meta/entity_class.config`、`meta/common.meta`、`meta/player.meta` | 实体元数据索引和定义 |
| `meta/config_class.config`、`meta/item.meta` 等配置 `.meta` 文件 | 配置元数据索引和字段定义 |
| `server/item.config`、`server/attribute.config`、`server/language.config`、`server/map.config` | 配置数据行 |
| `code/meta_define.hpp` | 生成的 C++ 名称和字段索引 |
| `code/AFMetaDefine.cs` | 生成的 C# 定义 |

部署前先检查生成的 XML 和定义。当前生成器不会另外导出一套客户端资源，也不会迁移数据库结构或重新加载运行中的进程。Kernel 加载这些数据时，需要把所需的 `meta/`、`server/` 输出放到运行目录的 `resource/` 下，详见[应用启动器](applications_CN.md#确定配置路径)。

## 修改普通配置工作簿

使用 `.xlsx` 格式。对于 `item.xlsx` 等普通配置，生成器读取第一张工作表，每列表示一个字段：

| 行 | 含义 | `item.xlsx` 第一列 |
| --- | --- | --- |
| 1 | 字段名 | `id` |
| 2 | 字段类型 | `uint64` |
| 3 | 目标标签 | `a` |
| 4 | 引用 | 空 |
| 5 | 说明 | `id` |
| 第 6 行起 | 数据值 | `100001`、`100002` 等 |

`entity_class.xlsx` 使用另一种布局描述实体类和字段，应以仓库附带工作簿为模板。修改 C++ 标识符时，不要顺带改变类名、字段名和反射字符串，它们属于资源约定。

<Aside type="caution" title="检查生成的资源范围">

现有导出器不提供完整的工作簿校验。不要依赖 `target` 标签隔离服务端和客户端字段，也不要假设空单元格会转换成应用默认值。部署前应检查输出 XML、字段类型和预期分发范围。

</Aside>

## 更新仓库中的生成头文件

资源变更经过检查后，在工作分支运行包装脚本。脚本导出仓库的 `build/resource/`，按项目 clang-format 规则格式化 `code/meta_define.hpp`，再复制到 `include/ark/meta_define.hpp`，会覆盖这些生成文件。

macOS/Linux 上选择 Python 环境和项目支持的 clang-format 版本：

```sh
PYTHON=python3 CLANG_FORMAT=clang-format sh build/tools/gen-config.sh
```

Windows 按需设置 `PYTHON`、`CLANG_FORMAT` 后运行 `build/tools/gen-config.bat`。直接运行 `config_tool.py` **不会**执行最后的头文件复制。

生成头文件使用 snake_case 的 C++ 名称，保留 `Player` 等资源字符串：

```cpp
#include "ark/meta_define.hpp"

const auto& class_name = ark::entity_meta_player::self_name();
const auto field_index = ark::meta_name_index::get_index(class_name, "map_id");
```

类或字段不存在时返回索引 `0`。C# 输出保留原有命名规范。资源命名和格式变化需要单独确定兼容规则，生成的 C++ 名称变化不代表资源 schema 已迁移。

## 工具目录中的其他内容

| 目录 | 当前用途 |
| --- | --- |
| `config_tool` | 本文介绍的 Excel 导出器 |
| `plugin_and_module_tool` | 使用旧接口的历史脚手架模板，不应作为当前 SDK 示例；创建方式见[模块](modules_CN.md)和[插件](plugins_CN.md)指南 |
| `gcm` | 仅有配置示例，当前目录未提供可执行的环境管理工具 |

工具源码位于 [build/tools](https://github.com/OpenArkStudio/ark/tree/main/build/tools)。应用的 XML 插件列表、日志配置与生成的 Kernel 资源是不同的配置输入。
