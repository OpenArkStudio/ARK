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
./scripts/clang_tidy.sh style-build src/application/main.cpp
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

## C++20 兼容层迁移

**源码不兼容变更：** ark 不再定义 `ARK_CPP11_OR_GREATER`、
`ARK_CPP14_OR_GREATER`、`ARK_CPP17_OR_GREATER`、`ARK_NOEXCEPT`、`ARK_CONSTEXPR`、
`ARK_FINAL` 和 `MAKE_BUILDIN_TYPE`。最低语言版本仍为 C++20。

| 移除的名称 | 迁移方式 |
| --- | --- |
| `ARK_CPP11_OR_GREATER`、`ARK_CPP14_OR_GREATER`、`ARK_CPP17_OR_GREATER` | ark 使用方删除过时的条件分支；独立支持其他语言版本的应用自行维护特性检查。 |
| `ARK_NOEXCEPT` | 直接使用 `noexcept`。 |
| `ARK_CONSTEXPR` | 直接使用 `constexpr`。 |
| `ARK_FINAL` | 直接使用 `final`。 |
| `MAKE_BUILDIN_TYPE(T)` | 若需保留自定义类型分类，显式特化 `ark::is_buildin_type<T>`，继续声明原有的 `yes`、`no` 枚举值。 |

移除 C++11 的 `std::make_unique` 回退实现；包含 `<memory>` 后使用标准 C++20 库。
平台导出、断言和所有权宏保持原有行为。

`is_buildin_type` 仍只识别 `uint16_t`、`uint32_t`、`uint64_t`、`int16_t`、
`int32_t` 和 `int64_t` 六种精确类型，以及与它们等同的原生类型别名；不会扩展到
所有整数类型，也不会移除 cv 或引用限定。`yes`、`no` 枚举值和
`check_buildin_type` 的签名、异常契约、参数推导保持不变：const 整数左值可通过
`const T&` 推导出未限定的类型，但显式检查 const 限定的 trait 仍返回 false。
常量类型和值保持不变，包括 UID 位分配和网络超时。

## 标准工具迁移

**源码不兼容变更：** 不再定义 `ARK_TO_STRING`、`ARRAY_LENTGH`、
`ARK_SRATIC_ASSERT`、`ARRAY_CLEAR`、`MEMORY_CLEAR` 和 `MEMORY_CLEAR_POINTER`。

| 移除的名称 | 迁移方式 |
| --- | --- |
| `ARK_TO_STRING(value)` | 包含 `<string>`，调用 `std::to_string(value)`。它与原别名调用的标准函数相同，包括 C++20 的浮点格式化行为。 |
| `ARRAY_LENTGH(array)` | 对有界数组，包含 `<iterator>` 并使用 `std::size(array)`。若需要查询类型的首维数组长度，包含 `<type_traits>` 并使用 `std::extent_v<T>`；`std::size` 不接受裸指针。 |
| `ARK_SRATIC_ASSERT` | 直接使用 `static_assert`。 |
| `ARRAY_CLEAR`、`MEMORY_CLEAR`、`MEMORY_CLEAR_POINTER` | 移除未使用的工具，不提供替代别名。根据对象的存储契约选择值初始化、类型明确的赋值或显式字节操作；不存在对所有对象都安全的清零操作。`MEMORY_CLEAR_POINTER` 还包含非法 token `0xx`。 |

现有数据和端点转换仍以相同的参数类型调用 `std::to_string`，数值输出和精度不变。
数组长度调用点保留原有的 `int` 变量、查找顺序、枚举映射和范围处理。
其余分配、所有权、运行时断言、平台、插件注册和字节序宏保持原有行为。
