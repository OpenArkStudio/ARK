# 代码风格

简体中文 | [English](code-style.md)

ark 自有的 C++ 代码和协议定义使用统一的命名规则。

| 对象 | 规则 | 示例 |
| --- | --- | --- |
| 命名空间、类型、函数、文件 | `snake_case` | `ark::plugin_manager`、`find_module()`、`plugin_manager.hpp` |
| 参数、局部变量、模板参数 | `snake_case` | `message_type`、`session_id` |
| 常量、枚举值 | `snake_case` | `data_type::dt_int64` |
| 私有或受保护成员 | `snake_case`，以 `_` 结尾 | `module_name_` |
| 公开结构体字段 | `snake_case` | `session_id` |
| 预处理宏 | `UPPER_SNAKE_CASE` | `ARK_DECLARE_MODULE_FUNCTIONS` |

使用四个空格缩进，函数左大括号跟在声明行末，控制语句必须带大括号。
定义之间、命名空间起始行之后保留空行。include 按系统库、第三方库、ark 自有代码排序，
各组之间保留空行。空函数也使用多行函数体。

使用 clang-format 18 和仓库中的 `.clang-format`。在仓库根目录执行：

```sh
./scripts/format.sh
./scripts/format.sh --check
```

可通过 `CLANG_FORMAT` 指定可执行文件。脚本覆盖已跟踪和新增的 C++、Protobuf 源文件，
第三方实现和构建目录中的生成代码保留各自规范。

语义命名检查通过 clang-tidy 读取 CMake 编译数据库，本次验证使用版本 22：

```sh
cmake -S . -B style-build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build style-build
./scripts/clang_tidy.sh style-build
./scripts/clang_tidy.sh style-build src/app/main.cpp
```

可通过 `CLANG_TIDY` 指定可执行文件。默认检查编译数据库中实际启用的源文件；
检查可选插件前，需要先启用对应构建选项。第三方 API、插件入口符号 `DllEntryPlugin`、
`DllExitPlugin`、配置键和反射字符串保留原有拼写。
clang-tidy 应与 CMake 配置使用同一套编译器工具链，以正确找到 C++ 标准库头文件。

## 协议与元数据生成

修改 `.proto` 源定义，通过 CMake 重新生成 C++ 文件。公开协议命名空间为
`ark::protobuf`，生成头文件使用 `<ark/proto/common_messages.pb.h>` 等路径。
`.pb.h` 是 Protobuf 生成器使用的文件后缀。

本次命名迁移保留消息字段编号、字段类型、重复规则、枚举数值和对外 JSON 字段名。
生成的 C++ 名称、描述符名称、`Any` 类型 URL 和 JSON 枚举文字会改变，使用方需要重新编译，
并调整依赖这些名称的代码。`tests/core/protobuf_messages.cpp` 使用固定的旧版消息字节验证二进制兼容性。

现有配置生成器也按此规范生成 C++，并在复制到 `include/ark` 前格式化 `meta_define.hpp`。
配置数据、反射字符串和 C# 输出保留原有名称。
