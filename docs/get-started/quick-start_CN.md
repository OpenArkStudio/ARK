# 快速开始

先按[安装与构建](installation_CN.md)编译 C++20 应用和示例插件。`git clone https://github.com/OpenArkStudio/ark.git` 创建的源码目录名为 `ark`。

## 运行示例应用

在仓库根目录执行下面的命令，将选定构建的二进制文件复制到运行目录，再进入 `build/`，确保相对配置路径能够正确解析。上文的 Release 构建在 macOS 或 Linux 上使用：

```sh
mkdir -p build/bin
cp -R app-build/bin/. build/bin/
cd build
./bin/app --busid=1.0.1.1 --name=sample1 --app_conf=conf/sample1.app.conf
```

Windows 使用：

```powershell
New-Item -ItemType Directory -Path build/bin -Force | Out-Null
Copy-Item app-build/bin/* build/bin/ -Recurse -Force
cd build
.\bin\app.exe --busid=1.0.1.1 --name=sample1 --app_conf=conf/sample1.app.conf
```

Debug 构建的程序名为 `app_d` 或 `app_d.exe`。[示例配置](https://github.com/OpenArkStudio/ark/blob/main/build/conf/sample1.app.conf)决定加载哪些插件；对应插件文件需要与应用来自同一次构建，并复制到运行目录的 `build/bin/`。命令行参数见[应用启动器](../guides/applications_CN.md)，示例实现见[创建插件](../guides/plugins_CN.md)。

## 配置与服务发现

- 仓库内的配置位于 `build/conf/`，数据资源位于 `build/resource/`。
- Excel 配置生成器是独立工具，其 Python、openpyxl 依赖和生成文件见[配置工具](../guides/tools_CN.md)。
- 使用 Consul 插件的部署需要自行准备 Consul 服务和匹配的应用配置。其他插件组合不一定需要 Consul。

<Aside type="caution" title="运行部署脚本前先核对配置">

现有部分多进程启动脚本仍沿用旧目录布局和服务设置。运行前先核对程序名、插件路径、端口和所选应用配置。

</Aside>

## 单独验证网络功能

[网络回归测试](../networking_CN.md)使用可编译运行的 C++ 程序，不需要 Python 或 Excel 配置生成器。
