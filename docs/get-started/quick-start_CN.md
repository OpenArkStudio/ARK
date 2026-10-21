# 快速开始

运行 Sample1，确认应用能够加载插件并进入模块生命周期。它只使用 `ark_utility_plugin` 和 `sample1_plugin`，不需要 Consul、数据库服务或 Excel 配置生成步骤。

## 编译应用和插件

先完成[安装与构建](installation_CN.md)。以下命令在仓库根目录执行，使用该文档中的 Release 输出目录 `app-build/bin/`。程序和插件文件应来自同一次构建。

## 准备运行目录

创建独立运行目录，放入程序、应用配置和日志配置，使运行文件与源码配置分开。

macOS/Linux：

```sh
mkdir -p run/sample1/bin run/sample1/conf
cp -R app-build/bin/. run/sample1/bin/
cp config/sample1.app.toml config/log.conf run/sample1/conf/
```

Windows PowerShell：

```powershell
New-Item -ItemType Directory -Path run/sample1/bin, run/sample1/conf -Force | Out-Null
Copy-Item app-build/bin/* run/sample1/bin/ -Recurse -Force
Copy-Item config/sample1.app.toml, config/log.conf run/sample1/conf/
```

运行所需的文件如下：

```text
run/sample1/
├── bin/       同一次构建的 app、ark_utility_plugin 和 sample1_plugin
└── conf/
    ├── sample1.app.toml
    └── log.conf
```

复制命令还会带入同次构建的其他产物，但 `sample1.app.toml` 只选择上述两个动态库。此示例不需要单独的 `sample1.conf` 或 `resource/` 目录。

## 启动 Sample1

进入 `run/sample1/` 后启动。应用配置中的 `bin/` 和 `conf/` 都相对于这个工作目录。

macOS/Linux：

```sh
cd run/sample1
./bin/app --busid=1.0.1.1 --name=sample1 \
  --app_conf=conf/sample1.app.toml --log_conf=conf/log.conf
```

Windows：

```powershell
cd run/sample1
.\bin\app.exe --busid=1.0.1.1 --name=sample1 `
  --app_conf=conf/sample1.app.toml --log_conf=conf/log.conf
```

Debug 程序名为 `app_d` 或 `app_d.exe`，插件文件在平台后缀前带有 `_d`。将整套 Debug 产物放入独立运行目录，XML 中的插件名称保持不变。

## 确认运行结果

控制台应出现包含 `sample1_module` 和 `PostInit` 的日志。示例注册的定时器每十秒输出一次 `timer log`，共五次。附带的日志配置还会在运行目录的 `binlog/` 下写入文件。

macOS/Linux 使用 Ctrl+C 停止，Windows 在应用控制台输入 `exit`。Sample1 应依次输出 `PreShut`、`Shut`。

<Aside type="caution" title="检查初始化结果，不只检查退出码">

当前启动器没有汇总启动成功与否的退出码约定。如果没有出现目标模块日志，按[应用启动器](../guides/applications_CN.md#排查启动问题)中的步骤检查配置和动态库错误。

</Aside>

## 接入自己的应用

- [创建模块](../guides/modules_CN.md)和[创建插件](../guides/plugins_CN.md)说明如何注册功能。
- [应用启动器](../guides/applications_CN.md)说明 Bus 标识、插件选择和配置路径。使用 Kernel 数据的应用还需要生成的 `resource/` 文件。
- [配置工具](../guides/tools_CN.md)提供独立的 Excel 资源导出流程，其 Python 依赖不属于本示例或 C++ 网络测试的运行要求。
- [网络回归测试](../networking_CN.md)可独立验证网络行为。选择 Consul 或数据库插件的应用需要另外准备对应服务和配置。
