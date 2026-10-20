# 应用插件清单

应用插件清单指定要加载的插件库及配置文件，通过 `-a` / `--app_conf` 传入 `.toml` 清单或原有 `.app.conf` XML 清单。TOML 解析失败时直接报错，不会回退到 XML。

## TOML 格式

仓库中的 [sample1.app.toml](../build/conf/sample1.app.toml) 与 [sample1.app.conf](../build/conf/sample1.app.conf) 等价，示例采用 TOML 1.0 语法：

```toml
[application]
plugin_library_dir = "bin/"
plugin_config_dir = "conf/"

[[plugins]]
name = "ark_utility_plugin"
config = ""

[[plugins]]
name = "sample1_plugin"
```

| 字段 | 类型及默认值 | 含义 |
| --- | --- | --- |
| `application` | 表，必填 | 应用清单设置。 |
| `application.plugin_library_dir` | 字符串，`""` | 插件库名称前的路径前缀。 |
| `application.plugin_config_dir` | 字符串，`""` | 插件配置文件名的路径前缀。 |
| `plugins` | 表数组，空 | 省略或在顶层设置 `plugins = []` 均表示不加载插件。 |
| `plugins[].name` | 非空字符串，必填 | 唯一的动态库名称，例如 `sample1_plugin`，不含扩展名；平台后缀及 Debug 的 `_d` 后缀仍由加载器补充。 |
| `plugins[].config` | 字符串，`""` | 配置文件名；省略或设置 `""` 均保留原有的空配置语义。 |

类型错误、未知字段、重复或空插件名、无效 TOML / UTF-8 和嵌入的 NUL 字符都会在插件启动前被拒绝。错误信息包含清单路径及可用的行列位置，不输出配置值。

加载器原样保留目录字符串，相对路径以进程工作目录为基准。库名和配置文件名通过字符串拼接追加到前缀后，因此目录前缀通常需要以 `/` 或平台适用的分隔符结尾，加载器不会自动补充分隔符。`config` 为空时，不要求存在对应的配置文件。

`[[plugins]]` 的书写顺序**不定义模块初始化顺序**，插件管理器仍接收无序映射。本次格式支持不引入依赖排序或运行时重新加载机制。

## 运行 sample1

按[应用构建说明](networking_CN.md)构建 `app`、`ark_utility_plugin` 和 `sample1_plugin`。假设构建目录为 `out`，从仓库根目录执行：

```sh
cp -R build/conf out/conf
cd out
./bin/app --busid=1.0.0.1 --name=sample1 -a conf/sample1.app.toml
```

Debug 构建使用 `app_d`。日志仍读取 `conf/log.conf`；总线、日志、数据库和业务配置的格式保持原样。

## XML 兼容性

原有包装节点和属性名继续有效：

```xml
<xml>
    <plugins plugin_lib_dir="bin/" plugin_conf_dir="conf/">
        <plugin name="ark_utility_plugin" conf=""/>
        <plugin name="sample1_plugin"/>
    </plugins>
</xml>
```

应用加载器要求根节点下包含 `plugins` 子节点，直接以 `<plugins>` 为根节点的文件不属于应用清单。目录属性和 `conf` 省略时仍为空字符串，未知 XML 属性仍被忽略。重复或空插件名和嵌入的 NUL 字符现在会在清单读取阶段被拒绝，避免将歧义或截断数据传入插件加载阶段。清单读取失败时，应用在调用 `plugin_manager::start()` 前以非零状态退出。

## C++ 和构建集成

`ark::config` 是静态 CMake 目标，公开头文件仅依赖 C++ 标准库。公开读取函数 `load_application_manifest()` 对 `.toml` 后缀使用 TOML，对其他路径使用 XML；应用的 `-a` 参数仍保留前述清单路径限制。

```cpp
#include <ark/application_manifest.hpp>

auto manifest = ark::load_application_manifest("conf/sample1.app.toml");
```

```cmake
target_link_libraries(my_loader PRIVATE ark::config)
```

构建应用或核心测试时，`ARK_BUILD_CONFIG` 默认为 `ON`，其他情况下默认为 `OFF`；应用构建要求启用此选项。目标内部使用 MIT 许可的 [toml++](https://github.com/marzer/tomlplusplus)：CMake 优先查找已安装的 3.4 或更新版本的 `tomlplusplus` 包，否则获取固定提交 `30172438cee64926dc41fdd9c11fb3ba5b2ba9de` 对应的 v3.4.0。此目标不会向 `ark::core` / `ark::ark` 的使用者传播解析器头文件和宏。XML 继续使用已有的 RapidXML 依赖。

运行 `cmake --build out --target check-application-manifest` 可执行原生回归；`ARK_BUILD_CONFIG=ON` 时，`check-core` 也会运行这些测试。
