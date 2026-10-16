# 代码规范

本规范用于 ark 的统一整理与后续开发。公开接口、内部实现和文件名统一采用 `snake_case`。

## 命名

| 对象 | 规则 | 示例 |
| --- | --- | --- |
| 命名空间 | 小写，多个单词用下划线连接 | `ark`、`ark::detail` |
| 类、结构体与类型别名 | `snake_case` | `plugin_manager`、`module`、`data_list` |
| 枚举类型与枚举值 | `snake_case` | `data_type::dt_int64` |
| 函数与方法 | `snake_case` | `find_module()`、`start()`、`stop()` |
| 局部变量与参数 | `snake_case` | `module_name`、`initial_buffer_size` |
| 私有与受保护成员 | `snake_case`，末尾加 `_` | `module_name_`、`state_` |
| 公开结构体字段 | `snake_case` | `session_id` |
| 编译期常量 | `snake_case` | `default_buffer_size` |
| 模板参数 | `snake_case` | `module_type`、`buffer_size` |
| 预处理宏 | `UPPER_SNAKE_CASE`，使用 `ARK_` 前缀 | `ARK_EXPORT` |
| C++ 文件 | `snake_case`，头文件用 `.hpp`，实现用 `.cpp` | `plugin_manager.hpp`、`plugin_manager.cpp` |

`ark::plugin_manager` 是统一命名后的公开类型形式。接口和实现通过职责命名；新接口不再使用 `AFI`、`AFC` 类型前缀。依赖库的标识符保留其原有写法。

## 排版与 include 顺序

- 使用 4 个空格缩进，禁止 Tab，列宽为 120。
- `include` 按系统库、第三方库、项目自身头文件分组，组间留一个空行。
- 函数定义的左花括号放在签名同一行；短函数和空函数体均展开书写。
- `if`、`else`、循环的语句体使用花括号，短分支也不省略。
- `include` 与代码、命名空间与内部定义、相邻类型或函数定义之间留空行。
- `clang-format` 负责排版；命名规则在接口审查和相应静态检查中执行。

下面的片段用于演示命名与排版：

```cpp
#include <cstddef>

namespace ark {

class session_counter
{
public:
    void add(std::size_t count) {
        count_ += count;
    }

    std::size_t count() const {
        return count_;
    }

private:
    std::size_t count_ = 0;
};

} // namespace ark
```

## 实现约定

- 使用 C++20，依赖通过目标声明；公开头文件显式包含其使用的标准库头文件。
- 用 RAII 和清晰的所有权管理资源；借用的指针、引用和视图需要明确有效期。
- 使用 `override`、`final` 和 `const` 表达接口约束。
- 注释使用英文，说明约束、原因和非显然行为。公共 API 写清生命周期、线程和错误处理契约。
- 格式修改与行为修改分别审查；迁移一个模块时同步更新其调用点、示例和测试。

## 本地检查与生成代码

使用 clang-format 18 与仓库中的配置：

```sh
./scripts/format.sh
./scripts/format.sh --check
./scripts/clang_tidy.sh style-build
```

clang-tidy 需要已配置构建的编译数据库及兼容的编译器工具链。通过 `CLANG_FORMAT`、`CLANG_TIDY` 选择非默认可执行文件。格式检查覆盖 ark 自有 C++ 与 Protobuf 源码；第三方实现和构建生成物保留各自规范。

协议定义使用 `snake_case` 与 `ark::protobuf` C++ 命名空间；`consul`、`osslog` 保留既有 package。生成器的 `.pb.h` 后缀不变。配置键、反射字符串、外部 JSON 名称及 C# 生成代码面向业务的名称保留必要拼写。二进制与源码兼容边界见 [Protobuf 消息](../reference/protobuf_CN.md)。
