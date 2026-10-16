# 应用启动器

启动器读取应用配置，加载所选插件的动态库。命令行解析逻辑见 [main.cpp](https://github.com/OpenArkStudio/ark/blob/main/src/app/main.cpp)。

| 参数 | 用途 |
| --- | --- |
| `--busid` / `-b` | 进程 Bus 地址 |
| `--name` / `-n` | 应用名称 |
| `--app_conf` / `-a` | 应用配置路径 |
| `--log_conf` / `-l` | 日志配置路径 |
| `--help` / `-h` | 输出参数说明 |

仓库提供了 [build/ 下的启动脚本](https://github.com/OpenArkStudio/ark/tree/main/build)与[配置](https://github.com/OpenArkStudio/ark/tree/main/build/conf)。运行前先核对应用、路径和进程标识。
