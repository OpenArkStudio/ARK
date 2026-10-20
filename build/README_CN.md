# 运行文件

[English](README.md)

构建产物保存在 `<cmake-build-directory>/bin`。Release、Debug 和 sanitizer 构建使用
独立目录。将同一次构建的程序和插件复制到运行目录的 `bin/`，并准备 `conf/`；使用
业务资源的应用还需要 `resource/`。本目录保存示例配置、资源与工具，不追踪生成的二进制文件。

## 启动一个应用

Shell 和 Windows 脚本使用相同参数：

```text
run.sh  <Release|Debug> <name> <busid> [runtime-directory]
run.bat <Release|Debug> <name> <busid> [runtime-directory]
```

运行目录默认为脚本所在目录。显式传入的相对路径以调用者的工作目录为基准。
Release 使用 `bin/app`（Windows 为 `app.exe`），Debug 使用 `bin/app_d`
（Windows 为 `app_d.exe`）。脚本读取 `conf/<name>.app.conf` 和 `conf/log.conf`，
在前台运行应用；`name` 同时作为 `--name` 的值。使用 TOML 配置、不同的进程名称或
其他启动参数时，直接调用程序。

按[Sample1 快速开始](../docs/get-started/quick-start_CN.md)准备运行目录后，在仓库根目录执行：

```sh
sh build/run.sh Release sample1 1.0.1.1 run/sample1
```

Windows 使用 `build\run.bat Release sample1 1.0.1.1 run\sample1`。
使用应用前，检查初始化日志。脚本的路径检查不验证插件、业务配置或服务就绪状态。

## 停止一个应用

macOS/Linux 优先使用 Ctrl+C，Windows 优先在应用控制台输入 `exit`。
确认目标进程的 PID 后，使用 `kill.sh <pid>` 请求 `SIGTERM`；`kill.bat <pid>` 会强制终止
该 PID。两个脚本都只接受一个显式 PID，并拒绝其他可执行文件名。执行前重新确认 PID 的
可执行文件与命令行；进程退出后，PID 可能被复用。脚本不按名称通配批量停止进程。

## 迁移旧命令

- 将 `rund.sh` / `rund.bat` 改为 `run.sh Debug ...` / `run.bat Debug ...`。
- 将 `sample1.sh`、`mysql_sample.sh` 和 `sample5http_test.sh` 改为
  `run.sh Debug <name> <busid>`；名称分别为 `sample1`、`mysql_sample`、`sample5`。
- 旧版无参数五服务启动顺序引用了不存在的 `plugin_conf/`，Release 与 Debug 的总线 ID
  也不一致。现在需要显式传入每个应用的名称与总线 ID，并按目标环境安排启动顺序。
  新脚本不再将应用转为守护进程。
- 使用 `cmake --build <cmake-build-directory> --target clean` 清理选定构建目录，
  替代 `vc_clear.bat`。
- 容器构建文件和集群示例已移至 [deployment/](../deployment/README_CN.md)。

`docs/site/legacy-source/` 按原始字节保留历史文章，其中的 `bin/rund.*` 命令不再适用，
请使用上面的当前命令。
